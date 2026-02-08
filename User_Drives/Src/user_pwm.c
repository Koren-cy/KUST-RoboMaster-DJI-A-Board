/* 包含头文件 ----------------------------------------------------------------*/
#include "../Inc/user_pwm.h"
#include "../../Core/Inc/bsp.h"

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 PWM 驱动
* @param user_pwm       PWM 驱动结构体指针
* @param htim      定时器硬件句柄
* @param channel   PWM 通道
* @param tim_clock 定时器时钟频率  (总线时钟频率)
*/
void PWM_Init(PWM_DRIVES *user_pwm, TIM_HandleTypeDef *htim, const uint32_t channel, const uint32_t tim_clock){
    user_pwm->htim = htim;
    user_pwm->channel = channel;
    user_pwm->clock = tim_clock / (htim->Init.Prescaler + 1);
    user_pwm->freq = user_pwm->clock / (__HAL_TIM_GET_AUTORELOAD(user_pwm->htim) + 1);
    user_pwm->duty = (float) __HAL_TIM_GET_COMPARE(user_pwm->htim, user_pwm->channel) / (float) (user_pwm->htim->Init.Period + 1);
    
    HAL_TIM_PWM_Start(htim, channel);
}

/**
* @brief 设置 PWM 占空比
* @param user_pwm  PWM 驱动结构体指针
* @param duty PWM 占空比 (0.0 ~ 1.0)
* @return 比较寄存器的值
*/
uint32_t PWM_Set_Duty(PWM_DRIVES *user_pwm, const float duty){
    user_pwm->duty = duty;
    const uint32_t compare_reg = (uint32_t)((float)(user_pwm->htim->Init.Period + 1) * duty);
    __HAL_TIM_SET_COMPARE(user_pwm->htim, user_pwm->channel, compare_reg);
    return compare_reg;
}

/**
* @brief 设置 PWM 频率
* @param user_pwm  PWM 驱动结构体指针
* @param freq PWM 输出频率 单位: Hz
* @return 自动重载寄存器的值
*/
uint32_t PWM_Set_Frequency(PWM_DRIVES *user_pwm, const uint32_t freq){
    user_pwm->freq = freq;
    const uint32_t reload_reg = user_pwm->clock / freq - 1;
    __HAL_TIM_SET_AUTORELOAD(user_pwm->htim, reload_reg);
    return reload_reg;
}

