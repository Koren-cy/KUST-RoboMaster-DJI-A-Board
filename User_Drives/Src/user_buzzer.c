#include "../../Core/Inc/bsp.h"
#ifdef HAL_TIM_MODULE_ENABLED
/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_buzzer.h"

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化蜂鸣器
* @param user_buzzer 蜂鸣器驱动结构体指针
* @param htim        定时器硬件句柄
* @param channel     PWM 通道
* @param tim_clock   定时器时钟频率 (总线时钟频率)
*/
void Buzzer_Init(BUZZER_DRIVES *user_buzzer, TIM_HandleTypeDef *htim, const uint32_t channel, const uint32_t tim_clock) {
    memset(user_buzzer, 0, sizeof(BUZZER_DRIVES));
    
    PWM_Init(&user_buzzer->pwm, htim, channel, tim_clock);
    user_buzzer->state = BUZZER_OFF;
    user_buzzer->frequency = 0;
    
    /* 初始化时关闭蜂鸣器 */
    Buzzer_Off(user_buzzer);
}

/**
* @brief 开启蜂鸣器
* @param user_buzzer 蜂鸣器驱动结构体指针
* @param frequency   蜂鸣器频率 单位：Hz
*/
void Buzzer_On(BUZZER_DRIVES *user_buzzer, const uint32_t frequency) {
    user_buzzer->state = BUZZER_ON;
    user_buzzer->frequency = frequency;
    
    PWM_Set_Frequency(&user_buzzer->pwm, frequency);
    PWM_Set_Duty(&user_buzzer->pwm, 0.5f);
}

/**
* @brief 关闭蜂鸣器
* @param user_buzzer 蜂鸣器驱动结构体指针
*/
void Buzzer_Off(BUZZER_DRIVES *user_buzzer) {
    user_buzzer->state = BUZZER_OFF;
    user_buzzer->frequency = 0;
    
    PWM_Set_Duty(&user_buzzer->pwm, 0.0f);
}

#endif /* HAL_TIM_MODULE_ENABLED */
