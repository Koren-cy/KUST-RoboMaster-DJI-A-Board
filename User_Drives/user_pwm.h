#ifndef __USER_PWM_H__
#define __USER_PWM_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief  PWM 驱动结构体
*/
typedef struct {
    TIM_HandleTypeDef *htim;  /* 定时器硬件句柄 */
    uint32_t channel;         /* PWM 通道 */
    uint32_t clock;           /* 定时器计数（预分频后）频率 */
    uint32_t freq;            /* PWM 输出频率 */
    float duty;               /* PWM 占空比 (0.0 ~ 1.0) */
} PWM_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void PWM_Init(PWM_DRIVES *user_pwm, TIM_HandleTypeDef *htim, uint32_t channel, uint32_t tim_clock);
uint32_t PWM_Set_Duty(PWM_DRIVES *user_pwm, float duty);
uint32_t PWM_Set_Frequency(PWM_DRIVES *user_pwm, uint32_t freq);


#endif  // __USER_PWM_H__