/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_delay.h"
#include "../../Core/Inc/bsp.h"

/* 私有变量 ------------------------------------------------------------------*/
static TIM_HandleTypeDef user_tick_timer;

/* 扩展变量 ------------------------------------------------------------------*/
uint32_t TimeFlag = 0;

/* 私有函数 ------------------------------------------------------------------*/

/**
* @brief 获取稳定时间标志
* @param start_flag 起始标志指针
* @param start_time 起始时间指针
*/
static inline void get_stable_time(uint32_t *start_flag, uint32_t *start_time) {
    *start_flag = TimeFlag;
    *start_time = __HAL_TIM_GET_COUNTER(&user_tick_timer);
    while (*start_flag != TimeFlag) {
        *start_flag = TimeFlag;
        *start_time = __HAL_TIM_GET_COUNTER(&user_tick_timer);
    }
}

/**
* @brief 计算经过的时间（单位：0.1us）
* @param start_flag   起始标志
* @param start_time   起始时间
* @param current_flag 当前标志
* @param current_time 当前时间
* @return 经过的时间（单位：0.1us）
*/
static inline uint32_t calculate_elapsed_time(const uint32_t start_flag, const uint32_t start_time, const uint32_t current_flag, const uint32_t current_time) {
    return (current_flag - start_flag) * 10000 + (current_time - start_time) * 5;
}

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化延时模块
* @param htim 定时器句柄指针
* @note  定时器配置要求：
*        - 计数频率：2MHz（每 0.5us 计数一次）
*        - 溢出周期：1ms（ARR = 2000 - 1）
*        - 需要在定时器溢出中断中调用 Delay_UpdateTimeFlag()
*/
void Delay_Init(const TIM_HandleTypeDef *htim) {
    user_tick_timer = *htim;
}

/**
* @brief 更新时间标志
* @note  此函数应在定时器中断回调函数中调用
*/
void Delay_Update_TimeFlag(void) {
    TimeFlag++;
}

/**
* @brief 微秒级延时
* @param us 延时时间（单位：微秒）
* @note  延时期间会调用循环事件处理函数
*/
void Delay_us(const uint32_t us) {
    uint32_t start_flag, start_time;
    get_stable_time(&start_flag, &start_time);

    uint32_t current_flag, current_time;

    do {
        get_stable_time(&current_flag, &current_time);
        LOOP_EVENT_Handle();
    } while (calculate_elapsed_time(start_flag, start_time, current_flag, current_time) * 100 < us);
}

/**
* @brief 毫秒级延时
* @param ms 延时时间（单位：毫秒）
* @note  延时期间会多次调用循环事件处理函数
*/
void Delay_ms(const uint32_t ms) {
    uint32_t start_flag, start_time;
    get_stable_time(&start_flag, &start_time);

    uint32_t current_flag, current_time;

    do {
        get_stable_time(&current_flag, &current_time);

        for (uint16_t i = 0; i < 10; i++)
            LOOP_EVENT_Handle();

    } while (calculate_elapsed_time(start_flag, start_time, current_flag, current_time) < ms * 10);
}

/**
* @brief 秒级延时
* @param s 延时时间（单位：秒）
* @note  延时期间会大量调用循环事件处理函数
*/
void Delay_s(const uint32_t s) {
    uint32_t start_flag, start_time;
    get_stable_time(&start_flag, &start_time);

    uint32_t current_flag, current_time;

    do {
        get_stable_time(&current_flag, &current_time);

        for (uint16_t i = 0; i < 10000; i++)
            LOOP_EVENT_Handle();

    } while (calculate_elapsed_time(start_flag, start_time, current_flag, current_time) < s * 10000);
}
