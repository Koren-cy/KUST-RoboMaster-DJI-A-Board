#ifndef DJI_A_BOARD_USER_HWT906_H
#define DJI_A_BOARD_USER_HWT906_H

//包含头文件

#include "main.h"
#include"../../User_Drives/user_uart.h"

/* 常量定义 ------------------------------------------------------------------*/
#define HWT906_BUFFLEN  (11)  /* HWT906 数据帧长度 */


/* 类型定义 ------------------------------------------------------------------*/
typedef struct {
    short acceleration_x;  // x轴加速度
    short acceleration_y;  // y轴加速度
    short acceleration_z;  // z轴加速度
    short temperature;  // 温度
}ACCELERATION;

typedef struct {
    short angular_velocity_x;  // x轴角速度
    short angular_velocity_y;  // y轴角速度
    short angular_velocity_z;  // z轴角速度
    short voltage;  // 电压
}ANGULAR_VELOCITY;

typedef struct {
    short angle_x;  // x轴角度
    short angle_y;  // y轴角度
    float angle_z;  // z轴角度
    short version;  // 版本号
}ANGLE;

typedef struct {
    //串口驱动结构体
    UART_DRIVES* user_uart;
    //陀螺仪数据
    ACCELERATION user_acceleration;  // 加速度数据
    ANGULAR_VELOCITY user_angular_velocity;  // 角速度数据
    ANGLE user_angle; //角度
} HWT906_DRIVES;

typedef enum {
    hwt906_time              = 0x50 ,
    hwt906_acceleration      = 0x51,
    hwt906_angular_velocity  = 0x52,
    hwt906_angle             = 0x53,
}HWT906_TYPE ;



/* 函数声明 ------------------------------------------------------------------*/

void HWT906_Init(HWT906_DRIVES* User_HWT906 , UART_DRIVES* user_hwt906);


#endif //DJI_A_BOARD_USER_HWT906_H