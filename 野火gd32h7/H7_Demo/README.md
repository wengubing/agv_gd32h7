# H7_Demo

H7新硬件，野火版本测试Demo工程

## can_test 模块说明

`can_test` 位于 `User/can_test.c` 与 `User/can_test.h`，是对底层 `BSP/can/bsp_can_fd.*` 的轻量测试封装。

### 结构
- 对外接口：
  - `CanTest_Init()`：初始化 CAN 外设与收发邮箱。
  - `CanTest_SendOnce()`：发送 1 帧测试报文并输出调试信息。
  - `CanTest_KeySend()`：按键触发单次发送。
- 全局对象：
  - `transmit_message`：发送邮箱描述符。
  - `receive_message`：接收邮箱描述符。
  - `can2_receive_flag`：接收完成标志（由中断流程配合置位）。

### 功能
- 完成 CAN2 的基础测试流程：初始化、发包、收包结果判断与串口打印。
- 默认发送扩展 ID（`CANX_ID`）与 8 字节测试数据（由底层 `CAN_SetMsg` 配置）。
- 支持“上电发送一次”与“按键触发发送”两种测试方式。

### 用法
1. 在应用中包含头文件：`#include "can_test.h"`
2. 系统初始化后调用：`CanTest_Init();`
3. 需要快速自检时调用：`CanTest_SendOnce();`
4. 需要按键触发发送时，在循环或任务中周期调用：`CanTest_KeySend();`

当前 `User/main.c` 中已演示启动后初始化并发送一次测试报文。