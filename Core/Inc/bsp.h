#ifndef __USER_BSP_H__
#define __USER_BSP_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../SEGGER_RTT/SEGGER_RTT.h"

/* 全局注册表 ----------------------------------------------------------------*/
#define MAX_LOOP_EVENT 32
void LOOP_EVENT_Handle(void);
typedef void (*LOOP_Event)(void);
extern LOOP_Event loop_event[MAX_LOOP_EVENT];
extern uint8_t loop_event_num;

/* JScope ------------------------------------------------------------------*/
#include "../../SEGGER_RTT/user_JScope_Transmit.h"
extern CCMRAM JScope_Transmit_t jscope_transmit;
extern CCMRAM uint8_t JScope_RTT_UpBuffer[BUFFER_SIZE_UP];

/* 接口定义 ------------------------------------------------------------------*/

// 调试串口
#include "../../User_Drives/user_uart.h"
extern UART_DRIVES user_debug_uart;
void user_debug_uart_callback(void * user_uart);

// 状态灯
#include "../../User_Drives/user_led.h"
extern LED_DRIVES user_red_led;
extern LED_DRIVES user_green_led;

// can 总线
#include "../../User_Drives/user_can.h"
extern CAN_DRIVES user_can_1;
void user_can_1_callback(void * user_can);
extern CAN_DRIVES user_can_2;
void user_can_2_callback(void * user_can);

// 蜂鸣器
#include "../../User_Drives/user_pwm.h"
extern PWM_DRIVES user_buzzer;

// 大疆电机
#include "../../User_Drives/User_Motor/user_dji_motor.h"
extern DJI_MOTOR_DRIVES test_GM6020;

// ADRC 控制器
#include "../../User_Algorithm/User_Controller/user_ladrc.h"
extern LADRC_Controller user_ladrc_1;
extern LADRC_Controller user_ladrc_2;
#include "../../User_Algorithm/User_Controller/user_adrc.h"
extern ADRC_Controller user_adrc_1;

// 串级 PID 控制器
#include "../../User_Algorithm/User_Controller/user_pid.h"
extern PID_Controller user_pid_1;
extern PID_Controller user_pid_2;
#include "../../User_Algorithm/User_Controller/user_inc_pid.h"
extern Incremental_PID_Controller user_inc_pid_1;
extern Incremental_PID_Controller user_inc_pid_2;
#include "../../User_Algorithm/User_Controller/user_cascade_controller.h"
extern Cascade_Controller user_cascade_controller_1;


#endif //__USER_BSP_H__
