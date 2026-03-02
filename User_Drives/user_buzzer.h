#ifndef USER_BUZZER_H
#define USER_BUZZER_H
#include "main.h"
#ifdef HAL_TIM_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "user_pwm.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 蜂鸣器状态枚举
*/
typedef enum {
    BUZZER_OFF = 0,  /* 蜂鸣器关闭 */
    BUZZER_ON = 1,   /* 蜂鸣器开启 */
} BUZZER_State;

/**
* @brief 蜂鸣器驱动结构体
*/
typedef struct {
    PWM_DRIVES pwm;      /* PWM 驱动 */
    BUZZER_State state;  /* 蜂鸣器状态 */
    uint32_t frequency;  /* 当前频率 */
} BUZZER_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void Buzzer_Init(BUZZER_DRIVES *user_buzzer, TIM_HandleTypeDef *htim, uint32_t channel, uint32_t tim_clock);
void Buzzer_On(BUZZER_DRIVES *user_buzzer, uint32_t frequency);
void Buzzer_Off(BUZZER_DRIVES *user_buzzer);

#endif /* HAL_TIM_MODULE_ENABLED */
#endif // USER_BUZZER_H
