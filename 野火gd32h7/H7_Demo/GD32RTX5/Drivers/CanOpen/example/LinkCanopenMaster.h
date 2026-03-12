/****************************Copyright (c)**********************************************
**				┌--------------------------┐
**				│	成都航发控制工程有限公司	│
**				│	http://www.hangfa.com	│
**				└--------------------------┘
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: LinkCanopenMaster.h
** 创 建 者: 邓杨
** 功    能: CANopen主站控制
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.1.0
** 创建时间: 2024年5月9日
** GD32H7适配: 2025年
**--------------------------------------------------------------------------------------
***************************************************************************************/

/* 保证本文件内容只被同一源文件包含一次 */
#ifndef LinkCanopenMaster_h
#define LinkCanopenMaster_h

/* 保证本文件内容只被同一源文件包含一次 */
#ifdef  LinkCanopenMaster_GLOBALS
    #define LinkCanopenMaster_EXT
#else
    #define LinkCanopenMaster_EXT  extern
#endif

/* 自定义头文件 */
#include "ObjectDictionary.h"
#include "drv_can_gd32h7.h"

#define SVO_Type  5   /* 0:金科伺服  1:台达伺服  2:威科伺服  3:惠斯通伺服  4:科亚伺服  5:Copley伺服 */

/* 驱动器工作控制模式 */
#define  DRIVER_MODE_Pulse             -4   /* Pulse Mode --- 脉冲模式 */
#define  DRIVER_MODE_ImmediateVelocity -3   /* Immediate Velocity Mode --- 立即速度模式 */
#define  DRIVER_MODE_POSATION           1   /* Profile Position Mode --- 位置控制模式 */
#define  DRIVER_MODE_VELOCITY           3   /* Profile Velocity Mode --- 速度控制模式 */
#define  DRIVER_MODE_Torque             4   /* Profile Torque Mode --- 扭矩控制模式 */
#define  DRIVER_MODE_Homing             6   /* Homing Mode --- 回零模式 */
#define  DRIVER_MODE_IntPosition        7   /* Interpolation Position Mode --- 插补位置模式 */
#define  DRIVER_MODE_CSP                8   /* Cyclic Synchronous Position Mode */
#define  DRIVER_MODE_CSV                9   /* Cyclic Synchronous Velocity Mode */
#define  DRIVER_MODE_CST               10   /* Cyclic Synchronous Torque Mode */

/* Copley驱动器工作模式
 * 0: programmed velocity mode
 * 1: Profile Velocity
 * 2: Current Loop
 */
#define CopleyWorkMode  1

#define eprintf(...)

/* CanFestival定时器配置 (TIMER2, APB1) */
#define CanFestivalTimer_Base       TIMER2
#define CanFestivalTimer_CLK        RCU_TIMER2
#define CanFestivalTimer_IT         TIMER_INT_CH0
#define CanFestivalTimer_FLAG       TIMER_INT_FLAG_CH0
#define CanFestivalTimer_IRQn       TIMER2_IRQn
#define CanFestivalTimer_IRQHandler TIMER2_IRQHandler

#define CanFestival_led             b_led3

#define USE_TPDO_CH_NUM  4   /* 最大值为4 */

#if (SVO_Type == 4)
  #define USE_RPDO_CH_NUM  2   /* 最大值为4 */
#else
  #define USE_RPDO_CH_NUM  1   /* 最大值为4 */
#endif

/* CANopen主站步骤定义 */
typedef enum
{
    Step_ParaIni            = 0,
    Step_DrivePowerDown     = 1,
    Step_DrivePowerUp       = 2,
    Step_Reset_Node         = 3,
    Step_Reset_Comunication = 4,
    Step_WaitBootUpOk       = 5,
    Step_Active             = 6,
    Step_CanOpenFault       = 7,
} CanOpenMasterStep_Type;

/* CANopen从站类型定义 */
typedef enum
{
    Can_SVOType_Null       = 0,   /* 无效 */
    Can_SVOType_Kinco      = 1,   /* 金科 */
    Can_SVOType_Delta      = 2,   /* 台达 */
    Can_SVOType_Windcon    = 3,   /* 威科 */
    Can_SVOType_Wheatstone = 4,   /* 惠斯通 */
    Can_SVOType_Keya       = 5,   /* 科亚 */
    Can_SVOType_Copley     = 6,   /* Copley */
} Can_SVOType_EnumDef;

/* 参数信息 数据段定义 */
typedef struct
{
    uint8_t               CheckQuantity;   /* 检测数量 */
    CAN_BAUD_TYPE         Baud;            /* 波特率 */
    Can_SVOType_EnumDef   SVOType;         /* 伺服类型 */
} ACC_ParaCanOpen_StDef;

/* CANOPEN主站错误信息 位域定义 */
typedef struct
{
    unsigned int CanBusErr  : 1;   /* CAN总线异常 断线时 */
    unsigned int CanNoSlv   : 1;   /* 没有can从站上线 */
    unsigned int CanLessSlv : 1;   /* can从站上线数量少 */
} ACC_CanOpenErr_BitDef;

/* CANopen主站控制器 */
typedef struct
{
    ACC_ParaCanOpen_StDef  Para;                            /* CANopen参数 */

    /* 外部变量 */
    bool                   in_CanOpenStart;                 /* CANopen启动 */

    CanOpenMasterStep_Type CanopenStep;                     /* CANopen主站步骤 */
    unsigned short         RepeatConnectCnt;                /* 重连次数 */
    unsigned short         SlaveOnLineNum;                  /* 上线数量 */
    unsigned short         TxCnt;                           /* 发送计数 */
    enum enum_nodeState    MasterStatu;                     /* 主站状态 */
    uint8_t                ConfigStep[MAX_NodeId_NUM];      /* 配置步骤 */
    uint8_t                SlaveParaConfigId[MAX_NodeId_NUM]; /* 从站参数配置ID */
    uint8_t                HeartMsgId[MAX_NodeId_NUM];      /* 心跳消息ID */
    unsigned char          PostrigerCnt[MAX_NodeId_NUM];
    int32_t                HeartJfcCnt;                     /* 心跳包计时 */

    ACC_CanOpenErr_BitDef  Err;                             /* 错误状态 */
} LinkCanOpen;

/* 全局对象定义 */
LinkCanopenMaster_EXT LinkCanOpen    CanMaster;   /* CANopen主站控制器 */

/* 全局函数声明 */
LinkCanopenMaster_EXT obj_link_sdo Obj_Clentx_SDO[MAX_NodeId_NUM];
LinkCanopenMaster_EXT obj_link_pdo Obj_Clentx_TPDO[USE_TPDO_CH_NUM][MAX_NodeId_NUM];
LinkCanopenMaster_EXT obj_link_pdo Obj_Clentx_RPDO[USE_RPDO_CH_NUM][MAX_NodeId_NUM];

LinkCanopenMaster_EXT void  CanObjectDict_heartbeatError(CO_Data* d, UNS8 nodeId);
LinkCanopenMaster_EXT UNS8  CanObjectDict_canSend(Message *m);
LinkCanopenMaster_EXT void  CanObjectDict_initialisation(CO_Data* d);
LinkCanopenMaster_EXT void  CanObjectDict_preOperational(CO_Data* d);
LinkCanopenMaster_EXT void  CanObjectDict_operational(CO_Data* d);
LinkCanopenMaster_EXT void  CanObjectDict_stopped(CO_Data* d);
LinkCanopenMaster_EXT void  CanObjectDict_post_sync(CO_Data* d);
LinkCanopenMaster_EXT void  CanObjectDict_post_TPDO(CO_Data* d);
LinkCanopenMaster_EXT void  CanObjectDict_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
LinkCanopenMaster_EXT void  CanObjectDict_post_SlaveBootup(CO_Data* d, UNS8 nodeid);

LinkCanopenMaster_EXT void  tsk_canopen_master(LinkCanOpen *pCm, CO_Data* d);
LinkCanopenMaster_EXT void  CanFestivalTimer_Dispatch(void);
LinkCanopenMaster_EXT void  CanFestivalTimer_Ctrl(FunctionalState NewState);

LinkCanopenMaster_EXT void  canfestival_timer_start(uint8_t pi, FunctionalState NewState);
LinkCanopenMaster_EXT void  can_master_init(CO_Data* d, FunctionalState NewState);
LinkCanopenMaster_EXT UNS8  canSend(CAN_PORT notused, Message *m);

LinkCanopenMaster_EXT void  Tsk_CanOpen_RxMsgPro(CO_Data* d);
LinkCanopenMaster_EXT void  Tsk_CanOpen_TxMsgPro(CO_Data* d);

#endif

/* ***********************************END OF FILE*********************************** */
