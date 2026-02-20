/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_can.h"
#include "../../Core/Inc/bsp.h"

/* 私有变量 ------------------------------------------------------------------*/
static CAN_DRIVES *can_drives[CAN_NUM];
static uint8_t can_num = 0;

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 CAN 总线
* @param user_can CAN 总线驱动结构体指针
* @param hcan     硬件句柄
* @param callback 用户自定义的 can 总线消息接收回调函数
*/
void CAN_Init(CAN_DRIVES* user_can, CAN_HandleTypeDef* hcan, const CAN_Callback callback){
    user_can->hcan = hcan;
    user_can->callback = callback;

    user_can->tx_conf.IDE = CAN_ID_STD;
    user_can->tx_conf.RTR = CAN_RTR_DATA;

    CAN_FilterTypeDef can_filter;
    can_filter.FilterActivation = ENABLE;
    can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter.FilterIdHigh = 0x0000;
    can_filter.FilterIdLow = 0x0000;
    can_filter.FilterMaskIdHigh = 0x0000;
    can_filter.FilterMaskIdLow = 0x0000;
    can_filter.FilterBank = can_num * 14;
    can_filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    can_filter.SlaveStartFilterBank = can_num * 14;

    HAL_CAN_ConfigFilter(hcan, &can_filter);
    HAL_CAN_Start(hcan);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

    can_drives[can_num] = user_can;
    can_num ++;
}

/**
* @brief 发送消息
* @param user_can CAN 总线驱动结构体指针
* @param id       报文标准标识符
* @param data     报文数据
* @param len      报文数据长度
*/
void CAN_Send(const CAN_DRIVES* user_can, const uint32_t id, const uint8_t *data, const uint8_t len){
    CAN_TxHeaderTypeDef tx_msg;

    tx_msg.StdId = id;
    tx_msg.ExtId = id;
    tx_msg.IDE = user_can->tx_conf.IDE;
    tx_msg.RTR = user_can->tx_conf.RTR;
    tx_msg.DLC = len;
    tx_msg.TransmitGlobalTime = DISABLE;

    if (HAL_CAN_AddTxMessage(user_can->hcan, &tx_msg, data, (uint32_t*)CAN_TX_MAILBOX0) == HAL_OK)
        return;
    if (HAL_CAN_AddTxMessage(user_can->hcan, &tx_msg, data, (uint32_t*)CAN_TX_MAILBOX1) == HAL_OK)
        return;
    if (HAL_CAN_AddTxMessage(user_can->hcan, &tx_msg, data, (uint32_t*)CAN_TX_MAILBOX2) == HAL_OK)
        return;
}


/* 覆写中断回调函数 -----------------------------------------------------------*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    for (uint8_t can_index = 0 ; can_index < can_num ; can_index++) {
        CAN_DRIVES* can_drive = can_drives[can_index];
        if (hcan == can_drive->hcan) {
            CAN_RxHeaderTypeDef RxHeader;

            if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, can_drive->rx_msg.Data) != HAL_OK)
                continue;

            can_drive->rx_msg.StdId = RxHeader.StdId;
            can_drive->rx_msg.ExtId = RxHeader.ExtId;

            can_drive->rx_msg.IDE = RxHeader.IDE;
            can_drive->rx_msg.RTR = RxHeader.RTR;
            can_drive->rx_msg.DLC = RxHeader.DLC;

            can_drive->callback(can_drive);
        }
    }
}