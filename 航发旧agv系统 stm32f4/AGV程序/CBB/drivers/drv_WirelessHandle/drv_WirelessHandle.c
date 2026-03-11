/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_WirelessHandle.c
** 创 建 人: 何维  Master
** 描    述: 无线遥控数据处理源文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.1 
** 创建日期: 2023年12月13日       
**--------------------------------------------------------------------------------------
***************************************************************************************/  
/*声明对象在此文件编译时实例化*/
#define  WIRELESS_HANDLE_GLOBALS

/*包含对象头文件*/
#include <math.h>
#include <stdlib.h>
#include "drv_WirelessHandle.h"


/***************************************************************************************
** 函数名称: RawDataUpdate
** 功能描述: 遥控器原始数据更新
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RawDataUpdate(WH_TypeDef *p)
{
	/*毫秒计数数据获取*/
	p->CommucationMsCnt = ((u32)p->p_DataInput[1]<<16) + p->p_DataInput[0];
	
	/*1s内接收到的数据包数量获取*/
	p->CountPerSecend = p->p_DataInput[3];
	
	/*遥控器休眠状态获取*/
	p->WirelessSleep = (bool)((p->p_DataInput[2]>>10)&0x01);
	
	/*摇杆三轴原始数据获取*/
	p->J7_X = (s8)(p->p_DataInput[6]>>8);
	p->J7_Y = (s8)(p->p_DataInput[7]);
	p->J7_Z = (s8)(p->p_DataInput[7]>>8);
	
	if(p->J7_X > 127)
	{
		p->J7_X = 127;
	}
	else if(p->J7_X < -127)
	{
		p->J7_X = -127;
	}
	
	if(p->J7_Y > 127)
	{
		p->J7_Y = 127;
	}
	else if(p->J7_Y < -127)
	{
		p->J7_Y = -127;
	}
	
	if(p->J7_Z > 127)
	{
		p->J7_Z = 127;
	}
	else if(p->J7_Z < -127)
	{
		p->J7_Z = -127;
	}
	
	/*遥控器各端口原始数据获取*/
	p->J41 = (p->p_DataInput[14]>>8)&0x0F;
	p->J42 = p->p_DataInput[15]&0x0F;
	p->J43 = (p->p_DataInput[15]>>8)&0x0F;
	p->J44 = p->p_DataInput[16]&0x0F;
	
	p->J11 = (p->p_DataInput[13]>>8)&0x03;
	p->J14 = (p->p_DataInput[13]>>10)&0x03;
	p->J17 = (p->p_DataInput[13]>>12)&0x03;
	p->J22 = (p->p_DataInput[13]>>14)&0x03;
	
	p->J12 = (bool)((p->p_DataInput[12]>>9)&0x01);
	p->J15 = (bool)((p->p_DataInput[12]>>10)&0x01);
	p->J18 = (bool)((p->p_DataInput[12]>>11)&0x01);
	p->J20 = (bool)((p->p_DataInput[12]>>12)&0x01);
	p->J23 = (bool)((p->p_DataInput[12]>>13)&0x01);
	p->J25 = (bool)((p->p_DataInput[12]>>14)&0x01);
	p->J27 = (bool)((p->p_DataInput[12]>>15)&0x01);
	
//	p->J10 = (bool)((p->p_DataInput[13]>>0)&0x01);
	p->J10 = (bool)((p->p_DataInput[16]>>0)&0x01);
	p->J13 = (bool)((p->p_DataInput[13]>>1)&0x01);
	p->J16 = (bool)((p->p_DataInput[13]>>2)&0x01);
	p->J19 = (bool)((p->p_DataInput[13]>>3)&0x01);
	p->J21 = (bool)((p->p_DataInput[13]>>4)&0x01);
	p->J24 = (bool)((p->p_DataInput[13]>>5)&0x01);
	p->J26 = (bool)((p->p_DataInput[13]>>6)&0x01);
	p->J28 = (bool)((p->p_DataInput[13]>>7)&0x01);
	
}

/***************************************************************************************
** 函数名称: ThreeAixsDataCal
** 功能描述: 摇杆数据处理
** 参    数: RawData三轴原始数据
** 返 回 值: res三轴掐头去尾映射数据[-107,107]
****************************************************************************************/
s16 ThreeAixsDataCal(s16 RawData)
{
	s16 res=0;
	f32 temp;
	s8 sign = 1;
	
	/*滤除零点附近20大小的波动值*/
	if(RawData >= 0)
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}
	
	if(abs(RawData) > 20.0f)
	{
		temp = (f32)(RawData - sign * 20.0f);
	}
	else
	{
		temp = 0.0f;
	}
	
	/*限幅*/
	if(fabs(temp)>97.0f)
	{
		temp = (f32)(sign * 97.0f);
	}
	
	/*线性映射*/
	temp *= (f32)(107.0f/97.0f);
	
	res = (s16)temp;
	
	return res;
}

/***************************************************************************************
** 函数名称: CtrlDataUpdate
** 功能描述: 遥控器控制数据更新
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void CtrlDataUpdate(WH_TypeDef *p)
{
	/*摇杆三轴控制数据求解*/
	p->XAxisOutput = ThreeAixsDataCal(p->J7_X);
	p->YAxisOutput = ThreeAixsDataCal(p->J7_Y);
	p->ZAxisOutput = ThreeAixsDataCal(p->J7_Z);
	
	/*工作模式获取*/
	p->WorkMode = p->J41;
	
	/*联动模式选择*/
	p->CarSel = (CAR_SEL)p->J42;
	
	/*行走档位更新*/
	p->WalkSpeedVal = p->J44;
	
	/*升降档位更新*/
	p->LiftSpeedVal = p->J43;
	
	/*避障控制更新*/
	p->ObsSel = (OBS_SEL)p->J43;
	
	/*照明控制*/
	p->Lighting = p->J10;
	
	/*急停控制*/
	//p->EmgStop = p->J13;
	p->EmgStop = (bool)((p->p_DataInput[12]>>8)&0x01);
	
	/*上升控制*/
	p->LiftUp = p->J15;
	
	/*下降控制*/
	p->LiftDown = p->J20;
	
	/*喇叭控制*/
	p->Horn = p->J12;
	
	/*自动对中使能*/
	p->AutoEnterShelf = p->J24;
	
	/*巡线使能*/
	p->LineStop = p->J26;
	
	/*自动对中使能（到位停止）*/
	p->ShelfStop = p->J28;
	
}
/***************************************************************************************
** 函数名称: WirelessHandleCtrlDataInit
** 功能描述: 遥控器控制数据初始化
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void WirelessHandleCtrlDataInit(WH_TypeDef *p)
{
	/*工作模式初始设置为静止*/
	p->WorkMode = 0x04;						/*根据具体项目遥控器面板接线做修改*/
	
	/*联动模式选择A车*/
	p->CarSel = Car_Sel_A;
		
	/*避障控制默认全部打开*/
	p->ObsSel = Obs_Sel_Off_None;
	
	/*照明默认关闭*/
	p->Lighting = false;
	
	/*急停默认未触发*/
	p->EmgStop = true;
	
	/*上升默认未触发*/
	p->LiftUp = false;
	
	/*下降默认未触发*/
	p->LiftDown = false;
	
	/*喇叭默认未触发*/
	p->Horn = false;
	
	/*自动对中使能默认未触发*/
	p->AutoEnterShelf = false;
	
	/*巡线使能默认未触发*/
	p->LineStop = false;
	
	/*自动对中使能（到位停止）默认未触发*/
	p->ShelfStop = false;
	
}

/***************************************************************************************
** 函数名称: WirelessHandle_DataProcess
** 功能描述: 遥控器控制数据初始化
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void WirelessHandle_DataProcess(WH_TypeDef *p)
{
	RawDataUpdate(p);
	
	if(1)//!p->WirelessSleep) //解决遥控器休眠前通信断开 休眠后通信恢复但数据未清零更新问题
	{
		CtrlDataUpdate(p);
	}
}

