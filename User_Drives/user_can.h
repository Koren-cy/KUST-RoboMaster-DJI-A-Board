#ifndef USER_CAN_H
#define USER_CAN_H
#include "main.h"
#ifdef HAL_CAN_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "../../Core/Inc/bsp_config.h"
#include "../../User_Architect/user_circular_queue.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief CAN 发送消息结构体
*/
typedef struct {
    uint32_t id;      /* 标准标识符或扩展标识符 */
    uint8_t data[8];  /* 数据段 */
    uint8_t len;      /* 数据长度 (0 ~ 8) */
} CAN_Tx_Msg;

/**
* @brief CAN 接收回调函数类型
* @param user_can CAN 驱动总线结构体指针
*/
typedef void (*CAN_Callback)(void* user_can);

/**
* @brief 发送配置
*/
typedef struct {
    volatile uint8_t IDE;  /* IDE 扩展标志 */
    volatile uint8_t RTR;  /* RTR 远程帧标志 */
} CAN_Tx_Config;

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
} CAN_Rx_Msg;

/**
* @brief can 总线驱动结构体
*/
typedef struct  {
    CAN_HandleTypeDef* hcan;                    /* can 总线硬件句柄 */
    CAN_Callback callbacks[CAN_CALLBACK_NUM];   /* 接收回调函数数组 */
    uint8_t callback_num;                       /* 已注册的回调函数数量 */
    volatile CAN_Tx_Config tx_conf;             /* 发送配置 */
    CAN_Rx_Msg rx_msg;                          /* 接收的消息 */
    CIRCULAR_QUEUE tx_queue;                    /* 发送队列 */
} CAN_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void CAN_Init(CAN_DRIVES* user_can, CAN_HandleTypeDef* hcan);
void CAN_RegisterCallback(CAN_DRIVES* user_can, CAN_Callback callback);
void CAN_Send(CAN_DRIVES* user_can, uint32_t id, const uint8_t *data, uint8_t len);

#endif /* HAL_CAN_MODULE_ENABLED */
#endif // USER_CAN_H