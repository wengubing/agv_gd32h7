/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Para_EEPROM.h
** 创 建 人: 文小兵
** 描    述: EEPROM参数对象通用定义、项目定义、通用操作方法
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2024年8月27日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __Para_EEPROM_h__
#define __Para_EEPROM_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  Para_EEPROM_GLOBALS
    #define Para_EEPROM_EXT
#else
    #define Para_EEPROM_EXT  extern
#endif

//自定义引用	
#include <stdbool.h>
#include "common_def.h"
#include "Para_Sys.h"
#include "AgvChassisController.h"
#include "ACC_Project.h"

//宏定义
#define PrjExpEepNum 20										//项目外部自定义参数个数

//项目1 用户参数对象定义
//新增eeprom参数后：eobj2sys、sys2eobj
typedef struct
{
	Para_Lift_StDef Lift;								    //举升参数
	Para_Obo_StDef Obo;                                     //虚拟里程计参数
	Para_EmgJudge_TypeDef EmgJudge;						    //故障检出参数
	Para_Inst_TypeDef Inst;                                 //安装标定参数
	CtrlCoef_TypeDef CtrlCoef;                              //控制系数参数
	Para_Auto_StDef Auto;									//自动运行参数
	Para_Other_StDef Other;                            		//其他参数
	u16 nc[30];												//保留
}EEP_ParaPrj1_StDef;

//项目2 用户参数对象定义
typedef struct
{
	u16	Para;												//
}EEP_ParaPrj2_StDef;

//项目用户参数对象定义
typedef union
{
	EEP_ParaPrj1_StDef Prj1;								//项目1
	EEP_ParaPrj2_StDef Prj2;								//项目2
}EEP_ParaPrj_UnDef;



//EEPROM 用户参数对象定义
typedef struct
{
	ACC_EqpID_EnumDef EqpID;								//设备编号
	ACC_ChassisType_TypeDef	ChassisType;					//底盘类型
	CAN_BAUD_TYPE CanBaud;									//can波特率
	ACC_ParaFunctionSwitch_StDef FunctionSwitch;            //功能使能参数

	s16 WheelAngleZero[Para_WheelAngle_MAXNUM];				//舵轮角度0位置标定 0.01°

	u16 nc[24];												//保留

	EEP_ParaPrj_UnDef Prj;									//项目定义参数

	u16 PrjExp[PrjExpEepNum];								//项目外部自定义参数
}EEP_ParaUser_StDef;

//EEPROM参数对象定义
typedef struct
{
	u32	WriteCnt;											//EE写入次数 0~1
	u32	WriteDate;											//最后一次写入时间:1512312359,15年12月31日23时59分
	u32	ProductSerial;										//产品序列号 4~5
	EEP_ParaUser_StDef User;								//用户数据 6~xxx0
	u32 CodeVersion;					                    //代码版本;								                //检查参数 xxx1~xxx2
	u16 Reserved[EEPARA_U16NUM - 7 - sizeof(EEP_ParaUser_StDef) / 2 - 2];//占位数据 xxx4~298
	//↑为固定定义，中间为用户定义，最后一个为和校验，总大小300 U16
	u16	CRCSum;												//CRC校验和 299
}EEP_Para_TypeDef;

//对象外部声明
Para_EEPROM_EXT EEP_Para_TypeDef EEPPara;					//EEPROM参数对象




#endif

//***********************************END OF FILE***********************************


