#include "../../Core/Inc/bsp.h"
#ifdef HAL_UART_MODULE_ENABLED
/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_uart.h"
#include <stdio.h>
#include <string.h>

/* 私有变量 ------------------------------------------------------------------*/
static UART_DRIVES *uart_drives[UART_NUM];
static uint8_t uart_num = 0;
static uint8_t is_init_loop_event_sign = 0;

/* 私有函数 ------------------------------------------------------------------*/

/**
* @brief 处理 UART 队列
* @note  处理接收和发送队列，该函数会自动注册在全局注册表（全局事件循环）
*/
static void UART_QueueHandle(void) {
    for (uint8_t uart_index = 0; uart_index < uart_num; uart_index++) {
        UART_DRIVES *uart = uart_drives[uart_index];

        // 处理接收队列
        if (RingBuffer_GetLength(&uart->rx_ringBuffer)) {
            for (uint8_t i = 0; i < uart->callback_num; i++) {
                uart->callbacks[i](uart);
            }
        }

        // 处理发送队列
        if (!Queue_IsEmpty(&uart->tx_queue)) {
            if (uart->status == UART_IDLE) {
                const Node* temp = Queue_Pop(&uart->tx_queue);
                HAL_UART_Transmit_DMA(uart->huart, (uint8_t*)temp->data, temp->len);
                uart->status = UART_SENDING;
            }
        }
    }
}

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 UART 驱动
* @param user_uart  UART 驱动结构体指针
* @param huart      UART 句柄
*/
void UART_Init(UART_DRIVES* user_uart, UART_HandleTypeDef* huart) {
    memset(user_uart, 0, sizeof(UART_DRIVES));
    
    user_uart->huart = huart;
    user_uart->status = UART_IDLE;
    user_uart->is_buffer_a = 1;

    uart_drives[uart_num] = user_uart;
    uart_num++;

    HAL_UARTEx_ReceiveToIdle_DMA(huart, user_uart->rx_buffer_a, UART_BUFFER_SIZE);

    if (is_init_loop_event_sign == 0) {
        loop_event[loop_event_num] = &UART_QueueHandle;
        loop_event_num++;
        is_init_loop_event_sign = 1;
    }
}

/**
* @brief 注册 UART 接收回调函数
* @param user_uart  UART 驱动结构体指针
* @param callback   接收回调函数
*/
void UART_RegisterCallback(UART_DRIVES* user_uart, const UART_Callback callback) {
    user_uart->callbacks[user_uart->callback_num] = callback;
    user_uart->callback_num++;
}

/**
* @brief 通过队列发送数据
* @param user_uart UART 驱动结构体指针
* @param str       要发送的字符串
*/
void UART_Send(UART_DRIVES* user_uart, const char* str) {
    Queue_Push(&user_uart->tx_queue, (char*)str, strlen(str));
}


/**
* @brief 根据帧头和帧尾获取数据
* @param user_uart UART 驱动结构体指针
* @param data      数据缓冲区
* @param head      帧头字符串
* @param tail      帧尾字符串
* @return 获取的数据长度
*/
uint16_t UART_GetDataWithHT(UART_DRIVES* user_uart, uint8_t *data, const char *head, const char *tail) {
    return RingBuffer_GetWith_H_T(&user_uart->rx_ringBuffer, data, head, tail);
}

/**
* @brief 根据帧头和指定长度获取数据
* @param user_uart UART 驱动结构体指针
* @param data      数据缓冲区
* @param head      帧头字符串
* @param len       要获取的数据长度
* @return 获取的数据长度
*/
uint16_t UART_GetDataWithHLen(UART_DRIVES* user_uart, uint8_t *data, const char *head, const uint16_t len) {
    return RingBuffer_GetWith_H_Len(&user_uart->rx_ringBuffer, data, head, len);
}

/**
* @brief 根据指定长度获取数据
* @param user_uart UART 驱动结构体指针
* @param data      数据缓冲区
* @param len       要获取的数据长度
* @return 获取的数据长度
*/
uint16_t UART_GetDataWithLen(UART_DRIVES* user_uart, uint8_t *data, uint16_t len) {
    return RingBuffer_GetWith_Len(&user_uart->rx_ringBuffer, data, len);
}

/**
* @brief 根据帧头获取数据（直到缓冲区末尾）
* @param user_uart UART 驱动结构体指针
* @param data      数据缓冲区
* @param head      帧头字符串
* @return 获取的数据长度
*/
uint16_t UART_GetDataWithH(UART_DRIVES* user_uart, uint8_t *data, const char *head) {
    return RingBuffer_GetWith_H_H(&user_uart->rx_ringBuffer, data, head);
}

/* 覆写中断回调函数 -----------------------------------------------------------*/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    for (uint8_t uart_index = 0; uart_index < uart_num; uart_index++) {
        UART_DRIVES *uart = uart_drives[uart_index];

        if (huart != uart->huart)
            continue;

        if (uart->tx_queue.popped) {
            Queue_FreeNode(uart->tx_queue.popped);
            uart->tx_queue.popped = NULL;
            uart->status = UART_IDLE;
        }
    }
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    for (uint8_t uart_index = 0; uart_index < uart_num; uart_index++) {
        UART_DRIVES *uart = uart_drives[uart_index];

        if (uart->huart != huart)
            continue;

        if (uart->is_buffer_a) {
            HAL_UARTEx_ReceiveToIdle_DMA(uart->huart, uart->rx_buffer_b, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(uart->huart->hdmarx, DMA_IT_HT);
            RingBuffer_Put(&uart->rx_ringBuffer, uart->rx_buffer_a, Size);
            uart->is_buffer_a = 0;
        } else {
            HAL_UARTEx_ReceiveToIdle_DMA(uart->huart, uart->rx_buffer_a, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(uart->huart->hdmarx, DMA_IT_HT);
            RingBuffer_Put(&uart->rx_ringBuffer, uart->rx_buffer_b, Size);
            uart->is_buffer_a = 1;
        }
    }
}

#endif /* HAL_UART_MODULE_ENABLED */

