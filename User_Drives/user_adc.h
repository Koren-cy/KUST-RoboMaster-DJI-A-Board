#ifndef __USER_ADC_H__
#define __USER_ADC_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "../../Core/Inc/bsp_config.h"

/* 类型定义 ------------------------------------------------------------------*/

/**
* @brief ADC 转换完成回调函数类型
* @param user_adc ADC 驱动结构体指针
*/
typedef void (*ADC_Callback)(void* user_adc);

/**
* @brief ADC 驱动结构体
*/
typedef struct {
    ADC_HandleTypeDef* hadc;              /* ADC 硬件句柄 */
    uint8_t channel_num;                  /* 使用的通道数量 */
    uint16_t adc_value[ADC_CHANNEL_MAX];  /* ADC 原始值缓冲区 */
    uint8_t vref_rank;                    /* 参考电压通道的规则组序号 */
    uint8_t resolution;                   /* ADC 精度 单位：bit */
    ADC_Callback callback;                /* 转换完成回调函数 */
} ADC_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void ADC_Init(ADC_DRIVES* user_adc, ADC_HandleTypeDef* hadc, uint8_t channel_num, uint8_t vref_rank, uint8_t resolution, ADC_Callback callback);
float ADC_GetVoltage(const ADC_DRIVES* user_adc, uint8_t channel_rank);

#endif //__USER_ADC_H__
