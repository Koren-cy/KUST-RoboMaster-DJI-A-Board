/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_queue.h"
#include <stdlib.h>
#include <string.h>

/* 函数体 -------------------------------------------------------------------*/

/**
* @brief 入队操作
* @param queue 队列指针
* @param data  数据指针
* @param len   数据长度
*/
void Queue_Push(Queue *queue, const void *data, uint16_t len) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    
    void *mem = malloc(len);
    memcpy(mem, data, len);

    newNode->data = mem;
    newNode->len = len;
    newNode->address = 0;
    newNode->next = NULL;
    
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    
    queue->size++;
}

/**
* @brief 入队操作 (带地址)
* @param queue   队列指针
* @param data    数据指针
* @param len     数据长度
* @param address 传输目的地址
*/
void Queue_PushWA(Queue *queue, const void *data, uint16_t len, uint16_t address) {
    Node *newNode = (Node*)malloc(sizeof(Node));

    void *mem = malloc(len);
    memcpy(mem, data, len);

    newNode->data = mem;
    newNode->len = len;
    newNode->address = address;
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->size++;
}

/**
* @brief 出队操作
* @param queue 队列指针
* @return 出队的节点指针，队列为空时返回 NULL
*/
Node* Queue_Pop(Queue *queue) {
    if (queue->front == NULL) {
        return NULL;
    }
    
    queue->popped = queue->front;
    queue->front = queue->front->next;
    
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    queue->size--;
    return queue->popped;
}

/**
* @brief 检查队列是否为空
* @param queue 队列指针
* @return 1: 队列为空, 0: 队列非空
*/
uint8_t Queue_IsEmpty(const Queue *queue) {
    return queue->front == NULL;
}

/**
* @brief 获取队列大小
* @param queue 队列指针
* @return 队列中的元素数量
*/
uint16_t Queue_GetSize(const Queue *queue) {
    return queue->size;
}

/**
* @brief 释放节点内存
* @param node 节点指针
*/
void Queue_FreeNode(Node *node) {
    if (node == NULL)
        return;
    
    if (node->data != NULL)
        free(node->data);
    
    free(node);
}

/**
* @brief 释放队列内存
* @param queue 队列指针
*/
void Queue_Free(Queue *queue) {
    while (!Queue_IsEmpty(queue)) {
        Node *node = Queue_Pop(queue);
        Queue_FreeNode(node);
    }
}
