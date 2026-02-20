#ifndef __USER_DELAY_H__
#define __USER_DELAY_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 扩展变量 ------------------------------------------------------------------*/
extern uint32_t TimeFlag;

/* 函数声明 ------------------------------------------------------------------*/
void Delay_Init(const TIM_HandleTypeDef *htim);
void Delay_Update_TimeFlag(void);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s (uint32_t s);


#endif //__USER_DELAY_H__
