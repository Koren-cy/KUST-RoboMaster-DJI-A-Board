/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_pid.h"
#include <string.h>

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 PID 控制器
* @param pid           PID 控制器结构体指针
* @param kp            PID 比例系数
* @param ki            PID 积分系数
* @param kd            PID 微分系数
* @param max_out       PID 输出限幅
* @param max_iout      PID 积分限幅
*/
void PID_Init(PID_Controller *pid, const float kp, const float ki, const float kd,
              const float max_out, const float max_iout) {
    memset(pid, 0, sizeof(PID_Controller));
    
    // 绑定接口函数
    pid->Set_Target = PID_Set_Target;
    pid->Calculate = PID_Calculate;
    pid->Get_Output = PID_Get_Output;
    
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->max_out = max_out;
    pid->max_iout = max_iout;
}

/* 接口函数实现 --------------------------------------------------------------*/

/**
* @brief 设定 PID 目标值
* @param controller 控制器结构体指针
* @param target     目标值
*/
void PID_Set_Target(void* controller, const float target) {
    PID_Controller* pid = (PID_Controller*)controller;
    pid->set = target;
}

/**
* @brief 计算 PID 控制输出
* @param controller 控制器结构体指针
* @param main_feedback   反馈值
* @param sub_feedback   接口定义参数，在该模块中无实际意义
* @return PID 控制输出值
*/
float PID_Calculate(void* controller, const float main_feedback, const float sub_feedback) {
    PID_Controller* pid = (PID_Controller*)controller;
    pid->fdb = main_feedback;

    pid->err[0] = pid->set - pid->fdb;

    pid->Pout = pid->kp *  pid->err[0];
    pid->Iout += pid->ki * pid->err[0];
    pid->Dout = pid->kd * (pid->err[0] - pid->err[1]);

    // 积分限幅
    if (pid->Iout > pid->max_iout) {
        pid->Iout = pid->max_iout;
    } else if (pid->Iout < -pid->max_iout) {
        pid->Iout = -pid->max_iout;
    }

    float out = pid->Pout + pid->Iout + pid->Dout;

    // 输出限幅
    if (out > pid->max_out) {
        out = pid->max_out;
    } else if (out < -pid->max_out) {
        out = -pid->max_out;
    }

    pid->out = out;
    pid->err[1] = pid->err[0];

    return out;
}

/**
* @brief 获取 PID 控制输出
* @param controller 控制器结构体指针
* @return PID 控制输出值
*/
float PID_Get_Output(void* controller) {
    const PID_Controller* pid = (PID_Controller*)controller;
    return pid->out;
}
