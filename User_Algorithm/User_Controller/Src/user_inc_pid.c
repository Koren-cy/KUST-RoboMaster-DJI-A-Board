/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_inc_pid.h"
#include <string.h>

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化增量式 PID 控制器
* @param pid           增量式 PID 控制器结构体指针
* @param kp            PID 比例系数
* @param ki            PID 积分系数
* @param kd            PID 微分系数
* @param max_increase  PID 输出增量限幅
* @param max_out       PID 输出限幅
*/
void Inc_PID_Init(Inc_PID_Controller *pid, const float kp, const float ki,
                          const float kd, const float max_increase, const float max_out) {
    memset(pid, 0, sizeof(Inc_PID_Controller));
    
    // 绑定接口函数
    pid->Set_Target = Inc_PID_Set_Target;
    pid->Calculate = Inc_PID_Calculate;
    pid->Get_Output = Inc_PID_Get_Output;
    
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->max_increase = max_increase;
    pid->max_out = max_out;
}

/* 接口函数实现 --------------------------------------------------------------*/

/**
* @brief 设定增量式 PID 目标值
* @param controller 控制器结构体指针
* @param target     目标值
*/
void Inc_PID_Set_Target(void* controller, const float target) {
    Inc_PID_Controller* pid = (Inc_PID_Controller*)controller;
    pid->set = target;
}

/**
* @brief 计算增量式 PID 控制输出
* @param controller      控制器结构体指针
* @param main_feedback   反馈值
* @param sub_feedback    接口定义参数，在该模块中无实际意义
* @return 增量式 PID 控制输出增量值
*/
float Inc_PID_Calculate(void* controller, const float main_feedback,
                                 const float sub_feedback) {
    Inc_PID_Controller* pid = (Inc_PID_Controller*)controller;
    pid->fdb = main_feedback;

    // 更新误差
    pid->err[2] = pid->err[1];
    pid->err[1] = pid->err[0];
    pid->err[0] = pid->set - pid->fdb;

    // 增量式PID计算
    float out = pid->kp * (pid->err[0] - pid->err[1]) +
               pid->ki * pid->err[0] +
               pid->kd * (pid->err[0] - 2.0f * pid->err[1] + pid->err[2]);

    // 输出增量限幅
    if (out > pid->max_increase) {
        out = pid->max_increase;
    } else if (out < -pid->max_increase) {
        out = -pid->max_increase;
    }

    out = out + pid->out;

    // 输出限幅
    if (out > pid->max_out) {
        out = pid->max_out;
    } else if (out < -pid->max_out) {
        out = -pid->max_out;
    }

    pid->out = out;

    return out;
}

/**
* @brief 获取增量式 PID 控制输出
* @param controller 控制器结构体指针
* @return 增量式 PID 控制输出增量值
*/
float Inc_PID_Get_Output(void* controller) {
    const Inc_PID_Controller* pid = (Inc_PID_Controller*)controller;
    return pid->out;
}
