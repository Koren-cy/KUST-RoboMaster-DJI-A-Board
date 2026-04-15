#include "../../Core/Inc/bsp.h"
#ifdef HAL_UART_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include <sys/types.h>
#include "../user_hwt906.h"
#include "../../User_Architect/user_codec.h"

/* 私有变量 ------------------------------------------------------------------*/
static HWT906_DRIVES* hwt906_drives[HWT906_NUM];
static uint8_t hwt906_num = 0;

/* 私有函数声明 --------------------------------------------------------------*/
static void HWT906_Handle(void* user_uart);
static char HWT906_GetSumCrc(HWT906_FRAME_TYPE type, const uint8_t* data, uint8_t len);

/* 函数体 --------------------------------------------------------------------*/

/**
 * @brief  初始化 HWT906
 * @param  user_hwt906  HWT906 驱动结构体指针
 * @param  user_uart    串口驱动结构体指针
 */
void HWT906_Init(HWT906_DRIVES* user_hwt906, UART_DRIVES* user_uart) {
    user_hwt906->user_uart = user_uart;
    UART_RegisterCallback(user_uart, HWT906_Handle);

    hwt906_drives[hwt906_num] = user_hwt906;
    hwt906_num++;
}

/* 私有函数 --------------------------------------------------------------------*/

/**
 * @brief  计算 HWT906 校验和
 * @param  type  数据包类型
 * @param  data  数据包内容
 * @param  len   数据长度
 * @retval 校验和
 */
static char HWT906_GetSumCrc(HWT906_FRAME_TYPE type, const uint8_t* data, uint8_t len) {
    char sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}
/**
 * @brief  HWT906 串口回调函数
 * @param  user_uart  串口驱动结构体指针
 */
static void HWT906_Handle(void* user_uart){
    const UART_DRIVES* uart = (UART_DRIVES*)user_uart;

    for (uint8_t hwt906_index = 0; hwt906_index < hwt906_num; hwt906_index++) {
        HWT906_DRIVES* hwt906 = hwt906_drives[hwt906_index];

        if (hwt906->user_uart->huart != uart->huart)
            continue;

        const uint8_t buffer_head[1] = {0x55};
        uint8_t* buf = hwt906->buf;

        /* 获取串口数据 */
        if (!UART_GetDataWithHLen(hwt906->user_uart, buf, (char*)buffer_head, HWT906_BUF_LEN)) {
            return;
        }

        /* 校验数据 */
        if (buf[10] != HWT906_GetSumCrc((HWT906_FRAME_TYPE)buf[1], buf, 10)) {
            return;
        }

        /* 根据数据包类型解析数据 */
        switch (buf[1]) {
            case HWT906_ACCELERATION: {
                CODEC codec = {0};
                Codec_Init(&codec, buf, Little_Endian);
                Codec_Decode_Skip(&codec, 16);
                hwt906->user_acceleration.acceleration_x = (short)Codec_Decode_Signed(&codec, 16);
                hwt906->user_acceleration.acceleration_y = (short)Codec_Decode_Signed(&codec, 16);
                hwt906->user_acceleration.acceleration_z = (short)Codec_Decode_Signed(&codec, 16);
                hwt906->user_acceleration.temperature    = (short)Codec_Decode_Signed(&codec, 16);
                break;
            }

            case HWT906_ANGULAR_VELOCITY: {
                CODEC codec = {0};
                Codec_Init(&codec, buf, Little_Endian);
                Codec_Decode_Skip(&codec, 16);
                hwt906->user_angular_velocity.angular_velocity_x = (short)Codec_Decode_Signed(&codec, 16);
                hwt906->user_angular_velocity.angular_velocity_y = (short)Codec_Decode_Signed(&codec, 16);
                hwt906->user_angular_velocity.angular_velocity_z = (short)Codec_Decode_Signed(&codec, 16);
                hwt906->user_angular_velocity.voltage            = (short)Codec_Decode_Signed(&codec, 16);
                break;
            }

            case HWT906_ANGLE: {
                CODEC codec = {0};
                Codec_Init(&codec, buf, Little_Endian);
                Codec_Decode_Skip(&codec, 16);
                hwt906->user_angle.angle_x = (float)Codec_Decode_Unsigned(&codec, 16) / 32768.0f * 180.0f;
                hwt906->user_angle.angle_y = (float)Codec_Decode_Unsigned(&codec, 16) / 32768.0f * 180.0f;
                hwt906->user_angle.angle_z = (float)Codec_Decode_Unsigned(&codec, 16) / 32768.0f * 180.0f;
                hwt906->user_angle.version = (short)Codec_Decode_Unsigned(&codec, 16);
                break;
            }

            default:
                break;
        }

        /* 触发回调函数 */
        for (uint8_t i = 0; i < hwt906->callback_num; i++) {
            hwt906->callbacks[i](hwt906);
        }
    }
}

/**
 * @brief  注册 HWT906 数据更新回调函数
 * @param  user_hwt906  HWT906 驱动结构体指针
 * @param  callback     回调函数
 */
void HWT906_RegisterCallback(HWT906_DRIVES* user_hwt906, const HWT906_Callback callback) {
    user_hwt906->callbacks[user_hwt906->callback_num] = callback;
    user_hwt906->callback_num++;
}

#endif /* HAL_UART_MODULE_ENABLED */
