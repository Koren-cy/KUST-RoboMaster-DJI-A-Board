#ifndef USER_PID_H
#define USER_PID_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "user_controller.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief PID 控制器结构体
*/
typedef struct {
    CONTROLLER_INTERFACE_FUNC      /* 控制器接口函数 */
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
    float err[2];                  /* 误差数组: [0]当前误差 [1]上次误差 */
} PID_Controller;

/* 函数声明 ------------------------------------------------------------------*/
void PID_Init(PID_Controller *pid, float kp, float ki, float kd,
              float max_out, float max_iout);

/* 接口函数声明 --------------------------------------------------------------*/
void PID_Set_Target(void* controller, float target);
float PID_Calculate(void* controller, float main_feedback, float sub_feedback);
float PID_Get_Output(void* controller);


#endif // USER_PID_H
