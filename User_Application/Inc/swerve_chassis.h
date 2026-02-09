#ifndef __SWERVE_CHASSIS_H__
#define __SWERVE_CHASSIS_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "user_dji_motor.h"

/* 常量定义 ------------------------------------------------------------------*/


/* 类型定义 ------------------------------------------------------------------*/

// 单个舵轮状态
typedef struct {
    DJI_MOTOR_DRIVES* wheel_motor; // 驱动轮电机
    float drive_speed_current;     // 驱动轮速度 (m/s)
    float drive_speed_target;      // 驱动轮目标速度 (m/s)
    int8_t reverse;                // 正转为1，反转为-1
    DJI_MOTOR_DRIVES* steer_motor; // 转向轮电机
    float steer_angle_current;     // 当前转向角度 (deg)
    float steer_angle_target;      // 目标转向角度 (deg)
    float steer_angle_offset;      // 转向角度零点偏移 (deg)
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
    DJI_MOTOR_DRIVES* fl_wheel,  DJI_MOTOR_DRIVES* fr_wheel,  DJI_MOTOR_DRIVES* rl_wheel,  DJI_MOTOR_DRIVES* rr_wheel,
    DJI_MOTOR_DRIVES* fl_steer,  DJI_MOTOR_DRIVES* fr_steer,  DJI_MOTOR_DRIVES* rl_steer,  DJI_MOTOR_DRIVES* rr_steer,
    float fl_steer_offset,       float fr_steer_offset,       float rl_steer_offset,       float rr_steer_offset,
    int8_t fl_reverse,           int8_t fr_reverse,           int8_t rl_reverse,           int8_t rr_reverse);

void SwerveChassis_Kinematics(SwerveChassisState* chassis, float vx, float vy, float omega);
void SwerveChassis_Set_Motor_Target(SwerveChassisState* chassis);
void SwerveChassis_InverseKinematics(SwerveChassisState* chassis);

#endif //__SWERVE_CHASSIS_H__
