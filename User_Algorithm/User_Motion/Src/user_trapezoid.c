#include "../../Core/Inc/bsp.h"

/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_trapezoid.h"
#include <string.h>

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化梯形速度曲线规划器
* @param tp         规划器结构体指针
* @param start_pos  初始位置
* @param v_max      最大速度约束
* @param a_max      最大加速度约束
*/
void Trap_Init(TRAP_Profile* tp, const float start_pos, const float v_max, const float a_max) {
    memset(tp, 0, sizeof(TRAP_Profile));
    tp->v_max = (v_max > TRAP_EPS) ? v_max : TRAP_EPS;
    tp->a_max = (a_max > TRAP_EPS) ? a_max : TRAP_EPS;
    tp->start_pos  = start_pos;
    tp->target_pos = start_pos;
    tp->pos        = start_pos;
}

/**
* @brief 规划到目标位置的梯形速度曲线
* @param tp          规划器结构体指针
* @param target_pos  目标位置
* @return            1: 规划成功 0: 位移过小
*/
void Trap_Plan(TRAP_Profile* tp, const float target_pos) {
    tp->start_pos  = tp->pos;
    tp->target_pos = target_pos;
    tp->elapsed    = 0.0f;

    tp->distance = target_pos - tp->start_pos;

    /* 位移过小: 直接运动到目标处 */
    if (fabsf(tp->distance) < TRAP_EPS) {
        tp->t_acc = 0.0f;
        tp->t_cruise = 0.0f;
        tp->total_time = 0.0f;
        tp->peak_vel = 0.0f;
        tp->pos = target_pos;
        tp->vel = 0.0f;
        tp->acc = 0.0f;
        return;
    }

    const float D = fabsf(tp->distance);
    const float a = tp->a_max;

    /* 加速到 v_max 再减速所需的最短位移 */
    const float d_min = tp->v_max * tp->v_max / a;

    if (D >= d_min) {
        /* 存在匀速段, 峰值速度为 v_max */
        tp->peak_vel = tp->v_max;
        tp->t_cruise = (D - d_min) / tp->peak_vel;
    } else {
        /* 三角形速度曲线, 达不到 v_max */
        tp->peak_vel = sqrtf(D * a);
        tp->t_cruise = 0.0f;
    }
    tp->t_acc = tp->peak_vel / a;
    tp->total_time = 2.0f * tp->t_acc + tp->t_cruise;

    /* 采样到规划起点状态 */
    tp->pos = tp->start_pos;
    tp->vel = 0.0f;
    tp->acc = 0.0f;
}

/**
* @brief 按步长推进并采样曲线
* @param tp 规划器结构体指针
* @param dt 步长 单位：秒
* @return   当前位置设定值
*/
float Trap_Update(TRAP_Profile* tp, const float dt) {
    const float t = tp->elapsed + dt;
    tp->elapsed = t;

    if (tp->total_time < TRAP_EPS) {
        tp->pos = tp->target_pos;
        tp->vel = 0.0f;
        tp->acc = 0.0f;
        return tp->pos;
    }

    if (t <= 0.0f) {
        tp->pos = tp->start_pos;
        tp->vel = 0.0f;
        tp->acc = 0.0f;
        return tp->pos;
    }

    if (t >= tp->total_time) {
        tp->pos = tp->target_pos;
        tp->vel = 0.0f;
        tp->acc = 0.0f;
        return tp->pos;
    }

    const float a  = tp->a_max;
    const float vp = tp->peak_vel;
    float p, v, ac;

    if (t < tp->t_acc) {
        /* 匀加速段 */
        ac = a;
        v  = a * t;
        p  = 0.5f * a * t * t;
    } else if (t < tp->t_acc + tp->t_cruise) {
        /* 匀速段 */
        const float td = t - tp->t_acc;
        ac = 0.0f;
        v  = vp;
        p  = 0.5f * a * tp->t_acc * tp->t_acc + vp * td;
    } else {
        /* 匀减速段 */
        const float td = t - tp->t_acc - tp->t_cruise;
        ac = -a;
        v  = vp - a * td;
        const float p_acc = 0.5f * a * tp->t_acc * tp->t_acc;
        const float p_cruise = vp * tp->t_cruise;
        p  = p_acc + p_cruise + vp * td - 0.5f * a * td * td;
    }

    /* 叠加方向与起点偏移 */
    const float dir = (tp->distance >= 0.0f) ? 1.0f : -1.0f;
    tp->pos = tp->start_pos + dir * p;
    tp->vel = dir * v;
    tp->acc = dir * ac;
    return tp->pos;
}
