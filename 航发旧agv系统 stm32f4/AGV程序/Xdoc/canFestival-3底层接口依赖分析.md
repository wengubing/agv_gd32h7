# canFestival-3 协议底层接口依赖分析

> 分析工程：ACC_Develop  
> 分析日期：2024年  
> 协议路径：`Drivers/CanOpen/CanFestival-3/`

---

## 一、架构层次总览

canFestival-3 在本工程中采用**分层架构**，自下而上依次为：

```
┌─────────────────────────────────────────────────────┐
│          应用层  (User / AgvChassisController)        │
├─────────────────────────────────────────────────────┤
│     CANopen主站管理  (LinkCanopenMaster.c/.h)          │
│  - 主站状态机  - SDO参数配置  - PDO数据交互             │
│  - 心跳监控   - NMT管理      - 从站上下线管理           │
├─────────────────────────────────────────────────────┤
│         canFestival-3 协议栈内核  (src/)               │
│  timer.c / pdo.c / sdo.c / nmt*.c / sync.c           │
│  emcy.c / lifegrd.c / lss.c / states.c / objacces.c  │
├───────────────────┬─────────────────────────────────┤
│  CAN 收发适配层    │       定时器驱动适配层             │
│  canSend()        │  drivers/STM32/canfestival_timer.c│
│  Tsk_CanOpen_RxMsgPro() │ setTimer() / getElapsedTime()│
├───────────────────┴─────────────────────────────────┤
│            底层硬件驱动 (Drivers/src/drv_can.c)        │
│  Set_CAN_Int()  can_send_msg()  CAN_RX/TX_FIFO       │
├─────────────────────────────────────────────────────┤
│           STM32 标准外设库 (Libraries/STM32F4xx)       │
│  CAN1/CAN2 硬件控制器  |  TIM12 硬件定时器             │
└─────────────────────────────────────────────────────┘
```

---

## 二、CAN 收发底层接口依赖

### 2.1 底层 CAN 驱动接口

**文件：** `Drivers/inc/drv_can.h` 、 `Drivers/src/drv_can.c`

| 接口函数/类型 | 说明 |
|---|---|
| `Set_CAN_Int(CAN_ID CANx, CAN_BAUD_TYPE can_baud, u32 addr, u8 can_pi)` | CAN 控制器初始化，配置波特率、滤波器地址、中断优先级 |
| `can_send_msg(CAN_ID Canx, CanTxMsg TxMessage)` | 发送单帧 CAN 报文 |
| `SYS_CAN1_RX_IRQHandler()` | CAN1 接收中断服务函数（映射至 `CAN1_RX0_IRQHandler`） |
| `SYS_CAN1_SCE_IRQHandler()` | CAN1 状态/错误中断 |
| `SYS_CAN2_RX_IRQHandler()` | CAN2 接收中断服务函数（映射至 `CAN2_RX0_IRQHandler`） |
| `SYS_CAN2_SCE_IRQHandler()` | CAN2 状态/错误中断 |
| `STM32_CAN[CANn]`（全局变量） | CAN 控制器实例数组（CAN1/CAN2），内含 RX/TX FIFO 缓冲区 |

**硬件资源：**
- CAN1：GPIOB Pin8（RX）/ Pin9（TX），APB1 总线，时钟 `RCC_APB1Periph_CAN1`
- CAN2：GPIOB Pin5（RX）/ Pin6（TX），APB1 总线，时钟 `RCC_APB1Periph_CAN2`
- RX/TX FIFO 深度：各 512 帧（`CAN_RX_BUF_LENGTH_MAX` / `CAN_TX_BUF_LENGTH_MAX`）

**支持波特率（`CAN_BAUD_TYPE`枚举）：**  
5K、10K、20K、40K、50K、80K、100K、125K、200K、250K、400K、500K、800K、1M

### 2.2 canFestival CAN 收发适配接口

**文件：** `example/LinkCanopenMaster.c`、`include/STM32/can_STM32.h`

#### （1）发送接口：`canSend()`

```c
// 由 canFestival 协议栈调用，将 CANopen Message 写入 CAN TX FIFO
unsigned char canSend(CAN_PORT notused, Message *m);
```

**实现流程：**
```
canFestival 协议栈内核
    → 调用 canSend(CAN_PORT, Message *)
        → 取 STM32_CAN[useCAN1].tx_fifo 写指针
        → 将 Message 转换为 CanTxMsg（StdId、RTR、IDE、DLC、Data[8]）
        → 写入 TX FIFO，更新 write_adr（环形缓冲）
        ← 返回 0x00（成功）
主循环任务 Tsk_CanOpen_TxMsgPro()
    → 从 TX FIFO 读取报文
    → 调用 can_send_msg() 发送到硬件
```

#### （2）接收接口：`Tsk_CanOpen_RxMsgPro()`

```c
// 应用层周期性调用，从 RX FIFO 取出报文提交给协议栈处理
void Tsk_CanOpen_RxMsgPro(CO_Data* d, STM32_CANi *PCAN);
```

**实现流程：**
```
CAN1_RX0_IRQHandler（中断）
    → 硬件接收 CAN 帧
    → 写入 STM32_CAN[useCAN1].rx_fifo（环形缓冲）

主循环任务 Tsk_CanOpen_RxMsgPro()
    → 从 RX FIFO 读取 CanRxMsg
    → 转换为 canFestival Message 结构
    → 调用 canDispatch(d, &m) 提交给协议栈
```

#### （3）canFestival 驱动层标准接口（`can_driver.h`）

这是 canFestival 协议栈对外定义的 **平台移植接口**，任何平台移植必须实现：

```c
UNS8 canReceive_driver(CAN_HANDLE, Message *);        // 接收一帧
UNS8 canSend_driver(CAN_HANDLE, Message const *);     // 发送一帧
CAN_HANDLE canOpen_driver(s_BOARD *);                 // 打开 CAN 总线
int canClose_driver(CAN_HANDLE);                      // 关闭 CAN 总线
UNS8 canChangeBaudRate_driver(CAN_HANDLE, char *);    // 改变波特率
```

> **注意：** 本工程 STM32 移植中，`canSend_driver` 对应 `canSend()`，接收通过中断+FIFO+轮询任务方式实现，无直接对应的 `canReceive_driver`。

#### （4）CANopen Message 数据结构（`include/can.h`）

```c
typedef struct {
    UNS16 cob_id;   // CAN 通信对象标识符（11位标准帧）
    UNS8  rtr;      // 远程帧标志（0=数据帧，1=远程帧）
    UNS8  len;      // 数据长度（0~8字节）
    UNS8  data[8];  // 数据内容
} Message;
```

---

## 三、定时器中断底层接口依赖

### 3.1 canFestival 定时器抽象接口（`include/timer.h`）

这是 canFestival 协议栈要求平台必须实现的 **定时器移植接口**：

| 接口函数 | 说明 |
|---|---|
| `void setTimer(TIMEVAL value)` | 设置下次定时器中断触发时间（相对当前值） |
| `TIMEVAL getElapsedTime(void)` | 获取自上次中断以来已流逝的时间（单位：μs） |
| `void TimeDispatch(void)` | 由定时器中断调用，驱动协议栈软定时器分发 |

**软定时器管理接口（协议栈内部）：**

```c
// 设置软件定时器（一次性或周期性）
TIMER_HANDLE SetAlarm(CO_Data* d, UNS32 id, TimerCallback_t callback, 
                      TIMEVAL value, TIMEVAL period);

// 删除软件定时器
TIMER_HANDLE DelAlarm(TIMER_HANDLE handle);

// 定时器回调函数类型
typedef void (*TimerCallback_t)(CO_Data* d, UNS32 id);
```

**时间值定义（`include/STM32/timerscfg.h`）：**

```c
#define TIMEVAL          UNS32          // 32位无符号整数
#define TIMEVAL_MAX      0xFFFF         // 最大定时值（65535 μs）
#define MS_TO_TIMEVAL(ms) ((ms) * 1000) // 毫秒转换为微秒
#define US_TO_TIMEVAL(us) (us)          // 微秒直接使用
```

### 3.2 STM32 定时器驱动实现（`drivers/STM32/canfestival_timer.c`）

**使用的硬件资源（`example/LinkCanopenMaster.h`）：**

| 配置项 | 值 | 说明 |
|---|---|---|
| `CanFestivalTimer_Base` | `TIM12` | 使用 TIM12 |
| `CanFestivalTimer_CLK` | `RCC_APB1Periph_TIM12` | APB1 总线时钟 |
| `CanFestivalTimer_IT` | `TIM_IT_CC1` | 输出比较通道1中断 |
| `CanFestivalTimer_FLAG` | `TIM_FLAG_CC1` | 比较标志位 |
| `CanFestivalTimer_IRQn` | `TIM8_BRK_TIM12_IRQn` | NVIC 中断号 |
| `CanFestivalTimer_IRQHandler` | `TIM8_BRK_TIM12_IRQHandler` | 中断服务函数名 |
| `CanFestivalTimer_PSC_Val` | `80` | 预分频（80MHz/80=1MHz=1μs/tick） |
| `CanFestivalTimer_ARR_Val` | `0xFFFF` | 自动重装值（65535） |
| `CanFestivalTimer_CCRX_Val` | `1000` | 初始比较值（1ms） |

**定时器驱动接口函数：**

```c
// 初始化 TIM12，配置预分频、自动重装、输出比较、NVIC中断优先级
void CanFestivalTimer_Config(TIM_TypeDef *Timx, UNS32 Rcc, UNS8 NvicIrq,
                             INTEGER8 pi, FunctionalState NewState);

// 使能/禁止定时器计数（TIM_Cmd）
void CanFestivalTimer_Enable(FunctionalState NewState);

// 使能/禁止定时器比较中断（TIM_ITConfig）
void CanFestivalTimer_Ctrl(FunctionalState NewState);

// 中断服务函数：清中断标志 → 调用 CanFestivalTimer_Dispatch()
void CanFestivalTimer_IRQHandler(void);

// 分发函数：记录当前计数值 → 调用 TimeDispatch()（驱动协议栈软定时器）
void CanFestivalTimer_Dispatch(void);
```

**关键实现原理：**

```
TIM12 硬件定时器（1μs分辨率，自由运行0~0xFFFF）
    |
    | 输出比较中断（CC1）
    ↓
CanFestivalTimer_IRQHandler()
    → 清除中断标志
    → CanFestivalTimer_Dispatch()
        → last_time_set = TIM_GetCounter(TIM12)  // 记录当前时刻
        → TimeDispatch()                          // 协议栈软定时器处理
            → 遍历 timers[] 表
            → 触发到期的 TimerCallback_t 回调
            → 通过 setTimer() 设置下次中断时刻

setTimer(value):
    capture = TIM_GetCapture1(TIM12)
    TIM_SetCompare1(TIM12, capture + value)   // 下次中断 = 当前比较值 + 延时

getElapsedTime():
    return TIM_GetCounter(TIM12) - last_time_set  // 已流逝时间（μs）
```

---

## 四、任务调用接口依赖

### 4.1 无 RTOS 强依赖，采用"中断+轮询"模式

本工程 canFestival-3 未直接使用 RTOS 任务管理 API，而是通过以下方式运行：

| 机制 | 说明 |
|---|---|
| **CAN 接收** | 由 `CAN1_RX0_IRQHandler` 中断将报文存入 RX FIFO |
| **CAN 发送** | 由主循环任务 `Tsk_CanOpen_TxMsgPro()` 从 TX FIFO 发出 |
| **协议栈驱动** | 由 `TIM12` 中断驱动 `TimeDispatch()` 定时触发 |
| **主站状态机** | 由主循环调用 `tsk_canopen_master()` 轮询推进 |

> **注释线索（`canfestival_timer.c` 第137行）：**  
> `// isr_sem_send (&sem_CanOpenDispatch); // 给任务发送信号量事件，用RTOS去任务执行`  
> 该行被注释掉，说明**预留了 RTOS 信号量方式**，当前使用中断直接调用。

### 4.2 应用层任务函数

**文件：** `example/LinkCanopenMaster.h` / `LinkCanopenMaster.c`

```c
// CANopen 主站主任务（状态机轮询，由主循环周期调用）
void tsk_canopen_master(LinkCanOpen *pCm, CO_Data* d, STM32_CANi *p_stCAN);

// CAN 接收报文处理任务（从 RX FIFO 取报文提交给协议栈）
void Tsk_CanOpen_RxMsgPro(CO_Data* d, STM32_CANi *PCAN);

// CAN 发送报文处理任务（从 TX FIFO 取报文调用底层发送）
void Tsk_CanOpen_TxMsgPro(CO_Data* d, STM32_CANi *PCAN);

// CANopen 初始化（配置CAN硬件 + 启动定时器 + 初始化协议栈）
void can_master_init(STM32_CANi *p_stCAN, CO_Data* d, FunctionalState NewState);

// 定时器启动（封装 CanFestivalTimer_Config()）
void canfestival_timer_start(u8 pi, FunctionalState NewState);
```

### 4.3 协议栈事件回调（需用户实现）

canFestival-3 通过**回调函数**通知应用层协议事件，本工程在 `LinkCanopenMaster.c` 中实现：

```c
// 心跳超时回调
void CanObjectDict_heartbeatError(CO_Data* d, UNS8 nodeId);

// 节点状态变化回调（对应协议栈状态机）
void CanObjectDict_initialisation(CO_Data* d);
void CanObjectDict_preOperational(CO_Data* d);
void CanObjectDict_operational(CO_Data* d);
void CanObjectDict_stopped(CO_Data* d);

// SYNC 报文发送后回调
void CanObjectDict_post_sync(CO_Data* d);

// TPDO 发送后回调
void CanObjectDict_post_TPDO(CO_Data* d);

// 紧急报文回调
void CanObjectDict_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);

// 从站启动完成回调
void CanObjectDict_post_SlaveBootup(CO_Data* d, UNS8 nodeid);
```

---

## 五、初始化调用流程

```
系统启动
    ↓
can_master_init(p_stCAN, &CanObjectDict_Data, ENABLE)
    ├── Set_CAN_Int(useCAN1, baud, addr, pi)    // 初始化CAN1硬件
    ├── setState(&CanObjectDict_Data, Initialisation)  // 协议栈状态机初始化
    └── canfestival_timer_start(pi, ENABLE)
            └── CanFestivalTimer_Config(TIM12, ...)   // 初始化TIM12定时器

主循环（周期运行）
    ├── Tsk_CanOpen_RxMsgPro(&CanObjectDict_Data, p_stCAN)  // RX 报文处理
    ├── Tsk_CanOpen_TxMsgPro(&CanObjectDict_Data, p_stCAN)  // TX 报文发送
    └── tsk_canopen_master(&CanMaster, &CanObjectDict_Data, p_stCAN)  // 主站状态机

TIM12 比较中断（每次到期）
    └── CanFestivalTimer_IRQHandler()
            └── TimeDispatch()        // 软定时器分发（PDO/SDO/NMT/SYNC/心跳定时事件）

CAN1_RX0_IRQHandler（每收到一帧）
    └── 写入 STM32_CAN[useCAN1].rx_fifo
```

---

## 六、配置参数汇总

**文件：** `include/config.h`（由应用层 `ObjectDictionary.h` 重定义）

| 配置项 | 值 | 说明 |
|---|---|---|
| `MAX_NodeId_NUM` | 27 | 最大支持从站数量 |
| `MAX_NodeId_Support` | 32 | 固定最大从站数 |
| `MAX_NB_TIMER` | MAX_NodeId_NUM+5 | 软定时器表大小 |
| `SDO_MAX_SIMULTANEOUS_TRANSFERS` | MAX_NodeId_NUM | 最大并发SDO传输数 |
| `SDO_TIMEOUT_MS` | 3000 | SDO 超时时间（ms） |
| `Producer_Heartbeat_Times` | 300 | 主站心跳周期（ms） |
| `SYNC_CommunicationCyclePeriod` | 20 | SYNC 报文周期（ms） |
| `NMT_GuardTime` | 200 | NMT 节点守护时间（ms） |

---

## 七、接口依赖关系汇总表

| 依赖类型 | 接口/函数 | 文件位置 | 方向 |
|---|---|---|---|
| **CAN 初始化** | `Set_CAN_Int()` | `Drivers/src/drv_can.c` | 应用→驱动 |
| **CAN 发送（底层）** | `can_send_msg()` | `Drivers/src/drv_can.c` | 应用→驱动 |
| **CAN 发送（协议栈）** | `canSend()` | `example/LinkCanopenMaster.c` | 协议栈→适配层 |
| **CAN 接收中断** | `CAN1_RX0_IRQHandler` | `Drivers/src/drv_can.c` | 硬件→驱动（中断） |
| **RX 报文分发** | `canDispatch()` | `canFestival-3/src/` | 适配层→协议栈 |
| **定时器初始化** | `CanFestivalTimer_Config()` | `drivers/STM32/canfestival_timer.c` | 应用→驱动 |
| **定时器中断** | `TIM8_BRK_TIM12_IRQHandler` | `drivers/STM32/canfestival_timer.c` | 硬件→驱动（中断） |
| **软定时器触发** | `TimeDispatch()` | `canFestival-3/src/timer.c` | 驱动→协议栈 |
| **设置下次中断时刻** | `setTimer()` | `drivers/STM32/canfestival_timer.c` | 协议栈→驱动 |
| **读取流逝时间** | `getElapsedTime()` | `drivers/STM32/canfestival_timer.c` | 协议栈→驱动 |
| **主站任务轮询** | `tsk_canopen_master()` | `example/LinkCanopenMaster.c` | 主循环→应用 |
| **事件回调** | `CanObjectDict_*` 系列 | `example/LinkCanopenMaster.c` | 协议栈→应用 |
