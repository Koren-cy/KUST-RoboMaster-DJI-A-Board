/* ==========================================
    Mock: Core/Inc/main.h
    Description:
    Host-side stub for the STM32 main.h header.
    Provides the types and macros that User_Algorithm
    modules need, without any STM32 hardware.
 ========================================== */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Minimal STM32 type stubs */
#include <stdint.h>

/* Memory section attribute - no-op on host */
#ifndef CCMRAM
#define CCMRAM
#endif

/* Exported function stubs */
void Error_Handler(void);

/* Private defines - empty on host */
#ifndef POWER_5V_Pin
#define POWER_5V_Pin           0
#endif
#ifndef POWER_5V_GPIO_Port
#define POWER_5V_GPIO_Port     ((void*)0)
#endif
/* ... other pin defines are harmless no-ops ... */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
