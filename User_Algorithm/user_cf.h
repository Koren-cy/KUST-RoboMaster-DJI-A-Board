#ifndef USER_CF_H
#define USER_CF_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
 * @brief 互补滤波器结构体
 */
typedef struct {
    float alpha;                       /* 互补滤波系数 0 ~ 1 */
    float output;                      /* 滤波器输出值 */
    float prev_signal1;                /* 上一次 signal1 的值 */
    float prev_signal2;                /* 上一次 signal2 的值 */
    float dt;                          /* 采样时间间隔 单位：秒 */
    uint8_t initialized;               /* 初次采集标志 */
} CF_Filter;

/* 函数声明 ------------------------------------------------------------------*/
void CF_Init(CF_Filter *filter, float time_constant, float dt);
float CF_Update(CF_Filter *filter, float signal1, float signal2);

#endif // USER_CF_H
