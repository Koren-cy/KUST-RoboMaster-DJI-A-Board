/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_circular_queue.h"
#include <string.h>
#include <stdlib.h>


/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化循环队列
* @param queue       循环队列结构体指针
* @param packet_size 单个数据包的长度 单位：字节
* @param capacity    队列容量，即数据包数量
*/
void Circular_Queue_Init(CIRCULAR_QUEUE *queue, const uint16_t packet_size, const uint16_t capacity) {
    queue->buffer = (uint8_t *)malloc(packet_size * capacity);

    queue->packet_size = packet_size;
    queue->capacity = capacity;
}

/**
* @brief 判断队列是否为空
* @param queue 循环队列结构体指针
* @return 1: 队列为空, 0: 队列非空
*/
static uint8_t Circular_Queue_Is_Empty(const CIRCULAR_QUEUE *queue) {
    return queue->count == 0;
}

/**
* @brief 判断队列是否已满
* @param queue 循环队列结构体指针
* @return 1: 队列已满, 0: 队列未满
*/
static uint8_t Circular_Queue_Is_Full(const CIRCULAR_QUEUE *queue) {
    return queue->count == queue->capacity;
}

/**
* @brief 获取队列中数据包数量
* @param queue 循环队列结构体指针
* @return 当前数据包数量
*/
uint16_t Circular_Queue_Get_Count(const CIRCULAR_QUEUE *queue) {
    return queue->count;
}

/**
* @brief 获取队列剩余空间
* @param queue 循环队列结构体指针
* @return 剩余空间大小
*/
uint16_t Circular_Queue_Get_Free_Space(const CIRCULAR_QUEUE *queue) {
    return queue->capacity - queue->count;
}

/**
* @brief 入队操作，在队列满时覆盖最旧数据
* @param queue  循环队列结构体指针
* @param packet 要入队的数据包指针
*/
void Circular_Queue_Enqueue(CIRCULAR_QUEUE *queue, const uint8_t *packet) {
    uint8_t *dest = queue->buffer + (queue->tail * queue->packet_size);
    memcpy(dest, packet, queue->packet_size);
    queue->tail = (queue->tail + 1) % queue->capacity;

    if (Circular_Queue_Is_Full(queue)) {
        queue->head = (queue->head + 1) % queue->capacity;
    } else {
        queue->count++;
    }
}

/**
* @brief 出队操作
* @param queue  循环队列结构体指针
* @param packet 用于存储出队数据包的指针
* @return 1: 出队成功, 0: 队列为空
*/
uint8_t Circular_Queue_Dequeue(CIRCULAR_QUEUE *queue, uint8_t *packet) {
    if (Circular_Queue_Is_Empty(queue))
        return 0;

    const uint8_t *src = queue->buffer + (queue->head * queue->packet_size);
    memcpy(packet, src, queue->packet_size);
    queue->head = (queue->head + 1) % queue->capacity;
    queue->count--;

    return 1;
}

/**
* @brief 丢弃队头数据包但不读取
* @param queue 循环队列结构体指针
* @return 1: 丢弃成功, 0: 队列为空
*/
uint8_t Circular_Queue_Discard(CIRCULAR_QUEUE *queue) {
    if (Circular_Queue_Is_Empty(queue))
        return 0;

    queue->head = (queue->head + 1) % queue->capacity;
    queue->count--;

    return 1;
}

/**
* @brief 查看队头数据包但不出队
* @param queue  循环队列结构体指针
* @param packet 用于存储队头数据包的指针
* @return 1: 查看成功, 0: 队列为空
*/
uint8_t Circular_Queue_Peek(const CIRCULAR_QUEUE *queue, uint8_t *packet) {
    if (Circular_Queue_Is_Empty(queue))
        return 0;

    const uint8_t *src = queue->buffer + (queue->head * queue->packet_size);
    memcpy(packet, src, queue->packet_size);

    return 1;
}

/**
* @brief 清空队列
* @param queue 循环队列结构体指针
*/
void Circular_Queue_Clear(CIRCULAR_QUEUE *queue) {
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
}
