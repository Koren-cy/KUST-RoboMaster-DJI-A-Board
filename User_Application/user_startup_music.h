#ifndef USER_STARTUP_MUSIC_H
#define USER_STARTUP_MUSIC_H
#include "main.h"
#ifdef HAL_TIM_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "../User_Drives/user_buzzer.h"
#include "../User_Architect/user_systick.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 启动音乐播放状态
*/
typedef enum {
    MUSIC_IDLE,      /* 空闲状态 */
    MUSIC_PLAYING,   /* 正在播放 */
    MUSIC_FINISHED,  /* 播放完成 */
} MUSIC_State;

/**
* @brief 启动音乐驱动结构体
*/
typedef struct {
    BUZZER_DRIVES *buzzer;         /* 蜂鸣器驱动指针 */
    const uint16_t (*music)[2];    /* 音乐数据指针 [频率, 时长] */
    SysTick_Task *task;            /* SysTick 任务指针 */
    uint16_t note_count;           /* 音符总数 */
    uint16_t current_note;         /* 当前音符索引 */
    uint16_t time_counter;         /* 时间计数器 */
    MUSIC_State state;             /* 播放状态 */
} STARTUP_MUSIC_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void StartupMusic_Init(STARTUP_MUSIC_DRIVES *music_player, BUZZER_DRIVES *buzzer,SysTick_Task *task,
                       const uint16_t (*music_data)[2], uint16_t note_count);

void StartupMusic_Start(STARTUP_MUSIC_DRIVES *music_player);

#endif /* HAL_TIM_MODULE_ENABLED */
#endif // USER_STARTUP_MUSIC_H
