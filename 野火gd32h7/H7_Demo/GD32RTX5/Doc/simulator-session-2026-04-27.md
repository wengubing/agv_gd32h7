# Simulator 诊断续接记录（2026-04-27）

## 1. 本次目标
- 通过 Keil UI 自动化复现并定位 simulator 启动/运行卡点。
- 让 Copilot 可自动执行“编译 -> 进调试 -> 运行 -> 监控 -> 出结论”。
- 给出可持续复现的记录，便于下一次继续排查。

## 2. 已确认问题与根因
- UI 截图确认曾出现 `error 65: access violation at 0x58024400`。
- 该问题发生在启动早期（`Reset_Handler -> SystemInit`）阶段。
- 另有启动期弹窗（improper close）会吞掉快捷键，导致看起来“按了运行但不动”。
- Keil 命令窗多次显示使用 Cortex-M4 Simulator，和 GD32H7(Cortex-M7)存在模型能力不匹配风险。

## 3. 关键修改（代码与配置）
- 启动阶段 simulator 兼容处理：
  - 文件: `CMSIS/GD/GD32H7xx/Source/system_gd32h7xx.c`
  - 修改点: `SystemInit` 在 `SIMULATOR_ENV` 下走最小化路径并提前返回，避免触发未建模外设访问。
  - 仍保留 `IRC64MSTB` 等待兼容分支保护。

- 全局仿真宏下发到 Keil 工程：
  - 文件: `Project/gd32h7.uvprojx`
  - 修改点: 多处 `<Define>` 下发 `SIMULATOR_ENV=1`，确保 system 文件也编译到 simulator 分支。

- 调试初始化脚本与内存映射：
  - 文件: `Project/simulator.ini`
  - 新增映射:
    - `MAP 0x58000000,0x58FFFFFF READ WRITE`
    - `MAP 0x40000000,0x40FFFFFF READ WRITE`
    - `MAP 0x50000000,0x50FFFFFF READ WRITE`
    - `MAP 0xE0000000,0xE00FFFFF READ WRITE`
  - 文件: `Project/gd32h7.uvoptx`
  - 修改点:
    - `sIfile` / `tIfile` 指向 `.\simulator.ini`
    - 关闭 watch 自动恢复：`sRwatch=0`, `tRwatch=0`
    - 清空 `WatchWindow1/2`（避免恢复表达式引发 error 65）

- 任务运行自检（仿真专用）：
  - 文件: `User/main.c`
  - 修改点:
    - 增加 `g_sim_led_heartbeat / g_sim_sender_heartbeat / g_sim_receiver_heartbeat`
    - `AppTaskStart` 在超时后检查心跳，未运行则 `__BKPT(0)`
    - 修复 `Sender_task` 的 `tick` 未初始化问题（已在循环前 `tick = osKernelGetTickCount();`）

- 自动化工具脚本：
  - 文件: `tools/verify-keil-simulator.ps1`
  - 功能: 编译、调试、UI 自动化、运行态诊断、JSON 报告输出。
  - 文件: `tools/capture-keil-ui.ps1`
  - 功能: 自动打开 Keil 并抓取 3 阶段截图（open/debug/run）用于人工定位。

## 4. 本次关键证据
- 自动验证报告（最新）:
  - `ztemp/sim_verify_report.json`
  - 关键结论:
    - `pass: true`
    - `reason` 包含: UI debug/run 完成，RTX5 任务自检未触发 BKPT。

- UI 截图（最新一轮）:
  - `ztemp/uv4_ui_20260427_170038_01_open.png`
  - `ztemp/uv4_ui_20260427_170038_02_debug.png`
  - `ztemp/uv4_ui_20260427_170038_03_run.png`
  - 元数据: `ztemp/uv4_ui_20260427_170038_meta.json`

- UI 截图（映射修复后一轮）:
  - `ztemp/uv4_ui_20260427_165809_01_open.png`
  - `ztemp/uv4_ui_20260427_165809_02_debug.png`
  - `ztemp/uv4_ui_20260427_165809_03_run.png`
  - 元数据: `ztemp/uv4_ui_20260427_165809_meta.json`

## 5. 当前结论
- 在“当前工程配置 + 自动化流程”下，simulator 可完成调试运行流程，且 RTX5 关键任务自检通过。
- 但 Keil 使用 M4 simulator 模型这一点仍是潜在风险，可能导致“可运行但不完全可信”的仿真行为。

## 6. 下次优先处理建议
- 优先确认是否可切换到更匹配 Cortex-M7 的仿真模型/工具链版本。
- 若仍使用当前 simulator：
  - 保持 `simulator.ini` 映射与 watch 关闭策略。
  - 继续用 `tools/capture-keil-ui.ps1` 做 UI 证据留档。
  - 保持 `User/main.c` 的心跳+BKPT 自检以便快速判定“任务是否真正调度”。

## 7. 推荐复现实验命令
- 验证脚本：
  - `powershell -ExecutionPolicy Bypass -File .\tools\verify-keil-simulator.ps1`
- UI 截图脚本：
  - `powershell -ExecutionPolicy Bypass -File .\tools\capture-keil-ui.ps1`
