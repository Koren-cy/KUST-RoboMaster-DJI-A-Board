#ifndef USER_CIRCULAR_QUEUE_H
#define USER_CIRCULAR_QUEUE_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 循环队列结构体
*/
typedef struct {
    uint8_t *buffer;      /* 数据缓冲区指针 */
    uint16_t packet_size; /* 单个数据包的长度 */
    uint16_t capacity;    /* 队列容量，即数据包数量 */
    uint16_t head;        /* 队头索引 */
    uint16_t tail;        /* 队尾索引 */
    uint16_t count;       /* 当前数据包数量 */
} CIRCULAR_QUEUE;

/* 函数声明 ------------------------------------------------------------------*/
void Circular_Queue_Init(CIRCULAR_QUEUE *queue, uint16_t packet_size, uint16_t capacity);
uint16_t Circular_Queue_Get_Count(const CIRCULAR_QUEUE *queue);
uint16_t Circular_Queue_Get_Free_Space(const CIRCULAR_QUEUE *queue);
void Circular_Queue_Enqueue(CIRCULAR_QUEUE *queue, const uint8_t *packet);
uint8_t Circular_Queue_Dequeue(CIRCULAR_QUEUE *queue, uint8_t *packet);
uint8_t Circular_Queue_Discard(CIRCULAR_QUEUE *queue);
uint8_t Circular_Queue_Peek(const CIRCULAR_QUEUE *queue, uint8_t *packet);
void Circular_Queue_Clear(CIRCULAR_QUEUE *queue);

#endif // USER_CIRCULAR_QUEUE_H
