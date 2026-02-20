#ifndef __USER_QUEUE_H__
#define __USER_QUEUE_H__

/* 包含头文件 -----------------------------------------------------------------*/
#include <stdint.h>

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 队列节点结构体
*/
typedef struct Node {
    void *data;         /* 数据指针 */
    uint16_t len;       /* 数据长度 */
    uint16_t address;   /* 传输目的地址 */
    struct Node *next;  /* 下一个节点指针 */
} Node;

/**
* @brief 队列结构体
*/
typedef struct {
    Node *front;    /* 队首指针 */
    Node *rear;     /* 队尾指针 */
    Node *popped;   /* 最近出队的节点指针 */
    uint16_t size;  /* 队列大小 */
} Queue;

/* 函数声明 ------------------------------------------------------------------*/
void Queue_Push(Queue *queue, const void *data, uint16_t len);
void Queue_PushWA(Queue *queue, const void *data, uint16_t len, uint16_t address);
Node* Queue_Pop(Queue *queue);
uint8_t Queue_IsEmpty(const Queue *queue);
uint16_t Queue_GetSize(const Queue *queue);
void Queue_FreeNode(Node *node);
void Queue_Free(Queue *queue);

#endif //__USER_QUEUE_H__
