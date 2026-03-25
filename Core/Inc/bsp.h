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

// 状态灯
#include "../../User_Drives/user_led.h"
extern LED_DRIVES user_red_led;
extern LED_DRIVES user_green_led;

// can 总线
#include "../../User_Drives/user_can.h"
extern CAN_DRIVES user_can_1;
extern CAN_DRIVES user_can_2;

void user_can_2_callback(void * user_can);
#define CAN_CONNET_ID 0x200
typedef struct {
    int16_t ω_theta_chassis;
    int16_t d_theta_turret;
    int16_t v_y;
    int16_t v_x;
} CAN_CONNECTION;
extern CAN_CONNECTION can_connection;

// 蜂鸣器
#include "../../User_Drives/user_buzzer.h"
extern BUZZER_DRIVES user_buzzer_1;

// 启动音乐
#include "../../User_Application/user_startup_music.h"
extern STARTUP_MUSIC_DRIVES user_startup_music;
extern SysTick_Task user_startup_music_task;


// PID 控制器
#include "../../User_Algorithm/User_Controller/user_pid.h"
extern PID_Controller FR_GM6020_PID;
extern PID_Controller FL_GM6020_PID;
extern PID_Controller RR_GM6020_PID;
extern PID_Controller RL_GM6020_PID;
extern PID_Controller FR_M3508_PID;
extern PID_Controller FL_M3508_PID;
extern PID_Controller RR_M3508_PID;
extern PID_Controller RL_M3508_PID;

// LADRC 控制器
#include "../../User_Algorithm/User_Controller/user_ladrc.h"
extern LADRC_Controller YAW_GM6020_LADRC;

// 大疆电机
#include "../../User_Drives/User_Motor/user_dji_motor.h"
extern DJI_MOTOR_DRIVES FR_GM6020;
extern DJI_MOTOR_DRIVES FL_GM6020;
extern DJI_MOTOR_DRIVES RR_GM6020;
extern DJI_MOTOR_DRIVES RL_GM6020;

extern DJI_MOTOR_DRIVES FR_M3508;
extern DJI_MOTOR_DRIVES FL_M3508;
extern DJI_MOTOR_DRIVES RR_M3508;
extern DJI_MOTOR_DRIVES RL_M3508;

extern DJI_MOTOR_DRIVES YAW_GM6020;

// 舵轮底盘
#include "../../User_Application/swerve_chassis.h"
extern SwerveChassisState user_swerve_chassis;

// 云台指向
extern float orientation_angle;

#endif // USER_BSP_H
