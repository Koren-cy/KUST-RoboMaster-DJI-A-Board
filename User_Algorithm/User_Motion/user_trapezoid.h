#ifndef USER_TRAPEZOID_H
#define USER_TRAPEZOID_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include <math.h>

/* 宏定义 --------------------------------------------------------------------*/
#define TRAP_EPS    (1e-6f)     /* 时间/距离 最小有效阈值 */

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 梯形速度曲线规划器结构体
* @note  加速度受限, 速度呈梯形
*/
typedef struct {
    float v_max;            /* 最大速度约束 */
    float a_max;            /* 最大加速度约束 */

    float start_pos;        /* 起点位置 */
    float target_pos;       /* 目标位置 */
    float distance;         /* 带符号位移 */

    float t_acc;            /* 加速段时长 */
    float t_cruise;         /* 匀速段时长 */
    float total_time;       /* 规划总时长 */
    float peak_vel;         /* 实际达到的峰值速度 */

    float elapsed;          /* 内部累计时间 */

    /* 当前采样输出 */
    float pos;              /* 当前位置 */
    float vel;              /* 当前速度 */
    float acc;              /* 当前加速度 */
} TRAP_Profile;

/* 函数声明 ------------------------------------------------------------------*/
void Trap_Init(TRAP_Profile* tp, float start_pos, float v_max, float a_max);
void Trap_Plan(TRAP_Profile* tp, float target_pos);

float Trap_Update(TRAP_Profile* tp, float dt);

#endif // USER_TRAPEZOID_H
