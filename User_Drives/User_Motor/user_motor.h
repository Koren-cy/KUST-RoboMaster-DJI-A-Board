#ifndef USER_MOTOR_H
#define USER_MOTOR_H

#define MOTOR_DRIVES_INTERFACE_FUNC \
    void  (*Set_Motor_State)(void* motor, float value);   /* 设置电机的运动状态 */ \
    float (*Get_Motor_Speed)(void* motor);                /* 获取电机转速 单位：rpm */ \
    float (*Get_Motor_Angle)(void* motor);                /* 获取电机多圈角度 单位：度 */ \
    float (*Get_Motor_Current)(void* motor);              /* 获取电机转矩电流 单位：mA */

/* 电机接口结构体 */
typedef struct {
    MOTOR_DRIVES_INTERFACE_FUNC
} MOTOR_INTERFACE;

#endif // USER_MOTOR_H