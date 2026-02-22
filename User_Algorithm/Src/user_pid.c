/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_pid.h"
#include "../../Core/Inc/bsp.h"

/* 函数体 --------------------------------------------------------------------*/

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
void PID_Init(PID_Controller *pid, const Err_Calculate err_calculate,
              const float kp, const float ki, const float kd,
              const float max_out, const float max_iout) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->max_out = max_out;
    pid->max_iout = max_iout;

    pid->err_calculate = err_calculate;

}

/**
* @brief 设定 PID 目标值
* @param pid      PID 控制器结构体指针
* @param target   目标值
*/
void PID_SetTarget(PID_Controller *pid, const float target) {
    pid->set = target;
}

/**
* @brief 计算 PID 控制输出
* @param pid      PID 控制器结构体指针
* @param feedback 反馈值
* @return PID 控制输出值
*/
float PID_Calculate(PID_Controller *pid, const float feedback) {
    pid->fdb = feedback;

    pid->err[0] = pid->err_calculate(pid->fdb, pid->set);

    pid->err[0] = FIR_Update(&user_fir_1, pid->err[0]);
    pid->err[0] = FIR_Update(&user_fir_1, pid->err[0]);

    pid->Pout = pid->kp * pid->err[0] * (1 / (pid->err[0] * pid->err[0] / 10000000.0f + 1));
    pid->Iout += pid->ki * pid->err[0];
    pid->Dout = pid->kd * (pid->err[0] - pid->err[1]) * (( 1 - (1 / (pid->err[0] * pid->err[0] / 1000000.0f + 1))) * 0.7f + 0.3f);

    pid->Dout = FIR_Update(&user_fir_2, pid->Dout);
    pid->Dout = FIR_Update(&user_fir_2, pid->Dout);

    // 积分限幅
    if (pid->Iout > pid->max_iout) {
        pid->Iout =  pid->max_iout * (1 / (pid->err[0] * pid->err[0] / 3000.0f + 1));
    } else if (pid->Iout < -pid->max_iout) {
        pid->Iout = -pid->max_iout * (1 / (pid->err[0] * pid->err[0] / 3000.0f + 1));
    }

    pid->out = pid->Pout + pid->Iout + pid->Dout;

    // 输出限幅
    if (pid->out > pid->max_out) {
        pid->out =  pid->max_out;
    } else if (pid->out < -pid->max_out) {
        pid->out = -pid->max_out;
    }

    pid->err[1] = pid->err[0];

    pid->out = FIR_Update(&user_fir_3, pid->out);
    pid->out = FIR_Update(&user_fir_3, pid->out);

    jscope_transmit.val_1 = pid->Pout;
    jscope_transmit.val_2 = pid->Iout;
    jscope_transmit.val_3 = pid->Dout;

    return pid->out;
}
