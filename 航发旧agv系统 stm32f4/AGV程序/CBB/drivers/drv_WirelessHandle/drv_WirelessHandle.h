/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_WirelessHandle.h
** 创 建 人: 何维  Master
** 描    述: 无线遥控数据处理头文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.1
** 创建日期: 2023年12月13日       
**--------------------------------------------------------------------------------------
***************************************************************************************/  
/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __DRV_WIRELESS_HANDLE_H__
#define __DRV_WIRELESS_HANDLE_H__

/*控制对象实例化只被指定文件编译时实例化一次*/
#ifdef  DRV_WIRELESS_HANDLE_GLOBALS
	#define DRV_WIRELESS_HANDLE_EXT
#else
	#define DRV_WIRELESS_HANDLE_EXT  extern
#endif

/*自定义引用*/																
#include <stdbool.h>
#include "common_def.h"


/*单联动档位定义*/
typedef enum
{
	Car_Sel_A = 0,							/*选中A车*/
	Car_Sel_B,								/*选中B车*/
	Car_Sel_Both,							/*联动*/
}CAR_SEL;

/*避障控制*/
typedef enum
{
	Obs_Sel_Off_All = 0,					/*全部关闭*/
	Obs_Sel_Off_Fb,							/*前后关闭*/
	Obs_Sel_Off_Lr,							/*左右关闭*/
	Obs_Sel_Off_None,						/*全部打开*/
}OBS_SEL;

/*遥控数据*/
typedef struct
{
	/*初始化参数*/
	u16*	p_DataInput;					/*通讯参数输入指针*/
		
	/*输出参数*/
	u32		CommucationMsCnt;				/*通讯毫秒计数*/
	
	u16		CountPerSecend;					/*1s内接收到遥控器数据包数量*/
	
	bool	WirelessSleep;					/*遥控器无线休眠状态：0--未休眠  1--已休眠*/
	
	s16		XAxisOutput,					/*解算后摇杆X值*/
			YAxisOutput,					/*解算后摇杆Y值*/
			ZAxisOutput;					/*解算后摇杆Z值*/
	
	u8	WorkMode;							/*工作模式*/
	
	CAR_SEL CarSel;
	
	u8		WalkSpeedVal,					/*行走速度*/
			LiftSpeedVal;					/*举升速度*/
	
	OBS_SEL	ObsSel;							/*避障控制*/
	
	bool	Lighting,						/*照明控制*/
			EmgStop,						/*急停*/
			LiftUp,							/*上升*/
			LiftDown,						/*下降*/
			Horn,							/*喇叭*/
			AutoEnterShelf,					/*自动对中使能（不停止）*/
			LineStop,						/*巡线使能，到位停止*/
			ShelfStop;						/*对中使能，到位停止*/
	
	s16		J7_X,							/*X轴原始值*/
			J7_Y,							/*Y轴原始值*/
			J7_Z;							/*Z轴原始值*/
	
	u16		J41,							/*遥控器J41端口原始值，值为多少则表示亚空气J41端口的第几位被置1*/
			J42,							/*遥控器J42端口原始值，值为多少则表示亚空气J42端口的第几位被置1*/
			J43,							/*遥控器J43端口原始值，值为多少则表示亚空气J43端口的第几位被置1*/
			J44;							/*遥控器J44端口原始值，值为多少则表示亚空气J44端口的第几位被置1*/
	
	u8		J11,							/*J11原始值,Bit0表示1脚状态，Bit1表示4脚状态*/
			J14,							/*J14原始值,Bit0表示1脚状态，Bit1表示4脚状态*/
			J17,							/*J17原始值,Bit0表示1脚状态，Bit1表示4脚状态*/
			J22;							/*J22原始值,Bit0表示1脚状态，Bit1表示4脚状态*/
	
	bool	J12,							/*J12原始值,1导通，0断开*/
			J15,							/*J15原始值,1导通，0断开*/
			J18,							/*J18原始值,1导通，0断开*/
			J20,							/*J20原始值,1导通，0断开*/
			J23,							/*J23原始值,1导通，0断开*/
			J25,							/*J25原始值,1导通，0断开*/
			J27,							/*J27原始值,1导通，0断开*/
			J10,							/*J10原始值,1导通，0断开*/
			J13,							/*J13原始值,1导通，0断开*/
			J16,							/*J16原始值,1导通，0断开*/
			J19,							/*J19原始值,1导通，0断开*/
			J21,							/*J21原始值,1导通，0断开*/
			J24,							/*J24原始值,1导通，0断开*/
			J26,							/*J26原始值,1导通，0断开*/
			J28;							/*J28原始值,1导通，0断开*/
	
}WH_TypeDef;	

/*函数声明*/
DRV_WIRELESS_HANDLE_EXT  void WirelessHandleCtrlDataInit(WH_TypeDef *p);
DRV_WIRELESS_HANDLE_EXT  void WirelessHandle_DataProcess(WH_TypeDef *p);

#endif
