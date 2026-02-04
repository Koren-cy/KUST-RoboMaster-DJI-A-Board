/* 包含头文件 ----------------------------------------------------------------*/
#include "user_dji_bus.h"
#include "../../Core/Inc/bsp.h"
#include <stdio.h>
#include "string.h"
#include "stdlib.h"

/* 私有变量 ------------------------------------------------------------------*/
static DBUS_DRIVES *dbus_drive = NULL;
static uint8_t dbus_buf[DBUS_BUFLEN];

/* 函数体 --------------------------------------------------------------------*/

/**
  * @brief      enable global uart it and do not use DMA transfer done it
  * @param[in]  huart: uart IRQHandler id
  * @param[in]  pData: receive buff
  * @param[in]  Size:  buff size
  * @retval     set success or fail
  */
static void DBUS_receive_dma_no_it(UART_HandleTypeDef* huart, uint8_t* pData){
	uint32_t tmp1 = 0;
	tmp1 = huart->RxState;

	if (tmp1 == HAL_UART_STATE_READY)
	{
		huart->pRxBuffPtr = pData;
		huart->RxXferSize = DBUS_MAX_LEN;
		huart->ErrorCode  = HAL_UART_ERROR_NONE;

		/* Enable the DMA Stream */
		HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, (uint32_t)pData, DBUS_MAX_LEN);

		/*
		 * Enable the DMA transfer for the receiver request by setting the DMAR bit
		 * in the UART CR3 register
		 */
		SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
	}
}

/**
  * @brief      returns the number of remaining data units in the current DMAy Streamx transfer.
  * @param[in]  dma_stream: where y can be 1 or 2 to select the DMA and x can be 0
  *             to 7 to select the DMA Stream.
  * @retval     The number of remaining data units in the current DMAy Streamx transfer.
  */
uint16_t dma_current_data_counter(DMA_Stream_TypeDef *dma_stream)
{
  /* Return the number of remaining data units for DMAy Streamx */
  return ((uint16_t)(dma_stream->NDTR));
}



/**
  * @brief       handle received rc data
  * @param[out]  dbus_drive:   structure to save handled rc data
  * @param[in]   buff: the buff which saved raw rc data
  * @retval
  */
void rc_callback_handler()
{
	dbus_drive->ch0  =   (dbus_buf[0]  >> 0  | dbus_buf[1]  << 8 ) & 0x07FF;
	dbus_drive->ch0  -=  1024;
	dbus_drive->ch1  =   (dbus_buf[1]  >> 3  | dbus_buf[2]  << 5 ) & 0x07FF;
	dbus_drive->ch1  -=  1024;
	dbus_drive->ch2  =   (dbus_buf[2]  >> 6  | dbus_buf[3]  << 2 |  dbus_buf[4] << 10) & 0x07FF;
	dbus_drive->ch2  -=  1024;
	dbus_drive->ch3  =   (dbus_buf[4]  >> 1  | dbus_buf[5]  << 7 ) & 0x07FF;
	dbus_drive->ch3  -=  1024;

	dbus_drive->roll =   (dbus_buf[16] >> 0  | dbus_buf[17] << 8 ) & 0x07FF;
	dbus_drive->roll -=  1024;

	dbus_drive->sw1  =   ((dbus_buf[5] >> 4) & 0x000C) >> 2;
	dbus_drive->sw2  =   ( dbus_buf[5] >> 4) & 0x0003;

	dbus_drive->is_update = 1;
}

/**
  * @brief      clear idle it flag after uart receive a frame data
  * @param[in]  huart: uart IRQHandler id
  * @retval
  */
static void DBUS_rx_idle_callback(UART_HandleTypeDef* huart)
{
	/* clear idle it flag avoid idle interrupt all the time */
	__HAL_UART_CLEAR_IDLEFLAG(huart);

	/* handle received data in idle interrupt */
	if (huart == dbus_drive->huart)
	{
		/* clear DMA transfer complete flag */
		__HAL_DMA_DISABLE(huart->hdmarx);

		/* handle dbus data dbus_buf from DMA */
		if ((DBUS_MAX_LEN - dma_current_data_counter(huart->hdmarx->Instance)) == DBUS_BUFLEN)
		{
			rc_callback_handler();
		}

		/* restart dma transmission */
		__HAL_DMA_SET_COUNTER(huart->hdmarx, DBUS_MAX_LEN);
		__HAL_DMA_ENABLE(huart->hdmarx);
	}
}

/**
  * @brief      请将该函数放在相应的串口中断的回调函数中。
  * @param[in]  huart: uart IRQHandler id
  * @retval
  */
void DBUS_receive_handler(UART_HandleTypeDef *huart)
{
	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) &&
			__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE))
	{
		DBUS_rx_idle_callback(huart);
	}
}

void DBUS_Init(DBUS_DRIVES* user_dbus, UART_HandleTypeDef* huart){
	user_dbus->is_update = 0;
	dbus_drive = user_dbus;
	user_dbus->huart = huart;

	/* open uart idle it */
	__HAL_UART_CLEAR_IDLEFLAG(dbus_drive->huart);
	__HAL_UART_ENABLE_IT(dbus_drive->huart, UART_IT_IDLE);

	DBUS_receive_dma_no_it(dbus_drive->huart, dbus_buf);
}


