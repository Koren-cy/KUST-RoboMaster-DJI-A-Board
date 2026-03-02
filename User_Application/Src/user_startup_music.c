#include "../../Core/Inc/bsp.h"
#ifdef HAL_TIM_MODULE_ENABLED
/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_startup_music.h"
#include "../../User_Architect/user_systick.h"

/* 私有变量 ------------------------------------------------------------------*/
static STARTUP_MUSIC_DRIVES *music_player_instance = NULL;

/* 私有函数声明 --------------------------------------------------------------*/
static void StartupMusic_Update(STARTUP_MUSIC_DRIVES *music_player);
static void StartupMusic_SysTick_Callback(void);

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief SysTick 回调函数
* @note  自动注册到 SysTick 回调表
*/
static void StartupMusic_SysTick_Callback(void) {
    if (music_player_instance != NULL) {
        StartupMusic_Update(music_player_instance);
    }
}

/**
* @brief 初始化启动音乐播放器
* @param music_player 启动音乐驱动结构体指针
* @param buzzer       蜂鸣器驱动指针
* @param music_data   音乐数据指针 格式: [频率(Hz), 时长(ms)]
* @param note_count   音符总数
*/
void StartupMusic_Init(STARTUP_MUSIC_DRIVES *music_player, BUZZER_DRIVES *buzzer, 
                       const uint16_t (*music_data)[2], const uint16_t note_count) {
    memset(music_player, 0, sizeof(STARTUP_MUSIC_DRIVES));
    
    music_player->buzzer = buzzer;
    music_player->music = music_data;
    music_player->note_count = note_count;
    music_player->current_note = 0;
    music_player->time_counter = 0;
    music_player->state = MUSIC_IDLE;
    
    music_player_instance = music_player;
    SysTick_RegisterCallback(StartupMusic_SysTick_Callback);
}

/**
* @brief 开始播放启动音乐
* @param music_player 启动音乐驱动结构体指针
*/
void StartupMusic_Start(STARTUP_MUSIC_DRIVES *music_player) {
    music_player->current_note = 0;
    music_player->time_counter = 0;
    music_player->state = MUSIC_PLAYING;
}

/**
* @brief 更新启动音乐播放状态
* @param music_player 启动音乐驱动结构体指针
*/
static void StartupMusic_Update(STARTUP_MUSIC_DRIVES *music_player) {
    if (music_player->state != MUSIC_PLAYING) {
        return;
    }
    
    /* 检查是否播放完成 */
    if (music_player->current_note >= music_player->note_count && music_player->time_counter == 0) {
        Buzzer_Off(music_player->buzzer);
        music_player->state = MUSIC_FINISHED;
        return;
    }
    
    /* 时间计数器递减 */
    if (music_player->time_counter > 0) {
        music_player->time_counter--;
        return;
    }
    
    /* 播放下一个音符 */
    if (music_player->current_note < music_player->note_count) {
        const uint16_t frequency = music_player->music[music_player->current_note][0];
        const uint16_t duration = music_player->music[music_player->current_note][1];
        
        if (frequency == 0) {
            Buzzer_Off(music_player->buzzer);
        } else {
            Buzzer_On(music_player->buzzer, frequency);
        }
        
        music_player->time_counter = duration;
        music_player->current_note++;
    }
}

#endif /* HAL_TIM_MODULE_ENABLED */
