/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp.h"
#include "bsp_config.h"
#include <stdio.h>

/* 全局注册表 --------------------------------------------------------------*/
void (*loop_event[MAX_LOOP_EVENT])(void) = {0};
uint8_t loop_event_num = 0;

void LOOP_EVENT_Handle(void) {
    for (uint8_t event_index = 0 ; event_index < loop_event_num ; event_index++) {
        loop_event[event_index]();
    }
}

/* 接口定义 --------------------------------------------------------------------*/








