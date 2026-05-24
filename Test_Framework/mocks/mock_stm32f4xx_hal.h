/* ==========================================
    Mock: stm32f4xx_hal.h
    Description:
    Host-side stub for the STM32F4xx HAL driver.
    Provides minimal type/function stubs needed
    by the User_Algorithm modules.
 ========================================== */

#ifndef __STM32F4xx_HAL_H
#define __STM32F4xx_HAL_H

#include <stdint.h>

/* ==========================================
 * HAL status codes
 * ========================================== */
typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef enum {
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;

/* ==========================================
 * Basic STM32 types (minimal stubs)
 * ========================================== */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
    volatile uint32_t BRR;
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t OR;
} TIM_TypeDef;

typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
} USART_TypeDef;

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t AFR[2];
} GPIO_InitTypeDef;

typedef struct {
    uint32_t Prescaler;
    uint32_t CounterMode;
    uint32_t Period;
    uint32_t ClockDivision;
    uint32_t AutoReloadPreload;
} TIM_Base_InitTypeDef;

typedef struct {
    uint32_t USART_BaudRate;
    uint16_t USART_WordLength;
    uint16_t USART_StopBits;
    uint16_t USART_Parity;
    uint16_t USART_Mode;
    uint16_t USART_HardwareFlowControl;
} USART_InitTypeDef;

typedef struct {
    void *Instance;
    uint32_t ErrorCode;
} UART_HandleTypeDef;

typedef struct {
    void *Instance;
    uint32_t ErrorCode;
} TIM_HandleTypeDef;

/* ==========================================
 * HAL defines
 * ========================================== */
#define TIM_CLOCKSOURCE_INT        0x00000000U
#define TIM_AUTOMATICOUTPUT_ENABLE 0x00004000U
#define TIM_AUTOMATICOUTPUT_DISABLE 0x00000000U

#define USART_WORDLENGTH_8B        0x00000000U
#define USART_WORDLENGTH_9B        0x00001000U
#define USART_STOPBITS_1           0x00000000U
#define USART_PARITY_NONE          0x00000000U
#define USART_PARITY_EVEN          0x00000400U
#define USART_PARITY_ODD           0x00000200U
#define USART_MODE_RX              0x00000004U
#define USART_MODE_TX              0x00000008U
#define USART_MODE_TX_RX           0x0000000CU
#define USART_HARDWAREFLOWCONTROL_NONE 0x00000000U

#define GPIO_SPEED_FREQ_LOW        0x00000000U
#define GPIO_SPEED_FREQ_MEDIUM     0x00000001U
#define GPIO_SPEED_FREQ_HIGH       0x00000002U
#define GPIO_SPEED_FREQ_VERY_HIGH  0x00000003U
#define GPIO_MODE_INPUT             0x00000000U
#define GPIO_MODE_OUTPUT_PP         0x00000001U
#define GPIO_MODE_AF_PP            0x00000002U
#define GPIO_NOPULL                0x00000000U
#define GPIO_PULLUP                0x00000001U
#define GPIO_PULLDOWN              0x00000002U

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0U))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  \
    CLEAR_REG((REG) & ~(CLEARMASK));          \
    SET_BIT((REG), (SETMASK))

#define UNUSED(X) (void)X

/* ==========================================
 * HAL MSP stubs
 * ========================================== */
void HAL_MspInit(void);
void HAL_MspDeInit(void);
void HAL_TIM_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_MspDeInit(TIM_HandleTypeDef *htim);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

/* ==========================================
 * HAL Init/DeInit stubs
 * ========================================== */
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_InitTick(uint32_t TickPriority);
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t Delay);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
uint32_t HAL_GetUIDw0(void);
uint32_t HAL_GetUIDw1(void);
uint32_t HAL_GetUIDw2(void);

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t PinState);
uint8_t HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);

/* ==========================================
 * Weak function stubs (no-op on host)
 * ========================================== */
__attribute__((weak)) void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
__attribute__((weak)) void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim);
__attribute__((weak)) void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim);
__attribute__((weak)) void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
__attribute__((weak)) void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
__attribute__((weak)) void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

/* ==========================================
 * System clock / RCC stubs
 * ========================================== */
void SystemClock_Config(void);

/* ==========================================
 * Error handler stub
 * ========================================== */
void Error_Handler(void);

#endif /* __STM32F4xx_HAL_H */
