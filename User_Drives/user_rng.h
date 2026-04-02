#ifndef USER_RNG_H
#define USER_RNG_H
#include "main.h"
#ifdef HAL_RNG_MODULE_ENABLED

/* 包含头文件 ----------------------------------------------------------------*/
#include "../../Core/Inc/bsp_config.h"

/* 宏定义 --------------------------------------------------------------------*/
#define RNG_POOL_SIZE          (256)   /* 随机数缓冲池大小，必须为 2 的幂次方 */
#define RNG_POOL_MASK          (RNG_POOL_SIZE - 1)

/* 类型定义 ------------------------------------------------------------------*/
/**
* @brief RNG 缓冲池结构体
*/
typedef struct {
    uint8_t pool[RNG_POOL_SIZE];  /* 随机字节缓冲池 */
    volatile uint16_t head;       /* 读指针 */
    volatile uint16_t tail;       /* 写指针 */
} RNG_POOL;

/**
* @brief RNG 驱动结构体
*/
typedef struct {
    RNG_HandleTypeDef* hrng;      /* RNG 硬件句柄 */
    RNG_POOL pool;                /* 随机数缓冲池 */
    volatile uint32_t seed;       /* 软件种子 */
} RNG_DRIVES;

/* 函数声明 ------------------------------------------------------------------*/
void RNG_Init(RNG_DRIVES* user_rng, RNG_HandleTypeDef* hrng);
uint32_t RNG_GetWord(RNG_DRIVES* user_rng);
uint16_t RNG_GetHalf(RNG_DRIVES* user_rng);
uint8_t  RNG_GetByte(RNG_DRIVES* user_rng);
uint32_t RNG_GetRange(RNG_DRIVES* user_rng, const uint32_t min, const uint32_t max);
float    RNG_GetFloat(RNG_DRIVES* user_rng);
float    RNG_GetRangeFloat(RNG_DRIVES* user_rng, const float min, const float max);

#endif /* HAL_RNG_MODULE_ENABLED */
#endif // USER_RNG_H
