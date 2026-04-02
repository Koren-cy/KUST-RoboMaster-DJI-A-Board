#include "../../Core/Inc/bsp.h"
#ifdef HAL_RNG_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_rng.h"
#include <string.h>

/* 私有宏 --------------------------------------------------------------------*/
#define RNG_SEED_INIT  0x8B9F7A3DU

/* 私有函数 ------------------------------------------------------------------*/

/**
* @brief 获取缓冲池中可用字节数
* @return 可用字节数
*/
static inline uint16_t RNG_Pool_Count() {
    return (uint16_t)(g_rng->pool->tail - g_rng->pool->head);
}

/**
* @brief 获取缓冲池剩余空间
* @return 剩余空间
*/
static inline uint16_t RNG_Pool_Free() {
    return (uint16_t)(RNG_POOL_SIZE - RNG_Pool_Count(g_rng->pool));
}

/**
* @brief 向缓冲池压入一个随机字节
* @param byte 随机字节
*/
static inline void RNG_Pool_Push(const uint8_t byte) {
    g_rng->pool->pool[pool->tail & RNG_POOL_MASK] = byte;
    g_rng->pool->tail++;
}

/**
* @brief 从缓冲池弹出一个随机字节
* @param byte 存储弹出字节的缓冲区
* @return 1 成功，0 缓冲池为空
*/
static inline uint8_t RNG_Pool_Pop(uint8_t* byte) {
    if (RNG_Pool_Count(g_rng->pool) == 0)
        return 0;
    *byte = g_rng->pool->pool[g_rng->pool->head & RNG_POOL_MASK];
    g_rng->pool->head++;
    return 1;
}

/* 全局驱动实例指针 --------------------------------------------*/
static RNG_DRIVES* g_rng = NULL;

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化 RNG 硬件随机数生成器
* @param user_rng RNG 驱动结构体指针
* @param hrng     RNG 硬件句柄
*/
void RNG_Init(RNG_DRIVES* user_rng, RNG_HandleTypeDef* hrng) {
    user_rng->hrng = hrng;
    user_rng->seed = RNG_SEED_INIT;

    memset(user_rng->pool.pool, 0, RNG_POOL_SIZE);
    user_rng->pool.head = 0;
    user_rng->pool.tail = 0;

    g_rng = user_rng;

    HAL_RNG_Init(user_rng->hrng);
    HAL_RNG_ReadyDataGenerationCallback(user_rng->hrng);
}

/**
* @brief 获取一个 32 位随机数
* @param user_rng RNG 驱动结构体指针
* @return 随机数
*/
uint32_t RNG_GetWord(RNG_DRIVES* user_rng) {
    if (RNG_Pool_Count(&user_rng->pool) >= 4) {
        uint32_t val;
        for (uint8_t i = 0; i < 4; i++) {
            uint8_t b;
            RNG_Pool_Pop(&user_rng->pool, &b);
            ((uint8_t*)&val)[i] = b;
        }
        return val;
    }

    uint32_t random32bit;
    if (HAL_RNG_GenerateRandomNumber(user_rng->hrng, &random32bit) != HAL_OK) {
        return 0;
    }

    user_rng->seed ^= user_rng->seed << 13;
    user_rng->seed ^= user_rng->seed >> 17;
    user_rng->seed ^= user_rng->seed << 5;
    user_rng->seed += random32bit;

    return user_rng->seed ^ random32bit;
}

/**
* @brief 获取一个 16 位随机数
* @param user_rng RNG 驱动结构体指针
* @return 随机数 0 ~ 65535
*/
uint16_t RNG_GetHalf(RNG_DRIVES* user_rng) {
    return (uint16_t)(RNG_GetWord(user_rng) >> 16);
}

/**
* @brief 获取一个 8 位随机数
* @param user_rng RNG 驱动结构体指针
* @return 随机数 0 ~ 255
*/
uint8_t RNG_GetByte(RNG_DRIVES* user_rng) {
    return (uint8_t)(RNG_GetWord(user_rng) >> 24);
}

/**
* @brief 获取一个指定范围内的 32 位随机数
* @param user_rng RNG 驱动结构体指针
* @param min     最小值
* @param max     最大值
* @return 随机数
*/
uint32_t RNG_GetRange(RNG_DRIVES* user_rng, const uint32_t min, const uint32_t max) {

    const uint32_t range = max - min + 1;

    if ((range & (range - 1)) == 0) {
        return min + (RNG_GetWord(user_rng) & (range - 1));
    }

    const uint32_t limit = (uint32_t)(0xFFFFFFFF / range) * range;
    uint32_t val;
    do {
        val = RNG_GetWord(user_rng);
    } while (val >= limit);

    return min + val % range;
}

/**
* @brief 获取一个 0.0 ~ 1.0 范围内的随机浮点数
* @param user_rng RNG 驱动结构体指针
* @return 随机数 范围是 0.0f ~ 1.0f
*/
float RNG_GetFloat(RNG_DRIVES* user_rng) {
    return (float)(RNG_GetWord(user_rng) >> 8) / 16777216.0f;
}

/**
* @brief 获取一个指定范围内的随机浮点数
* @param user_rng RNG 驱动结构体指针
* @param min      最小值
* @param max      最大值
* @return 随机数
*/
float RNG_GetRangeFloat(RNG_DRIVES* user_rng, const float min, const float max) {
    return min + RNG_GetFloat(user_rng) * (max - min);
}

/* 覆写中断回调函数 -----------------------------------------------------------*/

void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, const uint32_t random32bit) {
    if (g_rng == NULL)
        return;

    if (RNG_Pool_Free(&g_rng->pool) < 4)
        return;

    RNG_Pool_Push(&g_rng->pool, (uint8_t)(random32bit >> 0));
    RNG_Pool_Push(&g_rng->pool, (uint8_t)(random32bit >> 8));
    RNG_Pool_Push(&g_rng->pool, (uint8_t)(random32bit >> 16));
    RNG_Pool_Push(&g_rng->pool, (uint8_t)(random32bit >> 24));

    HAL_RNG_ReadyDataGenerationCallback(hrng);
}

#endif /* HAL_RNG_MODULE_ENABLED */
