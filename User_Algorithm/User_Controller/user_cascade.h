#ifndef USER_CASCADE_H
#define USER_CASCADE_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "user_controller.h"

/* 类型定义 ------------------------------------------------------------------*/

/**
* @brief 通用串级控制器结构体
*/
typedef struct {
    CONTROLLER_INTERFACE_FUNC
    
    CONTROLLER_INTERFACE* outer_controller;   /* 外环控制器指针 */
    CONTROLLER_INTERFACE* inner_controller;   /* 内环控制器指针 */
    
    float target;                             /* 外环目标值 */
    float output;                             /* 最终输出值 */
} Cascade_Controller;

/* 函数声明 ------------------------------------------------------------------*/
void Cascade_Controller_Init(Cascade_Controller *cascade,
                             CONTROLLER_INTERFACE* outer_controller,
                             CONTROLLER_INTERFACE* inner_controller);

/* 接口函数声明 --------------------------------------------------------------*/
void Cascade_Controller_Set_Target(void* controller, float target);
float Cascade_Controller_Calculate(void* controller, float outer_feedback, float inner_feedback);
float Cascade_Controller_Get_Output(void* controller);

#endif // USER_CASCADE_H
