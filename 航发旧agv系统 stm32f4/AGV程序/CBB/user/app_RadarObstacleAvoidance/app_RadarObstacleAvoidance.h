/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: app_RadarObstacleAvoidance.h
** 创 建 人: 何维  
** 描    述: 激光避障数据处理头文件，包含所需数据结构体和函数声明
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2023年12月20日       
**--------------------------------------------------------------------------------------
***************************************************************************************/  

//保证以下所有内容只被同一源文件引用一次*/
#ifndef __APP_RADAR_OBSTACLE_AVOIDANCE_H__
#define __APP_RADAR_OBSTACLE_AVOIDANCE_H__

/*控制对象实例化只被指定文件编译时实例化一次*/
#ifdef  RADAR_OBSTACLE_AVOIDANCE_GLOBALS
	#define RADAR_OBSTACLE_AVOIDANCE_EXT
#else
	#define RADAR_OBSTACLE_AVOIDANCE_EXT  extern
#endif

/*自定义引用*/																
#include <stdbool.h>
#include <stm32f4xx.h>

//宏定义
#define Brand_Other		false
#define Brand_Hinson	true

/*安装方式定义*/
typedef enum
{
	Pattern_Diagonal_L = 0,					/*左对角安装方式*/
        Pattern_Diagonal_R,						/*右对角安装方式*/
	Pattern_Center,							/*前后双雷达安装方式*/
	Pattern_Surround,						/*四周四雷达安装方式*/
}INSTALL_PATTERN;

/*雷达输入信号位定义*/
typedef enum
{
	InputState_Warning = 0,					/*BIT0:雷达最外层状态，触发时置1*/
	InputState_Caution,						/*BIT1:雷达中间层状态，触发时置1*/
	InputState_Emergency,					/*BIT2:雷达最内层状态，触发时置1*/
	InputState_Error,						/*BIT3:雷达Err信号，报警时置1*/
	InputState_Enable,						/*BIT4:雷达使能状态，启用时置1*/
}RADAR_INPUT;

//位域定义，便于访问操作，顺序不能改变
typedef struct
{
	unsigned char Outer : 1;				//外层触发
    unsigned char Middle : 1;				//中间层触发
    unsigned char Inner : 1;				//内层触发
    unsigned char Error : 1;				//故障
    unsigned char Enable : 1;				//使能
}RADAR_INPUT_BitDef;

//四周雷达三层障碍状态定义
typedef struct
{
	unsigned char ForwordRadar_Inner:1;         //前雷达最内层状态     1：触发 0：未触发
    unsigned char ForwordRadar_Middle:1;    	//前雷达中间层状态     1：触发 0：未触发
    unsigned char ForwordRadar_Out:1;        	//前雷达最外层状态     1：触发 0：未触发
    unsigned char BackRadar_Inner:1;		    //后雷达最内层状态     1：触发 0：未触发			          
    unsigned char BackRadar_Middle:1; 			//后雷达中间层状态     1：触发 0：未触发		
    unsigned char BackRadar_Out:1;       		//后雷达最外层状态     1：触发 0：未触发	
    unsigned char LeftRadar_Inner:1;		    //左雷达最内层状态     1：触发 0：未触发			
    unsigned char LeftRadar_Middle:1;    		//左雷达中间层状态     1：触发 0：未触发
    unsigned char LeftRadar_Out:1;        		//左雷达最外层状态     1：触发 0：未触发
    unsigned char RightRadar_Inner:1;			//右雷达最内层状态     1：触发 0：未触发			
    unsigned char RightRadar_Middle:1; 			//右雷达中间层状态     1：触发 0：未触发
    unsigned char RightRadar_Out:1; 			//右雷达最外层状态     1：触发 0：未触发
}RADAR_ObsSta_BitDef;

/*雷达状态定义*/
//顺序不能改变，影响到壁纸恢复延时
typedef enum
{
	State_Nomal = 0,						/*雷达无障碍物及故障*/
	State_Warning ,							/*雷达最外层触发*/
	State_Caution ,							/*雷达中间层触发*/
	State_Emergency ,						/*雷达最内层触发*/
	State_Error ,							/*雷达故障*/
}RADAR_STATE;

/*雷达输出信号位定义*/
typedef enum
{
	Output_Bit0 = 0,						/*雷达输出第0位*/
	Output_Bit1 ,							/*雷达输出第1位*/
	Output_Bit2 ,							/*雷达输出第2位*/
	Output_Bit3 ,							/*雷达输出第3位*/
	Output_Bit4 ,							/*雷达输出第4位*/
	Output_Bit5 ,							/*雷达输出第5位*/
}RADAR_OUTPUT;

//位域定义，便于访问操作，顺序不能改变
typedef struct
{
	unsigned char Ctrl1 : 1;				//控制位1
    unsigned char Ctrl2 : 1;				//控制位2
    unsigned char Ctrl3 : 1;				//控制位3
    unsigned char Ctrl4 : 1;				//控制位4
    unsigned char Ctrl5 : 1;				//控制位5
    unsigned char Ctrl6 : 1;				//控制位6
}RADAR_OUTPUT_BitDef;

/*雷达扫描区域定义*/
typedef enum
{
	SCAN_AREA0 = 0x00,						/*雷达扫描区域0*/
	SCAN_AREA1 = 0x01,						/*雷达扫描区域1*/
	SCAN_AREA2 = 0x02,						/*雷达扫描区域2*/
	SCAN_AREA3 = 0x03,						/*雷达扫描区域3*/
	
	SCAN_AREA4 = 0x04,						/*雷达扫描区域4*/
	SCAN_AREA5 = 0x05,						/*雷达扫描区域5*/
	SCAN_AREA6 = 0x06,						/*雷达扫描区域6*/
	SCAN_AREA7 = 0x07,						/*雷达扫描区域7*/
	
	SCAN_AREA8 = 0x08,						/*雷达扫描区域8*/
	SCAN_AREA9 = 0x09,						/*雷达扫描区域9*/
	SCAN_AREA10 = 0x0A,						/*雷达扫描区域10*/
	SCAN_AREA11 = 0x0B,						/*雷达扫描区域11*/
	
	SCAN_AREA12 = 0x0C,						/*雷达扫描区域12*/
	SCAN_AREA13 = 0x0D,						/*雷达扫描区域13*/
	SCAN_AREA14 = 0x0E,						/*雷达扫描区域14*/
	SCAN_AREA15 = 0x0F,						/*雷达扫描区域15*/
	
	SCAN_AREA16 = 0x10,						/*雷达扫描区域16*/
	SCAN_AREA17 = 0x11,						/*雷达扫描区域17*/
	SCAN_AREA18 = 0x12,						/*雷达扫描区域18*/
	SCAN_AREA19 = 0x13,						/*雷达扫描区域19*/
	
	SCAN_AREA20 = 0x14,						/*雷达扫描区域20*/
	SCAN_AREA21 = 0x15,						/*雷达扫描区域21*/
	SCAN_AREA22 = 0x16,						/*雷达扫描区域22*/
	SCAN_AREA23 = 0x17,						/*雷达扫描区域23*/
	
	SCAN_AREA24 = 0x18,						/*雷达扫描区域24*/
	SCAN_AREA25 = 0x19,						/*雷达扫描区域25*/
	SCAN_AREA26 = 0x1A,						/*雷达扫描区域26*/
	SCAN_AREA27 = 0x1B,						/*雷达扫描区域27*/
	
	SCAN_AREA28 = 0x1C,						/*雷达扫描区域28*/
	SCAN_AREA29 = 0x1D,						/*雷达扫描区域29*/
	SCAN_AREA30 = 0x1E,						/*雷达扫描区域30*/
	SCAN_AREA31 = 0x1F,						/*雷达扫描区域31*/
	
	SCAN_AREA32 = 0x10,						/*雷达扫描区域32*/
	SCAN_AREA33 = 0x11,						/*雷达扫描区域33*/
	SCAN_AREA34 = 0x12,						/*雷达扫描区域34*/
	SCAN_AREA35 = 0x13,						/*雷达扫描区域35*/
	
	SCAN_AREA36 = 0x14,						/*雷达扫描区域36*/
	SCAN_AREA37 = 0x15,						/*雷达扫描区域37*/
	SCAN_AREA38 = 0x16,						/*雷达扫描区域38*/
	SCAN_AREA39 = 0x17,						/*雷达扫描区域39*/
	
	SCAN_AREA40 = 0x18,						/*雷达扫描区域40*/
	SCAN_AREA41 = 0x19,						/*雷达扫描区域41*/
	SCAN_AREA42 = 0x1A,						/*雷达扫描区域42*/
	SCAN_AREA43 = 0x1B,						/*雷达扫描区域43*/
	
	SCAN_AREA44 = 0x1C,						/*雷达扫描区域44*/
	SCAN_AREA45 = 0x1D,						/*雷达扫描区域45*/
	SCAN_AREA46 = 0x1E,						/*雷达扫描区域46*/
	SCAN_AREA47 = 0x1F,						/*雷达扫描区域47*/
	
	SCAN_AREA48 = 0x10,						/*雷达扫描区域48*/
	SCAN_AREA49 = 0x11,						/*雷达扫描区域49*/
	SCAN_AREA50 = 0x12,						/*雷达扫描区域50*/
	SCAN_AREA51 = 0x13,						/*雷达扫描区域51*/
	
	SCAN_AREA52 = 0x14,						/*雷达扫描区域52*/
	SCAN_AREA53 = 0x15,						/*雷达扫描区域53*/
	SCAN_AREA54 = 0x16,						/*雷达扫描区域54*/
	SCAN_AREA55 = 0x17,						/*雷达扫描区域55*/
	
	SCAN_AREA56 = 0x18,						/*雷达扫描区域56*/
	SCAN_AREA57 = 0x19,						/*雷达扫描区域57*/
	SCAN_AREA58 = 0x1A,						/*雷达扫描区域58*/
	SCAN_AREA59 = 0x1B,						/*雷达扫描区域59*/
	
	SCAN_AREA60 = 0x1C,						/*雷达扫描区域60*/
	SCAN_AREA61 = 0x1D,						/*雷达扫描区域61*/
	SCAN_AREA62 = 0x1E,						/*雷达扫描区域62*/
	SCAN_AREA63 = 0x1F,						/*雷达扫描区域63*/
}RADAR_SCAN_AREA;

/*雷达编号定义*/
typedef enum
{
	Radar_Front = 0,						/*前雷达*/
	Radar_Back,								/*后雷达*/
	Radar_Left,								/*左雷达*/
	Radar_Right,							/*右雷达*/
	
	Radar_MaxNum,							/*雷达总数*/
}RADAR_DEFINE;

/*遥控数据*/
typedef struct
{
	/*初始化参数*/
	INSTALL_PATTERN	InstallPattern;						/*安装方式*/
	
	bool RadarBrand;									/*使用兴颂雷达时设置为1，否则设置为0*/
	
	u16 ChangeTime,										/*切区时间*/
		RecoverTime,									/*恢复时间*/
		XAxisMaxSpeedLv3,								/*X轴最外层触发时速度限制值*/
		XAxisMaxSpeedLv2,								/*X轴中间层触发时速度限制值*/
		YAxisMaxSpeedLv3,								/*Y轴最外层触发时速度限制值*/
		YAxisMaxSpeedLv2;								/*Y轴中间层触发时速度限制值*/

	//联动时要关闭的自身的障碍状态参数
	RADAR_STATE ForceCLR[Radar_MaxNum];
		
	/*实时输入参数*/
	u32	SysTimeCnt;										/*系统毫秒时间*/
	
	u8	ObstacleAreaSelect;								/*避障区域选择*/
	//u8	RadarInput[Radar_MaxNum];						/*雷达输入信号*/
    RADAR_INPUT_BitDef RadarInput[Radar_MaxNum];        //雷达输入信号
	
	s16	XAxisDataInput,									/*X轴输入参数*/
		YAxisDataInput,									/*Y轴输入参数*/
		ZAxisDataInput;									/*Z轴输入参数*/
	
	/*过程参数*/
	//执行的障碍状态源选择
	enum
	{
		ObsSel_Self = 0,								/*自己*/
		ObsSel_Asb,										/*联动*/
		ObsSel_Gather,									/*融合*/
		ObsSel_End = 65535,								/*结束*/
	}Sel;

	RADAR_STATE RadarState_Self[Radar_MaxNum];			/*雷达状态 自身*/
	//强制关闭某些障碍状态的结果
	RADAR_STATE RadarState_Self_ForceCLR[Radar_MaxNum];	
	RADAR_STATE RadarState_Asb[Radar_MaxNum];			/*雷达状态 联动传递*/
	RADAR_STATE RadarState[Radar_MaxNum];				/*雷达状态 执行*/
    RADAR_ObsSta_BitDef RadarObsSta;					/*四周雷达障碍状态*/
	u32	DelayTimeCnt;									/*延时切区毫秒时间*/
	u32 MsRecord_ObsRecver[Radar_MaxNum];				/*障碍恢复延时记录*/
	
	/*输出参数*/
	//u8 RadarOutput[Radar_MaxNum];						/*雷达输出信号*/
    RADAR_OUTPUT_BitDef RadarOutput[Radar_MaxNum];	    /*雷达输出信号*/
		
	RADAR_STATE RadarCtrl[Radar_MaxNum];				/*雷达控制状态*/

	s16 XAxisDataOutput,								/*X轴输出参数*/
		YAxisDataOutput,								/*Y轴输出参数*/
		ZAxisDataOutput;								/*Z轴输出参数*/
}ROA_TypeDef;	

/*函数声明*/
RADAR_OBSTACLE_AVOIDANCE_EXT  void ROA_DataProcess(ROA_TypeDef *ROAData);

#endif

/*****************************************************END OF FILE*********************************************/
