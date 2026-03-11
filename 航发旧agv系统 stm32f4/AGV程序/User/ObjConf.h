/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ObjConf.h
** 创 建 人: 文小兵  
** 描    述: 对象配置
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2024年5月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __ObjConf_h__
#define __ObjConf_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  ObjConf_GLOBALS
	#define ObjConf_EXT
#else
	#define ObjConf_EXT  extern
#endif

//自定义引用	
#include <stdbool.h>

//宏定义

//ms记录对象
typedef struct
{
    char nc;
    //unsigned int Tsk10ms;                                   //10毫秒任务
    //unsigned int Tsk100ms;                                  //100毫秒任务
    //unsigned int Tsk1s;                                     //1秒任务
    //unsigned int ForceReset;                                //强制功能复位
    //unsigned int AGVTskLed;                                 //agv自动任务指示闪烁  
    //unsigned int AGVResetLed;                               //agv复位指示闪烁
}MsRecord_StDef;


//外部对象
ObjConf_EXT MsRecord_StDef MsRecord;                        //ms记录对象

//外部方法
ObjConf_EXT bool ParaInit_EEPObj2Sys(void);					//从eeeprom对象中读取参数
ObjConf_EXT bool ParaInit(void);                            //系统参数初始化
ObjConf_EXT bool ParaInit_Sys2EEPObj(void);				 	//系统参数写入到eeeprom对象

ObjConf_EXT bool ObjInit(void);                             //对象初始化

//通用参数初始化
ObjConf_EXT bool Para_FunctionSwitch_Un(void);              //功能使能
ObjConf_EXT bool Para_EmgJudge_Un(void);                    //紧急停车判断

#endif

//************************************END OF FILE***********************************


