/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_uart.h"
#include "../../Core/Inc/bsp.h"
#include <stdio.h>
#include <stdlib.h>
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
            if (uart->callback != NULL)
                uart->callback(uart);
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
* @param callback   接收回调函数
*/
void UART_Init(UART_DRIVES* user_uart, UART_HandleTypeDef* huart, UART_Callback callback) {
    user_uart->huart = huart;
    user_uart->status = UART_IDLE;
    user_uart->is_buffer_a = 1;
    user_uart->callback = callback;

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
* @brief 通过队列发送数据
* @param user_uart UART 驱动结构体指针
* @param str       要发送的字符串
*/
void UART_Send(UART_DRIVES* user_uart, const char* str) {
    Queue_Push(&user_uart->tx_queue, (char*)str, strlen(str));
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

