# RTX5 System and Thread Viewer 排查记录

## 问题描述

Keil uVision5 调试时，`Debug → OS Support → System and Thread Viewer` 窗口打开后为空，
看不到任何 RTX5 线程，即使应用代码中已通过 `osThreadNew()` 创建了多个任务。

---

## 环境信息（已确认）

| 项目 | 内容 |
|---|---|
| 芯片 | GD32H759IM (Cortex-M7) |
| 工程路径 | `GD32RTX5\Project\gd32h7.uvprojx` |
| 编译器 | ARMCLANG V6.19 |
| RTOS | CMSIS-RTOS2 / Keil RTX5（Source 方式，非 Pack RTE） |
| 调试器 | DAP（CMSIS_AGDI） |
| **Keil uVision 版本** | **5.38.0.0**（2022年12月） |
| **SysViewer.dll 版本** | **2.30.24.0** |
| **Component_Viewer.xsd 内部 schema** | **1.1.0** |

---

## 已完成的修改

### 1. `RTX5/config/RTX_Config.h`
```c
#define OS_EVR_INIT   1   // 原为 0
```

### 2. `User/RTE_Components.h`
```c
// 新增（RTX5 5.9.1 需要两个宏同时定义）
#define RTE_Compiler_EventRecorder
#define RTE_CMSIS_View_EventRecorder
```
> 注：RTX5 5.7.0 检查 `RTE_Compiler_EventRecorder`，5.9.1 检查 `RTE_CMSIS_View_EventRecorder`，两个都保留以兼容。

### 3. `gd32h7.uvprojx` — 全局 C 编译器 Define
```xml
<Define>_RTE_</Define>   <!-- 原为空，使 rtx_def.h 能 include RTE_Components.h -->
```

### 4. `User/EventRecorder.c/.h/EventRecorderConf.h`
- 来源：`ARM.CMSIS-View 1.2.0` pack（通过 cpackget 安装，复制到 `User/`）
- 冲突修复：GD32 固件库 `gd32h7xx.h` 定义了 `typedef enum EventStatus`，与 EventRecorder 内部同名结构体冲突，在 `EventRecorder.c` 顶部规避：
```c
#define EventStatus GD_EventStatus
#include "RTE_Components.h"
#include CMSIS_device_header
#undef EventStatus
```
- 在 `gd32h7.uvprojx` User 组中注册为 FileType=1（.c）和 FileType=5（.h）

### 5. RTX5 源码升级至 5.9.1
- 原工程使用 RTX5 **5.7.0** 源码，`RTX5.scvd` 来自 **5.9.1** pack，版本不一致导致调试器用错误偏移解析 `osRtxInfo_t`
- 解决：用 `ARM.CMSIS-RTX 5.9.1` pack 中的源码覆盖 `RTX5\src\*.c` 和 `RTX5\inc\*.h`
- 验证：`rtx_os.h` 中 `osRtxVersionKernel = 50090001`（5.9.1）✅

### 6. `RTX5/config/RTX5.scvd`
- 来源：`C:\Keil_v5\ARM\PACK\ARM\CMSIS-RTX\5.9.1\RTX5.scvd`
- 在 `gd32h7.uvprojx` RTX5_Config 组注册（FileType=5）
- **schemaVersion 已改为 1.1.0**（原为 1.2.0，Keil 5.38 的 Component_Viewer.xsd 内部版本为 1.1.0，1.2.0 无法解析）

### 7. `gd32h7.uvoptx` — ARMRTXEVENTFLAGS
```
-L70 -Z18 -C0 -M1 -T1   // 原为 -M0（No RTOS），改为 -M1（RTX5 模式）
```

### 8. 安装的 Keil Pack
```powershell
$env:CMSIS_PACK_ROOT = 'C:\Keil_v5\ARM\PACK'
& "C:\Keil_v5\ARM\ctools\bin\cpackget.exe" init 'https://www.keil.com/pack/index.pidx'
& "C:\Keil_v5\ARM\ctools\bin\cpackget.exe" add 'ARM.CMSIS' 'ARM.CMSIS-View' 'ARM.CMSIS-RTX' -a
```

| Pack | 版本 | 用途 |
|---|---|---|
| ARM.CMSIS | 5.9.0 | CMSIS 核心头文件 |
| ARM.CMSIS-View | 1.2.0 | EventRecorder 真实实现 |
| ARM.CMSIS-RTX | 5.9.1 | RTX5 5.9.1 源码 + RTX5.scvd |

---

## 当前构建状态

- 全量 Rebuild：`0 Error(s), 0 Warning(s)` ✅
- 固件已通过 DAP 烧录：`Application running ...` ✅
- Program Size：`Code=117332 RO=7112 RW=39852 ZI=71012`
- `osRtxInfo` 符号地址：`0x24005edc`，size=`0xa4=164`，位于 AXI SRAM（DAP 可访问）✅

---

## 工程改动文件清单

| 文件 | 改动类型 | 说明 |
|---|---|---|
| `RTX5/config/RTX_Config.h` | 修改 | `OS_EVR_INIT` 0→1 |
| `RTX5/src/*.c` | 替换 | RTX5 源码 5.7.0 → 5.9.1 |
| `RTX5/inc/*.h` | 替换 | RTX5 头文件 5.7.0 → 5.9.1 |
| `RTX5/config/RTX5.scvd` | 新增 | 来自 ARM.CMSIS-RTX 5.9.1，schemaVersion 改为 1.1.0 |
| `User/RTE_Components.h` | 修改 | 新增 `RTE_Compiler_EventRecorder` 和 `RTE_CMSIS_View_EventRecorder` |
| `User/EventRecorder.c` | 新增 | 官方实现，含 EventStatus 冲突宏规避 |
| `User/EventRecorder.h` | 新增 | 官方头文件 |
| `User/EventRecorderConf.h` | 新增 | 官方配置 |
| `Project/gd32h7.uvprojx` | 修改 | ①全局 `<Define>_RTE_</Define>`；②User 组注册 EventRecorder 三文件；③RTX5_Config 组注册 RTX5.scvd |
| `Project/gd32h7.uvoptx` | 修改 | `ARMRTXEVENTFLAGS` 的 `-M0` → `-M1` |

---

## 仍未解决

### ❶ Thread Viewer 仍为空（未解决）

**已排除的原因**：
- RTX5 内核确实在运行（任务控制逻辑正常，排除内核未启动）
- `osRtxInfo` 符号存在且地址正确
- EventRecorder 函数已链接（`EventRecorderInitialize` 等符号均存在）
- `OS_EVR_INIT=1`、`RTE_CMSIS_View_EventRecorder` 宏均正确
- `bEvRecOn=1`、`sRSysVw=1`、`tRSysVw=1` 均已设置
- RTX5 源码版本与 scvd 版本一致（均为 5.9.1）

**当前怀疑方向**：
- Keil 5.38 对 scvd 的支持机制与新版本不同，即使改了 schemaVersion 也可能存在内容兼容性问题
- **建议升级 Keil MDK 至 5.39 或更新版本**，新版对 scvd 1.2.0 完整支持，是最彻底的解法

### ❷ "Event Recorder not located in uninitialized memory!" 警告

不影响功能，待后续在 scatter file 中为 EventRecorder 缓冲添加 `UNINIT` 段处理。


## 问题描述

Keil uVision5 调试时，`Debug → OS Support → System and Thread Viewer` 窗口打开后为空，
看不到任何 RTX5 线程，即使应用代码中已通过 `osThreadNew()` 创建了多个任务。

---

## 环境信息

| 项目 | 内容 |
|---|---|
| 芯片 | GD32H759IM (Cortex-M7) |
| 工程路径 | `GD32RTX5\Project\gd32h7.uvprojx` |
| 编译器 | ARMCLANG V6.19 |
| RTOS | CMSIS-RTOS2 / Keil RTX5（Source 方式，非 Pack RTE） |
| 调试器 | DAP（CMSIS_AGDI） |
| Keil 版本 | uVision5，安装于 `C:\Keil_v5` |

---

## 根因分析

Thread Viewer 需要以下三个条件**同时满足**：

1. **真实的 Event Recorder 实现**：RTX5 通过 `EventRecord2/4()` 把线程状态变化写入环形缓冲，调试器从缓冲中读取并显示。
2. **RTX5.scvd 描述文件注册到工程**：调试器需要此文件解析事件 ID 与线程信息。
3. **RTX5 内核必须真正启动**：`osKernelStart()` 执行之前，没有任何线程可以显示。

原始工程存在如下问题：

| # | 问题 | 位置 |
|---|---|---|
| 1 | `OS_EVR_INIT=0`，Event Recorder 不自动初始化 | `RTX5/config/RTX_Config.h` |
| 2 | `RTE_Components.h` 缺少 `#define RTE_Compiler_EventRecorder`，RTX5 编译时走 fallback 分支，发出 `#warning` 并跳过 EventRecorder 注册 | `User/RTE_Components.h` |
| 3 | 工程全局 Defines 缺少 `_RTE_`，导致 `rtx_def.h` 不 `#include "RTE_Components.h"`，前述宏对 RTX5 源文件不可见 | `gd32h7.uvprojx` → `<Define>` |
| 4 | 工程和 Keil 安装目录均无 `EventRecorder.c/.h/EventRecorderConf.h`，链接时出现 6 个 `L6218E Undefined symbol` | 全局 |
| 5 | 无 `RTX5.scvd`，调试器无法解析 RTX5 事件 | 全局 |
| 6 | **`main.c` 中 `osKernelStart()` 之前有一段已注释但仍疑似阻塞的 `while(1)` 测试代码**，调试时程序实际是否走到 `osKernelStart()` 需确认（当前 `g_Sys_1ms_Counter` 在增长，说明 TIMER6 ISR 正常，但 Thread Viewer 仍空，需单步确认内核是否真正启动） | `User/main.c` 第 195-199 行 |

---

## 已完成的修改

### 1. `RTX5/config/RTX_Config.h`

```c
// 改前
#define OS_EVR_INIT   0
// 改后
#define OS_EVR_INIT   1
```

### 2. `User/RTE_Components.h`

```c
// 新增
#define RTE_Compiler_EventRecorder
```

### 3. `gd32h7.uvprojx` — 全局 C 编译器 Define

```xml
<!-- 改前 -->
<Define></Define>
<!-- 改后 -->
<Define>_RTE_</Define>
```

### 4. `User/EventRecorder.c` / `User/EventRecorder.h` / `User/EventRecorderConf.h`

- 首次：自建最小占位 shim（可链接，但不产生事件数据）。
- **最终**：用 `cpackget` 安装官方 pack 后，替换为 pack 中真实实现：
  - `C:\Keil_v5\ARM\PACK\ARM\CMSIS-View\1.2.0\EventRecorder\Source\EventRecorder.c`
  - `C:\Keil_v5\ARM\PACK\ARM\CMSIS-View\1.2.0\EventRecorder\Include\EventRecorder.h`
  - `C:\Keil_v5\ARM\PACK\ARM\CMSIS-View\1.2.0\EventRecorder\Config\EventRecorderConf.h`
- 文件均复制到 `User/` 目录并在工程 User 组中注册。

**已知冲突修复**：GD32 固件库 `gd32h7xx.h` 中存在 `typedef enum { DISABLE, ENABLE } EventStatus;`，与 EventRecorder 内部同名结构体冲突，在 `EventRecorder.c` 开头用宏规避：

```c
// 在 #include CMSIS_device_header 之前
#define EventStatus GD_EventStatus
#include "RTE_Components.h"
#include CMSIS_device_header
#undef EventStatus
```

### 5. `RTX5/config/RTX5.scvd`

- 来源：`C:\Keil_v5\ARM\PACK\ARM\CMSIS-RTX\5.9.1\RTX5.scvd`（安装后复制）
- 在 `gd32h7.uvprojx` RTX5_Config 组中以 FileType=5 注册：

```xml
<File>
  <FileName>RTX5.scvd</FileName>
  <FileType>5</FileType>
  <FilePath>..\RTX5\config\RTX5.scvd</FilePath>
</File>
```

### 6. 安装的 Keil Pack（通过 cpackget）

```powershell
$env:CMSIS_PACK_ROOT = 'C:\Keil_v5\ARM\PACK'
& "C:\Keil_v5\ARM\ctools\bin\cpackget.exe" init 'https://www.keil.com/pack/index.pidx'
& "C:\Keil_v5\ARM\ctools\bin\cpackget.exe" add 'ARM.CMSIS' 'ARM.CMSIS-View' 'ARM.CMSIS-RTX' -a
```

| Pack | 版本 | 用途 |
|---|---|---|
| ARM.CMSIS | 5.9.0 | RTX5 源码、CMSIS 核心 |
| ARM.CMSIS-View | 1.2.0 | EventRecorder 真实实现 |
| ARM.CMSIS-RTX | 5.9.1 | RTX5.scvd 调试描述文件 |

---

## 当前构建状态

- 全量 Rebuild：`0 Error(s), 0 Warning(s)` ✅
- 固件已通过 DAP 烧录：`Application running ...` ✅
- Program Size：`Code=117852 RO=7092 RW=39852 ZI=71012`

---

## 仍未解决 / 待确认事项

### ❶ Thread Viewer 仍为空（最高优先级）

**当前现象**：Keil 调试器已连接、程序已运行（`g_Sys_1ms_Counter` 持续增长），但 `System and Thread Viewer` 窗口无任何线程条目。

**最可能的原因**：

> `main()` 第 195-199 行有一段临时测试代码 `while(1)` 被注释包裹，但在调试截图中，程序当前停在 `main():163`（刚进 `main()`），**尚未确认程序是否真正执行到 `osKernelStart()`**。

**下一步排查步骤**：

1. 在 Keil 中打开 `User/main.c`，在 `osKernelStart()` 行（约第 209 行）设一个断点。
2. 按 F5 全速运行，看程序是否能到达该断点。
3. 如果**无法到达**：说明前面某处有阻塞或 HardFault，需检查：
   - `main.c` 第 195-199 行的注释是否完整（`while(1)` 是否真的被注释掉）
   - `CanTest_Init()` 或 `SysTimer_Init()` 中是否有死等
   - 是否有 HardFault（查 Registers 面板的 `CFSR/HFSR`）
4. 如果**能到达**：继续 F5，等进入任意线程后暂停，Thread Viewer 应出现线程列表。

### ❷ "Event Recorder not located in uninitialized memory!" 警告

**影响**：复位/断点后 Event Recorder 缓冲被清零，历史事件丢失。**不影响 Thread Viewer 功能本身**，可后续处理。

**修复方法**（待处理）：  
在 scatter file（`.sct`）中添加 NoInit 段并修改 `EventRecorderConf.h` 或在链接器 scatter 中为 EventRecorder 缓冲指定 `UNINIT` 属性。

---

## 工程改动文件清单

| 文件 | 改动类型 | 说明 |
|---|---|---|
| `RTX5/config/RTX_Config.h` | 修改 | `OS_EVR_INIT` 0→1 |
| `User/RTE_Components.h` | 修改 | 新增 `#define RTE_Compiler_EventRecorder` |
| `User/EventRecorder.c` | 新增 | 官方实现（含 `EventStatus` 冲突宏规避） |
| `User/EventRecorder.h` | 新增 | 官方头文件 |
| `User/EventRecorderConf.h` | 新增 | 官方配置（EVENT_RECORD_COUNT=64, TS_SOURCE=DWT） |
| `RTX5/config/RTX5.scvd` | 新增 | 来自 ARM.CMSIS-RTX 5.9.1 pack |
| `Project/gd32h7.uvprojx` | 修改 | ① `<Define>_RTE_</Define>`；② User 组增加 EventRecorder 三文件；③ RTX5_Config 组增加 RTX5.scvd |
