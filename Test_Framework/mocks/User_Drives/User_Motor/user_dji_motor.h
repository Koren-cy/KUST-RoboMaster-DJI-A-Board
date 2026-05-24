/* Mock: User_Drives/User_Motor/user_dji_motor.h - minimal stub */
#ifndef USER_DJI_MOTOR_H
#define USER_DJI_MOTOR_H

#include <stdint.h>

typedef enum {
    DJI_MOTOR_2006,
    DJI_MOTOR_3508,
    DJI_MOTOR_6020,
    DJI_MOTOR_6623
} DJI_MotorType;

#define DJI_MOTOR_NUM 8

typedef struct {
    int16_t angle;
    int16_t rpm;
    int16_t current;
    int16_t temp;
} DJI_MotorFeedback;

typedef struct {
    DJI_MotorType type;
    DJI_MotorFeedback fb;
    int16_t target;
} DJI_MOTOR_DRIVES;

void DJI_Motor_Init(DJI_MOTOR_DRIVES *m, DJI_MotorType type);
void DJI_Motor_SetTarget(DJI_MOTOR_DRIVES *m, int16_t target);

#endif
