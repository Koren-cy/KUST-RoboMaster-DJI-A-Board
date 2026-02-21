/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_adc.h"
#include "../../Core/Inc/bsp.h"

/* 私有变量 ------------------------------------------------------------------*/
static ADC_DRIVES *adc_drives[ADC_NUM];
static uint8_t adc_num = 0;

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 ADC 驱动
* @param user_adc    ADC 驱动结构体指针
* @param hadc        ADC 硬件句柄
* @param channel_num 使用的通道数量
* @param vref_rank   参考电压通道的规则组序号，如果填 0，则使用 3.3 V 作为参考电压
* @param resolution  ADC 分辨率 单位：bit
* @param callback    转换完成回调函数，如果填 NULL，则不执行回调
* @note ADC 使能扫描模式，使能连续转换模式，使能连续请求，配置规则组，开启 DMA 并开启循环模式
*/
void ADC_Init(ADC_DRIVES* user_adc, ADC_HandleTypeDef* hadc, const uint8_t channel_num, const uint8_t vref_rank, const uint8_t resolution, const ADC_Callback callback) {
    user_adc->hadc = hadc;
    user_adc->channel_num = channel_num;
    user_adc->vref_rank = vref_rank;
    user_adc->resolution = resolution;
    user_adc->callback = callback;

    HAL_ADC_Start_DMA(user_adc->hadc, (uint32_t*)user_adc->adc_value, user_adc->channel_num);

    adc_drives[adc_num] = user_adc;
    adc_num++;
}

/**
* @brief 获取指定通道的电压值
* @param user_adc      ADC 驱动结构体指针
* @param channel_rank  通道索引，对应规则组的序号，从 1 开始
* @return 电压值 单位：V
*/
float ADC_GetVoltage(const ADC_DRIVES* user_adc, const uint8_t channel_rank) {
    if (user_adc->vref_rank)
        return (float)user_adc->adc_value[channel_rank - 1] * 1.2f / (float)user_adc->adc_value[user_adc->vref_rank - 1];
    else
        return (float)user_adc->adc_value[channel_rank - 1] * 3.3f / (float)(1 << user_adc->resolution);
}

/* 覆写中断回调函数 -----------------------------------------------------------*/

/**
* @brief ADC 转换完成回调函数
* @param hadc ADC 硬件句柄
*/
// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
//     for (uint8_t adc_index = 0; adc_index < adc_num; adc_index++) {
//         ADC_DRIVES* adc_drive = adc_drives[adc_index];
//
//         if (hadc != adc_drive->hadc)
//             continue;
//
//         if (adc_drive->callback != NULL) {
//             adc_drive->callback(adc_drive);
//         }
//     }
// }
