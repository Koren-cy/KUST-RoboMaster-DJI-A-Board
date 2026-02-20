#ifndef __USER_CAN_H__
#define __USER_CAN_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief CAN 接收回调函数类型
* @param user_can CAN 驱动总线结构体指针
*/
typedef void (*CAN_Callback)(void* user_can);

/**
* @brief 发送配置
*/
typedef struct {
    uint8_t IDE;  /* IDE 扩展标志 */
    uint8_t RTR;  /* RTR 远程帧标志 */
} CanTxConfig_t;

/**
* @brief 接收的消息
*/
typedef struct {
    uint32_t StdId;   /* 标准标识符11位 (0 ~ 0x7FF) */
    uint32_t ExtId;   /* 扩展标识符29位 (0 ~ 0x1FFFFFFF) */
    uint8_t IDE;      /* IDE 扩展标志 */
    uint8_t RTR;      /* RTR 远程帧标志 */
    uint8_t DLC;      /* 数据段的长度 (0 ~ 8) */
    uint8_t Data[8];  /* 数据段的内容 */
} CanRxMsg_t;

/**
* @brief can 总线驱动结构体
*/
typedef struct  {
    CAN_HandleTypeDef* hcan;  /* can 总线硬件句柄 */
    CAN_Callback callback;    /* 接收回调函数 */
    CanTxConfig_t tx_conf;    /* 发送配置 */
    CanRxMsg_t rx_msg;        /* 接收的消息 */
} CAN_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void CAN_Init(CAN_DRIVES* user_can, CAN_HandleTypeDef* hcan, CAN_Callback callback);
void CAN_Send(const CAN_DRIVES* user_can, uint32_t id, const uint8_t *data, uint8_t len);


#endif //__USER_CAN_H__