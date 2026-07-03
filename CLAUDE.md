# CLAUDE.md

本文件为 Claude Code（claude.ai/code）在此仓库中工作时提供指导。

## 项目概述

昆明理工大学昆蓬南冥战队 RoboMaster 大疆 A 板固件。MCU：STM32F427IIH6（Cortex-M4, 180MHz, 2MB Flash, 192KB SRAM + 64KB CCMRAM）。裸机 C 语言，无 RTOS。

## 构建与工具

```bash
# 主要 IDE：STM32CubeIDE — 打开 DJI-A-Board.ioc，生成代码，在 IDE 中构建
# CMake 命令行构建（需要 STM32CubeCLT 或 arm-none-eabi-gcc 工具链）：
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
ninja -C build

# ST-Link 烧录（OpenOCD）：
openocd -f "DJI-A-Board Debug.cfg" -c "program build/DJI-A-Board.elf verify reset exit"

# 静态分析（cppcheck 已安装在项目本地 .claude/tools/cppcheck/）：
.claude/tools/cppcheck/PFiles/Cppcheck/cppcheck.exe --enable=all --suppress=missingIncludeSystem -I Core/Inc -I Drivers/STM32F4xx_HAL_Driver/Inc Core/ User_*/
```

**CubeMX 代码生成规则**：`.ioc` 文件是外设配置的唯一真相源。CubeMX 生成代码后，`main.c`、`stm32f4xx_it.c`、`stm32f4xx_hal_msp.c`、`CMakeLists.txt` 会被覆盖。用户代码只能放在 `/* USER CODE BEGIN ... */` 和 `/* USER CODE END ... */` 标记之间——**绝对不要在这些标记之外编辑** CubeMX 管理的文件。

**GCC 工具链**：`arm-none-eabi-gcc`，C11 标准，软件浮点。关键参数：`-mcpu=cortex-m4 -mthumb -Ofast`（Release）/ `-Og -g`（Debug）。CMSIS-DSP 可用（`ARM_MATH_CM4`）。

## 架构

### 初始化三步模式

1. **零填充**在 `bsp.c` 中——全局驱动结构体用 `= {0}` 声明（如 `UART_DRIVES ros_uart = {0}`）
2. **硬件初始化**在 `main.c` → `MX_*_Init()`（CubeMX 自动生成）
3. **应用初始化**在 `main.c` 的 USER CODE 区域——调用各种 `*_Init()` 函数（如 `PWM_Init()`、`PID_Init()`）

### 模块声明模式

新模块在 `bsp.h` 中声明，在 `bsp.c` 中定义（零填充）。两个文件共同充当中央注册表。添加新外设的流程：
1. `bsp.h` — `extern MY_DRIVES my_device;`
2. `bsp.c` — `MY_DRIVES my_device = {0};`
3. `main.c` USER CODE — `MyDevice_Init(&my_device, ...);`

**端到端示例**——添加一个 ADC 驱动模块：
1. 创建 `User_Drives/User_Peripheral/user_adc.h`：定义 `ADC_DRIVES` 结构体、回调类型、API 函数声明
2. 创建 `User_Drives/User_Peripheral/user_adc.c`：实现 `ADC_Init()`、`ADC_Start()`、`ADC_RegisterCallback()`
3. 在 `bsp.h` 中添加 `#include "../../User_Drives/User_Peripheral/user_adc.h"` 和 `extern ADC_DRIVES battery_adc;`
4. 在 `bsp.c` 中添加 `ADC_DRIVES battery_adc = {0};`
5. 在 `main.c` 的 `/* USER CODE BEGIN 2 */` 中调用 `ADC_Init(&battery_adc, &hadc1);`
6. 在 `stm32f4xx_it.c` 的中断处理函数中调用驱动的 HAL 回调（如 `HAL_ADC_ConvCpltCallback()`）
注意：`bsp.h` 中的 `#include` 使用相对于项目根目录的路径（如 `../../User_Drives/...`），因为 CubeMX 在 `Core/Inc/` 下生成代码。

### 事件循环

主循环调用 `LOOP_EVENT_Handle()`，遍历 `loop_event[]` 函数指针数组（最多 32 个）。各模块将周期性回调注册到此数组中。

### SysTick 任务调度器

`user_systick.c` 管理最多 32 个定时任务。支持单次和重复模式，可配置周期。用于 LED 闪烁、启动音乐播放等。通过 `SysTick_InitTask()` / `SysTick_StartTask()` 配置。

### 驱动架构

每种外设类型都有一个自定义驱动结构体，封装 STM32 HAL 句柄：
- `UART_DRIVES` — 双缓冲 DMA 接收、环形缓冲区、发送队列、回调数组
- `CAN_DRIVES` — CAN 滤波管理、回调数组
- `PWM_DRIVES` — 频率/占空比抽象
- `PID_Controller` — 位置式 PID，带抗积分饱和和输出限幅
- 其他控制器：增量式 PID、串级、并联、ADRC、LADRC

所有驱动遵循统一模式：结构体 → `_Init()` → `_Set*()`/`_Get*()` → 回调注册。

### 算法层

`User_Algorithm/` 包含纯计算模块（PID、FIR、牛顿法、坐标变换、互补滤波）。这些模块不依赖硬件，可以通过 mock HAL 头文件在 PC 上进行单元测试。

### CCMRAM

64KB 紧耦合内存用于延迟敏感的数据（JScope 发送缓冲区）。用 `CCMRAM` 宏定义（`__attribute__((section(".ccmram")))`）。**CCMRAM 不能被 DMA 访问**——只能放置纯 CPU 访问的数据。

### SEGGER RTT 调试输出

`SEGGER_RTT/` 通过 SWD/JTAG 实现非侵入式调试输出，无需占用 UART 引脚。用 `SEGGER_RTT_printf()` 替代 `printf` 进行日志输出。JScope（`user_JScope_Transmit.c/h`）基于 RTT 实现实时变量波形传输到宿主机 JScope 软件——发送缓冲区特意放在 CCMRAM 中以减少延迟。调试时用 J-Link 或 ST-Link 连接 SWD 接口即可同时烧录和查看日志。

## 外设映射

| 外设 | 引脚 | 用途 | 配置 |
|------|------|------|------|
| CAN1 | PB12(RX) PD0(TX) | 底盘驱动电机 | 1 Mbps |
| CAN2 | PB13(RX) PD1(TX) | 云台电机 | 1 Mbps |
| USART1 | PB6(TX) PB7(RX) | DBUS 遥控器 | 100k 波特率，偶校验 |
| USART3 | PC10(TX) PC11(RX) | 通用串口 | 115200 |
| USART6 | PG14(TX) PG9(RX) | 调试/ROS 串口 | 115200 |
| TIM2 CH1 | PA0 | 右电机 PWM | 50 Hz，16 位 |
| TIM2 CH2 | PA1 | 左电机 PWM | 50 Hz，16 位 |
| TIM12 CH1 | PH6 | 蜂鸣器 PWM | 变频 |
| LED_RED | PE11 | 状态指示 | 低电平有效 |
| LED_GREEN | PF14 | 状态指示 | 低电平有效 |
| HWT906 | 独占一个 USART | 姿态传感器（IMU） | 串口，需独占 |
| STP23 | 独占一个 USART | 单点激光测距 | 串口，需独占 |

时钟：HSE 12 MHz → PLL（M=6, N=180, P=2）→ SYSCLK 180 MHz，APB1 45 MHz，APB2 90 MHz，Over-Drive 已启用，Flash 5 等待周期。

## 代码约定

- 用户代码使用中文注释
- 变量命名：局部变量 `snake_case`，结构体成员 `PascalCase`（如 `loop_event_num`、`UART_DRIVES`）
- 自定义驱动类型以 `_DRIVES` 结尾（如 `UART_DRIVES`、`PWM_DRIVES`、`LED_DRIVES`）
- 函数前缀与模块名一致：`PID_*`、`UART_*`、`CAN_*`、`PWM_*`、`Queue_*`
- `main.c` 中的 CubeMX 用户区域：`/* USER CODE BEGIN 0 */` 到 `/* USER CODE BEGIN 4 */`，以及 `/* USER CODE BEGIN WHILE */`
- 优先使用零填充初始化（`= {0}`），而非逐成员显式初始化

## 关键约束

- 禁止动态内存分配（裸机，无堆）
- ISR 必须短小——使用回调进行延迟处理
- UART 的 DMA 缓冲区使用双缓冲（乒乓缓冲）
- ISR 与主循环之间共享的变量必须用 `volatile` 修饰
- STM32F4 是 Cortex-M4（非 M7）：无数据缓存、无 TCM、强序内存模型——Cortex-M7 的缓存一致性和内存屏障问题在此不适用
- CAN 滤波器配置：CAN1 和 CAN2 共享 28 个滤波器组，在 `stm32f4xx_hal_msp.c` 的 `HAL_CAN_MspInit()` 中配置。给两个 CAN 各分配 14 个滤波器组（`CAN_FILTERCONFIGMODE` + `HAL_CAN_ConfigFilter()`）。CAN2 的时钟必须先于 CAN1 的滤波器初始化，否则 CAN2 收发失败
