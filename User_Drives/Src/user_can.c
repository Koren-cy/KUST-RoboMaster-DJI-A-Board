#include "../../Core/Inc/bsp.h"
#ifdef HAL_CAN_MODULE_ENABLED
/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_can.h"

/* 私有变量 ------------------------------------------------------------------*/
static CAN_DRIVES *can_drives[CAN_NUM];
static uint8_t can_num = 0;
static uint8_t is_init_loop_event_sign = 0;

/* 私有函数 ------------------------------------------------------------------*/

/**
* @brief 处理 CAN 发送队列
* @note  该函数会自动注册在全局注册表
*/
static void CAN_QueueHandle(void) {
    for (uint8_t can_index = 0; can_index < can_num; can_index++) {
        CAN_DRIVES *can = can_drives[can_index];

        // 处理发送队列
        CAN_Tx_Msg tx_msg;
        if (Circular_Queue_Peek(&can->tx_queue, (uint8_t*)&tx_msg)) {
            CAN_TxHeaderTypeDef tx_header;
            tx_header.StdId = tx_msg.id;
            tx_header.ExtId = tx_msg.id;
            tx_header.IDE = can->tx_conf.IDE;
            tx_header.RTR = can->tx_conf.RTR;
            tx_header.DLC = tx_msg.len;
            tx_header.TransmitGlobalTime = DISABLE;

            uint32_t tx_mailbox;
            if (HAL_CAN_AddTxMessage(can->hcan, &tx_header, tx_msg.data, &tx_mailbox) == HAL_OK) {
                Circular_Queue_Discard(&can->tx_queue);
            }
        }
    }
}

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 CAN 总线
* @param user_can CAN 总线驱动结构体指针
* @param hcan     硬件句柄
*/
void CAN_Init(CAN_DRIVES* user_can, CAN_HandleTypeDef* hcan){

    user_can->hcan = hcan;

    user_can->tx_conf.IDE = CAN_ID_STD;
    user_can->tx_conf.RTR = CAN_RTR_DATA;

    Circular_Queue_Init(&user_can->tx_queue, sizeof(CAN_Tx_Msg), 6);

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

    // 注册到全局事件循环
    if (is_init_loop_event_sign == 0) {
        loop_event[loop_event_num] = &CAN_QueueHandle;
        loop_event_num++;
        is_init_loop_event_sign = 1;
    }
}

/**
* @brief 注册 CAN 接收回调函数
* @param user_can CAN 总线驱动结构体指针
* @param callback 用户自定义的 can 总线消息接收回调函数
*/
void CAN_RegisterCallback(CAN_DRIVES* user_can, const CAN_Callback callback){
    user_can->callbacks[user_can->callback_num] = callback;
    user_can->callback_num++;
}

/**
* @brief 发送消息
* @param user_can CAN 总线驱动结构体指针
* @param id       报文标准标识符
* @param data     报文数据
* @param len      报文数据长度
*/
void CAN_Send(CAN_DRIVES* user_can, const uint32_t id, const uint8_t *data, const uint8_t len){
    CAN_Tx_Msg tx_msg;
    tx_msg.id = id;
    tx_msg.len = len;
    
    memcpy(tx_msg.data, data, tx_msg.len);
    Circular_Queue_Enqueue(&user_can->tx_queue, (uint8_t*)&tx_msg);
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

            for (uint8_t i = 0; i < can_drive->callback_num; i++) {
                can_drive->callbacks[i](can_drive);
            }
        }
    }
}

#endif /* HAL_CAN_MODULE_ENABLED */