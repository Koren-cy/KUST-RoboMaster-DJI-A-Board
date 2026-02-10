#ifndef __USER_CODEC_H__
#define __USER_CODEC_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief 字节序类型
* @note 在帧结构中，偏移量小的靠近帧头的字节是高位则为大端序。
*/
typedef enum {
    Little_Endian,  /* 小端序 */
    Big_Endian,     /* 大端序 */
} Byte_Order;

/**
* @brief 编解码器结构体
*/
typedef struct {
    uint8_t *ptr;            /* 数据缓冲区 */
    uint16_t byte_index;     /* 字节索引 */
    uint8_t bit_index;       /* 当前字节中的比特索引 */
    Byte_Order byte_order;   /* 字节序 */
} CODEC;

/* 函数声明 ------------------------------------------------------------------*/
void Codec_Init(CODEC *codec, uint8_t *ptr, Byte_Order byte_order);

uint32_t Codec_Decode_Unsigned(CODEC *codec, uint8_t bitwidth);
int32_t Codec_Decode_Signed(CODEC *codec, uint8_t bitwidth);

void Codec_Encode_Unsigned(CODEC *codec, uint32_t value, uint8_t bitwidth);
void Codec_Encode_Signed(CODEC *codec, int32_t value, uint8_t bitwidth);

// 2¹ = 2,      2⁹ = 512,       2¹⁷ = 131_072,      2²⁵ = 33_554_432,
// 2² = 4,      2¹⁰ = 1_024,    2¹⁸ = 262_144,      2²⁶ = 67_108_864,
// 2³ = 8,      2¹¹ = 2_048,    2¹⁹ = 524_288       2²⁷ = 134_217_728,
// 2⁴ = 16,     2¹² = 4_096,    2²⁰ = 1_048_576,    2²⁸ = 268_435_456
// 2⁵ = 32,     2¹³ = 8_192,    2²¹ = 2_097_152,    2²⁹ = 536_870_912,
// 2⁶ = 64,     2¹⁴ = 16_384,   2²² = 4_194_304,    2³⁰ = 1_073_741_824,
// 2⁷ = 128,    2¹⁵ = 32_768,   2²³ = 8_388_608,    2³¹ = 2_147_483_648,
// 2⁸ = 256,    2¹⁶ = 65_536,   2²⁴ = 16_777_216,   2³² = 4_294_967_296,

#endif //__USER_CODEC_H__
