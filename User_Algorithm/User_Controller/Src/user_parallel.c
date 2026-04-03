/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_parallel.h"
#include <string.h>

/* 函数体 --------------------------------------------------------------------*/

/**
 * @brief 初始化并连控制器
 * @param parallel        并连控制器结构体指针
 * @param controller_a    并联控制器A指针
 * @param controller_b    并联控制器B指针
 * @param coef_a          控制器A输出混合系数
 * @param coef_b          控制器B输出混合系数
 */
void Parallel_Controller_Init(Parallel_Controller* parallel,
                              CONTROLLER_INTERFACE* controller_a,
                              CONTROLLER_INTERFACE* controller_b,
                              const float coef_a, const float coef_b) {
    memset(parallel, 0, sizeof(Parallel_Controller));

    // 绑定接口函数
    parallel->Set_Target = Parallel_Controller_Set_Target;
    parallel->Calculate = Parallel_Controller_Calculate;
    parallel->Get_Output = Parallel_Controller_Get_Output;

    // 绑定子控制器
    parallel->controller_a = controller_a;
    parallel->controller_b = controller_b;

    // 初始化混合系数
    parallel->coef_a = coef_a;
    parallel->coef_b = coef_b;

    // 初始化状态变量
    parallel->target = 0.0f;
    parallel->output = 0.0f;
    parallel->output_a = 0.0f;
    parallel->output_b = 0.0f;
}

/* 接口函数实现 --------------------------------------------------------------*/

/**
 * @brief 设定并连控制器目标值
 * @param controller 控制器结构体指针
 * @param target     目标值
 * @note 目标值同时传递给两个子控制器
 */
void Parallel_Controller_Set_Target(void* controller, const float target) {
    Parallel_Controller* parallel = (Parallel_Controller*)controller;
    parallel->target = target;
}

/**
 * @brief 计算并连控制器输出
 * @param controller      控制器结构体指针
 * @param feedback_a      控制器A的反馈值（可为任意物理量）
 * @param feedback_b      控制器B的反馈值（可为任意物理量）
 * @return 混合后总输出值 = coef_a * out_a + coef_b * out_b
 */
float Parallel_Controller_Calculate(void* controller, const float feedback_a, const float feedback_b) {
    Parallel_Controller* parallel = (Parallel_Controller*)controller;
    const CONTROLLER_INTERFACE* ctrl_a = (CONTROLLER_INTERFACE*)parallel->controller_a;
    const CONTROLLER_INTERFACE* ctrl_b = (CONTROLLER_INTERFACE*)parallel->controller_b;

    // 目标值同时传递给两个子控制器
    ctrl_a->Set_Target(parallel->controller_a, parallel->target);
    ctrl_b->Set_Target(parallel->controller_b, parallel->target);

    // 独立计算两个控制器输出
    parallel->output_a = ctrl_a->Calculate(parallel->controller_a, feedback_a, 0.0f);
    parallel->output_b = ctrl_b->Calculate(parallel->controller_b, feedback_b, 0.0f);

    // 混合输出：加权求和
    parallel->output = parallel->coef_a * parallel->output_a
                     + parallel->coef_b * parallel->output_b;

    return parallel->output;
}

/**
 * @brief 获取并连控制器最终输出
 * @param controller 控制器结构体指针
 * @return 并连控制器最终混合输出值
 */
float Parallel_Controller_Get_Output(void* controller) {
    const Parallel_Controller* parallel = (Parallel_Controller*)controller;
    return parallel->output;
}
