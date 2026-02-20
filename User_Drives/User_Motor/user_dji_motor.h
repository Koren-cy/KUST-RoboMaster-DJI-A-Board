#ifndef __USER_DJI_MOTOR_H__
#define __USER_DJI_MOTOR_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"
#include "../../User_Algorithm/user_pid.h"
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
    GM6020,        /* GM6020 无刷云台电机 */
    M3508_gear,    /* M3508 减速电机 */
    M3508_direct,  /* M3508 直驱电机 */
    M2006,         /* M2006 电机 */
} Dji_Motor_Type;

/**
* @brief 电调型号
*/
typedef enum {
    GM6020_Controller,  /* GM6020 电调 */
    C620_Controller,    /* C620 电调 */
    C610_Controller,    /* C610 电调 */
} Dji_Controller_Type;

/**
* @brief 控制模式
*/
typedef enum {
    Rotor_angle,      /* 转子角度控制 */
    Rotor_speed,      /* 转子速度控制 */
    Torque_current,   /* 转矩电流控制 */
    OpenLoop_current, /* 开环电流控制 */
} Dji_Control_Mode;

/**
* @brief 大疆电机驱动结构体
*/
typedef struct {
    CAN_DRIVES *can;                      /* CAN 总线驱动结构体指针 */
    Dji_Motor_Type motor_type;            /* 电机型号 */
    Dji_Controller_Type controller_type;  /* 电调型号 */
    Dji_Control_Mode control_mode;        /* 控制模式 */
    float target;                         /* 开环模式目标值 */
    uint8_t id;                           /* 电机 ID (1 ~ 7) */
    uint16_t ctrl_id;                     /* 控制帧 ID */
    uint16_t fdb_id;                      /* 反馈帧 ID */
    uint16_t rotor_angle;                 /* 转子角度 (0 ~ 8191) */
    int16_t rotor_speed;                  /* 转子速度 单位: rpm */
    int16_t torque_current;               /* 转矩电流 */
    uint8_t temperate;                    /* 温度 单位: 摄氏度 */
    PID_Controller pid_controller;        /* PID 控制器 */
} DJI_MOTOR_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void DJI_Motor_Init(DJI_MOTOR_DRIVES *user_motor, CAN_DRIVES* user_can, uint8_t id,
                    Dji_Motor_Type motor_type, Dji_Control_Mode mode,
                    float kp, float ki, float kd, float max_out, float max_iout);

void DJI_Motor_Set_Target(DJI_MOTOR_DRIVES *user_motor, float target);
void DJI_Motor_Handle(const CAN_DRIVES* user_can);
void DJI_Motor_Execute(const CAN_DRIVES* user_can);

#endif //__USER_DJI_MOTOR_H__
