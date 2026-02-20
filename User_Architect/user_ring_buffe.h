#ifndef __USER_RING_BUFFER_H__
#define __USER_RING_BUFFER_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

/* 宏定义 --------------------------------------------------------------------*/
#define RING_BUFFER_SIZE 1024

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 环形缓冲区结构体
*/
typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];  /* 缓冲区数组 */
    uint16_t read_index;               /* 读索引 */
    uint16_t write_index;              /* 写索引 */
} RING_BUFFER;

/* 函数声明 ------------------------------------------------------------------*/
uint16_t RingBuffer_GetLength(const RING_BUFFER *buffer);
uint16_t RingBuffer_Put(RING_BUFFER *buffer, const uint8_t *data, uint16_t length);
uint16_t RingBuffer_GetWithHT(RING_BUFFER *buffer, uint8_t *data, const char *head, const char *tail);
uint16_t RingBuffer_GetWithHLen(RING_BUFFER *buffer, uint8_t *data, const char *head, uint16_t len);
uint16_t RingBuffer_GetWithLen(RING_BUFFER *buffer, uint8_t *data, uint16_t len);
uint16_t RingBuffer_GetWithH(RING_BUFFER *buffer, uint8_t *data, const char *head);

#endif //__USER_RING_BUFFER_H__