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
UART_DRIVES ros_uart = {0};
int32_t direction_angle = 0; // -180 ~ 180 度
int32_t current_angle = 0;   // -180 ~ 180 度
int32_t axial_speed = 0;     // -1000 ~ 1000 无纲量
int32_t forward_speed = 0;   // -1000 ~ 1000 无纲量
void ros_uart_callback (void *user_uart) {
    UART_DRIVES *uart = (UART_DRIVES*)user_uart;
    uint8_t receive_data[64];

    if (UART_GetDataWithH(uart, receive_data, "\n")) {
        sscanf((char *)&receive_data[1], "\n%d,%d,%d",
        (int *)&direction_angle,
        (int *)current_angle,
        (int *)&forward_speed);

        PID_Set_Target(&direction_pid_controller, (float)direction_angle);
        axial_speed = (int32_t)PID_Calculate(&direction_pid_controller, (float)current_angle, 0);

        const int32_t left_val  = forward_speed - axial_speed;
        const int32_t right_val = forward_speed + axial_speed;

        if (left_val < 0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        if (left_val > 0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
        if (right_val < 0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
        if (right_val > 0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

        Trap_Plan(&left_duty_planner,  (float)abs(left_val)  / 2000.0f);
        Trap_Plan(&right_duty_planner, (float)abs(right_val) / 2000.0f);
    }
}

// 状态灯
LED_DRIVES user_red_led   = {0};
LED_DRIVES user_green_led = {0};

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

// PWM 电机
PWM_DRIVES left_motor;
PWM_DRIVES right_motor;

// 电机占空比梯形平滑
TRAP_Profile left_duty_planner  = {0};
TRAP_Profile right_duty_planner = {0};
SysTick_Task servo_ramp_task    = {0};
void Servo_Ramp_Callback(void *arg) {
    (void)arg;
    const float left_duty  = Trap_Update(&left_duty_planner,  SERVO_RAMP_DT);
    const float right_duty = Trap_Update(&right_duty_planner, SERVO_RAMP_DT);
    PWM_Set_Duty(&left_motor,  left_duty);
    PWM_Set_Duty(&right_motor, right_duty);
}

// PID 方向角度闭环
PID_Controller direction_pid_controller = {0};