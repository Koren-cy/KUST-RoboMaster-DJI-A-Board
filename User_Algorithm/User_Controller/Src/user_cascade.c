/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_cascade.h"
#include <string.h>

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化通用串级控制器
* @param cascade           串级控制器结构体指针
* @param outer_controller  外环控制器指针
* @param inner_controller  内环控制器指针
*/
void Cascade_Controller_Init(Cascade_Controller *cascade,
                             CONTROLLER_INTERFACE* outer_controller,
                             CONTROLLER_INTERFACE* inner_controller) {
    memset(cascade, 0, sizeof(Cascade_Controller));
    
    // 绑定接口函数
    cascade->Set_Target = Cascade_Controller_Set_Target;
    cascade->Calculate = Cascade_Controller_Calculate;
    cascade->Get_Output = Cascade_Controller_Get_Output;
    
    // 绑定内外环控制器
    cascade->outer_controller = outer_controller;
    cascade->inner_controller = inner_controller;
    
    // 初始化状态变量
    cascade->target = 0.0f;
    cascade->output = 0.0f;
}

/* 接口函数实现 --------------------------------------------------------------*/

/**
* @brief 设定串级控制器外环目标值
* @param controller 控制器结构体指针
* @param target     外环目标值
*/
void Cascade_Controller_Set_Target(void* controller, const float target) {
    Cascade_Controller* cascade = (Cascade_Controller*)controller;
    cascade->target = target;
}

/**
* @brief 计算串级控制器输出
* @param controller      控制器结构体指针
* @param outer_feedback  外环反馈值
* @param inner_feedback  内环反馈值
* @return 串级控制器输出值
* @note 外环控制器计算，输出作为内环目标值
*       内环控制器计算，输出作为最终控制量
*/
float Cascade_Controller_Calculate(void* controller, const float outer_feedback, const float inner_feedback) {
    Cascade_Controller* cascade = (Cascade_Controller*)controller;
    const CONTROLLER_INTERFACE* outer = (CONTROLLER_INTERFACE*)cascade->outer_controller;
    const CONTROLLER_INTERFACE* inner = (CONTROLLER_INTERFACE*)cascade->inner_controller;
    
    // 设置外环目标值
    outer->Set_Target(cascade->outer_controller, cascade->target);
    
    // 计算外环输出，作为内环目标值
    const float outer_output = outer->Calculate(cascade->outer_controller, outer_feedback, 0.0f);
    
    // 设置内环目标值为外环输出
    inner->Set_Target(cascade->inner_controller, outer_output);
    
    // 计算内环输出
    cascade->output = inner->Calculate(cascade->inner_controller, inner_feedback, 0.0f);
    
    return cascade->output;
}

/**
* @brief 获取串级控制器输出
* @param controller 控制器结构体指针
* @return 串级控制器输出值
*/
float Cascade_Controller_Get_Output(void* controller) {
    const Cascade_Controller* cascade = (Cascade_Controller*)controller;
    return cascade->output;
}
