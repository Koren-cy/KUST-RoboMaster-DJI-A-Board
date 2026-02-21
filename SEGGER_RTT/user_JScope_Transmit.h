#ifndef __USER_JSCOPE_TRANSMIT_H__
#define __USER_JSCOPE_TRANSMIT_H__

#include "main.h"

/* 宏定义 ----------------------------------------------------------------*/
#define JSCOPE_BUFFER_NAME      "JScope_T4F4F4F4"   // JScope 缓冲区名称

/* 类型定义 --------------------------------------------------------------*/
#pragma pack(push, 1)
typedef struct {
    uint32_t timestamp;  /* 时间戳 单位：us */
    float val_1;
    float val_2;
    float val_3;
} JScope_Transmit_t;
#pragma pack(pop)

/* 函数声明 ------------------------------------------------------------------*/
void JScope_Init(TIM_HandleTypeDef *htim);
void JScope_Transmit(uint32_t delta_time);

#endif //__USER_JSCOPE_TRANSMIT_H__