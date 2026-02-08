#ifndef __USER_UART_H__
#define __USER_UART_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"
#include "../../User_Architect/Inc/user_queue.h"
#include "../../User_Architect/Inc/user_ring_buffe.h"

/* 宏定义 --------------------------------------------------------------------*/
#define UART_BUFFER_SIZE  (128)

/* 类型定义 ------------------------------------------------------------------*/

/**
* @brief UART 接收回调函数类型
* @param user_uart UART 驱动结构体指针
*/
typedef void (*UART_Callback)(void* user_uart);

/**
* @brief UART 状态
*/
typedef enum {
    UART_IDLE,     /* 空闲状态 */
    UART_SENDING,  /* 发送中 */
} UART_Status;

/**
* @brief UART 驱动结构体
*/
typedef struct {
    UART_HandleTypeDef* huart;                /* UART 硬件句柄 */
    UART_Status status;                       /* UART 状态 */
    uint8_t is_buffer_a;                      /* 当前使用缓冲区是否是 A 标志 */
    uint8_t rx_buffer_a[UART_BUFFER_SIZE];    /* 接收缓冲区 A */
    uint8_t rx_buffer_b[UART_BUFFER_SIZE];    /* 接收缓冲区 B */
    RING_BUFFER rx_ringBuffer;                /* 接收环形缓冲区 */
    Queue tx_queue;                           /* 发送队列 */
    UART_Callback callback;                   /* 接收回调函数 */
} UART_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void UART_Init(UART_DRIVES* user_uart, UART_HandleTypeDef* huart, UART_Callback callback);
void UART_Send(UART_DRIVES* user_uart, const char* str);

#endif //__USER_UART_H__
