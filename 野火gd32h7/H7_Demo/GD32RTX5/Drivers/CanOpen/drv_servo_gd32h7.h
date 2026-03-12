/****************************Copyright (c)**********************************************
**				┌--------------------------┐
**				│	成都航发控制工程有限公司	│
**				│	http://www.hangfa.com	│
**				└--------------------------┘
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_servo_gd32h7.h
** 创 建 者: 文小兵
** 功    能: GD32H7 CanOpen伺服驱动器数据结构定义及公共工具宏
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建时间: 2025年
**--------------------------------------------------------------------------------------
***************************************************************************************/

#ifndef __DRV_SERVO_GD32H7_H__
#define __DRV_SERVO_GD32H7_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*====================================================================================
 * 公共类型定义（移植自 common_def.h）
 *====================================================================================*/
typedef unsigned char   u8;
typedef signed   char   s8;
typedef unsigned short  u16;
typedef signed   short  s16;
typedef unsigned int    u32;
typedef signed   int    s32;
typedef float           f32;

/*====================================================================================
 * 通用延时宏（非阻塞延时判断）
 * Delay_Fuction(当前ms计数, 存储ms值, 延时ms) —— 返回 bool
 *====================================================================================*/
#ifndef Delay_Fuction
#define Delay_Fuction(A, B, C)  ((u32)((A) - (B)) > (u32)(C))
#endif

/*====================================================================================
 * 字节联合体（用于数据拆分/合并）
 *====================================================================================*/
typedef union {
    unsigned char   B4_U8[4];
    signed   char   B4_S8[4];
    unsigned short  B4_U16[2];
    signed   short  B4_S16[2];
    unsigned int    B4_U32;
    signed   int    B4_S32;
    float           B4_F32;
} BYTE4_UNION;

typedef union {
    unsigned char   B2_U8[2];
    signed   char   B2_S8[2];
    unsigned short  B2_U16;
    signed   short  B2_S16;
} BYTE2_UNION;

/*====================================================================================
 * LED调试指示（占位定义，不影响功能）
 *====================================================================================*/
extern volatile u32 g_CanFestival_led_cnt;
#define b_led3  g_CanFestival_led_cnt

/*====================================================================================
 * 系统毫秒计数器
 *====================================================================================*/
extern volatile u32 g_Sys_1ms_Counter;

/*====================================================================================
 * 伺服驱动器类型枚举
 *====================================================================================*/
typedef enum {
    SvoType_Null    = 0,    /* 无效 */
    SvoType_Copley  = 10,   /* Copley */
    SvoType_Kinco   = 20,   /* 金科 4极 */
    SvoType_Kinco_5 = 21,   /* 金科 5极 */
    SvoType_Delta   = 30,   /* 台达 */
    SvoType_Fdk     = 40,   /* 威科 */
    SvoType_Hst     = 50,   /* 惠斯通 */
    SvoType_Keya    = 60,   /* 科亚 */
} SvoType_EnumDef;

/*====================================================================================
 * 伺服驱动器数据结构
 *====================================================================================*/
typedef struct {
    /* 配置参数 */
    SvoType_EnumDef SvoType;        /* 伺服类型 */

    /* 控制参数 */
    bool    En;                     /* 使能 */
    bool    EnLast;                 /* 上次使能状态 */
    u32     MsRecord;               /* 记录时刻 */

    /* 实时命令数据 */
    s32     CMDSpeed;               /* 速度命令 */
    short   CMDTorque;              /* 转矩命令 */
    int     CMDPosition;            /* 位置目录 */
    short   CMDCtrl;                /* 控制命令 */
    u16     CMDLimitTorque;         /* 转矩限制寄存器 */

    /* 反馈信息 */
    int     SN;                     /* 伺服序列号 */
    short   Voltage;                /* 电压 */
    short   Current;                /* 电流 */
    short   Temperature;            /* 温度 */
    short   StatusIO;               /* IO状态 */
    short   StatusWord;             /* 状态字 */
    int     EventWord;              /* 事件字 */
    short   WorkMode;               /* 工作模式 */
    short   Speed;                  /* 速度 */
    short   Torque;                 /* 转矩 */
    int     Position;               /* 位置 */
    unsigned short TimeoutCnt;      /* 超时计数 */
    unsigned short CommErrCnt;      /* 通信错误计数 */
    unsigned int   FaultCode;       /* 故障码 */
    int     LoadCnt;                /* 负载计数器 0-100 */
    u16     LoadAdd;                /* 负载增加阈值 */
    s16     LoadReduce;             /* 负载减少量 */

    /* 通信状态 */
    short   SlaveStatus;            /* 从站状态 */
    short   ConnectCnt;             /* 连接次数 */
    short   SlaveHeartbeatError;    /* 从机heartbeat/node guard状态 */
} SVO_DATA_TypeDef;

/*====================================================================================
 * 伺服数组（需在drv_servo_gd32h7.c中定义）
 *====================================================================================*/
#include "config.h"   /* 获取 MAX_NodeId_NUM (需在编译器include path中包含 CanFestival-3/include) */

#ifdef DRV_SERVO_GD32H7_GLOBALS
    #define DRV_SERVO_EXT
#else
    #define DRV_SERVO_EXT extern
#endif

DRV_SERVO_EXT SVO_DATA_TypeDef SVO[MAX_NodeId_NUM];
DRV_SERVO_EXT volatile u32     g_Sys_1ms_Counter;
DRV_SERVO_EXT volatile u32     g_CanFestival_led_cnt;

#endif /* __DRV_SERVO_GD32H7_H__ */

/************************************END OF FILE************************************/
