/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: bsp.h
** 创 建 人: hangfa  
** 描    述: 硬件初始化
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2016.04.09
** 标准化重构日期：2024.05.25
**--------------------------------------------------------------------------------------
***************************************************************************************/  
//保证以下所有内容只被同一源文件引用一次
#ifndef __BSP_H__
#define __BSP_H__

//头文件
#include "includes.h"	

//确保仅在该文件编译时实例化一次对象
#ifdef  bsp_GLOBALS
	#define bsp_EXT
#else
	#define bsp_EXT  extern
#endif

//EEPROM工作流程定义
typedef enum
{
    INIT_NULL=0,
    INIT_OK,
    INIT_ERROR,
}SYS_INI;

//系统状态拓展 不直接参与控制报警 仅在hmi显示
typedef union
{
	struct
	{
		u16 EEP_CodeVersionErr : 1;      					//参数版本异常
		u16 EEP_ReadErr : 1;      							//eeprom读取错误
		u16 nc : 14;      									//保留
	}Bit;
	u16 All;
}SysBitExpand_StDef;

//系统对象存储大小统计
typedef struct
{
    u16 Acc;                                                //底盘核心模块对象
    u16 Auto;                                               //自动模块对象
    u16 PathStack;                                          //路径栈对象
    u16 Hmi;                                                //人机交互模块对象
    u16 SVO;                                                //伺服信息对象
    u16 CanOpen_Obj;                                        //canopen对象
}SysObjSizeOf_StDef;

//硬件信息
typedef struct
{
    u32 uID[3];                                             //设备ID
	f32 CoreTemp;                                           //芯片温度
	SysBitExpand_StDef SysBitExpand;						//系统状态拓展位
    SysObjSizeOf_StDef SysObjSizeOf;						//系统对象大小统计
}HardWareInfo_StDef;

//全局对象声明
bsp_EXT RCC_ClocksTypeDef RCC_ClocksStatus;                 //系统时钟结构体
bsp_EXT HardWareInfo_StDef HardInfo;                        //硬件信息
//全局函数声明
bsp_EXT u8 bsp_Init(void);                                  //硬件初始化

#endif

//************************************END OF FILE***********************************
