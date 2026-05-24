/* Mock: User_Drives/user_uart.h - minimal stub */
#ifndef USER_UART_H
#define USER_UART_H

#include <stdint.h>

typedef void (*UART_Callback)(void *user_uart);

typedef struct {
    volatile uint8_t rx_data;
    volatile uint8_t tx_busy;
    UART_Callback tx_callback;
    UART_Callback rx_callback;
} UART_DRIVES;

void UART_Init(UART_DRIVES *dev, uint32_t baud);
void UART_Transmit(UART_DRIVES *dev, uint8_t *data, uint16_t len);

#endif
