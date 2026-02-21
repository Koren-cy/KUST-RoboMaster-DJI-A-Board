#ifndef __USER_LADRC_H__
#define __USER_LADRC_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 类型定义 ------------------------------------------------------------------*/

/**
 * @brief 线性扩张状态观测器(LESO)结构体
 */
typedef struct {
    float wc;          /* 观测器带宽 */
    float z1;          /* 状态观测值1 */
    float z2;          /* 状态观测值2 */
    float z3;          /* 扩张状态观测值(总扰动) */
} LESO_Controller;

/**
 * @brief 线性状态误差反馈控制律(LSEF)结构体
 */
typedef struct {
    float kp;          /* 比例增益 */
    float kd;          /* 微分增益 */
} LSEF_Controller;

/**
 * @brief LADRC 控制器结构体
 */
typedef struct {
    LESO_Controller leso;      /* 线性扩张状态观测器 */
    LSEF_Controller lsef;      /* 线性状态误差反馈控制律 */
    
    float b0;                  /* 控制增益补偿 */
    float max_out;             /* 输出限幅 */
    float dt;                  /* 采样时间 */
    
    float set;                 /* 目标值 */
    float fdb;                 /* 反馈值 */
    float out;                 /* 控制输出 */
    float u0;                  /* 线性组合输出 */
} LADRC_Controller;

/* 函数声明 ------------------------------------------------------------------*/
void LADRC_Init(LADRC_Controller *ladrc,
                float wc, float kp, float kd,
                float b0, float max_out, float dt);

void LADRC_SetTarget(LADRC_Controller *ladrc, float target);

float LADRC_Calculate(LADRC_Controller *ladrc, float feedback);

#endif //__USER_LADRC_H__
