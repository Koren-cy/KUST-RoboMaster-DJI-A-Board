/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_trapezoid.h"
#include <string.h>

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化梯形速度跟踪器
* @param tp         跟踪器结构体指针
* @param start_pos  初始位置
* @param v_max      最大速度约束
* @param a_max      最大加速度约束
*/
void Trap_Init(TRAP_Profile* tp, const float start_pos, const float v_max, const float a_max) {
    memset(tp, 0, sizeof(TRAP_Profile));
    tp->v_max = (v_max > TRAP_EPS) ? v_max : TRAP_EPS;
    tp->a_max = (a_max > TRAP_EPS) ? a_max : TRAP_EPS;
    tp->target_pos = start_pos;
    tp->pos        = start_pos;
}

/**
* @brief 刷新目标设定值
* @param tp          跟踪器结构体指针
* @param target_pos  新目标设定值
*/
void Trap_Plan(TRAP_Profile* tp, const float target_pos) {
    tp->target_pos = target_pos;
}

/**
* @brief 按步长推进跟踪器并采样
* @param tp 跟踪器结构体指针
* @param dt 步长 单位：秒
* @return   当前位置设定值
*/
float Trap_Update(TRAP_Profile* tp, const float dt) {
    if (dt < TRAP_EPS) {
        return tp->pos;
    }

    const float a = tp->a_max;
    const float d = tp->target_pos - tp->pos;   /* 剩余位移 */

    float v_cmd = sqrtf(2.0f * a * fabsf(d));
    if (v_cmd > tp->v_max) {
        v_cmd = tp->v_max;
    }
    v_cmd = (d >= 0.0f) ? v_cmd : -v_cmd;

    /* 当前速度按加速度上限逼近期望速度 */
    const float dv_max = a * dt;
    float dv = v_cmd - tp->vel;
    if (dv >  dv_max) dv =  dv_max;
    if (dv < -dv_max) dv = -dv_max;
    tp->vel += dv;

    /* 速度限幅 */
    if (tp->vel >  tp->v_max) tp->vel =  tp->v_max;
    if (tp->vel < -tp->v_max) tp->vel = -tp->v_max;

    /* 积分出位置 */
    const float pos_new = tp->pos + tp->vel * dt;

    /* 抗离散过冲: 若本步越过目标, 吸附到目标并停住 */
    if ((d >= 0.0f && pos_new >= tp->target_pos) ||
        (d <  0.0f && pos_new <= tp->target_pos)) {
        tp->pos = tp->target_pos;
        tp->vel = 0.0f;
        tp->acc = 0.0f;
        return tp->pos;
    }

    tp->acc = dv / dt;
    tp->pos = pos_new;
    return tp->pos;
}
