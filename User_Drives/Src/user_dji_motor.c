/* 包含头文件 ----------------------------------------------------------------*/
#include <math.h>
#include "../Inc/user_dji_motor.h"
#include "../../Core/Inc/bsp.h"

/* 私有变量 ------------------------------------------------------------------*/
static DJI_MOTOR_DRIVES *motor_drives[DJI_MOTOR_NUM];
static uint8_t motor_num = 0;

/* 私有函数 ------------------------------------------------------------------*/

/**
* @brief 计算角度误差 ( 0 ~ 8191 循环 )
* @param feedback 反馈角度值
* @param target   目标角度值
* @return 最小角度误差
*/
static float angle_error(const float feedback, const float target) {
    float error = target - feedback;

    if (fabsf(target - feedback - 8192.0f) < fabsf(error))
        error = target - feedback - 8192.0f;

    if (fabsf(target - feedback + 8192.0f) < fabsf(error))
        error = target - feedback + 8192.0f;

    return error;
}

/**
* @brief 计算常规误差
* @param feedback 反馈值
* @param target   目标值
* @return 误差值
*/
static float general_error(const float feedback, const float target) {
    return target - feedback;
}

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 设置电机目标值
* @param user_motor  大疆电机驱动结构体指针
* @param target 目标值
*/
void DJI_Motor_Set_Target(DJI_MOTOR_DRIVES *user_motor, float target) {
    user_motor->target = target;
}

/**
* @brief 初始化大疆电机
* @param user_motor      大疆电机驱动结构体指针
* @param user_can  CAN 总线结构体指针
* @param id         电机 ID (1 ~ 7)
* @param motor_type 电机型号
* @param mode       控制模式
* @param kp         PID 比例系数
* @param ki         PID 积分系数
* @param kd         PID 微分系数
* @param max_out    PID 输出限幅
* @param max_iout   PID 积分限幅
*/
void DJI_Motor_Init(DJI_MOTOR_DRIVES* user_motor, CAN_DRIVES* user_can, uint8_t id, 
                    Dji_Motor_Type motor_type, Dji_Control_Mode mode, 
                    float kp, float ki, float kd, float max_out, float max_iout) {
    user_motor->can = user_can;
    user_motor->id = id;
    user_motor->motor_type = motor_type;
    user_motor->control_mode = mode;
    user_motor->target = 0;

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

    switch (user_motor->control_mode) {
        case Rotor_angle:
            PID_Init(&user_motor->pid_controller, angle_error, kp, ki, kd, max_out, max_iout);
            break;
        case Rotor_speed:
        case Torque_current:
            PID_Init(&user_motor->pid_controller, general_error, kp, ki, kd, max_out, max_iout);
            break;
        case OpenLoop_current:
            break;
    }

    motor_drives[motor_num] = user_motor;
    motor_num++;
}

/**
* @brief 处理电机反馈数据
* @param user_can CAN 总线驱动结构体指针
* @note  需要在相应的 CAN 总线接收回调函数中调用
*/
void DJI_Motor_Handle(const CAN_DRIVES* user_can) {
    for (uint8_t motor_index = 0; motor_index < motor_num; motor_index++) {
        DJI_MOTOR_DRIVES *motor = motor_drives[motor_index];

        if (motor->can->hcan != user_can->hcan || motor->fdb_id != user_can->rx_msg.StdId)
            continue;

        const uint8_t *data = user_can->rx_msg.Data;
        motor->rotor_angle    = (uint16_t)(data[0] << 8 | data[1]);
        motor->rotor_speed    = (int16_t) (data[2] << 8 | data[3]);
        motor->torque_current = (int16_t) (data[4] << 8 | data[5]);
        motor->temperate      = data[6];

        if (motor->control_mode == OpenLoop_current)
            continue;

        switch (motor->control_mode) {
            case Rotor_speed:
                PID_Calculate(&motor->pid_controller, motor->target, (float)motor->rotor_speed);
                break;
            case Rotor_angle:
                PID_Calculate(&motor->pid_controller, motor->target, (float)motor->rotor_angle);
                break;
            case Torque_current:
                PID_Calculate(&motor->pid_controller, motor->target, (float)motor->torque_current);
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
void DJI_Motor_Execute(const CAN_DRIVES* user_can) {
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
            current_target = (int16_t)motor->pid_controller.out;
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
