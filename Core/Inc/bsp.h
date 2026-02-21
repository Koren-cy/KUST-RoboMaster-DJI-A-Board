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

// ADC
#include "../../User_Drives/user_adc.h"
extern ADC_DRIVES user_adc_1;

// FIR 滤波器
#include "../../User_Algorithm/user_fir.h"
extern FIR_Filter user_fir_1;
extern float fir_coeffs[31];

#endif //__USER_BSP_H__
