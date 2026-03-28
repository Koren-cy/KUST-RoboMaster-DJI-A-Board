#ifndef __USER_DJI_MOTOR_OLD_H__
#define __USER_DJI_MOTOR_OLD_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"
#include "user_motor.h"
#include "../../User_Drives/user_can.h"

/* 宏定义 --------------------------------------------------------------------*/
#define GM6020_CURRENT_CONTROL_ID_1   (0x1FE)
#define GM6020_CURRENT_CONTROL_ID_2   (0x2FE)
#define GM6020_FEEDBACK_BASE_ID       (0x204)

#define C6x0_CURRENT_CONTROL_ID_1     (0x200)
#define C6x0_CURRENT_CONTROL_ID_2     (0x1FF)
#define C6x0_FEEDBACK_BASE_ID         (0x200)


// 可以看到，大疆不同型号电调的 ID 可能存在冲突。
// 其中 C610 电调和 C620 电调的 ID 全部一致。
// 故统一定义为 C6x0 电调。

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 电机型号
*/
typedef enum {
    GM6020_old,        /* GM6020 无刷云台电机 */
    M3508_gear_old,    /* M3508 减速电机 */
    M3508_direct_old,  /* M3508 直驱电机 */
    M2006_old,         /* M2006 电机 */
} Dji_Motor_OLD_Type;

/**
* @brief 电调型号
*/
typedef enum {
    GM6020_Controller_old,  /* GM6020 电调 */
    C620_Controller_old,    /* C620 电调 */
    C610_Controller_old,    /* C610 电调 */
} Dji_Controller_OLD_Type;

/**
* @brief 控制模式
*/
typedef enum {
    Rotor_angle_old,      /* 转子角度控制 */
    Rotor_speed_old,      /* 转子速度控制 */
    Torque_current_old,   /* 转矩电流控制 */
    OpenLoop_current_old, /* 开环电流控制 */
} Dji_Control_OLD_Mode;

/**
* @brief 误差计算函数指针类型
* @param feedback 反馈值
* @param target   目标值
* @return 误差值
*/
typedef float (*Err_Calculate)(const float feedback, const float target);

/**
* @brief PID 控制器结构体
*/
typedef struct {
    float kp;                      /* PID 比例系数 */
    float ki;                      /* PID 积分系数 */
    float kd;                      /* PID 微分系数 */
    float max_out;                 /* PID 输出限幅 */
    float max_iout;                /* PID 积分限幅 */
    float set;                     /* 目标值 */
    float fdb;                     /* 反馈值 */
    float out;                     /* PID 总输出 */
    float Pout;                    /* 比例项输出 */
    float Iout;                    /* 积分项输出 */
    float Dout;                    /* 微分项输出 */
    Err_Calculate err_calculate;   /* 误差计算函数指针 */
    float err[2];                  /* 误差数组: [0]当前误差 [1]上次误差 */
} PID_Controller_OLD;

/**
* @brief 大疆电机驱动结构体
*/
typedef struct {
    MOTOR_DRIVES_INTERFACE_FUNC
    CAN_DRIVES *can;                          /* CAN 总线驱动结构体指针 */
    Dji_Motor_OLD_Type motor_type;            /* 电机型号 */
    Dji_Controller_OLD_Type controller_type;  /* 电调型号 */
    Dji_Control_OLD_Mode control_mode;        /* 控制模式 */
    float target;                             /* 目标值 */
    uint8_t id;                               /* 电机 ID (1 ~ 7) */
    uint16_t ctrl_id;                         /* 控制帧 ID */
    uint16_t fdb_id;                          /* 反馈帧 ID */
    uint16_t rotor_angle;                     /* 转子角度 (0 ~ 8191) */
    int16_t rotor_speed;                      /* 转子速度 单位: rpm */
    int16_t torque_current;                   /* 转矩电流 */
    uint8_t temperate;                        /* 温度 单位: 摄氏度 */
    PID_Controller_OLD pid_controller;        /* PID 控制器 */
} DJI_MOTOR_OLD_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void DJI_Motor_Old_Init(DJI_MOTOR_OLD_DRIVES *user_motor, CAN_DRIVES* user_can, uint8_t id,
                    Dji_Motor_OLD_Type motor_type, Dji_Control_OLD_Mode mode,
                    float kp, float ki, float kd, float max_out, float max_iout);

void DJI_Motor_Old_Execute(CAN_DRIVES* user_can);

/* 接口函数声明 --------------------------------------------------------------*/
void DJI_Motor_Old_Set_State(void* motor, float value);
float DJI_Motor_Old_Get_Speed(void* motor);
float DJI_Motor_Old_Get_Angle(void* motor);
float DJI_Motor_Old_Get_Current(void* motor);


#endif //__USER_DJI_MOTOR_OLD_H__
