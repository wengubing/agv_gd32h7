/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: IOConf.c
** 创 建 人: 文小兵  
** 描    述: IO抽象对象定义
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期:  2024年8月23日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define IOConf_GLOBALS

//自定义引用			
#include "IOConf.h"

#include "ACC_Project.h"

//外部对象声明

//内部方法声明
//输入io对象初始化
bool ObjInit_IOInputs(void)
{
	u16 i = 0;
	IO_Inputs_TypeDef* p2IOs = &g_IOInputs;
	IO_Input_TypeDef* p2IO = (IO_Input_TypeDef*)&g_IOInputs;

	for (i = 0; i < p2IOs->Number; i++)
	{
		p2IO[i].Filter.PressTimePara.PressShortDown = 100;
		p2IO[i].Filter.PressTimePara.PressMidDown = 200;
		p2IO[i].Filter.PressTimePara.PressNormalDown = 300;
		p2IO[i].Filter.PressTimePara.PressShortLongDown = 1000;
		p2IO[i].Filter.PressTimePara.PressLongDown = 2000;
	}
	return 1;
}

//输出io对象初始化
bool ObjInit_IOOutputs(void)
{
	if (1)
		return 1;
	else
		return 0;
}


//***********************************END OF FILE***********************************

