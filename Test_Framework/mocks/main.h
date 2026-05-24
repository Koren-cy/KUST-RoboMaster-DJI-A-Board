/* ==========================================
    Mock: Core/Inc/main.h
    Host-side stub for the STM32 main.h header.
    Includes the mock STM32 HAL and SEGGER RTT.
 ========================================== */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard integer types */
#include <stdint.h>

/* ---- STM32 HAL types and function stubs ---- */
#include "stm32f4xx_hal.h"

/* ---- SEGGER RTT stubs ---- */
#include "SEGGER_RTT.h"

/* Memory section attribute - no-op on host */
#define CCMRAM

/* ---- End of C linkage block ---- */
#ifdef __cplusplus
}
#endif

/* ==========================================
 * Pin/port defines (harmless on host)
 * ========================================== */
#define POWER_5V_Pin          0
#define POWER_5V_GPIO_Port   ((void*)0)
#define Buzzer_Pin           0
#define Buzzer_GPIO_Port      ((void*)0)
#define LED_RED_Pin          0
#define LED_RED_GPIO_Port     ((void*)0)
#define LED_GREEN_Pin        0
#define LED_GREEN_GPIO_Port  ((void*)0)

#endif /* __MAIN_H */
