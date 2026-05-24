#ifndef USER_TIMER_H
#define USER_TIMER_H
#include "main.h"
#ifdef HAL_TIM_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "../../Core/Inc/bsp_config.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief  定时器周期回调函数类型
* @param  user_timer 定时器驱动结构体指针
*/
typedef void (*TIMER_Callback)(void* user_timer);

/**
* @brief  定时器驱动结构体
*/
typedef struct {
    TIM_HandleTypeDef* htim;                        /* 定时器硬件句柄 */
    uint32_t clock;                                 /* APB 定时器时钟线的时钟频率 */
    uint32_t frequency;                             /* 定时器频率 */
    TIMER_Callback callbacks[TIMER_CALLBACK_NUM];   /* 周期回调函数数组 */
    uint8_t callback_num;                           /* 已注册的回调函数数量 */
} TIMER_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void TIMER_Init(TIMER_DRIVES* user_timer, TIM_HandleTypeDef* htim, uint32_t tim_clock);
void TIMER_Set_Frequency(TIMER_DRIVES* user_timer, uint32_t frequency);

void TIMER_Start(const TIMER_DRIVES* user_timer);
void TIMER_Stop(const TIMER_DRIVES* user_timer);

void TIMER_RegisterCallback(TIMER_DRIVES* user_timer, TIMER_Callback callback);


#endif /* HAL_TIM_MODULE_ENABLED */
#endif // USER_TIMER_H
