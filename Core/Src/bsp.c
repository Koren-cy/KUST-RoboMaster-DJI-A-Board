/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp.h"
#include <string.h>
#include <stdio.h>
#include "../../User_Algorithm/user_coord.h"


/* 主循环注册表 --------------------------------------------------------------*/
void (*loop_event[MAX_LOOP_EVENT])(void) = {0};
uint8_t loop_event_num = 0;

void LOOP_EVENT_Handle(void) {
    for (uint8_t event_index = 0 ; event_index < loop_event_num ; event_index++) {
        loop_event[event_index]();
    }
}

/* JScope ------------------------------------------------------------------*/
JScope_Transmit_t jscope_transmit = {0};
uint8_t JScope_RTT_UpBuffer[BUFFER_SIZE_UP] = {0};

/* 接口定义 --------------------------------------------------------------------*/

// 调试串口
UART_DRIVES user_debug_uart = {0};
UART_DRIVES user_uart_3 = {0};

// 状态灯
LED_DRIVES user_red_led = {0};
LED_DRIVES user_green_led = {0};

// can 总线
CAN_DRIVES user_can_1 = {0};
CAN_DRIVES user_can_2 = {0};

void user_can_2_callback(void * user_can) {
    const CAN_DRIVES *can = (CAN_DRIVES*)user_can;
    uint8_t receive_data[8];

    memcpy(receive_data, can->rx_msg.Data, 8);

}

// 蜂鸣器
BUZZER_DRIVES user_buzzer_1 = {0};

// 启动音乐
STARTUP_MUSIC_DRIVES user_startup_music = {0};
SysTick_Task user_startup_music_task = {0};


// LED 闪烁
SysTick_Task LED_Blink_Task = {0};
void LED_Blink_Callback(void *arg) {
    const LED_DRIVES* led = (LED_DRIVES*)arg;
    LED_Toggle(led);
}

// 大疆电机
DJI_MOTOR_DRIVES user_top_motor    = {0};
DJI_MOTOR_DRIVES user_bottom_motor = {0};
DJI_MOTOR_DRIVES user_left_motor   = {0};
DJI_MOTOR_DRIVES user_right_motor  = {0};
