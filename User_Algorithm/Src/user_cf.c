/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_cf.h"
#include <string.h>
#include <math.h>

/* 函数体 --------------------------------------------------------------------*/

/**
 * @brief 根据时间常数初始化互补滤波器
 * @param filter         互补滤波器结构体指针
 * @param time_constant  时间常数，决定低通滤波器的截止频率 单位：秒
 * @param dt             采样时间间隔 单位：秒
 * @note 时间常数越大，signal1的权重越大，滤波器响应越慢
 */
void CF_Init(CF_Filter *filter, const float time_constant, const float dt) {
    memset(filter, 0, sizeof(CF_Filter));

    filter->alpha = time_constant / (time_constant + dt);
    filter->dt = dt;
    filter->initialized = 0;
}

/**
 * @brief 更新互补滤波器并计算输出
 * @param filter   互补滤波器结构体指针
 * @param signal1  输入信号 1  通常为低频特性好的信号
 * @param signal2  输入信号 2  通常为高频特性好的信号
 * @return 融合后的输出值
 */
float CF_Update(CF_Filter *filter, const float signal1, const float signal2) {
    if (!filter->initialized) {
        filter->output = signal1;
        filter->prev_signal1 = signal1;
        filter->prev_signal2 = signal2;
        filter->initialized = 1;
        return filter->output;
    }
    
    filter->output = filter->alpha
                   * (filter->output + signal2 - filter->prev_signal2)
                   + (1.0f - filter->alpha) * signal1;
    
    filter->prev_signal1 = signal1;
    filter->prev_signal2 = signal2;
    
    return filter->output;
}
