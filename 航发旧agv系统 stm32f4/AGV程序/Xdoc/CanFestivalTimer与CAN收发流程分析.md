# CanFestivalTimer 工作机制与 CAN 收发完整流程分析

> 工程：航发旧AGV系统 STM32F4 / AGV程序  
> 分析路径：`Drivers/CanOpen/CanFestival-3/drivers/STM32/canfestival_timer.c`  
> 分析日期：2026年3月

---

## 一、CanFestivalTimer 工作机制

### 1. 硬件定时器配置（`CanFestivalTimer_Config()`）

| 配置项 | 值 | 说明 |
|---|---|---|
| **硬件外设** | TIM12 | APB1 总线上的通用定时器 |
| **时钟源** | APB1 = 80MHz | 代码注释标明 |
| **预分频 PSC** | 80 − 1 | 80MHz ÷ 80 = **1MHz → 每个 tick = 1 μs** |
| **自动重装 ARR** | 0xFFFF（65535） | 计数器自由运行 0→65535 循环 |
| **比较寄存器 CCR1** | 初始值 = 1000 | 第一次中断在 **1 ms** 后触发 |
| **比较模式** | `TIM_OCMode_Timing`（纯定时，不输出引脚） | 达到 CCR1 值时产生 CC1 中断 |
| **CCR 预装载** | **禁用** | 写入 CCR1 立即生效，不等下一个 ARR 溢出 |
| **中断号** | `TIM8_BRK_TIM12_IRQn` | TIM8 刹车 & TIM12 共用同一中断向量 |
| **中断抢占优先级** | `pi`（由调用者传入） | SubPriority = 0 |

宏定义汇总（`LinkCanopenMaster.h`）：

```c
#define CanFestivalTimer_Base        TIM12
#define CanFestivalTimer_CLK         RCC_APB1Periph_TIM12
#define CanFestivalTimer_IT          TIM_IT_CC1
#define CanFestivalTimer_FLAG        TIM_FLAG_CC1
#define CanFestivalTimer_IRQn        TIM8_BRK_TIM12_IRQn
#define CanFestivalTimer_IRQHandler  TIM8_BRK_TIM12_IRQHandler
#define CanFestivalTimer_PSC_Val     80       // 80MHz / 80 = 1MHz = 1μs/tick
#define CanFestivalTimer_ARR_Val     0xFFFF   // 自由运行最大值
#define CanFestivalTimer_CCRX_Val    1000     // 初始比较值（1ms）
```

---

### 2. setTimer / getElapsedTime 工作原理

TIM12 的计数器**永远自由运行，不重置**。CanFestival 的"定时"通过**滑动输出比较**实现：

```c
// 设置下次中断触发时刻（相对当前比较值偏移 value 个 μs）
void setTimer(TIMEVAL value)
{
    UNS32 capture = TIM_GetCapture1(TIM12);       // 读当前比较寄存器值
    TIM_SetCompare1(TIM12, capture + value);       // 下次中断 = 当前比较值 + 延时(μs)
}

// 获取自上次中断以来已流逝的时间（μs）
TIMEVAL getElapsedTime(void)
{
    UNS16 timer = TIM_GetCounter(TIM12);           // 读当前计数器值
    return timer >= last_time_set
           ? timer - last_time_set
           : last_time_set - timer;                // 粗略处理 16-bit 回绕
}
```

---

### 3. 中断服务函数内部逻辑

```
TIM8_BRK_TIM12_IRQHandler()            ← 硬件 CC1 比较匹配，产生中断
  │
  ├─ TIM_GetITStatus(TIM12, TIM_IT_CC1) 确认是 CC1 中断
  ├─ TIM_ClearITPendingBit()            ← 清除中断标志
  └─ CanFestivalTimer_Dispatch()
       ├─ last_time_set = TIM_GetCounter(TIM12)   ← 记录"本次中断时刻"
       └─ TimeDispatch()                          ← 驱动协议栈软定时器
            ├─ overrun = getElapsedTime()         ← 计算已超出时间
            ├─ real_total_sleep_time = total_sleep_time + overrun
            ├─ 遍历 timers[] 软定时器表（MAX_NB_TIMER = NodeNum+5 个槽位）
            │     ├─ 已到期（一次性） → 标记 TIMER_TRIG
            │     ├─ 已到期（周期性） → 重置 val = interval - overrun%interval，标记 TIMER_TRIG_PERIOD
            │     └─ 未到期          → val -= real_total_sleep_time，更新 next_wakeup
            ├─ total_sleep_time = next_wakeup
            ├─ setTimer(next_wakeup)              ← 设置下次中断触发时刻
            └─ 依次执行所有 TIMER_TRIG 的 callback(d, id)
                 （PDO定时发送、SDO超时、SYNC周期、心跳、NMT守护等）
```

> **注意：** 中断里注释掉了 `isr_sem_send(&sem_CanOpenDispatch)`，说明**预留了 RTOS 信号量接口**，当前裸机模式下直接在中断中调用 `TimeDispatch()`。

---

## 二、从硬件收到一帧数据到回复发送到总线的完整路径

以 **SDO 请求/应答** 为典型场景（PDO、心跳应答等路径类似）：

### 路径总览

```
总线来帧
  ↓ [硬件 CAN 控制器]
  ① CAN1_RX0 中断 → 写入软件 RX FIFO（ring buffer）
  ↓ [主循环轮询]
  ② Tsk_CanOpen_RxMsgPro() → 从 RX FIFO 取帧 → 转换结构体 → canDispatch()
  ↓ [canFestival 协议栈]
  ③ canDispatch() 按 COB-ID 路由 → proceedSDO() / proceedPDO() / ...
  ↓ [协议栈生成回复]
  ④ canSend() → 写入软件 TX FIFO（ring buffer）
  ↓ [主循环轮询]
  ⑤ Tsk_CanOpen_TxMsgPro() → 从 TX FIFO 取帧 → can_send_msg() → CAN 硬件邮箱
  ↓ [硬件 CAN 控制器]
  回复帧出现在总线上
```

---

### 详细步骤说明

#### ① 硬件层：CAN 控制器中断接收

```c
// drv_can.c
void SYS_CAN1_RX_IRQHandler(void)   // 映射至 CAN1_RX0_IRQHandler
{
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET)  // FIFO0 有新帧
    {
        while (CAN_MessagePending(CAN1, CAN_FIFO0))
        {
            CAN_Receive(CAN1, CAN_FIFO0,
                &STM32_CAN[useCAN1].rx_fifo.can_rx_msg[write_adr]); // 存入软件环形缓冲
            if (++write_adr >= CAN_RX_BUF_LENGTH_MAX) write_adr = 0;
        }
        CAN_FIFORelease(CAN1, CAN_FIFO0);   // 释放硬件 FIFO 槽位
    }
}
```

- 中断优先级 = `can_pi`（调用 `Set_CAN_Int()` 时传入）
- 软件 RX FIFO 深度：512 帧（`CAN_RX_BUF_LENGTH_MAX`）
- **中断只做搬运，不做任何协议栈处理**

#### ② 主循环：取帧并提交给协议栈

```c
// LinkCanopenMaster.c
void Tsk_CanOpen_RxMsgPro(CO_Data* d, STM32_CANi *p_stCAN)
{
    Message Msg;
    Msg.len    = rx_fifo.can_rx_msg[read_adr].DLC;
    Msg.cob_id = rx_fifo.can_rx_msg[read_adr].StdId;
    Msg.rtr    = rx_fifo.can_rx_msg[read_adr].RTR;
    memcpy(Msg.data, rx_fifo.can_rx_msg[read_adr].Data, Msg.len);

    CanFestivalTimer_Ctrl(DISABLE);  // 关 TIM12 比较中断，防并发
    canDispatch(d, &Msg);            // 提交给协议栈
    CanFestivalTimer_Ctrl(ENABLE);   // 恢复中断

    if (++read_adr >= CAN_RX_BUF_LENGTH_MAX) read_adr = 0;
}
```

#### ③ 协议栈层：canDispatch() 按 COB-ID 路由

```c
// states.c
void canDispatch(CO_Data* d, Message *m)
{
    switch (m->cob_id >> 7)
    {
        case SYNC:      proceedSYNC(d);       break;  // 0x000/0x080
        case PDO1tx:
        case PDO1rx:
        // ...PDO2~4
                        proceedPDO(d, m);     break;  // 更新对象字典 RPDO 映射变量
        case SDOtx:
        case SDOrx:     proceedSDO(d, m);     break;  // 解析 SDO 命令，生成应答
        case NODE_GUARD: proceedNODE_GUARD(d,m); break;
        case NMT:       proceedNMTstateChange(d,m); break;
    }
}
```

#### ④ 协议栈生成回复并写入 TX FIFO

以 SDO 为例，`proceedSDO()` 解析请求后构造应答，调用：

```c
// LinkCanopenMaster.c
unsigned char canSend(CAN_PORT notused, Message *m)
{
    CAN_TX_FIFO *pBuf = &STM32_CAN[useCAN1].tx_fifo;
    pBuf->can_Tx_msg[write_adr].StdId = m->cob_id;
    pBuf->can_Tx_msg[write_adr].RTR   = m->rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    pBuf->can_Tx_msg[write_adr].IDE   = CAN_ID_STD;
    pBuf->can_Tx_msg[write_adr].DLC   = m->len;
    for (i = 0; i < m->len; i++)
        pBuf->can_Tx_msg[write_adr].Data[i] = m->data[i];
    if (++write_adr >= CAN_RX_BUF_LENGTH_MAX) write_adr = 0;
    return 0x00;   // 立即返回，不阻塞
}
```

#### ⑤ 主循环：从 TX FIFO 取帧发送到硬件

```c
// LinkCanopenMaster.c
void Tsk_CanOpen_TxMsgPro(CO_Data* d, STM32_CANi *p_stCAN)
{
    can_send_msg(p_stCAN->ini.CANx,
                 p_stCAN->tx_fifo.can_Tx_msg[read_adr]);  // 写入 CAN 发送邮箱
    if (++read_adr >= CAN_RX_BUF_LENGTH_MAX) read_adr = 0;
}

// drv_can.c
u8 can_send_msg(CAN_ID Canx, CanTxMsg TxMessage)
{
    TransmitMailbox_Num = CAN_Transmit(CAN_BASE[Canx], &TxMessage);
    while (CAN_TransmitStatus(...) != CANTXOK && timeout < 0xFFFE)
        timeout++;                                         // 轮询等待发送完成
}
```

---

## 三、关键设计特点总结

| 特点 | 说明 |
|---|---|
| **接收：中断 → FIFO → 主循环** | 中断只搬运帧到软件 FIFO，协议栈处理在主循环，避免中断里做复杂运算 |
| **发送：协议栈 → FIFO → 主循环** | `canSend()` 只入 TX FIFO，实际发送在主循环，不阻塞协议栈 |
| **定时器：自由运行 + 滑动比较** | TIM12 计数器不重置，CC1 滑动追赶，精度 1 μs，最大周期 65535 μs |
| **并发保护** | `Tsk_CanOpen_RxMsgPro` 调用 `canDispatch` 前后禁/开 TIM12 中断，防止 `TimeDispatch` 和 `canDispatch` 同时修改协议栈状态 |
| **预留 RTOS 接口** | 中断里注释掉了 `isr_sem_send(&sem_CanOpenDispatch)`，当前裸机直接在中断调用 |
| **软件 FIFO 深度** | RX/TX 各 512 帧，环形缓冲，无锁（中断写/主循环读，单生产者单消费者） |

---

## 四、软定时器与 CANopen 协议事件对应关系

| 软定时器回调 | 触发来源 | 典型周期 |
|---|---|---|
| PDO 定时发送 | `SetAlarm()` in `pdo.c` | 由对象字典 0x1800~0x1BFF 配置 |
| SDO 超时检测 | `SetAlarm()` in `sdo.c` | `SDO_TIMEOUT_MS` = 3000 ms |
| SYNC 报文发送 | `SetAlarm()` in `sync.c` | `SYNC_CommunicationCyclePeriod` = 20 ms |
| 心跳报文发送 | `SetAlarm()` in `lifegrd.c` | `Producer_Heartbeat_Times` = 300 ms |
| NMT 节点守护 | `SetAlarm()` in `lifegrd.c` | `NMT_GuardTime` = 200 ms |
