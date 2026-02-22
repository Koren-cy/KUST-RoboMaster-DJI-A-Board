/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp.h"
#include "bsp_config.h"
#include <stdio.h>

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
void user_debug_uart_callback(void * user_uart) {
    UART_DRIVES *uart = (UART_DRIVES*)user_uart;
}

// 状态灯
LED_DRIVES user_red_led = {0};
LED_DRIVES user_green_led = {0};

// can 总线
CAN_DRIVES user_can_1 = {0};
void user_can_1_callback(void * user_can) {
    const CAN_DRIVES *can = (CAN_DRIVES*)user_can;
    DJI_Motor_Handle(can);
}

CAN_DRIVES user_can_2 = {0};
void user_can_2_callback(void * user_can) {
    const CAN_DRIVES *can = (CAN_DRIVES*)user_can;
}

// 蜂鸣器
PWM_DRIVES user_buzzer = {0};

// ADC
ADC_DRIVES user_adc_1 = {0};

// FIR 滤波器
FIR_Filter user_fir_1 = {0};
FIR_Filter user_fir_2 = {0};
FIR_Filter user_fir_3 = {0};
FIR_Filter user_fir_4 = {0};
FIR_Filter user_fir_5 = {0};
float fir_coeffs[6] = {
    0.04502611607f,   0.1661649346f,   0.2888089418f,   0.2888089418f,   0.1661649346f,
    0.04502611607f
};

// 大疆电机
DJI_MOTOR_DRIVES test_GM6020 = {0};