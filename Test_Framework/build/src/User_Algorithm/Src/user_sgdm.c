#include "../../Core/Inc/bsp.h"

/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_sgdm.h"

/* 私有函数声明 --------------------------------------------------------------*/
static void SGDM_ComputeResidual(SGDM_OPTIMIZER* optimizer);
static void SGDM_ComputeGradient(SGDM_OPTIMIZER* optimizer);
static void SGDM_UpdateParams(SGDM_OPTIMIZER* optimizer);

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 SGDM 优化器
* @param optimizer    优化器结构体指针
* @param n            参数个数
* @param residual_func 残差函数指针
*/
void SGDM_Init(SGDM_OPTIMIZER* optimizer, const uint8_t n, const SGDM_ResidualFunc residual_func) {
    optimizer->n = n;
    optimizer->residual_func = residual_func;
    optimizer->iterations = 0;
    optimizer->converged = 0;
    optimizer->residual = 0.0f;
    optimizer->residual_sq = 0.0f;
    optimizer->lr = 0.01f;
    optimizer->momentum = 0.9f;
    optimizer->eps = SGDM_EPSILON;
    optimizer->grad_eps = 1e-5f;
    optimizer->max_iter = SGDM_MAX_ITERS;
    optimizer->param = NULL;

    for (uint8_t i = 0; i < SGDM_MAX_PARAMS; i++) {
        optimizer->x[i] = 0.0f;
        optimizer->v[i] = 0.0f;
        optimizer->grad[i] = 0.0f;
    }
}

/**
* @brief 设置学习率
* @param optimizer 优化器结构体指针
* @param lr        学习率
*/
void SGDM_SetLearningRate(SGDM_OPTIMIZER* optimizer, const float lr) {
    optimizer->lr = lr;
}

/**
* @brief 设置动量系数
* @param optimizer 优化器结构体指针
* @param momentum  动量系数 (建议范围 0.5 ~ 0.99)
*/
void SGDM_SetMomentum(SGDM_OPTIMIZER* optimizer, const float momentum) {
    optimizer->momentum = momentum;
}

/**
* @brief 设置收敛阈值
* @param optimizer 优化器结构体指针
* @param eps       收敛阈值
*/
void SGDM_SetEpsilon(SGDM_OPTIMIZER* optimizer, const float eps) {
    optimizer->eps = eps;
}

/**
* @brief 设置最大迭代次数
* @param optimizer 优化器结构体指针
* @param max_iter  最大迭代次数
*/
void SGDM_SetMaxIterations(SGDM_OPTIMIZER* optimizer, const uint16_t max_iter) {
    optimizer->max_iter = max_iter;
}

/**
* @brief 执行优化求解
* @param optimizer 优化器结构体指针
* @param param     额外参数数组指针
* @param x0        参数初始值数组
* @return          收敛标志 1: 收敛 0: 未收敛
*/
uint8_t SGDM_Solve(SGDM_OPTIMIZER* optimizer, const float* param, const float* x0) {
    if (optimizer->n == 0 || optimizer->residual_func == NULL)
        return 0;

    for (uint8_t i = 0; i < optimizer->n; i++)
        optimizer->x[i] = x0[i];

    optimizer->param = param;

    optimizer->iterations = 0;
    optimizer->converged = 0;

    for (uint16_t iter = 0; iter < optimizer->max_iter; iter++) {
        SGDM_ComputeResidual(optimizer);

        if (optimizer->residual_sq < optimizer->eps) {
            optimizer->converged = 1;
            break;
        }

        SGDM_ComputeGradient(optimizer);
        SGDM_UpdateParams(optimizer);

        optimizer->iterations++;
    }

    SGDM_ComputeResidual(optimizer);
    return optimizer->converged;
}

/**
* @brief 计算残差平方和
* @param optimizer 优化器结构体指针
*/
static void SGDM_ComputeResidual(SGDM_OPTIMIZER* optimizer) {
    optimizer->residual_sq = 0.0f;
    const float r = optimizer->residual_func(optimizer->x, optimizer->param);
    optimizer->residual_sq += r * r;
    optimizer->residual = sqrtf(optimizer->residual_sq);
}

/**
* @brief 数值微分法计算梯度
* @param optimizer 优化器结构体指针
* @note  采用中心差分法，精度较高
*/
static void SGDM_ComputeGradient(SGDM_OPTIMIZER* optimizer) {
    float x_backup[SGDM_MAX_PARAMS];
    float r_plus, r_minus;

    for (uint8_t i = 0; i < optimizer->n; i++)
        x_backup[i] = optimizer->x[i];

    for (uint8_t j = 0; j < optimizer->n; j++) {
        for (uint8_t i = 0; i < optimizer->n; i++)
            optimizer->x[i] = x_backup[i];
        optimizer->x[j] += optimizer->grad_eps;
        r_plus = optimizer->residual_func(optimizer->x, optimizer->param);

        for (uint8_t i = 0; i < optimizer->n; i++)
            optimizer->x[i] = x_backup[i];
        optimizer->x[j] -= optimizer->grad_eps;
        r_minus = optimizer->residual_func(optimizer->x, optimizer->param);

        optimizer->grad[j] = (r_plus * r_plus - r_minus * r_minus) / (2.0f * optimizer->grad_eps);
    }

    for (uint8_t i = 0; i < optimizer->n; i++)
        optimizer->x[i] = x_backup[i];
}

/**
* @brief 使用动量法更新参数
* @param optimizer 优化器结构体指针
*/
static void SGDM_UpdateParams(SGDM_OPTIMIZER* optimizer) {
    for (uint8_t i = 0; i < optimizer->n; i++) {
        optimizer->v[i] = optimizer->momentum * optimizer->v[i] - optimizer->lr * optimizer->grad[i];
        optimizer->x[i] += optimizer->v[i];
    }
}

/**
* @brief 获取优化结果
* @param optimizer 优化器结构体指针
* @return          最优参数向量指针
*/
const float* SGDM_GetX(const SGDM_OPTIMIZER* optimizer) {
    return optimizer->x;
}

/**
* @brief 获取最终残差范数
* @param optimizer 优化器结构体指针
* @return          残差范数 (残差平方和开根号)
*/
float SGDM_GetResidual(const SGDM_OPTIMIZER* optimizer) {
    return optimizer->residual;
}