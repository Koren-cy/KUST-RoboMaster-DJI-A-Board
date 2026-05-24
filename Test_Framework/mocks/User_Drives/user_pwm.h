/* Mock: User_Drives/user_pwm.h - minimal stub */
#ifndef USER_PWM_H
#define USER_PWM_H

#include <stdint.h>

typedef struct {
    void *tim;
    uint32_t channel;
    uint32_t frequency;
} PWM_DRIVES;

void PWM_Init(PWM_DRIVES *pwm, uint32_t freq);
void PWM_SetDuty(PWM_DRIVES *pwm, float duty);

#endif
