#ifndef __USER_LED_H__
#define __USER_LED_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief LED 状态枚举
*/
typedef enum {
    LED_OFF = 0,  /* LED 熄灭状态 */
    LED_ON = 1,   /* LED 点亮状态 */
} LED_State;

/**
* @brief LED 驱动结构体
*/
typedef struct {
    GPIO_TypeDef* GPIO;  /* 存储 GPIO 端口 */
    uint16_t pin;        /* 存储 GPIO 引脚 */
    uint8_t is_reversal; /* 存储是否反转逻辑，1 为反转，0 为正常 */
} LED_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void LED_Init(LED_DRIVES* user_led, GPIO_TypeDef* GPIO, uint16_t pin, uint8_t is_reversal);
void LED_Set_State(const LED_DRIVES* user_led, LED_State state);
void LED_On(const LED_DRIVES* user_led);
void LED_Off(const LED_DRIVES* user_led);
void LED_Toggle(const LED_DRIVES* user_led);
LED_State LED_Get_State(const LED_DRIVES* user_led);

#endif //__USER_LED_H__