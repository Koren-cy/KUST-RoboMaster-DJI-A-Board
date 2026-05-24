#include "../../Core/Inc/bsp.h"
#ifdef HAL_TIM_MODULE_ENABLED
/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_pwm.h"
#include <math.h>

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 PWM 驱动
* @param user_pwm   PWM 驱动结构体指针
* @param htim       定时器硬件句柄
* @param channel    PWM 通道
* @param bit_width  PWM 位数
* @param clock  APB 定时器时钟线的时钟频率
*/
void PWM_Init(PWM_DRIVES *user_pwm, TIM_HandleTypeDef *htim, const uint32_t channel, const PWM_BIT_WIDTH bit_width, const uint32_t clock){
    user_pwm->htim = htim;
    user_pwm->channel = channel;
    user_pwm->bit_width = bit_width;
    user_pwm->clock = clock;
    user_pwm->frequency = clock / ((htim->Init.Prescaler + 1) * (__HAL_TIM_GET_AUTORELOAD(htim) + 1));
    user_pwm->duty = (float) __HAL_TIM_GET_COMPARE(htim, channel) / (float) (htim->Init.Period + 1);
}

/**
* @brief 启动 PWM 输出
* @param user_pwm   PWM 驱动结构体指针
*/
void PWM_Start(const PWM_DRIVES* user_pwm){
    HAL_TIM_PWM_Start(user_pwm->htim, user_pwm->channel);
}

/**
* @brief 停止 PWM 输出
* @param user_pwm   PWM 驱动结构体指针
*/
void PWM_Stop(const PWM_DRIVES* user_pwm){
    HAL_TIM_PWM_Stop(user_pwm->htim, user_pwm->channel);
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
* @param frequency PWM 输出频率 单位: Hz
* @return 自动重载寄存器的值
*/
uint32_t PWM_Set_Frequency(PWM_DRIVES *user_pwm, const uint32_t frequency){
    user_pwm->frequency = frequency;

    uint32_t prescaler_reg = 0;
    uint32_t reload_reg = 0;

    const uint32_t max_value = (user_pwm->bit_width == PWM_16BIT) ? 65535 : 4294967295;

    for (prescaler_reg = 0; prescaler_reg <= max_value; prescaler_reg++) {
        reload_reg = (uint32_t)ceilf((float)user_pwm->clock / (float)(frequency * (prescaler_reg + 1))) - 1;
        if (reload_reg <= max_value) {
            break;
        }
    }

    if (reload_reg > max_value) {
        return 0;
    }

    const uint32_t compare_reg = (uint32_t)((float)(reload_reg + 1) * user_pwm->duty);

    const uint8_t is_running = HAL_TIM_PWM_GetState(user_pwm->htim) == HAL_TIM_STATE_BUSY;

    if (is_running)
        HAL_TIM_PWM_Stop(user_pwm->htim, user_pwm->channel);

    __HAL_TIM_SET_PRESCALER(user_pwm->htim, prescaler_reg);
    __HAL_TIM_SET_AUTORELOAD(user_pwm->htim, reload_reg);
    __HAL_TIM_SET_COMPARE(user_pwm->htim, user_pwm->channel, compare_reg);
    __HAL_TIM_SET_COUNTER(user_pwm->htim, 0);

    if (is_running)
        HAL_TIM_PWM_Start(user_pwm->htim, user_pwm->channel);

    return reload_reg;
}

#endif /* HAL_TIM_MODULE_ENABLED */

