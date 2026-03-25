#ifndef USER_SYSTICK_H
#define USER_SYSTICK_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef void (*SysTick_Callback)(void *arg);

typedef enum {
    Task_ONCE,      // 单次任务
    Task_REPEAT     // 重复任务
} TaskMode;

typedef enum {
    Task_STOPPED,   // 已停止
    Task_RUNNING    // 运行中
} TaskState;

typedef struct {
    SysTick_Callback callback;  // 回调函数
    volatile void *arg;         // 回调参数
    uint32_t delay;             // 延迟时间 单位：ms
    uint32_t period;            // 周期时间 单位：ms
    uint32_t counter;           // 内部计数器
    TaskMode mode;              // 任务模式
    TaskState state;            // 任务状态
} SysTick_Task;

/* 函数声明 ------------------------------------------------------------------*/
void SysTick_InitTask(SysTick_Task *Task, void *arg, uint32_t delay, uint32_t period, TaskMode mode, SysTick_Callback callback);
void SysTick_StartTask(SysTick_Task *Task);
void SysTick_StopTask(SysTick_Task *Task);
void SysTick_ResetTask(SysTick_Task *Task);
void SysTick_DeleteTask(SysTick_Task *Task);
void SysTick_Handle(void);

#endif // USER_SYSTICK_H
