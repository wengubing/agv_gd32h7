# GD32RTX5 工程说明

## 1. 主要功能

本工程基于 **GD32H7（Cortex-M7）+ RTX5**，当前实现了以下核心功能：

- RTX5 多任务框架搭建（CMSIS-RTOS2 接口）
- LED 周期闪烁任务
- 按键扫描与信号量触发
- 消息队列收发示例（发送/接收任务）
- 二值信号量同步示例
- CAN2 基础收发测试（含中断接收）
- CanFestival 定时调度基础接入（用于 CANopen 调度）
- USART0 调试打印

---

## 2. 设计思路

工程采用“**底层驱动 + 中间驱动/协议 + 应用任务**”分层：

1. **BSP 层**负责 GPIO、USART、CAN、基础时钟/延时等硬件初始化。
2. **Drivers 层**封装业务驱动与协议（如 Servo、CanOpen/CanFestival）。
3. **User 层**负责系统启动、任务创建、任务调度与中断入口。
4. **RTX5**提供线程、消息队列、信号量等内核对象，实现模块解耦。

主程序中完成缓存/FPU/中断分组初始化后，进入 RTX5 内核；由启动任务统一创建应用任务，再进入周期调度。

---

## 3. 基本框架

### 3.1 启动流程（简要）

1. `main()`：
   - 使能 I-Cache / D-Cache
   - 设置中断优先级分组
   - 使能 FPU
   - 初始化 `TIMER6` 系统定时中断（50us 基准）
   - 初始化 KEY、USART、CAN
   - `osKernelInitialize()` + 创建启动任务 + `osKernelStart()`

2. `AppTaskStart()`：
   - 初始化应用对象（消息队列、二值信号量）
   - 创建各应用线程

3. 任务运行：
   - LED、按键、消息队列收发、信号量同步、CANopen Rx/Tx 任务并行运行

### 3.2 线程与内核对象（当前）

- 线程：`LED_task`、`Key_task`、`Sender_task`、`Receiver_task`、`Semaphore_task`、`CanOpenRx_task`、`CanOpenTx_task`
- 内核对象：
  - 消息队列 `msgQueue`
  - 二值信号量 `binarySemaphore`

---

## 4. 目录说明

- `BSP/`：板级驱动（CAN、KEY、LED、SYSTICK、USART）
- `CBB/`：通用基础组件（通用算法/基础定义等）
- `CMSIS/`：ARM CMSIS、RTOS2 适配与内核接口头文件
- `Drivers/`：驱动与协议层（Servo、CAN、CanOpen/CanFestival、Common）
- `GD32H7xx_FWLIB/`：GD32H7 官方标准外设库
- `Project/`：Keil 工程文件（`gd32h7.uvprojx`）
- `RTX5/`：RTX5 内核配置与源码
- `User/`：应用入口与中断（`main.c`、`can_test.c`、`gd32h7xx_it.c` 等）
- `Doc/`：项目说明文档
- `ztemp/`：编译中间文件与输出文件目录

---

## 5. 片上资源分配（重点：定时器）

> 以下内容按当前源码配置整理。

### 5.1 定时器资源

1. **TIMER6（系统软时基）**
   - 用途：50us 周期中断，累计形成 1ms 计数基准（`g_Sys_1ms_Counter`）
   - 位置：`User/main.c`
   - 中断：`TIMER6_IRQHandler`
   - 配置要点：
     - 计数时钟目标：1MHz
     - 周期：50us（`period = 50-1`）
     - NVIC 优先级：`(5, 0)`

2. **TIMER4（CanFestival/CANopen 调度定时器）**
   - 用途：CanFestival 调度定时基准，触发 `TimeDispatch()`
   - 位置：`Drivers/CanOpen/CanFestival-3/drivers/GD32/canfestival_timer.c`
   - 模式：CH0 Output Compare Timing
   - 中断：`TIMER4_IRQHandler`
   - 配置要点：
     - 预分频：80 分频到 1MHz（1us 计数）
     - NVIC 优先级：`(1, 0)`

3. **SysTick**
   - 用途：RTX5 内核节拍（由系统接管）及 BSP 延时函数（`Delay_Ms/Delay_Us` 等）
   - 位置：`BSP/systick/*`、`User/gd32h7xx_it.c`（用户侧 SysTick 中断处理留空/由 RTX5接管）

### 5.2 通信与调试资源

1. **CAN2**
   - 功能：CAN 收发测试与中断接收
   - 引脚：
     - CAN_RX：`PD12`（AF5）
     - CAN_TX：`PD13`（AF5）
   - 中断：`CAN2_Message_IRQn`（处理函数宏映射为 `CAN2_Message_IRQHandler`）
   - 位置：`BSP/can/bsp_can_fd.*`、`User/gd32h7xx_it.c`

2. **USART0（调试串口）**
   - 功能：日志打印/调试输出
   - 引脚：
     - TX：`PA9`（AF7）
     - RX：`PA10`（AF7）
   - 波特率：115200
   - 位置：`BSP/usart/bsp_usart.*`

### 5.3 GPIO 资源

- LED：
  - LED1：`PD11`
  - LED2：`PF7`
  - LED3：`PD4`
- KEY：
  - KEY1：`PA0`
  - KEY2：`PD7`

---

## 6. 说明

- 本文档描述的是当前工程代码状态，后续若新增外设、任务或更改中断优先级，请同步更新该文件。
- 若用于正式版本，建议在此基础上补充：任务优先级表、堆栈评估、异常处理策略、版本变更记录。

---

## 7. Keil Simulator 自动验证脚本（Copilot 可调用）

新增脚本：`tools/verify-keil-simulator.ps1`

用途：

- 调用 Keil UV4 命令行进行重建，并解析编译日志
- 调用 Keil UV4 命令行 `-d` 进行调试探测（无桌面会话下的 API/进程验证）
- 读取 `Project/gd32h7.uvoptx` 中的 Simulator 配置（`uSim`）
- 通过 UI 自动化（发送快捷键）控制 uVision 进入/运行/退出调试会话
- 生成验证报告：`ztemp/sim_verify_report.json`

示例（在工程根目录 PowerShell 执行）：

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\verify-keil-simulator.ps1
```

可选参数：

- `-Uv4Path`：Keil UV4 路径（默认 `C:/Keil_v5/UV4/UV4.exe`）
- `-ProjectFile`：Keil 项目文件（默认 `Project/gd32h7.uvprojx`）
- `-Target`：目标工程名（默认 `Project`）
- `-RunSeconds`：发送 F5 后保持运行秒数（默认 3）
- `-SkipUiAutomation`：仅做日志与配置验证，不执行 UI 自动化

判定通过条件：

1. 编译日志显示 `0 Error(s)`
2. `uvoptx` 检测到 `uSim = 1`
3. 满足以下任一条件：
   - UI 自动化流程成功发送并完成调试/运行操作（若未使用 `-SkipUiAutomation`）
   - 命令行调试探测成功，且 `UV4 -d` 进程 CPU 增量大于 0（表示仿真在执行）

---

## 8. 会话记录

- 2026-04-27 simulator 诊断与修复记录：`Doc/simulator-session-2026-04-27.md`

---

## 9. 调试期 Thread Viewer 不显示 RTX5 线程（排查与修复）

### 9.1 现象

- 进入 Debug 后，`System and Thread Viewer` 窗口为空。
- 但工程可编译、可进入仿真/调试，任务心跳自检可通过。

### 9.2 已确认前提

- `RTX5` 源码已正确纳入工程（`rtx_kernel.c`、`rtx_lib.c` 等）。
- 目标镜像中存在 `osRtxInfo` 与 `osRtxConfig` 符号（见 `ztemp/gd32h7.map`）。
- `uvoptx` 已开启 `sRSysVw/tRSysVw`（System Viewer 开关）。

### 9.3 根因与处理

在 Cortex-M7 上调试时，若 D-Cache 开启，调试器对内存的读取可能与 CPU 缓存视图不一致，导致 Thread Viewer 读取 RTX5 内核对象时显示为空。

当前修复：在 `User/main.c` 中保留 I-Cache，且仅在“未连接调试器”时启用 D-Cache。

- 调试连接存在（`CoreDebug->DHCSR.C_DEBUGEN=1`）：不启用 D-Cache，保证线程可视化一致性。
- 非调试运行：启用 D-Cache，保持运行性能。

可通过宏 `RTX5_DEBUGGER_CACHE_SAFE` 控制该行为（默认开启）。

### 9.4 建议验证流程

1. 执行 `tools/verify-keil-simulator.ps1 -SkipUiAutomation`，确认构建与调试探测通过。
2. 进入 Debug 后先运行一段时间，再暂停（Break）。
3. 打开 `System and Thread Viewer`，确认可见 Idle/Timer/User 线程。