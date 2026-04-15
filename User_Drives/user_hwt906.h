#ifndef __USER_HWT906_H__
#define __USER_HWT906_H__
#include "main.h"
#ifdef HAL_UART_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "user_uart.h"
#include "../../Core/Inc/bsp_config.h"

/* 宏定义 --------------------------------------------------------------------*/
#define HWT906_BUF_LEN  11  /* HWT906 数据帧长度 */

/* 类型定义 ------------------------------------------------------------------*/
/**
 * @brief HWT906 数据包类型
 */
typedef enum {
    HWT906_TIME             = 0x50,  /* 时间数据包 */
    HWT906_ACCELERATION     = 0x51,  /* 加速度数据包 */
    HWT906_ANGULAR_VELOCITY = 0x52,  /* 角速度数据包 */
    HWT906_ANGLE            = 0x53,  /* 角度数据包 */
} HWT906_FRAME_TYPE;

/**
 * @brief 加速度数据结构体
 */
typedef struct {
    short acceleration_x;  /* X 轴加速度 */
    short acceleration_y;  /* Y 轴加速度 */
    short acceleration_z;  /* Z 轴加速度 */
    short temperature;     /* 温度 */
} AccelerationData;

/**
 * @brief 角速度数据结构体
 */
typedef struct {
    short angular_velocity_x;  /* X 轴角速度 */
    short angular_velocity_y;  /* Y 轴角速度 */
    short angular_velocity_z;  /* Z 轴角速度 */
    short voltage;             /* 电压 （非蓝牙产品，该数据无效）*/
} AngularVelocityData;

/**
 * @brief 角度数据结构体
 */
typedef struct {
    float angle_x;  /* X 轴角度 */
    float angle_y;  /* Y 轴角度 */
    float angle_z;  /* Z 轴角度 */
    short version;  /* 版本号 */
} AngleData;

/**
 * @brief HWT906 回调函数类型
 * @param user_hwt906 HWT906 驱动结构体指针
 */
typedef void (*HWT906_Callback)(void* user_hwt906);

/**
 * @brief HWT906 驱动结构体
 */
typedef struct {
    UART_DRIVES* user_uart;                         /* 串口驱动结构体指针 */
    AccelerationData user_acceleration;             /* 加速度数据 */
    AngularVelocityData user_angular_velocity;      /* 角速度数据 */
    AngleData user_angle;                           /* 角度数据 */
    uint8_t buf[HWT906_BUF_LEN];                    /* 接收数据缓冲区 */
    HWT906_Callback callbacks[HWT906_CALLBACK_NUM]; /* 回调函数数组 */
    uint8_t callback_num;                           /* 已注册的回调函数数量 */
} HWT906_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void HWT906_Init(HWT906_DRIVES* user_hwt906, UART_DRIVES* user_uart);
void HWT906_RegisterCallback(HWT906_DRIVES* user_hwt906, HWT906_Callback callback);

#endif /* HAL_UART_MODULE_ENABLED */
#endif /* __USER_HWT906_H__ */
