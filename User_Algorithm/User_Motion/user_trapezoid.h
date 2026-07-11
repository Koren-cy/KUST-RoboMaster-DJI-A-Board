#ifndef USER_TRAPEZOID_H
#define USER_TRAPEZOID_H

/* 包含头文件 ----------------------------------------------------------------*/
#include <math.h>

/* 宏定义 --------------------------------------------------------------------*/
#define TRAP_EPS    (1e-6f)     /* 距离/步长 最小有效阈值 */

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 梯形速度曲线跟踪器结构体
*/
typedef struct {
    float v_max;            /* 最大速度约束 */
    float a_max;            /* 最大加速度约束 */

    float target_pos;       /* 目标设定值 */

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
