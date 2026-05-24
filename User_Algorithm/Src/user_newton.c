#include "../../Core/Inc/bsp.h"

/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_newton.h"

/* 私有函数声明 --------------------------------------------------------------*/
static void Newton_Evaluate(NEWTON_SOLVER* solver);
static void Newton_ComputeJacobian(NEWTON_SOLVER* solver);

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化牛顿迭代法求解器
* @param solver 求解器结构体指针
* @param n      未知数个数
*/
void Newton_Init(NEWTON_SOLVER* solver, const uint8_t n) {
    solver->n = n;
    solver->eps = NEWTON_EPSILON;
    solver->max_iter = NEWTON_MAX_ITERS;
}

/**
* @brief 注册方程函数
* @param solver 求解器结构体指针
* @param func   方程函数指针
*/
void Newton_SetFunc(NEWTON_SOLVER* solver, const NEWTON_Func func) {
    solver->funcs[solver->func_idx] = func;
    solver->func_idx++;
}

/**
* @brief 求解 N 元非线性方程组
* @param solver 求解器结构体指针
* @param param  参数数组指针
* @param x0     初始解向量
* @return       收敛标志 1: 收敛 0: 未收敛
*/
uint8_t Newton_Solve(NEWTON_SOLVER* solver, const float* param, const float* x0) {
    memcpy(solver->x, x0 , solver->n * sizeof(float));

    solver->param = param;

    float J[NEWTON_MAX_DIMS * NEWTON_MAX_DIMS];

    solver->iterations = 0;
    solver->converged = 0;

    for (uint16_t iter = 0; iter < solver->max_iter; iter++) {
        Newton_Evaluate(solver);

        if (solver->residual < solver->eps) {
            solver->converged = 1;
            break;
        }

        Newton_ComputeJacobian(solver);

        for (uint8_t i = 0; i < solver->n * solver->n; i++)
            J[i] = solver->J[i];

        uint8_t singular = 0;
        for (uint8_t row = 0; row < solver->n; row++) {
            float max_val = 0.0f;
            uint8_t max_row = row;
            for (uint8_t k = row; k < solver->n; k++) {
                float abs_val = fabsf(J[k * solver->n + row]);
                if (abs_val > max_val) {
                    max_val = abs_val;
                    max_row = k;
                }
            }

            if (max_val < 1e-12f) {
                singular = 1;
                break;
            }

            if (max_row != row) {
                for (uint8_t col = 0; col < solver->n; col++) {
                    const float temp = J[row * solver->n + col];
                    J[row * solver->n + col] = J[max_row * solver->n + col];
                    J[max_row * solver->n + col] = temp;
                }
                const float temp_f = solver->f[row];
                solver->f[row] = solver->f[max_row];
                solver->f[max_row] = temp_f;
            }

            const float pivot = J[row * solver->n + row];
            for (int8_t col = (int8_t)(solver->n - 1); col >= row; col--)
                J[row * solver->n + col] /= pivot;
            solver->f[row] /= pivot;

            for (uint8_t k = 0; k < solver->n; k++) {
                if (k == row)
                    continue;
                const float factor = J[k * solver->n + row];
                if (fabsf(factor) < 1e-12f)
                    continue;
                for (int8_t col = (int8_t)(solver->n - 1); col >= row; col--)
                    J[k * solver->n + col] -= factor * J[row * solver->n + col];
                solver->f[k] -= factor * solver->f[row];
            }
        }

        if (singular)
            break;

        for (uint8_t i = 0; i < solver->n; i++)
            solver->x[i] -= solver->f[i];

        solver->iterations++;
    }

    Newton_Evaluate(solver);
    return solver->converged;
}

/**
* @brief 计算方程残差向量及残差范数
* @param solver 求解器结构体指针
*/
static void Newton_Evaluate(NEWTON_SOLVER* solver) {
    solver->residual = 0.0f;
    for (uint8_t i = 0; i < solver->n; i++) {
        solver->f[i] = solver->funcs[i](solver->x, solver->param);
        solver->residual += solver->f[i] * solver->f[i];
    }
    solver->residual = sqrtf(solver->residual);
}

/**
* @brief 数值微分法计算雅可比矩阵
* @param solver 求解器结构体指针
* @note  采用中心差分法，精度较高
*/
static void Newton_ComputeJacobian(NEWTON_SOLVER* solver) {
    float x_backup[NEWTON_MAX_DIMS];
    float f_plus[NEWTON_MAX_DIMS];
    float f_minus[NEWTON_MAX_DIMS];

    for (uint8_t i = 0; i < solver->n; i++)
        x_backup[i] = solver->x[i];

    for (uint8_t j = 0; j < solver->n; j++) {
        /* 计算 f(x + h*e_j) */
        for (uint8_t i = 0; i < solver->n; i++)
            solver->x[i] = x_backup[i];
        solver->x[j] += NEWTON_DERIV_EPS;
        for (uint8_t i = 0; i < solver->n; i++)
            f_plus[i] = solver->funcs[i](solver->x, solver->param);

        /* 计算 f(x - h*e_j) */
        for (uint8_t i = 0; i < solver->n; i++)
            solver->x[i] = x_backup[i];
        solver->x[j] -= NEWTON_DERIV_EPS;
        for (uint8_t i = 0; i < solver->n; i++)
            f_minus[i] = solver->funcs[i](solver->x, solver->param);

        /* 计算雅可比矩阵第 j 列 */
        for (uint8_t i = 0; i < solver->n; i++)
            solver->J[i * solver->n + j] = (f_plus[i] - f_minus[i]) / (2.0f * NEWTON_DERIV_EPS);
    }

    for (uint8_t i = 0; i < solver->n; i++)
        solver->x[i] = x_backup[i];
}

/**
* @brief 获取求解结果
* @param solver 求解器结构体指针
* @return 解向量指针
*/
const float* Newton_GetX(const NEWTON_SOLVER* solver) {
    return solver->x;
}

/**
* @brief 获取最终残差范数
* @param solver 求解器结构体指针
* @return 残差范数
*/
float Newton_GetResidual(const NEWTON_SOLVER* solver) {
    return solver->residual;
}
