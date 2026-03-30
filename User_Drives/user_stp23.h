#ifndef USER_STP23_H
#define USER_STP23_H
#include "main.h"
#ifdef HAL_UART_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "user_uart.h"
#include "../../Core/Inc/bsp_config.h"

/* 宏定义 --------------------------------------------------------------------*/
#define STP23_DATA_FRAME_LEN  (47)      /* STP23 数据帧长度 */
#define STP23_SAMPLE_NUM      (12)      /* STP23 数据帧长度 */

/* 类型定义 ------------------------------------------------------------------*/

/**
* @brief ADC 转换完成回调函数类型
* @param user_stp23 STP23 单点激光测距模块驱动结构体指针
*/
typedef void (*STP23_Callback)(void* user_stp23);

/**
* @brief STP23 单点激光测距模块驱动结构体
*/
typedef struct {
    UART_DRIVES *uart;                                /* UART 驱动结构体指针 */
    uint8_t is_update;                                /* 数据更新标志 */
    uint16_t temperature;                             /* 模块温度 原始数据 无纲量 */
    float distance;                                   /* 测量距离 单位：mm */
    uint16_t raw_distance[STP23_SAMPLE_NUM];          /* 测量距离 原始数据 无纲量 */
    uint8_t rx_buffer[STP23_DATA_FRAME_LEN];          /* 接收数据缓冲区 */
    STP23_Callback callbacks[STP23_CALLBACK_NUM];     /* 转换完成回调函数数组 */
    uint8_t callback_num;                             /* 已注册的回调函数数量 */
} STP23_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void STP23_Init(STP23_DRIVES *user_stp23, UART_DRIVES* user_uart);
void STP23_RegisterCallback(STP23_DRIVES* user_stp23, STP23_Callback callback);

#endif /* HAL_UART_MODULE_ENABLED */
#endif // USER_STP23_H
