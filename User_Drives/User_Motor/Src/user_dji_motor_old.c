/* 包含头文件 ----------------------------------------------------------------*/
#include <math.h>
#include "../user_dji_motor_old.h"
#include "../../Core/Inc/bsp.h"

/* 私有变量 ------------------------------------------------------------------*/
static DJI_MOTOR_OLD_DRIVES *motor_drives[DJI_MOTOR_NUM];
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


/**
* @brief 初始化 PID 控制器
* @param pid           PID 控制器结构体指针
* @param err_calculate 误差计算函数指针
* @param kp            PID 比例系数
* @param ki            PID 积分系数
* @param kd            PID 微分系数
* @param max_out       PID 输出限幅
* @param max_iout      PID 积分限幅
*/
static void PID_Init_Old(PID_Controller_OLD *pid, const Err_Calculate err_calculate,
              const float kp, const float ki, const float kd,
              const float max_out, const float max_iout) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->max_out = max_out;
    pid->max_iout = max_iout;

    pid->err[0] = 0.0f;
    pid->err[1] = 0.0f;

    pid->Pout = 0.0f;
    pid->Iout = 0.0f;
    pid->Dout = 0.0f;
    pid->out = 0.0f;

    pid->err_calculate = err_calculate;

    pid->set = 0.0f;
    pid->fdb = 0.0f;
}

/**
* @brief 计算 PID 控制输出
* @param pid      PID 控制器结构体指针
* @param target   目标值
* @param feedback 反馈值
* @return PID 控制输出值
*/
static float PID_Calculate_Old(PID_Controller_OLD *pid, const float target, const float feedback) {
    pid->set = target;
    pid->fdb = feedback;

    pid->err[0] = pid->err_calculate(pid->fdb, pid->set);

    pid->Pout = pid->kp * pid->err[0];

    const float p_range = 100.0f;
    if (pid->err[0] > p_range) {
        pid->Pout = pid->kp * ((pid->err[0] - p_range) * 0.1f + p_range);
    }
    if (pid->err[0] < -p_range) {
        pid->Pout = pid->kp * ((pid->err[0] + p_range) * 0.1f - p_range);
    }

    pid->Iout += pid->ki * pid->err[0];

    pid->Dout = pid->kd * (pid->err[0] - pid->err[1]);

    if        (fabsf(pid->err[0] - pid->err[1]) < 5) {
        pid->Dout = pid->Dout * 0.30f;
    } else if (fabsf(pid->err[0] - pid->err[1]) < 45) {
        pid->Dout = pid->Dout * 0.40f;
    } else if (fabsf(pid->err[0] - pid->err[1]) < 125) {
        pid->Dout = pid->Dout * 0.50f;
    } else if (fabsf(pid->err[0] - pid->err[1]) < 250) {
        pid->Dout = pid->Dout * 0.60f;
    } else if (fabsf(pid->err[0] - pid->err[1]) < 500) {
        pid->Dout = pid->Dout * 0.70f;
    } else if (fabsf(pid->err[0] - pid->err[1]) < 1000) {
        pid->Dout = pid->Dout * 0.80f;
    } else if (fabsf(pid->err[0] - pid->err[1]) < 2000) {
        pid->Dout = pid->Dout * 0.90f;
    }

    // 积分限幅
    if (pid->Iout > pid->max_iout) {
        pid->Iout = pid->max_iout;
    } else if (pid->Iout < -pid->max_iout) {
        pid->Iout = -pid->max_iout;
    }

    pid->out = pid->Pout + pid->Iout + pid->Dout;

    // 输出限幅
    if (pid->out > pid->max_out) {
        pid->out = pid->max_out;
    } else if (pid->out < -pid->max_out) {
        pid->out = -pid->max_out;
    }

    pid->err[1] = pid->err[0];

    return pid->out;
}

/**
* @brief 处理电机反馈数据
* @param user_can CAN 总线驱动结构体指针
* @note  会在初始化时自动注册到 CAN 总线接收回调函数表
*/
static void DJI_Motor_Old_Handle(void* user_can) {
    const CAN_DRIVES* can = (CAN_DRIVES*)user_can;
    for (uint8_t motor_index = 0; motor_index < motor_num; motor_index++) {
        DJI_MOTOR_OLD_DRIVES *motor = motor_drives[motor_index];

        if (motor->can->hcan != can->hcan || motor->fdb_id != can->rx_msg.StdId)
            continue;

        const uint8_t *data = can->rx_msg.Data;
        motor->rotor_angle    = (uint16_t)(data[0] << 8 | data[1]);
        motor->rotor_speed    = (int16_t) (data[2] << 8 | data[3]);
        motor->torque_current = (int16_t) (data[4] << 8 | data[5]);
        motor->temperate      = data[6];

        if (motor->control_mode == OpenLoop_current_old)
            continue;

        switch (motor->control_mode) {
            case Rotor_speed_old:
                PID_Calculate_Old(&motor->pid_controller, motor->target, (float)motor->rotor_speed);
                break;
            case Rotor_angle_old:
                PID_Calculate_Old(&motor->pid_controller, motor->target, (float)motor->rotor_angle);
                break;
            case Torque_current_old:
                PID_Calculate_Old(&motor->pid_controller, motor->target, (float)motor->torque_current);
                break;
            default:
                break;
        }
    }
}



/* 函数体 --------------------------------------------------------------------*/

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
void DJI_Motor_Old_Init(DJI_MOTOR_OLD_DRIVES* user_motor, CAN_DRIVES* user_can, uint8_t id,
                    Dji_Motor_OLD_Type motor_type, Dji_Control_OLD_Mode mode,
                    float kp, float ki, float kd, float max_out, float max_iout) {
    // 绑定接口
    user_motor->Set_Motor_State = DJI_Motor_Old_Set_State;
    user_motor->Get_Motor_Speed = DJI_Motor_Old_Get_Speed;
    user_motor->Get_Motor_Angle = DJI_Motor_Old_Get_Angle;
    user_motor->Get_Motor_Current = DJI_Motor_Old_Get_Current;

    user_motor->can = user_can;
    user_motor->id = id;
    user_motor->motor_type = motor_type;
    user_motor->control_mode = mode;
    user_motor->target = 0;

    switch (motor_type) {
        case GM6020_old:
            user_motor->controller_type = GM6020_Controller_old;
            break;
        case M3508_direct_old:
        case M3508_gear_old:
            user_motor->controller_type = C620_Controller_old;
            break;
        case M2006_old:
            user_motor->controller_type = C610_Controller_old;
            break;
    }

    switch (user_motor->controller_type) {
        case GM6020_Controller_old:
            user_motor->fdb_id  = GM6020_FEEDBACK_BASE_ID + id;
            if (1 <= id && id <= 4) user_motor->ctrl_id = GM6020_CURRENT_CONTROL_ID_1;
            if (5 <= id && id <= 7) user_motor->ctrl_id = GM6020_CURRENT_CONTROL_ID_2;
            break;
        case C620_Controller_old:
        case C610_Controller_old:
            user_motor->fdb_id = C6x0_FEEDBACK_BASE_ID + id;
            if (1 <= id && id <= 4) user_motor->ctrl_id = C6x0_CURRENT_CONTROL_ID_1;
            if (5 <= id && id <= 8) user_motor->ctrl_id = C6x0_CURRENT_CONTROL_ID_2;
            break;
    }

    switch (user_motor->control_mode) {
        case Rotor_angle_old:
            PID_Init_Old(&user_motor->pid_controller, angle_error, kp, ki, kd, max_out, max_iout);
            break;
        case Rotor_speed_old:
        case Torque_current_old:
            PID_Init_Old(&user_motor->pid_controller, general_error, kp, ki, kd, max_out, max_iout);
            break;
        case OpenLoop_current_old:
            break;
    }


    uint8_t is_callback_register = 0;
    for (uint8_t motor_index = 0; motor_index < motor_num; motor_index++) {
        const DJI_MOTOR_OLD_DRIVES *motor = motor_drives[motor_index];
        if (motor->can->hcan == user_can->hcan)
            is_callback_register = 1;
    }

    motor_drives[motor_num] = user_motor;
    motor_num++;

    if (is_callback_register == 0)
        CAN_RegisterCallback(user_can, DJI_Motor_Old_Handle);
}

/**
* @brief 执行电机控制指令
* @param user_can CAN 总线驱动结构体指针
* @note  将所有电机控制指令一并打包发送
*/
void DJI_Motor_Old_Execute(CAN_DRIVES* user_can) {
    uint8_t GM6020_control_id_1_frame[8] = {0};
    uint8_t GM6020_control_id_2_frame[8] = {0};
    uint8_t C6x0_control_id_1_frame[8] = {0};
    uint8_t C6x0_control_id_2_frame[8] = {0};
    uint8_t GM6020_control_id_1_sign = 0;
    uint8_t GM6020_control_id_2_sign = 0;
    uint8_t C6x0_control_id_1_sign = 0;
    uint8_t C6x0_control_id_2_sign = 0;

    for (uint8_t motor_index = 0; motor_index < motor_num; motor_index++) {
        const DJI_MOTOR_OLD_DRIVES *motor = motor_drives[motor_index];
        if (motor->can != user_can)
            continue;

        int16_t current_target = 0;

        if (motor->control_mode == OpenLoop_current_old) {
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
        const DJI_MOTOR_OLD_DRIVES *motor = motor_drives[motor_index];
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
* @brief 设置电机目标值
* @param motor  大疆电机驱动结构体指针
* @param target 目标值
*/
void DJI_Motor_Old_Set_State(void *motor, float target) {
    DJI_MOTOR_OLD_DRIVES *user_motor = (DJI_MOTOR_OLD_DRIVES *) motor;
    if (user_motor->control_mode == Rotor_angle_old) {
        user_motor->target = target / 360.0f * 8191.0f;
    } else {
        user_motor->target = target;
    }
}

/**
* @brief 获取电机转速
* @param motor 大疆电机驱动结构体指针
* @return 电机转速 单位：rpm
*/
float DJI_Motor_Old_Get_Speed(void* motor) {
    const DJI_MOTOR_OLD_DRIVES *user_motor = (DJI_MOTOR_OLD_DRIVES *) motor;
    return (float)user_motor->rotor_speed;
}

/**
* @brief 获取电机角度
* @param motor 大疆电机驱动结构体指针
* @return 电机角度 单位：度
*/
float DJI_Motor_Old_Get_Angle(void* motor) {
    const DJI_MOTOR_OLD_DRIVES *user_motor = (DJI_MOTOR_OLD_DRIVES *) motor;
    return (float)user_motor->rotor_angle / 8191.0f * 360.0f;
}

/**
* @brief 获取电机转矩电流
* @param motor 大疆电机驱动结构体指针
* @return 转矩电流 单位：mA
*/
float DJI_Motor_Old_Get_Current(void* motor) {
    const DJI_MOTOR_OLD_DRIVES *user_motor = (DJI_MOTOR_OLD_DRIVES *) motor;
    return (float)user_motor->torque_current / 16384.0f * 3000.0f;
}

