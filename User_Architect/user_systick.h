#ifndef USER_SYSTICK_H
#define USER_SYSTICK_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef void (*SysTick_Callback)(void);

/* 函数声明 ------------------------------------------------------------------*/
void SysTick_RegisterCallback(SysTick_Callback callback);
void SysTick_UnregisterCallback(SysTick_Callback callback);
void SysTick_Handle(void);

#endif // USER_SYSTICK_H
