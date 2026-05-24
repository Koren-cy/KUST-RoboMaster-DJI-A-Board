#ifndef USER_SGDM_H
#define USER_SGDM_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include <math.h>

/* 宏定义 --------------------------------------------------------------------*/
#define SGDM_MAX_PARAMS     (6)       /* 支持的最大参数个数 */
#define SGDM_MAX_SAMPLES    (256)     /* 最大样本数量 */
#define SGDM_MAX_ITERS      (1000)    /* 最大迭代次数 */
#define SGDM_EPSILON        (1e-8f)   /* 收敛阈值 */

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 残差函数类型
* @param x      参数数组指针
* @param param  额外参数数组指针
* @return 残差值
*/
typedef float (*SGDM_ResidualFunc)(const float* x, const float* param);

/**
* @brief SGDM 优化器结构体
* @note  基于随机梯度下降动量法最小化残差平方和
*/
typedef struct {
    uint8_t n;                         /* 参数个数 */
    const float* param;                /* 额外参数数组指针 */
    SGDM_ResidualFunc residual_func;   /* 残差函数指针 */
    float x[SGDM_MAX_PARAMS];          /* 当前参数向量 */
    float v[SGDM_MAX_PARAMS];          /* 动量向量 */
    float grad[SGDM_MAX_PARAMS];       /* 梯度向量 */
    float residual;                    /* 当前残差范数 */
    float residual_sq;                 /* 当前残差平方和 */
    uint16_t iterations;               /* 实际迭代次数 */
    uint8_t converged;                 /* 收敛标志 */
    float lr;                          /* 学习率 */
    float momentum;                    /* 动量系数 */
    float eps;                         /* 收敛阈值 */
    float grad_eps;                    /* 梯度计算步长 */
    uint16_t max_iter;                 /* 最大迭代次数 */
} SGDM_OPTIMIZER;

/* 函数声明 ------------------------------------------------------------------*/

void SGDM_Init(SGDM_OPTIMIZER* optimizer, const uint8_t n, const SGDM_ResidualFunc residual_func);
void SGDM_SetLearningRate(SGDM_OPTIMIZER* optimizer, const float lr);
void SGDM_SetMomentum(SGDM_OPTIMIZER* optimizer, const float momentum);
void SGDM_SetEpsilon(SGDM_OPTIMIZER* optimizer, const float eps);
void SGDM_SetMaxIterations(SGDM_OPTIMIZER* optimizer, const uint16_t max_iter);

uint8_t SGDM_Solve(SGDM_OPTIMIZER* optimizer, const float* param, const float* x0);

const float* SGDM_GetX(const SGDM_OPTIMIZER* optimizer);
float SGDM_GetResidual(const SGDM_OPTIMIZER* optimizer);

#endif // USER_SGDM_H
