/* 包含头文件 ----------------------------------------------------------------*/
#include "../Inc/user_dji_bus.h"
#include "../../Core/Inc/bsp.h"

/* 私有变量 ------------------------------------------------------------------*/
static DBUS_DRIVES *dbus_drive = NULL;
static uint8_t dbus_buf[DBUS_BUF_LEN];

/* 私有函数 ------------------------------------------------------------------*/

/**
* @brief 遥控器数据解析函数
*/
static void rc_callback_handler(void) {
    dbus_drive->ch0  = (int16_t)(((dbus_buf[0]  >> 0 | dbus_buf[1]  << 8) & 0x07FF) - 1024);
    dbus_drive->ch1  = (int16_t)(((dbus_buf[1]  >> 3 | dbus_buf[2]  << 5) & 0x07FF) - 1024);
    dbus_drive->ch2  = (int16_t)(((dbus_buf[2]  >> 6 | dbus_buf[3]  << 2  | dbus_buf[4] << 10) & 0x07FF) - 1024);
    dbus_drive->ch3  = (int16_t)(((dbus_buf[4]  >> 1 | dbus_buf[5]  << 7) & 0x07FF) - 1024);

    dbus_drive->roll = (int16_t)(((dbus_buf[16] >> 0 | dbus_buf[17] << 8) & 0x07FF) - 1024);

    dbus_drive->sw1  = ((dbus_buf[5] >> 4) & 0x000C) >> 2;
    dbus_drive->sw2  = ((dbus_buf[5] >> 4) & 0x0003) >> 0;

    dbus_drive->is_update = 1;
}

/**
* @brief UART 空闲中断回调函数
* @param huart UART 句柄指针
* @note  在空闲中断中处理接收到的数据帧
*/
static void dbus_rx_idle_callback(const UART_HandleTypeDef* huart) {
    /* 清除空闲中断标志位 */
    __HAL_UART_CLEAR_IDLEFLAG(huart);

    if (huart == dbus_drive->huart) {
        /* 停止 DMA 传输 */
        __HAL_DMA_DISABLE(huart->hdmarx);

        /* 检查接收数据长度是否正确 */
        const uint16_t dma_current_data_counter = ((uint16_t)(huart->hdmarx->Instance->NDTR));
        if (DBUS_MAX_LEN - dma_current_data_counter == DBUS_BUF_LEN) {
            rc_callback_handler();
        }

        /* 重启 DMA 传输 */
        __HAL_DMA_SET_COUNTER(huart->hdmarx, DBUS_MAX_LEN);
        __HAL_DMA_ENABLE(huart->hdmarx);
    }
}

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化大疆遥控器接收
* @param user_dbus  遥控器驱动结构体指针
* @param huart UART 句柄指针
*/
void DBUS_Init(DBUS_DRIVES* user_dbus, UART_HandleTypeDef* huart) {
    user_dbus->is_update = 0;
    user_dbus->huart = huart;
    dbus_drive = user_dbus;

    /* 清除空闲中断标志并使能空闲中断 */
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

    huart->pRxBuffPtr = dbus_buf;
    huart->RxXferSize = DBUS_MAX_LEN;
    huart->ErrorCode  = HAL_UART_ERROR_NONE;

    /* 启动 DMA 流 */
    HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, (uint32_t)dbus_buf, DBUS_MAX_LEN);

    /* 使能 UART DMA 接收 */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
}

/**
* @brief 遥控器接收处理函数
* @param huart UART 句柄指针
* @note  需要在相应的 UART 中断回调函数中调用
*/
void DBUS_Handler(const UART_HandleTypeDef* huart) {
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) &&
        __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE)) {
        dbus_rx_idle_callback(huart);
    }
}


