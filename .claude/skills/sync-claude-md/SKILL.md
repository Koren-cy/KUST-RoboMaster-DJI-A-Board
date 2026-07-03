---
name: sync-claude-md
description: 检查并同步 CLAUDE.md，确保其与 .ioc、bsp.h、bsp_config.h 及项目模块结构一致
disable-model-invocation: true
---

# 同步 CLAUDE.md

对比以下三个真相源与 CLAUDE.md，报告差异并更新：

## 检查清单

### 1. 外设配置（来源：`.ioc` + `main.c`）

从 `.ioc` 提取当前启用的外设、引脚、参数。从 `main.c` 的 `MX_*_Init()` 函数确认外设是否已初始化。与 CLAUDE.md 的外设映射表逐行对比：
- 新增了哪些外设？
- 哪些外设已被移除或替换？
- 引脚分配 / 波特率 / 参数是否变化？

### 2. 时钟配置（来源：`bsp_config.h` + `main.c` 的 `SystemClock_Config()`）

检查 SYSCLK、APB1、APB2 频率是否与 CLAUDE.md 一致。PLLM/PLLN/PLLP/PLLQ 等参数是否变更。

### 3. 模块注册表（来源：`bsp.h` + `bsp.c`）

从 `bsp.h` 提取所有 `extern` 声明，从 `bsp.c` 提取所有结构体定义。检查 CLAUDE.md 中的模块列表是否完整。

### 4. 目录结构（来源：`User_*/` 目录）

扫描 `User_Algorithm/`、`User_Drives/`、`User_Architect/`、`User_Application/` 中的 `.c` 文件。发现 CLAUDE.md 未记录的新模块。

### 5. 构建命令（来源：`CMakeLists.txt`、`.claude/tools/`）

检查 CMake 编译选项、cppcheck 路径等是否仍然有效。

## 输出格式

```
## 需要更新

| 章节 | 变更 |
|------|------|
| 外设映射 | +CAN2 移至 PB13/PD1 |
| 时钟 | 无变化 |
| 模块列表 | +user_stp23.c（已在 bsp.h 注册） |
| 约束 | +新增 HWT906 传感器模块 |
| 构建 | 无变化 |

## 无需更新

（列出所有确认一致的章节）
```

## 执行

直接更新 CLAUDE.md。更新后告知用户变更了哪些内容。不要询问"是否更新"——这就是你要做的事。
