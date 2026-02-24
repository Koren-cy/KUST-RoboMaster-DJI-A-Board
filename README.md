# 大疆A板基础框架

该项目为RoboMaster大赛，昆明理工大学昆蓬南冥战队的大疆A板的基础框架。

## 硬件平台

- **MCU**: STM32F427IIH6 (180MHz, Cortex-M4)

## 项目结构

```
├── Core/                           # 框架使用时主要关注下面四个文件即可
│   ├── Inc/
│   │   └── bsp.h                   # 在此处声明要使用的模块
│   └── Src/
│       ├── bsp.c                   # 在此处用零初始化（零填充）模块
│       ├── main.c                  # 主程序，用 Init 函数真正初始化（而非零填充）模块
│       └── stm32f4xx_it.c          # 中断处理
├── Drivers/
│   └── CMSIS/
│       └── DSP/                    # DSP 数学库
├── SEGGER_RTT/
│   ├── SEGGER_RTT.c/h              # RTT 核心功能
│   └── user_JScope_Transmit.c/h    # JScope 数据传输
├── User_Algorithm/
│   │   ├── user_coord.c/h          # 坐标变换
│   │   └── user_fir.c/h            # FIR 滤波器
│   └── User_Controller/
│       ├── user_pid.c/h            # 位置式 PID
│       ├── user_inc_pid.c/h        # 增量式 PID
│       ├── user_adrc.c/h           # 自抗扰控制器
│       ├── user_ladrc.c/h          # 线性自抗扰控制器
│       └── user_cascade.c/h        # 串级控制器
├── User_Application/
│   └── swerve_chassis.c/h          # 舵轮底盘控制
├── User_Architect/
│   ├── user_codec.c/h              # 编解码工具
│   ├── user_delay.c/h              # 延时功能
│   ├── user_queue.c/h              # 无锁队列
│   └── user_ring_buffe.c/h         # 环形缓冲区
├── User_Drives/
│   ├── user_adc.c/h                # ADC 采集
│   ├── user_can.c/h                # CAN 总线
│   ├── user_dji_bus.c/h            # DBUS 遥控器协议
│   ├── user_led.c/h                # LED 控制
│   ├── user_pwm.c/h                # PWM 输出
│   ├── user_uart.c/h               # UART 通信
│   └── User_Motor/                 # 电机驱动模块
│       └── user_dji_motor.c/h      # 大疆电机驱动
├── User_Lib/
│   └── user_music.h                # 音乐播放
└── User_Status/                    # 状态管理模块
```

## 许可证

本项目基于 STMicroelectronics 提供的 HAL 库开发。
