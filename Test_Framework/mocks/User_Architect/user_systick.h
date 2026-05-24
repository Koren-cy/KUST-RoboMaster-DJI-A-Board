/* Mock: User_Architect/user_systick.h - self-contained stub */
#ifndef USER_SYSTICK_H
#define USER_SYSTICK_H

#include <stdint.h>

typedef void (*SysTick_Callback)(void *arg);

typedef enum {
    Task_ONCE,
    Task_REPEAT
} TaskMode;

typedef enum {
    Task_STOPPED,
    Task_RUNNING
} TaskState;

typedef struct {
    SysTick_Callback callback;
    volatile void *arg;
    uint32_t delay;
    uint32_t period;
    uint32_t last_tick;
    TaskMode mode;
    TaskState state;
} SysTick_Task;

void SysTick_Init(void);
uint32_t SysTick_GetTick(void);

#endif
