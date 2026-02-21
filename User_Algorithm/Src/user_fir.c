/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_fir.h"

#include <stdlib.h>
#include <string.h>

/* 函数体 --------------------------------------------------------------------*/

/**
 * @brief 初始化FIR滤波器
 * @param fir     FIR滤波器结构体指针
 * @param coeffs  滤波器系数数组指针
 * @param order   滤波器阶数
 */
void FIR_Init(FIR_Filter *fir, const float *coeffs, const uint16_t order) {
    fir->order = order;

    fir->buffer = malloc((order + 1) * sizeof(float));
    fir->coeffs = malloc((order + 1) * sizeof(float));
    fir->state_buffer = malloc((order + 1) * sizeof(float));

    memcpy(fir->coeffs, coeffs, (order + 1) * sizeof(float));

    arm_fir_init_f32(&fir->instance, fir->order + 1, fir->coeffs, fir->state_buffer, 1);
}

/**
 * @brief 更新 FIR 滤波器并计算输出
 * @param fir    FIR 滤波器结构体指针
 * @param input  输入信号值
 * @return 滤波后的输出值
 */
float FIR_Update(FIR_Filter *fir, const float input) {
    float output;

    for (uint16_t i = 0; i < fir->order; i++)
        fir->buffer[i+1] = fir->buffer[i];
    fir->buffer[0] = input;

    arm_fir_f32(&fir->instance, fir->buffer, &output, 1);
    
    return output;
}