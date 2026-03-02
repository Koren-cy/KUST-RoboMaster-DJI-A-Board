#ifndef USER_INC_PID_H
#define USER_INC_PID_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "user_controller.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 增量式PID 控制器结构体
*/
typedef struct {
    CONTROLLER_INTERFACE_FUNC      /* 控制器接口函数 */
    float kp;                      /* PID 比例系数 */
    float ki;                      /* PID 积分系数 */
    float kd;                      /* PID 微分系数 */
    float max_increase;            /* PID 输出增量限幅 */
    float max_out;                 /* PID 输出限幅 */
    float set;                     /* 目标值 */
    float fdb;                     /* 反馈值 */
    float out;                     /* PID 输出增量 */
    float err[3];                  /* 误差数组: [0]当前误差 [1]上次误差 [2]上上次误差 */
} Inc_PID_Controller;

/* 函数声明 ------------------------------------------------------------------*/
void Inc_PID_Init(Inc_PID_Controller *pid, float kp, float ki, float kd,
                          float max_increase, float max_out);

/* 接口函数声明 --------------------------------------------------------------*/
void Inc_PID_Set_Target(void* controller, float target);
float Inc_PID_Calculate(void* controller, float main_feedback, float sub_feedback);
float Inc_PID_Get_Output(void* controller);


#endif // USER_INC_PID_H
