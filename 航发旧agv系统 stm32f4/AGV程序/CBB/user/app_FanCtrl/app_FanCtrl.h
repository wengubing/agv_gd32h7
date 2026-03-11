/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_FanCtrl.h   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.5.27        
* Description: 散热风扇控制
*******************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __app_FanCtrl_h__
#define __app_FanCtrl_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  app_FanCtrl_GLOBALS
	#define app_FanCtrl_EXT
#else
	#define app_FanCtrl_EXT  extern
#endif

//头文件
#include <stdbool.h>
#include "..\..\..\CBB\user\basecalfunc\common_def.h"

//散热控制方式定义
typedef enum
{
	FanCtrl_Mode_Null = 0,									//无效
	FanCtrl_Mode_Time,										//定时控制
	FanCtrl_Mode_Temperature,								//根据温度控制
}FanCtrl_Mode_StDef;

//风扇类型定义
typedef enum
{
	FanCtrl_InterfaceType_Null = 0,							//无效
	FanCtrl_InterfaceType_IO,								//定时控制
	FanCtrl_InterfaceType_Speed,							//根据温度控制
}FanCtrl_InterfaceType_StDef;

//风扇控制对象定义
typedef struct
{
	//初始化内容
	FanCtrl_Mode_StDef Mode;								//控制类型
	FanCtrl_InterfaceType_StDef FanType;					//风扇控制接口类型
	u32 in_TemperatureFanOn;                                //散热打开温度
	u32 in_TemperatureFanOff;                               //散热关闭温度
	unsigned short in_TimeFanOn;							//散热打开时间 单位s
	unsigned short in_TimeFanOff;                           //散热关闭时间 单位s
    u16  in_Temperature_Environment_FanOff;                 //环境温度关闭阈值 参数设65535即关闭该功能

	//实时输入
	u32 in_Ms;												//系统毫秒输入
	u32 in_Temperature;                                     //温度输入 单位度
    u32 in_Temperature_Environment;                          //环境温度输入原始值----超低车专属

	//内容状态
	u32 CtrlLastTime;                                    	//上次风扇控制时间记录

	//实时输出
	bool FanCtrl;                                           //风扇控制输出
    float out_Temperature_Environment;                      //环境温度计算值输出----超低车专属，单位0.1°
}FanCtrl_StDef;

//全局函数声明
app_FanCtrl_EXT bool FanCtrl(FanCtrl_StDef *p);				//风扇控制		

#endif

//***********************************END OF FILE***********************************

