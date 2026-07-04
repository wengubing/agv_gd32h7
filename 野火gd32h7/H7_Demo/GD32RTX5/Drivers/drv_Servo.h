/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_Servo.h
** 创 建 人: 文小兵
** 描    述: 伺服抽象定义
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0
** 创建日期: 2024年5月9日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __drv_Servo_H__
#define __drv_Servo_H__

//确保仅在该文件编译时实例化一次对象
#ifdef		drv_Servo_GLOBALS
	#define drv_Servo_EXT
#else
	#define drv_Servo_EXT  extern
#endif

//头文件引用		
//#include "LinkCanopenMaster.h"
#include <stdbool.h>
#include <common_def.h>
#include "..\..\CBB\user\basecalfunc\basecalfunc.h"
#include "config.h"

//宏定义
#define SVO_MAXNUM											MAX_NodeId_NUM//32//最大伺服数量

	
//驱动器类型定义
typedef enum
{
	SvoType_Null	= 0,	//非法的
	SvoType_Copley	= 10,	//Copley
	SvoType_Kinco	= 20,	//步科 4代  
	SvoType_Kinco_5 = 21,	//步科 5代
	SvoType_Delta	= 30,	//台达
	SvoType_Fdk		= 40,	//风得控
	SvoType_Hst		= 50,	//惠斯通	
	SvoType_Keya  	= 60,	//科亚	
	
}SvoType_EnumDef;

//驱动器 驱动电机的方向，会在底层设置电机旋转方向时生效
typedef enum
{
	SvoSvoDriveDirection_Normal		= 0,	//默认的状态
	SvoSvoDriveDirection_Reverse	= 1,	//反转的状态
}SvoDirection_EnumDef;

//驱动器信息 对象定义
typedef struct
{
	//配置初始化
	SvoType_EnumDef				SvoType;		//伺服类型
	SvoDirection_EnumDef		SvoDirection;	//伺服电机的默认运转方向
	//通信站号、工作模式

	//控制参数
	bool 			En;				//使能
	bool 			EnLast;			//上次使能状态
	u32 			MsRecord;		//记录时间

	//实时控制命令
	s32 			CMDSpeed;		//速度命令
	short 			CMDTorque;		//力矩命令
	int 			CMDPosition;	//位置目录
	short 			CMDCtrl;		//控制命令
	u16 			CMDLimitTorque;	//力矩限制寄存器

	//反馈信息
	int 			SN;				//伺服序列号
	short 			Voltage;		//电压
	short			Current;		//电流
	short 			Temperature;	//温度
	short 			StatusIO;		//IO状态
	short 			StatusWord;		//状态字
	int 			EventWord;		//事件字	
	short 			WorkMode;		//工作模式
	short 			Speed;			//速度
	short 			Torque;			//力矩
	int 			Position;		//位置
	unsigned short 	TimeoutCnt;		//超时计数
	unsigned short 	CommErrCnt;		//通信错误计数
	unsigned int 	FaultCode;		//故障码 类型为无符号，避免负数在if判断时出错
	int 			LoadCnt;		//负载保护计数 0-100

	//通信状态
	short 			SlaveStatus;	//从站状态
	short 			ConnectCnt;		//连接次数
	short 			SlaveHeartbeatError;          //从机heartbeat或者node guard状态
}SVO_DATA_TypeDef;

//驱动器信息分析统计 对象定义
typedef struct
{
	Extreme_StDef Temp;		//温度
	Extreme_StDef LoadCnt;	//负载计数 历史记录
}SVOs_StDef;

//全局对象声明
drv_Servo_EXT SVO_DATA_TypeDef 	SVO[SVO_MAXNUM];	//伺服对象
drv_Servo_EXT SVOs_StDef 		SVOs;				//伺服对象分析统计

//全局方法声明
drv_Servo_EXT bool SVO_EnableOperation(u32 InMs);	//伺服使能控制
drv_Servo_EXT bool SVOs_Analysis(void);				//伺服分析统计

#endif

//***********************************END OF FILE***********************************

