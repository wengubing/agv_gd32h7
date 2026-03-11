/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: AgvSimulator.h
** 创 建 人: 文小兵
** 描    述: Agv系统模拟器
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2025年8月11日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __AgvSimulator_h__
#define __AgvSimulator_h__

//确保仅在该文件编译时实例化一次对象
#ifdef AgvSimulator_GLOBALS
#define AgvSimulator_EXT
#else
#define AgvSimulator_EXT  extern
#endif

//自定义引用	
#include <stdbool.h>
#include "common_def.h"
#include "AgvChassisController.h"
#include "PowerOn.h"

//模拟器状态机定义
typedef enum
{
    ASM_Stm_Null = 0,
    ASM_Stm_Init,
    ASM_Stm_BMS,
    ASM_Stm_PowerUp,
    ASM_Stm_Ready,
    ASM_Stm_Run,
    ASM_Stm_Stop,
    ASM_Stm_Error,
    ASM_Stm_Reset,
    ASM_Stm_End = 0xFFFF,
}ASM_Stm_Typedef;

//自动化测试配置
typedef struct 
{
    u32 En : 1;                                             //总开关
    u32 Auto : 1;                                           //自动测试开关
    u32 StaticCheck : 1;                                    //静态检查
    u32 ModeChange : 1;                                     //模式切换
    u32 CarEstop : 1;                                       //车辆急停
    u32 ModeSwitch2Inf : 1;                                 //切换到横向模式
	u32 ModeSwitch2Rot : 1;                                 //切换到自转模式
    u32 CheckTgtAngCross : 1;                               //交于一点测试
}AutoTest_Config_Typedef;

//自动化测试结果
typedef struct
{
    u32 Static : 1;                                         //静态测试
    u32 ModeChange : 1;                                     //模式切换
    u32 CarEstop : 1;                                       //车辆急停
    u32 ModeSwitch2Inf : 1;                                 //切换到横向模式
	u32 ModeSwitch2Rot : 1;                                 //切换到自转模式
    u32 CheckTgtAngCross : 1;                               //交于一点测试
}AutoTest_Result_Bit_Typedef;

//测试结果共用体
typedef union
{
    AutoTest_Result_Bit_Typedef Bit;
    u32 u32;
}AutoTest_Result_Typedef;

//车体急停测试状态机
typedef enum
{
    AuT_CSP_Stm_Init = 0,
    AuT_CSP_Stm_WaitPowerUp,                                //等待上电启动正常
    AuT_CSP_Stm_WaitStill,                                  //等待进入静止且无报警状态
    AuT_CSP_Stm_OprCSP,                                     //模拟操作急停
    AuT_CSP_Stm_Wait,                                       //等待延时
    AuT_CSP_Stm_Judge,                                      //判断是否检出故障
    AuT_CSP_Stm_End = 0xFFFF,
}AutoTest_CarEStop_Stm_Typedef;

//底盘模式切换测试 切横向
typedef enum
{
    AuT_ModeSwitch2Inf_Stm_Init = 0,
    AuT_ModeSwitch2Inf_Stm_WaitStill,                       //等待进入静止且无报警状态 并模拟操作
    AuT_ModeSwitch2Inf_Stm_Wait,                            //等待延时
    AuT_ModeSwitch2Inf_Stm_Judge,                           //判断是否到位
    AuT_ModeSwitch2Inf_Stm_End = 0xFFFF,
}AutoTest_ModeSwitch2Inf_Stm_Typedef;

//底盘模式切换测试 切自转
typedef enum
{
    AuT_ModeSwitch2Rot_Stm_Init = 0,
    AuT_ModeSwitch2Rot_Stm_WaitStill,                       //等待进入静止且无报警状态 并模拟操作
    AuT_ModeSwitch2Rot_Stm_Wait,                            //等待延时
    AuT_ModeSwitch2Rot_Stm_Judge,                           //判断是否到位
    AuT_ModeSwitch2Rot_Stm_End = 0xFFFF,
}AutoTest_ModeSwitch2Rot_Stm_Typedef;

//模组交于一点测试
typedef enum
{
    AuT_CTAC_Stm_Init = 0,
    AuT_CTAC_Stm_WaitStill,                                 //等待进入静止且无报警状态 并模拟操作
    AuT_CTAC_Stm_Wait,                                      //等待延时
    AuT_CTAC_Stm_Judge,                                     //判断是否到位
    AuT_CTAC_Stm_End = 0xFFFF,
}AutoTest_CheckTgtAngCross_Stm_Typedef;

//交点计算中间变量定义
typedef struct
{
	PointAnf2Line_TypeDef Lines[8];                         //直线参数
	Point_XY_StDef CrossP[28];  						    //交点坐标
	char Num;                                               //交点数量
	f32 Diagonal;                                           //交点的最小外接矩形的对角线长度
}AuT_CrossP_Typedef;

//代码自动化测试
typedef struct 
{
    u32 Ms;
    AutoTest_Config_Typedef Config;
    AutoTest_Result_Typedef Result;

    AutoTest_CarEStop_Stm_Typedef CSP_Stm;
    u32 MsRecord_CSP;
    AutoTest_ModeSwitch2Inf_Stm_Typedef MS2Inf_Stm;
	AutoTest_ModeSwitch2Rot_Stm_Typedef MS2Rot_Stm;
	AutoTest_CheckTgtAngCross_Stm_Typedef CTAC_Stm;

	AuT_CrossP_Typedef CrossP;
    u32 MsRecord;
}AutoTest_Typedef;

//上一次的状态记录
typedef struct
{
    ACC_PowerOnStep_TypeDef PowerOnStep;
    MC_WorkStep_TypeDef MC_WorkStep;
    ACC_FaultsType_TypeDef FaultsResponseSTM;
    ACCMode_TypeDef Mode;
}LastST_TypeDef;

//上一次的 故障位记录
typedef struct
{
    u64 EStop_Rmc : 1;                                      //遥控器急停
	u64 EStop_Car : 1;                                      //车体急停
	u64 WheelAngCalErr : 1;                                 //轮系角度解算异常
	u64 wheelAngleLimited : 1;                              //轮系角度限位
	u64 ModeSwitchTimeOut : 1;                              //模式切换超时

}LastErrBit_TypeDef;

//调试日志
typedef struct
{
    LastST_TypeDef LastST;
    LastErrBit_TypeDef LastErrBit;
}Debug_Log_TypeDef;

//Agv系统模拟器 对象定义
typedef struct
{
	u32 Ms;
	u16 CommCntDelay;										//modbus通信计数延时
	u32 MsRecord_CommCnt;                                   //时间戳
	ASM_Stm_Typedef Stm;                                    //启动模拟器状态机
	OrgCtrlCmd_TypeDef OrgRmcCmd;							//模拟原始遥控命令
    AutoTest_Typedef AutoTest;								//代码自动化测试
    Debug_Log_TypeDef DBLog;                                //调试日志
}AgvSimulator_TypeDef;

//对象外部声明

//方法外部声明
bool ASM_Run(AgvSimulator_TypeDef* p);
bool ASM_WirelessRemote(AgvSimulator_TypeDef* p);
bool ASM_WirelessRemote_Horn(AgvSimulator_TypeDef* p);
bool LogicErrDetect(void);

#endif
//***********************************END OF FILE***********************************


