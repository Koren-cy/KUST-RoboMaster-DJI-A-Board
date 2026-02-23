#ifndef __SWERVE_CHASSIS_H__
#define __SWERVE_CHASSIS_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../User_Drives/User_Motor/user_motor.h"

/* 常量定义 ------------------------------------------------------------------*/


/* 类型定义 ------------------------------------------------------------------*/

// 单个舵轮状态
typedef struct {
    MOTOR_INTERFACE* wheel_motor;     // 驱动轮电机
    float drive_speed_current;     // 驱动轮速度 (m/s)
    float drive_speed_target;      // 驱动轮目标速度 (m/s)
    int8_t reverse;                // 正转为1，反转为-1
    MOTOR_INTERFACE* steer_motor;     // 转向轮电机
    float steer_angle_current;     // 当前转向角度 (deg)
    float steer_angle_target;      // 目标转向角度 (deg)
} SwerveWheel;

// 舵轮底盘状态
typedef struct {
    float vx_target;
    float vy_target;
    float omega_target;
    float vx_current;
    float vy_current;
    float omega_current;
    float wheelbase_radius;    // 轮子到底盘中心的距离 (m)
    float wheel_radius;        // 轮子的半径 (m)
    float ratio;               // 轮电机减速比
    SwerveWheel wheel_fl;
    SwerveWheel wheel_fr;
    SwerveWheel wheel_rl;
    SwerveWheel wheel_rr;
} SwerveChassisState;

/* 函数声明 ------------------------------------------------------------------*/
void SwerveChassis_Init(SwerveChassisState* chassis, float wheelbase_radius, float wheel_radius, float ratio,
    MOTOR_INTERFACE* fl_wheel,  MOTOR_INTERFACE* fr_wheel,  MOTOR_INTERFACE* rl_wheel,  MOTOR_INTERFACE* rr_wheel,
    MOTOR_INTERFACE* fl_steer,  MOTOR_INTERFACE* fr_steer,  MOTOR_INTERFACE* rl_steer,  MOTOR_INTERFACE* rr_steer,
    int8_t fl_reverse,       int8_t fr_reverse,       int8_t rl_reverse,       int8_t rr_reverse);

void SwerveChassis_Kinematics(SwerveChassisState* chassis, float vx, float vy, float omega);
void SwerveChassis_Set_Motor_Target(SwerveChassisState* chassis);
void SwerveChassis_InverseKinematics(SwerveChassisState* chassis);

#endif //__SWERVE_CHASSIS_H__
