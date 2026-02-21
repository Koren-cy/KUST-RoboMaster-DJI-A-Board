#ifndef __USER_FIR_H__
#define __USER_FIR_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "../../Drivers/CMSIS/DSP/Include/arm_math.h"

/* 宏定义 --------------------------------------------------------------------*/

/* 类型定义 ------------------------------------------------------------------*/
/**
 * @brief FIR滤波器结构体
 */
typedef struct {
    arm_fir_instance_f32 instance;     /* ARM DSP 库 FIR 滤波器实例 */
    float *state_buffer;               /* 滤波器状态缓冲区 */
    float *coeffs;                     /* 滤波器系数数组 */
    float *buffer;                     /* 数据缓冲区 */
    uint16_t order;                    /* 滤波器阶数 */
} FIR_Filter;

/* 函数声明 ------------------------------------------------------------------*/
void FIR_Init(FIR_Filter *fir, const float *coeffs, uint16_t order);
float FIR_Update(FIR_Filter *fir, float input);

#endif //__USER_FIR_H__
