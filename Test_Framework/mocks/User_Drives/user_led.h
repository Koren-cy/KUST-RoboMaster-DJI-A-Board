/* Mock: User_Drives/user_led.h - minimal stub */
#ifndef USER_LED_H
#define USER_LED_H

#include <stdint.h>

typedef enum {
    LED_OFF = 0,
    LED_ON = 1
} LED_State;

typedef struct {
    void *GPIO;
    uint16_t pin;
    uint8_t is_reversal;
} LED_DRIVES;

void LED_Init(LED_DRIVES *led);
void LED_SetState(LED_DRIVES *led, LED_State state);

#endif
