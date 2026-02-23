#ifndef __USER_PID_6020_POSITION_H__
#define __USER_PID_6020_POSITION_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 类型定义 ------------------------------------------------------------------*/
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
} PID_Controller;

/* 函数声明 ------------------------------------------------------------------*/
void PID_Init(PID_Controller *pid, Err_Calculate err_calculate, 
              float kp, float ki, float kd,
              float max_out, float max_iout);

void PID_SetTarget(PID_Controller *pid, float target);

float PID_Calculate(PID_Controller *pid, float feedback);

  // DJI_Motor_Init(&test_GM6020, &user_can_1, 1, GM6020, Rotor_angle, 45.0f , 1.0f, 2000.0f, 16000, 1500);


#endif //__USER_PID_6020_POSITION_H__
