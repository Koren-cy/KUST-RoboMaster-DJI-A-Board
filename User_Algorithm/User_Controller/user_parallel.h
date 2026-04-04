#ifndef USER_PARALLEL_H
#define USER_PARALLEL_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "user_controller.h"

/* 类型定义 ------------------------------------------------------------------*/

/**
 * @brief 并连控制器结构体
 */
typedef struct {
    CONTROLLER_INTERFACE_FUNC

    CONTROLLER_INTERFACE* controller_a;      /* 并联控制器A指针 */
    CONTROLLER_INTERFACE* controller_b;       /* 并联控制器B指针 */

    float coef_a;                             /* 控制器A输出混合系数 */
    float coef_b;                             /* 控制器B输出混合系数 */
    float max_out;                            /* 最大输出限幅 */

    float target;                             /* 目标值 */
    float output;                             /* 最终混合输出 */
    float output_a;                           /* 控制器A独立输出 */
    float output_b;                           /* 控制器B独立输出*/
} Parallel_Controller;

/* 函数声明 ------------------------------------------------------------------*/

void Parallel_Controller_Init(Parallel_Controller* parallel,
                              CONTROLLER_INTERFACE* controller_a,
                              CONTROLLER_INTERFACE* controller_b,
                              float coef_a, float coef_b, float max_out);

/* 接口函数声明 --------------------------------------------------------------*/

void Parallel_Controller_Set_Target(void* controller, float target);
float Parallel_Controller_Calculate(void* controller, float feedback_a, float feedback_b);
float Parallel_Controller_Get_Output(void* controller);
void Parallel_Controller_Set_MaxOut(void* controller, float max_out);
float Parallel_Controller_Get_MaxOut(void* controller);


#endif // USER_PARALLEL_H
