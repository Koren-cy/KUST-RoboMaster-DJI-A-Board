#ifndef USER_UART_H
#define USER_UART_H
#include "main.h"
#ifdef HAL_UART_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "../../Core/Inc/bsp_config.h"
#include "../../User_Architect/user_queue.h"
#include "../../User_Architect/user_ring_buffe.h"

/* 宏定义 --------------------------------------------------------------------*/
#define UART_BUFFER_SIZE      (256)

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
    UART_HandleTypeDef* huart;                    /* UART 硬件句柄 */
    UART_Status status;                           /* UART 状态 */
    uint8_t is_buffer_a;                          /* 当前使用缓冲区是否是 A 标志 */
    uint8_t rx_buffer_a[UART_BUFFER_SIZE];        /* 接收缓冲区 A */
    uint8_t rx_buffer_b[UART_BUFFER_SIZE];        /* 接收缓冲区 B */
    RING_BUFFER rx_ringBuffer;                    /* 接收环形缓冲区 */
    Queue tx_queue;                               /* 发送队列 */
    uint8_t receive_new_data;                     /* 接收缓冲区是否更新标志 */
    UART_Callback callbacks[UART_CALLBACK_NUM];   /* 接收回调函数数组 */
    uint8_t callback_num;                         /* 已注册的回调函数数量 */
} UART_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void UART_Init(UART_DRIVES* user_uart, UART_HandleTypeDef* huart);
void UART_RegisterCallback(UART_DRIVES* user_uart, UART_Callback callback);
void UART_Send_String(UART_DRIVES* user_uart, const char* str);
void UART_Send_Data(UART_DRIVES* user_uart, const char* data, uint16_t len);

uint16_t UART_GetDataWithHT(UART_DRIVES* user_uart, uint8_t *data, const char *head, const char *tail);
uint16_t UART_GetDataWithHTLen(UART_DRIVES* user_uart, uint8_t *data, const char *head, const char *tail, uint16_t len);
uint16_t UART_GetDataWithHLen(UART_DRIVES* user_uart, uint8_t *data, const char *head, uint16_t len);
uint16_t UART_GetDataWithLen(UART_DRIVES* user_uart, uint8_t *data, uint16_t len);
uint16_t UART_GetDataWithH(UART_DRIVES* user_uart, uint8_t *data, const char *head);
uint16_t UART_GetAllDate(UART_DRIVES* user_uart, uint8_t *data);

void UART_Printf(UART_DRIVES* user_uart, const char* format, ...);
int32_t UART_Scanf(UART_DRIVES* user_uart, const char *format, ...);

#endif /* HAL_UART_MODULE_ENABLED */
#endif // USER_UART_H
