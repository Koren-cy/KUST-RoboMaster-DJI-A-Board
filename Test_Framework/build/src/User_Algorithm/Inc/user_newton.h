#ifndef USER_NEWTON_H
#define USER_NEWTON_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include <math.h>

/* 宏定义 --------------------------------------------------------------------*/
#define NEWTON_MAX_DIMS     (6)       /* 支持的最大维度 */
#define NEWTON_MAX_ITERS    (100)     /* 最大迭代次数 */
#define NEWTON_EPSILON      (1e-6f)   /* 收敛阈值 */
#define NEWTON_DERIV_EPS    (1e-5f)   /* 数值微分步长 */

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 方程函数类型
* @param x      未知数数组指针
* @param param  参数数组指针
* @return  方程在 x 处的函数值
*/
typedef float (*NEWTON_Func)(const float* x, const float* param);

/**
* @brief 牛顿迭代法求解器结构体
* @note  支持 N 元方程组
*/
typedef struct {
    uint8_t n;                                  /* 未知数个数 */
    const float* param;                         /* 参数数组指针 */
    uint8_t func_idx;                           /* 已注册方程个数*/
    NEWTON_Func funcs[NEWTON_MAX_DIMS];         /* 方程函数指针数组 */
    float x[NEWTON_MAX_DIMS];                   /* 当前未知数解向量 */
    float f[NEWTON_MAX_DIMS];                   /* 当前方程残差向量 */
    float J[NEWTON_MAX_DIMS * NEWTON_MAX_DIMS]; /* 雅可比矩阵 */
    float residual;                             /* 当前残差范数 */
    uint16_t iterations;                        /* 实际迭代次数 */
    uint8_t converged;                          /* 收敛标志*/
    float eps;                                  /* 收敛阈值 */
    uint16_t max_iter;                          /* 最大迭代次数 */
} NEWTON_SOLVER;

/* 函数声明 ------------------------------------------------------------------*/
void Newton_Init(NEWTON_SOLVER* solver, uint8_t n);
void Newton_SetFunc(NEWTON_SOLVER* solver, NEWTON_Func func);

uint8_t Newton_Solve(NEWTON_SOLVER* solver, const float* param, const float* x0);

const float* Newton_GetX(const NEWTON_SOLVER* solver);
float Newton_GetResidual(const NEWTON_SOLVER* solver);

#endif // USER_NEWTON_H
