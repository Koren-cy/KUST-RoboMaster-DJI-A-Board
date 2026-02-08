#ifndef __USER_DJI_BUS_H__
#define __USER_DJI_BUS_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 宏定义 --------------------------------------------------------------------*/
#define DBUS_MAX_LEN      (64)
#define DBUS_BUF_LEN      (18)

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 大疆遥控器数据结构体
*/
typedef struct {
    UART_HandleTypeDef* huart;   /* UART 硬件句柄 */
    uint8_t is_update;           /* 数据更新标志 */
    int16_t ch0;                 /* 通道 0 数据 右手水平 (-660 ~ 660) */
    int16_t ch1;                 /* 通道 1 数据 右手垂直 (-660 ~ 660) */
    int16_t ch2;                 /* 通道 2 数据 左手水平 (-660 ~ 660) */
    int16_t ch3;                 /* 通道 3 数据 左手垂直 (-660 ~ 660) */
    int16_t roll;                /* 拨轮数据 */
    uint8_t sw1;                 /* 左侧开关状态 */
    uint8_t sw2;                 /* 右侧开关状态 */
} DBUS_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void DBUS_Init(DBUS_DRIVES* user_dbus, UART_HandleTypeDef* huart);
void DBUS_Handler(const UART_HandleTypeDef* huart);

#endif //__USER_DJI_BUS_H__
