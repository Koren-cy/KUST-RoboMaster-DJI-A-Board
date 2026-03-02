/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_systick.h"

/* 私有变量定义 --------------------------------------------------------------*/
SysTick_Callback systick_callback[MAX_SYSTICK_CALLBACK] = {0};
uint8_t systick_callback_num = 0;

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 注册 SysTick 回调函数
* @param callback 回调函数指针
* @note  回调函数会在每次 SysTick 中断时被调用
*/
void SysTick_RegisterCallback(const SysTick_Callback callback) {
    systick_callback[systick_callback_num] = callback;
    systick_callback_num++;
}

/**
* @brief 注销 SysTick 回调函数
* @param callback 要注销的回调函数指针
* @note  从回调列表中移除指定的回调函数
*/
void SysTick_UnregisterCallback(const SysTick_Callback callback) {
    for (uint8_t i = 0; i < systick_callback_num; i++) {
        if (systick_callback[i] == callback) {
            systick_callback[i] = systick_callback[systick_callback_num - 1];
            systick_callback[systick_callback_num - 1] = NULL;
            systick_callback_num--;
            return;
        }
    }
}

/**
* @brief 处理所有已注册的 SysTick 回调函数
* @note  应在 SysTick_Handler 中断中调用
*/
void SysTick_Handle(void) {
    for (uint8_t i = 0; i < systick_callback_num; i++) {
        if (systick_callback[i] != NULL) {
            systick_callback[i]();
        }
    }
}
