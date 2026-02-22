/* 包含头文件 ----------------------------------------------------------------*/
#include "./user_JScope_Transmit.h"
#include "../Core/Inc/bsp.h"

/* 私有宏定义 ----------------------------------------------------------------*/
#define JSCOPE_BUFFER_INDEX     1                   // JScope 使用的 RTT 缓冲区索引

/* 函数体 --------------------------------------------------------------------*/

/**
 * @brief 初始化 JScope 传输模块
 * @param htim 定时器句柄，如果为 NULL 则不初始化定时器
 * @note  配置 RTT 上行缓冲区用于 JScope 数据传输
 */
void JScope_Init(TIM_HandleTypeDef *htim) {
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(JSCOPE_BUFFER_INDEX, JSCOPE_BUFFER_NAME, &JScope_RTT_UpBuffer[0], BUFFER_SIZE_UP, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    if (htim != NULL)
        HAL_TIM_Base_Start_IT(htim);
}

/**
 * @brief 发送 JScope 数据
 * @note  通过 RTT 将数据发送到 JScope，通常将这个函数放在定时器中断回调，定时发送数据
 */
void JScope_Transmit(const uint32_t delta_time) {
    jscope_transmit.timestamp += delta_time;
    SEGGER_RTT_Write(JSCOPE_BUFFER_INDEX, &jscope_transmit, sizeof(jscope_transmit));
}
