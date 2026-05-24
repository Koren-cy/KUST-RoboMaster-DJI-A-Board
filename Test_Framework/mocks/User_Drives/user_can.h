/* Mock: User_Drives/user_can.h - minimal stub */
#ifndef USER_CAN_H
#define USER_CAN_H

#include <stdint.h>

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
} CAN_Tx_Msg;

typedef void (*CAN_Callback)(void *user_can);

typedef struct {
    volatile uint8_t rx_flag;
    CAN_Callback callback;
} CAN_DRIVES;

void CAN_Init(CAN_DRIVES *dev);
void CAN_Transmit(CAN_DRIVES *dev, CAN_Tx_Msg *msg);

#endif
