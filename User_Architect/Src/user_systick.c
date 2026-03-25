/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_systick.h"

/* 私有变量 ------------------------------------------------------------------*/
static SysTick_Task *systick_tasks[MAX_SYSTICK_TASK] = {0};
static uint8_t task_count = 0;

/* 函数实现 ------------------------------------------------------------------*/

/**
 * @brief 初始化并执行 SysTick 任务
 * @param Task:   任务结构体指针
 * @param arg:    回调函数参数
 * @param delay:  延迟时间 单位：ms
 * @param period: 周期时间，仅对重复任务有效 单位：ms
 * @param mode:   任务模式
 * @param callback 回调函数
 */
void SysTick_InitTask(SysTick_Task *Task, void *arg, const uint32_t delay, const uint32_t period, const TaskMode mode, const SysTick_Callback callback) {
    Task->arg = arg;
    Task->delay = delay;
    Task->period = period;
    Task->counter = 0;
    Task->mode = mode;
    Task->state = Task_STOPPED;
    Task->callback = callback;

    systick_tasks[task_count] = Task;
    task_count++;
}

/**
 * @brief 开始 SysTick 任务
 * @param Task: 任务结构体指针
 */
void SysTick_StartTask(SysTick_Task *Task) {
    Task->state = Task_RUNNING;
}

/**
 * @brief 停止 SysTick 任务
 * @param Task: 任务结构体指针
 */
void SysTick_StopTask(SysTick_Task *Task) {
    Task->state = Task_STOPPED;
}

/**
 * @brief 重置 SysTick 任务
 * @param Task: 任务结构体指针
 */
void SysTick_ResetTask(SysTick_Task *Task) {
    Task->counter = 0;
    Task->state = Task_RUNNING;
}

/**
 * @brief  SysTick 任务处理函数
 * @note  该函数需要在 SysTick_Handler 中调用
 */
void SysTick_Handle(void) {
    for (uint8_t i = 0; i < task_count; i++) {
        SysTick_Task *task = systick_tasks[i];
        
        if (task->state == Task_STOPPED) {
            continue;
        }

        task->counter++;

        if (task->counter >= task->delay) {
            task->callback((void *)task->arg);

            switch (task->mode) {
                case Task_ONCE:
                    task->state = Task_STOPPED;
                    break;
                case Task_REPEAT:
                    task->counter = 0;
                    task->delay = task->period;
                    break;
            }
        }
    }
}