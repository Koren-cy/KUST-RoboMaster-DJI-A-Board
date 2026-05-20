#ifndef USER_BSP_H
#define USER_BSP_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../SEGGER_RTT/SEGGER_RTT.h"

/* 全局注册表 ----------------------------------------------------------------*/
#define MAX_LOOP_EVENT 32
void LOOP_EVENT_Handle(void);
typedef void (*LOOP_Event)(void);
extern LOOP_Event loop_event[MAX_LOOP_EVENT];
extern uint8_t loop_event_num;

/* SysTick 系统滴答定时器回调函数表 ------------------------------------------------------------*/
#include "../../User_Architect/user_systick.h"

/* JScope ------------------------------------------------------------------*/
#include "../../SEGGER_RTT/user_JScope_Transmit.h"
extern CCMRAM JScope_Transmit_t jscope_transmit;
extern CCMRAM uint8_t JScope_RTT_UpBuffer[BUFFER_SIZE_UP];

/* 接口定义 ------------------------------------------------------------------*/

// 调试串口
#include "../../User_Drives/user_uart.h"
extern UART_DRIVES user_debug_uart;
extern UART_DRIVES user_uart_3;

// 状态灯
#include "../../User_Drives/user_led.h"
extern LED_DRIVES user_red_led;
extern LED_DRIVES user_green_led;

// can 总线
#include "../../User_Drives/user_can.h"
extern CAN_DRIVES user_can_1;
extern CAN_DRIVES user_can_2;
void user_can_2_callback(void * user_can);

// 蜂鸣器
#include "../../User_Drives/user_buzzer.h"
extern BUZZER_DRIVES user_buzzer_1;

// 启动音乐
#include "../../User_Application/user_startup_music.h"
extern STARTUP_MUSIC_DRIVES user_startup_music;
extern SysTick_Task user_startup_music_task;

// LED 闪烁
extern SysTick_Task LED_Blink_Task;
void LED_Blink_Callback(void *arg);

// 大疆电机
#include "../../User_Drives/User_Motor/user_dji_motor.h"
extern DJI_MOTOR_DRIVES user_top_motor;
extern DJI_MOTOR_DRIVES user_bottom_motor;
extern DJI_MOTOR_DRIVES user_left_motor;
extern DJI_MOTOR_DRIVES user_right_motor;

// 方程求解器
#include "../../User_Algorithm/user_newton.h"
extern NEWTON_SOLVER user_newton_solver;
float res_func_1(const float* x, const float* param);
float res_func_2(const float* x, const float* param);


#endif // USER_BSP_H
