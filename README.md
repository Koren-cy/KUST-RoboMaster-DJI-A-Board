# 大疆A板基础框架

该项目为RoboMaster大赛，昆明理工大学昆蓬南冥战队的大疆A板的基础框架。

## 硬件平台

- **MCU**: STM32F427IIH6 (180MHz, Cortex-M4)

## 项目结构

```
├── Core/                           # STM32 核心代码
│   ├── Inc/                        # 头文件
│   │   ├── bsp.h                   # 板级支持包
│   │   ├── main.h                  # 主程序头文件
│   │   └── stm32f4xx_hal_conf.h    # HAL 配置
│   ├── Src/                        # 源文件
│   │   ├── bsp.c                   # 板级支持包实现
│   │   ├── main.c                  # 主程序
│   │   ├── stm32f4xx_hal_msp.c     # HAL MSP 初始化
│   │   └── stm32f4xx_it.c          # 中断处理
│   └── Startup/                    # 启动文件
├── Drivers/                        # STM32 HAL 驱动库
│   ├── CMSIS/                      # CMSIS 标准接口
│   │   ├── Device/                 # STM32F4 设备支持
│   │   ├── DSP/                    # DSP 数学库
│   │   └── Include/                # CMSIS 头文件
│   └── STM32F4xx_HAL_Driver/       # STM32F4 HAL 驱动
├── SEGGER_RTT/                     # SEGGER RTT 调试工具
│   ├── SEGGER_RTT.c/h              # RTT 核心功能
│   └── user_JScope_Transmit.c/h    # JScope 数据传输
├── User_Algorithm/                 # 算法模块
│   ├── Src/
│   │   ├── user_coord.c/h          # 坐标变换
│   │   └── user_fir.c/h            # FIR 滤波器
│   └── User_Controller/            # 控制器模块
│       ├── user_pid.c/h            # PID 控制器
│       ├── user_inc_pid.c/h        # 增量式 PID
│       ├── user_adrc.c/h           # 自抗扰控制器
│       ├── user_ladrc.c/h          # 线性自抗扰控制器
│       └── user_cascade_controller.c/h  # 串级控制器
├── User_Application/               # 应用层
│   └── swerve_chassis.c/h          # 舵轮底盘控制
├── User_Architect/                 # 架构组件
│   ├── user_codec.c/h              # 编解码工具
│   ├── user_delay.c/h              # 延时功能
│   ├── user_queue.c/h              # 无锁队列
│   └── user_ring_buffe.c/h         # 环形缓冲区
├── User_Drives/                    # 驱动层
│   ├── user_adc.c/h                # ADC 采集
│   ├── user_can.c/h                # CAN 总线
│   ├── user_dji_bus.c/h            # DBUS 遥控器协议
│   ├── user_led.c/h                # LED 控制
│   ├── user_pwm.c/h                # PWM 输出
│   ├── user_uart.c/h               # UART 通信
│   └── User_Motor/                 # 电机驱动模块
│       └── user_dji_motor.c/h      # 大疆电机驱动
├── User_Lib/                       # 用户库
│   └── user_music.h                # 音乐播放
└── User_Status/                    # 状态管理模块
```

## 许可证

本项目基于 STMicroelectronics 提供的 HAL 库开发。
