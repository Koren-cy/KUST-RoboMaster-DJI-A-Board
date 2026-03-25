#include "../../Core/Inc/bsp.h"
#ifdef HAL_CAN_MODULE_ENABLED
/* 包含头文件 ----------------------------------------------------------------*/
#include <math.h>
#include "../user_dji_motor.h"

/* 私有变量 ------------------------------------------------------------------*/
static DJI_MOTOR_DRIVES* motor_drives[DJI_MOTOR_NUM];
static uint8_t motor_num = 0;

/* 私有函数声明 --------------------------------------------------------------*/

static void DJI_Motor_Handle(void* user_can);

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化大疆电机
* @param user_motor          大疆电机驱动结构体指针
* @param user_can            CAN 总线结构体指针
* @param id                  电机 ID (1 ~ 7)
* @param rotor_angle_offset  电机零点偏移量 单位：角度
* @param motor_type          电机型号
* @param mode                控制模式
* @param controller          PID 或 ADRC 等控制器，开环模式下可以设置为 NULL
*/
void DJI_Motor_Init(DJI_MOTOR_DRIVES *user_motor, CAN_DRIVES* user_can, const uint8_t id, const float rotor_angle_offset,
                    const Dji_Motor_Type motor_type, const Dji_Control_Mode mode, CONTROLLER_INTERFACE* controller) {

    // 绑定接口
    user_motor->Set_Motor_State = DJI_Motor_Set_State;
    user_motor->Get_Motor_Speed = DJI_Motor_Get_Speed;
    user_motor->Get_Motor_Angle = DJI_Motor_Get_Angle;
    user_motor->Get_Motor_Current = DJI_Motor_Get_Current;
    
    user_motor->can = user_can;
    user_motor->id = id;
    user_motor->motor_type = motor_type;
    user_motor->control_mode = mode;
    user_motor->rotor_angle_offset = (int32_t)((float)rotor_angle_offset / 360.0f * 8191.0f);
    user_motor->controller = controller;

    switch (motor_type) {
        case GM6020:
            user_motor->controller_type = GM6020_Controller;
            break;
        case M3508_direct:
        case M3508_gear:
            user_motor->controller_type = C620_Controller;
            break;
        case M2006:
            user_motor->controller_type = C610_Controller;
            break;
    }

    switch (user_motor->controller_type) {
        case GM6020_Controller:
            user_motor->fdb_id  = GM6020_FEEDBACK_BASE_ID + id;
            if (1 <= id && id <= 4) user_motor->ctrl_id = GM6020_CURRENT_CONTROL_ID_1;
            if (5 <= id && id <= 7) user_motor->ctrl_id = GM6020_CURRENT_CONTROL_ID_2;
            break;
        case C620_Controller:
        case C610_Controller:
            user_motor->fdb_id = C6x0_FEEDBACK_BASE_ID + id;
            if (1 <= id && id <= 4) user_motor->ctrl_id = C6x0_CURRENT_CONTROL_ID_1;
            if (5 <= id && id <= 8) user_motor->ctrl_id = C6x0_CURRENT_CONTROL_ID_2;
            break;
    }

    uint8_t is_callback_register = 0;
    for (uint8_t motor_index = 0; motor_index < motor_num; motor_index++) {
        const DJI_MOTOR_DRIVES *motor = motor_drives[motor_index];
        if (motor->can->hcan == user_can->hcan)
            is_callback_register = 1;
    }

    motor_drives[motor_num] = user_motor;
    motor_num++;

    if (is_callback_register == 0)
        CAN_RegisterCallback(user_can, DJI_Motor_Handle);

}

/**
* @brief 处理电机反馈数据
* @param user_can CAN 总线驱动结构体指针
* @note  会在初始化时自动注册到 CAN 总线接收回调函数表
*/
static void DJI_Motor_Handle(void* user_can) {
    const CAN_DRIVES* can = (CAN_DRIVES*)user_can;
    for (uint8_t motor_index = 0; motor_index < motor_num; motor_index++) {
        DJI_MOTOR_DRIVES *motor = motor_drives[motor_index];

        if (motor->can->hcan != can->hcan || motor->fdb_id != can->rx_msg.StdId)
            continue;

        const uint16_t last_rotor_angle = motor->rotor_angle;

        const uint8_t *data = can->rx_msg.Data;
        motor->rotor_angle    = (uint16_t)(data[0] << 8 | data[1]);
        motor->rotor_speed    = (int16_t) (data[2] << 8 | data[3]);
        motor->torque_current = (int16_t) (data[4] << 8 | data[5]);
        motor->temperate      = data[6];

        int32_t delta_angle = motor->rotor_angle - last_rotor_angle;
        
        if (delta_angle > 4096) {
            delta_angle -= 8192;
        } else if (delta_angle < -4096) {
            delta_angle += 8192;
        }
        
        motor->total_angle += delta_angle;
        motor->total_angle += motor->rotor_angle_offset;
        motor->rotor_angle_offset = 0;

        if (motor->control_mode == OpenLoop_current)
            continue;

        switch (motor->control_mode) {
            case Rotor_speed:
                motor->controller->Calculate(motor->controller, DJI_Motor_Get_Speed(motor),0.0f);
                break;
            case Rotor_angle:
                motor->controller->Calculate(motor->controller, DJI_Motor_Get_Angle(motor),DJI_Motor_Get_Speed(motor));
                break;
            case Torque_current:
                motor->controller->Calculate(motor->controller, DJI_Motor_Get_Current(motor),0.0f);
                break;
            default:
                break;
        }
    }
}

/**
* @brief 执行电机控制指令
* @param user_can CAN 总线驱动结构体指针
* @note  将所有电机控制指令一并打包发送
*/
void DJI_Motor_Execute(CAN_DRIVES* user_can) {
    uint8_t GM6020_control_id_1_frame[8] = {0};
    uint8_t GM6020_control_id_2_frame[8] = {0};
    uint8_t C6x0_control_id_1_frame[8] = {0};
    uint8_t C6x0_control_id_2_frame[8] = {0};
    uint8_t GM6020_control_id_1_sign = 0;
    uint8_t GM6020_control_id_2_sign = 0;
    uint8_t C6x0_control_id_1_sign = 0;
    uint8_t C6x0_control_id_2_sign = 0;

    for (uint8_t motor_index = 0; motor_index < motor_num; motor_index++) {
        const DJI_MOTOR_DRIVES *motor = motor_drives[motor_index];
        if (motor->can != user_can)
            continue;

        int16_t current_target = 0;

        if (motor->control_mode == OpenLoop_current) {
            current_target = (int16_t)motor->target;
        } else {
            current_target = (int16_t)motor->controller->Get_Output(motor->controller);
        }

        switch (motor->ctrl_id) {
            case GM6020_CURRENT_CONTROL_ID_1:
                GM6020_control_id_1_frame[2 * motor->id - 2] = (uint8_t)(current_target >> 8);
                GM6020_control_id_1_frame[2 * motor->id - 1] = (uint8_t)(current_target >> 0);
                GM6020_control_id_1_sign = 1;
                break;
            case GM6020_CURRENT_CONTROL_ID_2:
                GM6020_control_id_2_frame[2 * motor->id - 10] = (uint8_t)(current_target >> 8);
                GM6020_control_id_2_frame[2 * motor->id - 9]  = (uint8_t)(current_target >> 0);
                GM6020_control_id_2_sign = 1;
                break;
            case C6x0_CURRENT_CONTROL_ID_1:
                C6x0_control_id_1_frame[2 * motor->id - 2] = (uint8_t)(current_target >> 8);
                C6x0_control_id_1_frame[2 * motor->id - 1] = (uint8_t)(current_target >> 0);
                C6x0_control_id_1_sign = 1;
                break;
            case C6x0_CURRENT_CONTROL_ID_2:
                C6x0_control_id_2_frame[2 * motor->id - 10] = (uint8_t)(current_target >> 8);
                C6x0_control_id_2_frame[2 * motor->id - 9]  = (uint8_t)(current_target >> 0);
                C6x0_control_id_2_sign = 1;
                break;
            default:
                break;
        }
    }

    for (uint8_t motor_index = 0; motor_index < motor_num; motor_index++) {
        const DJI_MOTOR_DRIVES *motor = motor_drives[motor_index];
        if (motor->can != user_can)
            continue;

        if (GM6020_control_id_1_sign) {
            CAN_Send(user_can, GM6020_CURRENT_CONTROL_ID_1, GM6020_control_id_1_frame, 8);
            GM6020_control_id_1_sign = 0;
        }
        if (GM6020_control_id_2_sign) {
            CAN_Send(user_can, GM6020_CURRENT_CONTROL_ID_2, GM6020_control_id_2_frame, 8);
            GM6020_control_id_2_sign = 0;
        }
        if (C6x0_control_id_1_sign) {
            CAN_Send(user_can, C6x0_CURRENT_CONTROL_ID_1, C6x0_control_id_1_frame, 8);
            C6x0_control_id_1_sign = 0;
        }
        if (C6x0_control_id_2_sign) {
            CAN_Send(user_can, C6x0_CURRENT_CONTROL_ID_2, C6x0_control_id_2_frame, 8);
            C6x0_control_id_2_sign = 0;
        }

    }
}

/* 接口函数实现 --------------------------------------------------------------*/
/**
* @brief 设置电机运动状态
* @param motor 电机驱动结构体指针
* @param value 目标值
*/
void DJI_Motor_Set_State(void* motor, const float value) {
    DJI_MOTOR_DRIVES* dji_motor = (DJI_MOTOR_DRIVES*)motor;
    if (dji_motor->control_mode == OpenLoop_current) {
        dji_motor->target = value;
    } else {
        dji_motor->controller->Set_Target(dji_motor->controller, value);
    }
}

/**
* @brief 获取电机转速
* @param motor 电机驱动结构体指针
* @return 电机转速 单位：rpm
*/
float DJI_Motor_Get_Speed(void* motor) {
    const DJI_MOTOR_DRIVES* dji_motor = (DJI_MOTOR_DRIVES*)motor;
    return (float)dji_motor->rotor_speed;
}

/**
* @brief 获取电机多圈角度
* @param motor 电机驱动结构体指针
* @return 电机角度 单位：度
*/
float DJI_Motor_Get_Angle(void* motor) {
    const DJI_MOTOR_DRIVES* dji_motor = (DJI_MOTOR_DRIVES*)motor;
    return (float)dji_motor->total_angle / 8191.0f * 360.0f;
}

/**
* @brief 获取电机转矩电流
* @param motor 电机驱动结构体指针
* @return 转矩电流 单位：mA
*/
float DJI_Motor_Get_Current(void* motor) {
    const DJI_MOTOR_DRIVES* dji_motor = (DJI_MOTOR_DRIVES*)motor;
    return (float)dji_motor->torque_current / 16384.0f * 3000.0f;
}

#endif /* HAL_CAN_MODULE_ENABLED */
