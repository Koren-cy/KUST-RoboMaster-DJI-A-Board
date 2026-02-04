#ifndef __USER_CAN_H__
#define __USER_CAN_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief  消息接收回调函数
*/
typedef void (*CAN_Callback)(void*);

/**
* @brief  发送配置
*/
typedef struct {
    uint8_t IDE;  /*存储 IDE 扩展标志 */
    uint8_t RTR;  /*存储 RTR 远程帧标志*/
} CanTxConfig_t;

/**
* @brief  接收的消息
*/
typedef struct {
    uint32_t StdId;   /*存储报文的标准标识符11位，0-0x7FF. */
    uint32_t ExtId;   /*存储报文的扩展标识符29位，0-0x1FFFFFFF. */
    uint8_t IDE;      /*存储 IDE 扩展标志 */
    uint8_t RTR;      /*存储 RTR 远程帧标志*/
    uint8_t DLC;      /*存储报文数据段的长度，0-8 */
    uint8_t Data[8];  /*存储报文数据段的内容 */
    uint8_t FMI;      /*存储本报文是由经过筛选器存储进FIFO的，0-0xFF */
} CanRxMsg_t;

/**
* @brief  can 总线结构体
*/
typedef struct  {
    CAN_HandleTypeDef* hcan;  /*存储了报文的标准标识符11位，0-0x7FF. */
    CAN_Callback callback;    /*存储了报文的标准标识符11位，0-0x7FF. */
    CanTxConfig_t tx_conf;    /*存储了报文的标准标识符11位，0-0x7FF. */
    CanRxMsg_t rx_msg;        /*存储了报文的标准标识符11位，0-0x7FF. */
} CAN_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void CAN_Init(CAN_DRIVES* user_can, CAN_HandleTypeDef* hcan, const CAN_Callback callback);
void CAN_Send(const CAN_DRIVES* user_can, const uint32_t id, const uint8_t *data, const uint8_t len);


#endif //__USER_CAN_H__