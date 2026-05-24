/* Mock: User_Application/user_startup_music.h - minimal stub */
#ifndef USER_STARTUP_MUSIC_H
#define USER_STARTUP_MUSIC_H

#include <stdint.h>
#include "user_buzzer.h"
#include "user_systick.h"

typedef enum {
    MUSIC_IDLE,
    MUSIC_PLAYING,
    MUSIC_FINISHED
} MUSIC_State;

typedef struct {
    BUZZER_DRIVES *buzzer;
    const uint16_t (*music)[2];
    uint16_t length;
    uint16_t current_note;
    MUSIC_State state;
    uint32_t note_start_tick;
    SysTick_Task task;
} STARTUP_MUSIC_DRIVES;

void Startup_Music_Init(STARTUP_MUSIC_DRIVES *m, BUZZER_DRIVES *bz);
void Startup_Music_Play(STARTUP_MUSIC_DRIVES *m);

#endif
