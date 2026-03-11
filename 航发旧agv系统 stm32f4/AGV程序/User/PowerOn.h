/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: PowerOn.h
** 创 建 人: 文小兵  
** 描    述: 上电启动过程分离到该文件中
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月28日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __PowerOn_h__
#define __PowerOn_h__

//确保仅在该文件编译时实例化一次对象
#ifdef PowerOn_GLOBALS
    #define PowerOn_EXT
#else
    #define PowerOn_EXT  extern
#endif
#include <stm32f4xx.h>
	
//自定义引用	

//-----------------------------------上电启动控制对象定义-------------------------
//MC状态定义
typedef enum
{
	MC_WorkStep_Init 	= 0,		//初始化
	MC_WorkStep_PowerUp	= 1,		//上电启动
	MC_WorkStep_Run		= 2,		//运行
	MC_WorkStep_Fail	= 3,		//启动失败
	MC_WorkStep_Num		= 4,		//枚举数量
	MC_WorkStep_End 	= 65535,	//u16最大值
}MC_WorkStep_TypeDef;

//canopen启动状态定义
typedef enum
{
	ACC_CanopenStep_Init 		= 0,	//初始化
	ACC_CanopenStep_Precharge	= 1,	//预充
	ACC_CanopenStep_Active		= 2,	//启动成功
	ACC_CanopenStep_Fault		= 3,	//故障
}ACC_CanopenStep_TypeDef;

//上电启动状态定义
typedef enum
{
	ACC_PowerOnStep_Init 				= 0,	//初始化
	ACC_PowerOnStep_BatteryCheck		= 1,	//电池启动检查
	ACC_PowerOnStep_ControlVoltageCheck	= 2,	//系统电压检查
	ACC_PowerOnStep_CanopenCheck		= 3,	//canopen状态检查
	ACC_PowerOnStep_PrechargeOff		= 4,	//关闭预充
	ACC_PowerOnStep_StartupComplete		= 5,	//启动完成
	ACC_PowerOnStep_StartupFail			= 6,	//启动失败

	//上面的枚举保持不变，避免hmi的显示异常
	//注意不要直接使用枚举的数值区间做逻辑判断
	ACC_PowerOnStep_ANC_PowerON			= 7,	//安川m3板卡上电
	ACC_PowerOnStep_ANC_Check			= 8,	//安川m3板卡状态检查
}ACC_PowerOnStep_TypeDef;

//电池类型定义
typedef enum
{
	ACC_BatteryType_LeadBatteries				= 0  ,	//铅酸
	ACC_CanopenStep_Self_LithiumBattery_Low		= 11 ,	//自制低压锂电
	ACC_CanopenStep_Self_LithiumBattery_High	= 12 ,	//自制高压锂电
	ACC_CanopenStep_Cable_Power_Supply			= 21 ,  //外接线缆供电
	ACC_CanopenStep_Others						= 200,	//其它
}ACC_BatteryType_TypeDef;

//上电启动 数据对象定义
typedef struct
{
	//初始化参数
	ACC_BatteryType_TypeDef BatteryType					;	//电池类型

	//实时输入
	uint32_t 				in_Ms						;	//ms输入
	uint8_t 				in_CarEmgStop			:1	;	//车体急停按下
	uint8_t 				in_BatteryReady			:1	;	//电池启动正常
	uint8_t 				in_ControlVoltageReady	:1	;	//系统控制电压正常
	ACC_CanopenStep_TypeDef in_CanopenStep				;	//canopen启动状态
	

	//内部成员
	ACC_PowerOnStep_TypeDef PowerOnStep					;	//启动状态
	uint32_t				MsRecord					;	//ms记录

	//输出
	uint8_t Ctrl_CanOpenStart						:1	;	//canopen启动
	uint8_t Ctrl_Precharge							:1	;	//预充控制
	uint8_t Ctrl_MainContactor						:1	;	//主接触器
	uint8_t Ctrl_CtrllPower							:1	;	//伺服控制电源
	uint8_t Ctrl_MainDCDC							:1	;	//主DCDC输入控制
	uint8_t Ctrl_CtrlVoltageBypass					:1	;	//系统旁路控制
	uint8_t Ctrl_PowerOnBypass						:1	;	//系统开关 旁路控制	
	uint8_t Ctrl_MotorPrecharge						:1	;	//驱动器预充控制
	uint8_t Ctrl_MotorContactor						:1	;	//驱动器主接触器
	uint8_t Ctrl_ANC								:1	;	//安川m3板卡电源
}ACC_PowerOn_TypeDef;

//对象外部声明


//方法外部声明
PowerOn_EXT void ACC_PowerOn_Process(u32 in_Ms,ACC_PowerOn_TypeDef *p);//上电启动过程

#endif

//***********************************END OF FILE***********************************


