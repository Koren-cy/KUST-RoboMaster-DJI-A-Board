/* Mock: User_Architect/user_systick.h */
#ifndef USER_SYSTICK_H
#define USER_SYSTICK_H

#include <stdint.h>

typedef struct {
    void (*callback)(void *);
    void *arg;
    uint32_t period_ms;
    uint32_t last_tick;
} SysTick_Task;

void SysTick_Init(void);
uint32_t SysTick_GetTick(void);

#endif
