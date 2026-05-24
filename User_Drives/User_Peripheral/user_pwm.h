#ifndef USER_PWM_H
#define USER_PWM_H
#include "main.h"
#ifdef HAL_TIM_MODULE_ENABLED

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief PWM 位数
*/
typedef enum {
    PWM_16BIT,
    PWM_32BIT
} PWM_BIT_WIDTH;


/**
* @brief  PWM 驱动结构体
*/
typedef struct {
    TIM_HandleTypeDef *htim;  /* 定时器硬件句柄 */
    PWM_BIT_WIDTH bit_width;  /* PWM 位数 */
    uint32_t channel;         /* PWM 通道 */
    uint32_t clock;           /* APB 定时器时钟线的时钟频率 */
    uint32_t frequency;       /* PWM 输出频率 */
    float duty;               /* PWM 占空比 (0.0 ~ 1.0) */
} PWM_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void PWM_Init(PWM_DRIVES *user_pwm, TIM_HandleTypeDef *htim, uint32_t channel, PWM_BIT_WIDTH bit_width, uint32_t clock);
void PWM_Start(const PWM_DRIVES* user_pwm);
void PWM_Stop(const PWM_DRIVES* user_pwm);

uint32_t PWM_Set_Duty(PWM_DRIVES *user_pwm, float duty);
uint32_t PWM_Set_Frequency(PWM_DRIVES *user_pwm, uint32_t frequency);

#endif /* HAL_TIM_MODULE_ENABLED */
#endif // USER_PWM_H