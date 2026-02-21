#ifndef __USER_ADRC_H__
#define __USER_ADRC_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 类型定义 ------------------------------------------------------------------*/

/**
 * @brief 跟踪微分器(TD)结构体
 */
typedef struct {
    float r;           /* 快速跟踪因子 */
    float h;           /* 滤波因子 */
    float x1;          /* 跟踪信号 */
    float x2;          /* 跟踪信号微分 */
} TD_Controller;

/**
 * @brief 扩张状态观测器(ESO)结构体
 */
typedef struct {
    float beta1;       /* 观测器增益1 */
    float beta2;       /* 观测器增益2 */
    float beta3;       /* 观测器增益3 */
    float delta;       /* 线性段宽度 */
    float z1;          /* 状态观测值1 */
    float z2;          /* 状态观测值2 */
    float z3;          /* 扩张状态观测值(总扰动) */
} ESO_Controller;

/**
 * @brief 非线性状态误差反馈控制律(NLSEF)结构体
 */
typedef struct {
    float beta1;       /* 非线性增益1 */
    float beta2;       /* 非线性增益2 */
    float alpha1;      /* 非线性因子1 */
    float alpha2;      /* 非线性因子2 */
    float delta;       /* 线性段宽度 */
} NLSEF_Controller;

/**
 * @brief ADRC 控制器结构体
 */
typedef struct {
    TD_Controller td;          /* 跟踪微分器 */
    ESO_Controller eso;        /* 扩张状态观测器 */
    NLSEF_Controller nlsef;    /* 非线性状态误差反馈控制律 */
    
    float b0;                  /* 控制增益补偿 */
    float max_out;             /* 输出限幅 */
    float dt;                  /* 采样时间 */
    
    float set;                 /* 目标值 */
    float fdb;                 /* 反馈值 */
    float out;                 /* 控制输出 */
    float u0;                  /* 非线性组合输出 */
} ADRC_Controller;

/* 函数声明 ------------------------------------------------------------------*/
void ADRC_Init(ADRC_Controller *adrc,
               float r, float h,
               float beta1_eso, float beta2_eso, float beta3_eso, float delta_eso,
               float beta1_nlsef, float beta2_nlsef, float alpha1, float alpha2, float delta_nlsef,
               float b0, float max_out, float dt);

void ADRC_SetTarget(ADRC_Controller *adrc, float target);

float ADRC_Calculate(ADRC_Controller *adrc, float feedback);

#endif //__USER_ADRC_H__
