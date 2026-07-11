#ifndef USER_BSP_H
#define USER_BSP_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../SEGGER_RTT/SEGGER_RTT.h"

/* 全局配置 ----------------------------------------------------------------*/
// 电机占空比梯形平滑
#define SERVO_RAMP_TICK_MS   20                              /* 刷新周期 ms */
#define SERVO_RAMP_DT        (SERVO_RAMP_TICK_MS / 1000.0f)  /* 固定步长 s */
#define SERVO_RAMP_VMAX      0.2f                            /* 占空比最大变化速度  (/s) */
#define SERVO_RAMP_AMAX      2.0f                            /* 占空比最大变化加速度 (/s^2) */

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
#include "../../User_Drives/User_Peripheral/user_uart.h"
extern int32_t direction_angle;
extern int32_t current_angle;
extern int32_t axial_speed;
extern int32_t forward_speed;
extern UART_DRIVES ros_uart;
void ros_uart_callback (void *user_uart);

// 状态灯
#include "../../User_Drives/User_Basic/user_led.h"
extern LED_DRIVES user_red_led;
extern LED_DRIVES user_green_led;

// 蜂鸣器
#include "../../User_Drives/User_Basic/user_buzzer.h"
extern BUZZER_DRIVES user_buzzer_1;

// 启动音乐
#include "../../User_Application/user_startup_music.h"
extern STARTUP_MUSIC_DRIVES user_startup_music;
extern SysTick_Task user_startup_music_task;

// LED 闪烁
extern SysTick_Task LED_Blink_Task;
void LED_Blink_Callback(void *arg);

// PWM 电机
#include "../../User_Drives/User_Peripheral/user_pwm.h"
extern PWM_DRIVES left_motor;
extern PWM_DRIVES right_motor;

// 电机占空比梯形平滑
#include "../../User_Algorithm/User_Motion/user_trapezoid.h"
extern TRAP_Profile left_duty_planner;
extern TRAP_Profile right_duty_planner;
extern SysTick_Task servo_ramp_task;
void Servo_Ramp_Callback(void *arg);

// PID 方向角度闭环
#include "../../User_Algorithm/User_Controller/user_pid.h"
extern PID_Controller direction_pid_controller;

#endif // USER_BSP_H
