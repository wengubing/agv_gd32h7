/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Fun_Template.h
** 创 建 人: Master 
** 描    述: 项目配置文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.1
** 创建日期: 
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __Fun_Template_h__
#define __Fun_Template_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  Fun_Template_GLOBALS
    #define Fun_Template_EXT
#else
    #define Fun_Template_EXT  extern
#endif

//自定义引用
#include <stdbool.h>
#include "common_def.h"
#include "EQPComm_Init.h"

//自定义项目异常拓展 非受控停车
typedef struct
{
	u32 xxx1 : 1;									//xxx1故障
	u32 xxx2 : 1;									//xxx2故障
}ACC_Faults_LoseControl_PrjExp_BitDef_Template;
typedef union
{
	ACC_Faults_LoseControl_PrjExp_BitDef_Template Bit;		//位域
	u32 All;										//汇总
}ACC_Faults_LoseControl_PrjExp_UnionDef_Template;

//自定义项目异常拓展 受控停车
typedef struct
{
	u32 xxx1 : 1;									//xxx1故障
	u32 xxx2 : 1;									//xxx2故障
}ACC_Faults_ControlledStop_PrjExp_BitDef_Template;
typedef union
{
	ACC_Faults_ControlledStop_PrjExp_BitDef_Template Bit;	//位域
	u32 All;										//汇总
}ACC_Faults_ControlledStop_PrjExp_UnionDef_Template;

//自定义项目异常拓展 提示
typedef struct
{
	u32 xxx1 : 1;									//xxx1故障
	u32 xxx2 : 1;									//xxx2故障
}ACC_Faults_Warn_PrjExp_BitDef_Template;
typedef union
{
	ACC_Faults_Warn_PrjExp_BitDef_Template Bit;		//位域
	u32 All;										//汇总
}ACC_Faults_Warn_PrjExp_UnionDef_Template;

//项目外部自定义参数
typedef struct
{
	u16 xxx1;										//xxx1参数
}Para_EEPROM_PrjExp_StDef_Template;
/*自动对齐设计
typedef union
{
	Para_EEPROM_PrjExp_StDef Used;					//位域
	u16 Reserved[PrjExpEepNum - sizeof(Para_EEPROM_PrjExp_StDef)/2];//此定义用于自动对齐
}Para_EEPROM_PrjExp1_StDef;
*/

//goc 项目拓展 注意和系统的goc对象一样，只用于调试方便查看
typedef struct
{
	ACC_Faults_LoseControl_PrjExp_UnionDef_Template* p2PrjExp_LoseControl;
	ACC_Faults_ControlledStop_PrjExp_UnionDef_Template* p2PrjExp_ControlledStop;
	ACC_Faults_Warn_PrjExp_UnionDef_Template* p2PrjExp_Warn;
	Para_EEPROM_PrjExp_StDef_Template* p2PrjExp_EEPara;
}GOC_Template;

//项目自定义界面
typedef struct
{
	u16 xxx1;												//这是一个例子
}Screen_Typedef_Template;

//多倍通参数配置切换 状态机
typedef enum
{
	DBCOMSET_STM_Init = 0,								//初始化，等待配置开始
	DBCOMSET_STM_SwitchProtocol,						//切换串口协议
	DBCOMSET_STM_Wait,									//等待
	DBCOMSET_STM_SendCmd,								//发送配置指令1次？
	DBCOMSET_STM_WaitFeedBack,							//等待成功返回，持续检查长度和字符串匹配，超时跳返回超时
	DBCOMSET_STM_Timeout,								//返回超时
	DBCOMSET_STM_Success,								//返回成功，跳停止
	DBCOMSET_STM_Stop,									//停止,串口协议切换回modbus，返回初始化
}DBCOMSET_STM_Typedef;

//多倍通参数配置切换
typedef struct
{
	//参数
	u16 FeedBackTimeOut;									//反馈超时时间 ms

	bool En;												//1:开启通过串口切换多倍通配置功能
	//输入
	u16 In_Cmd;												//目标指令,暂未使用
	u16 In_SN;												//目标模块编号，暂未使用，由代码预制固定
	u16 In_Data;											//目标参数,目标模式：11-12 ~ 91-92
	bool In_SetStart;										//开始配置 自恢复按钮 1:开始
	bool In_SetExit;										//停止或退出配置 自恢复按钮 1:退出
	/*反馈信息 （字符串 + 长度）*/
	EQP_DBSET_WObj_StDef FeedBackStr;						//返回字符串缓冲区
	char FeedBackStr_Len;									//返回字符串长度 非0为有效
	
	//过程
	//发送命令
	//char SendCmdStr[13];									//发送字符串缓冲区
	EQP_DBSET_WObj_StDef SendCmdObj;						//发送结构化对象

	//状态机 任何时候有停止指令，都会跳到停止里
	DBCOMSET_STM_Typedef STM;
	/*
	初始化，等待配置开始
	切换串口协议，
	等待1s，
	发送配置指令1次？，
	等待成功返回，持续检查长度和字符串匹配，超时跳返回超时
	返回超时，
	返回成功，跳停止，
	停止,串口协议切换回modbus，返回初始化
	*/
	u32 MsRecord;											//ms记录



	//输出
	//状态机
	
}DBCOMSET_Typedef_Template;


//实例声明
Fun_Template_EXT GOC_Template goc_Template;
Fun_Template_EXT DBCOMSET_Typedef_Template g_DBComSet_Template;

//方法外部声明
Fun_Template_EXT void ACC_GetCmdFormWirelessRemote_Auto_Template(void);
Fun_Template_EXT bool ACC_GetCmdFormWirelessRemote_Template(void);
Fun_Template_EXT bool EmgJudge_ProjectExtend_Template(void);
Fun_Template_EXT bool HMI_Screen_PrjExp_Template(void);
Fun_Template_EXT bool PrjExp_AccStart_Template(void);
Fun_Template_EXT bool PrjExp_AccEnd_Template(void);
Fun_Template_EXT bool PrjExp_TskAccStart_Template(void);
Fun_Template_EXT bool PrjExp_MR_Init_Asb_Axis_Template(void);

#endif

//***********************************END OF FILE***********************************


