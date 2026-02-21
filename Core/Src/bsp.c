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
    CAN_DRIVES *can = (CAN_DRIVES*)user_can;
}

CAN_DRIVES user_can_2 = {0};
void user_can_2_callback(void * user_can) {
    CAN_DRIVES *can = (CAN_DRIVES*)user_can;
}

// 蜂鸣器
PWM_DRIVES user_buzzer = {0};

// ADC
ADC_DRIVES user_adc_1 = {0};

// FIR 滤波器
FIR_Filter user_fir_1 = {0};
float fir_coeffs[31] = {
    -0.004558113404f,  -0.00512371771f,  -0.004717321601f,  -0.002941794228f,  0.0005305341328f,
     0.005903918296f,   0.01321454626f,     0.0223021321f,    0.03280295804f,    0.04416700453f,
      0.05569869652f,   0.06661761552f,     0.0761327371f,    0.08352165669f,    0.08820542693f,
      0.08980982006f,   0.08820542693f,    0.08352165669f,     0.0761327371f,    0.06661761552f,
      0.05569869652f,   0.04416700453f,    0.03280295804f,     0.0223021321f,    0.01321454626f,
     0.005903918296f, 0.0005305341328f,  -0.002941794228f,  -0.004717321601f,   -0.00512371771f,
    -0.004558113404f
};