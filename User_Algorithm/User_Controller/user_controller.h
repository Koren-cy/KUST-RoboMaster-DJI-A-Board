#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#define CONTROLLER_INTERFACE_FUNC \
    void  (*Set_Target)(void* controller, float target);                              /* 设置控制器目标值 */ \
    float (*Calculate)(void* controller, float main_feedback, float sub_feedback);    /* 计算控制器输出 sub_feedback 参数仅对串级控制器有效 */ \
    float (*Get_Output)(void* controller);                                            /* 获取控制器输出 */

/* 控制器接口结构体 */
typedef struct {
    CONTROLLER_INTERFACE_FUNC
} CONTROLLER_INTERFACE;

#endif // USER_CONTROLLER_H
