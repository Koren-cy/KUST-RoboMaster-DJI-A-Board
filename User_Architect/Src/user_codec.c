/* 包含头文件 ----------------------------------------------------------------*/
#include "../Inc/user_codec.h"

/* 私有宏定义 ----------------------------------------------------------------*/
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/* 私有函数 ------------------------------------------------------------------*/

/**
* @brief 获取指定位宽的掩码
* @param bitwidth 位宽
* @return 掩码值
*/
static uint32_t get_mask(const uint8_t bitwidth) {
    if (32 - bitwidth <= 0 )
        return 0xFFFFFFFF;
    if (32 - bitwidth == 32)
        return 0x00000000;
    return 0xFFFFFFFFU >> (32 - bitwidth);
}

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化编解码器
* @param codec      编解码器结构体指针
* @param ptr        数据缓冲区指针
* @param length     缓冲区长度
* @param byte_order 字节序
*/
void Codec_Init(CODEC *codec, uint8_t *ptr, const uint16_t length, const Byte_Order byte_order) {
    codec->ptr = ptr;
    codec->byte_order = byte_order;
    codec->byte_index = 0;
    codec->bit_index = 7;
}


/**
* @brief 从缓冲区解码无符号数据
* @param codec    编解码器结构体指针
* @param bitwidth 解码位宽
* @return 解码得到的无符号值
*/
uint32_t Codec_Decode_Unsigned(CODEC *codec, const uint8_t bitwidth) {
    uint32_t result = 0;
    uint8_t bits_remaining = bitwidth;
    while (bits_remaining > 0) {
        const uint8_t bits_available_in_byte = codec->bit_index + 1;
        const uint8_t bits_to_read = MIN(bits_remaining, bits_available_in_byte);

        const uint8_t mask = get_mask(bits_to_read);
        const uint32_t bits_decoded = (uint32_t) (codec->ptr[codec->byte_index] >> (bits_available_in_byte - bits_to_read)) & mask;


        if (codec->byte_order == Big_Endian) {
            result = (result << bits_to_read) | bits_decoded;
        } else {
            result = (bits_decoded << (bitwidth - bits_remaining)) | result;
        }

        bits_remaining  -= bits_to_read;

        if (codec->bit_index + 1 == bits_to_read) {
            codec->bit_index = 7;
            codec->byte_index++;
        }else {
            codec->bit_index -= bits_to_read;
        }
    }
    return result;
}

/**
* @brief 从缓冲区解码有符号数据
* @param codec    编解码器结构体指针
* @param bitwidth 解码位宽
* @return 解码得到的有符号值
*/
int32_t Codec_Decode_Signed(CODEC *codec, const uint8_t bitwidth) {
    if (bitwidth == 0 || bitwidth > 32)
        return 0;

    const uint32_t unsigned_value = Codec_Decode_Unsigned(codec, bitwidth);

    const uint32_t sign_bit = 1U << (bitwidth - 1);
    if (unsigned_value & sign_bit) {
        const uint32_t sign_extend_mask = ~ get_mask(bitwidth);
        return (int32_t)(unsigned_value | sign_extend_mask);
    } else {
        return (int32_t)unsigned_value;
    }
}

/**
* @brief 向缓冲区编码无符号数据
* @param codec    编解码器结构体指针
* @param value    要编码的无符号值
* @param bitwidth 编码位宽
*/
void Codec_Encode_Unsigned(CODEC *codec, uint32_t value, const uint8_t bitwidth) {
    value = value & get_mask(bitwidth);
    uint8_t bits_remaining = bitwidth;
    while (bits_remaining > 0) {
        const uint8_t bits_available_in_byte = codec->bit_index + 1;
        const uint8_t bits_to_write = MIN(bits_remaining, bits_available_in_byte);

        uint8_t bits_to_encode;
        if (codec->byte_order == Big_Endian) {
            bits_to_encode = (value >> (bits_remaining - bits_to_write)) & get_mask(bits_to_write);
        } else {
            bits_to_encode = (value >> (bitwidth - bits_remaining)) & get_mask(bits_to_write);
        }

        codec->ptr[codec->byte_index] |= bits_to_encode << (codec->bit_index - bits_to_write + 1);

        bits_remaining -= bits_to_write;

        if (codec->bit_index + 1 == bits_to_write) {
            codec->bit_index = 7;
            codec->byte_index++;
        } else {
            codec->bit_index -= bits_to_write;
        }
    }
}

/**
* @brief 向缓冲区编码有符号数据
* @param codec    编解码器结构体指针
* @param value    要编码的有符号值
* @param bitwidth 编码位宽
*/
void Codec_Encode_Signed(CODEC *codec, const int32_t value, const uint8_t bitwidth) {
    Codec_Encode_Unsigned(codec, value & get_mask(bitwidth), bitwidth);
}
