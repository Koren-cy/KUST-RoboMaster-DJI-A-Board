#ifndef USER_BSP_CONFIG_H
#define USER_BSP_CONFIG_H

/* 全局配置 ------------------------------------------------------------------*/


/* 接口定义 ------------------------------------------------------------------*/
#define UART_NUM              (5)         /* 开发板的 UART 总数 */
#define UART_CALLBACK_NUM     (8)         /* 最大回调函数数量 */

#define CAN_NUM               (2)         /* 开发板的 CAN 总数 */
#define CAN_CALLBACK_NUM      (8)         /* 最大回调函数数量 */

#define ADC_NUM               (3)         /* 开发板的 ADC 总数 */
#define ADC_CHANNEL_MAX       (16)        /* 单个 ADC 的最大通道数 */
#define ADC_CALLBACK_NUM      (8)         /* 最大回调函数数量 */

#define DJI_MOTOR_NUM         (16)        /* 大疆电机最大挂载数量 */

#define STP23_NUM             (5)         /* 最多可挂载的 STP23 单点激光测距传感器数量 该传感器需要独占串口 所以通常不会超过总串口数量 */
#define STP23_CALLBACK_NUM    (8)         /* STP23 单点激光测距模块每个实例的最大回调函数数量 */


/* 全局结构体 ----------------------------------------------------------------*/


#endif // USER_BSP_CONFIG_H
