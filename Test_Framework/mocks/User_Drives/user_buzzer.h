/* Mock: User_Drives/user_buzzer.h - minimal stub */
#ifndef USER_BUZZER_H
#define USER_BUZZER_H

#include <stdint.h>
#include "user_pwm.h"

typedef enum {
    BUZZER_OFF = 0,
    BUZZER_ON = 1
} BUZZER_State;

typedef struct {
    PWM_DRIVES pwm;
    BUZZER_State state;
    uint32_t frequency;
} BUZZER_DRIVES;

void BUZZER_Init(BUZZER_DRIVES *bz, uint32_t freq);
void BUZZER_SetState(BUZZER_DRIVES *bz, BUZZER_State state);

#endif
