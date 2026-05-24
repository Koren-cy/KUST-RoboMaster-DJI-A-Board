/* Mock: Core/Inc/bsp.h - complete minimal stub, no hardware deps */
#ifndef USER_BSP_H
#define USER_BSP_H

#include "main.h"
#include <stdint.h>
#include <string.h>
#include <math.h>

/* No SEGGER RTT on host - define stubs inline */
#define BUFFER_SIZE_UP 1024
#define JScope_Transmit_t void
extern uint8_t JScope_RTT_UpBuffer[BUFFER_SIZE_UP];

/* ---- Loop event registry ---- */
#define MAX_LOOP_EVENT 32
typedef void (*LOOP_Event)(void);
extern LOOP_Event loop_event[MAX_LOOP_EVENT];
extern uint8_t loop_event_num;
void LOOP_EVENT_Handle(void);

/* ---- Minimal driver type definitions ---- */
typedef struct { int unused; } UART_DRIVES;
typedef struct { int unused; } LED_DRIVES;
typedef struct { int unused; } CAN_DRIVES;
typedef struct { int unused; } BUZZER_DRIVES;
typedef struct { int unused; } STARTUP_MUSIC_DRIVES;
typedef struct { int unused; } DJI_MOTOR_DRIVES;

/* ---- Systick task ---- */
typedef struct {
    void (*callback)(void *);
    void *arg;
    uint32_t period_ms;
    uint32_t last_tick;
} SysTick_Task;

/* ---- Extern global variables (no-op stubs) ---- */
extern UART_DRIVES user_debug_uart;
extern UART_DRIVES user_uart_3;
extern LED_DRIVES user_red_led;
extern LED_DRIVES user_green_led;
extern CAN_DRIVES user_can_1;
extern CAN_DRIVES user_can_2;
extern BUZZER_DRIVES user_buzzer_1;
extern STARTUP_MUSIC_DRIVES user_startup_music;
extern SysTick_Task user_startup_music_task;
extern SysTick_Task LED_Blink_Task;
extern DJI_MOTOR_DRIVES user_top_motor;
extern DJI_MOTOR_DRIVES user_bottom_motor;
extern DJI_MOTOR_DRIVES user_left_motor;
extern DJI_MOTOR_DRIVES user_right_motor;
void user_can_2_callback(void *user_can);
void LED_Blink_Callback(void *arg);

#define CCMRAM

#endif /* USER_BSP_H */
