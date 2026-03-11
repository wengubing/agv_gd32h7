/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Conf_User.c
** 创 建 人: 文小兵
** 描    述: 外部项目配置文件 该文件用于核心代码之外通过系统接口配置相关内容 但是与其它项目配置文件不同 该文件不和具体某个项目绑定
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2025年8月12日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define Conf_User_GLOBALS

//自定义引用		
#include "Conf_User.h"
#include "ACC_Project.h"
#include "Conf_Template.h"
#include "Debug.h"
//根据项目修改具体头文件
//#include "Conf_465.h"

//外部对象声明

//内部方法声明

//运行Init 外部
bool Run_Init_User(void)
{
	/*user自定义Init代码，注意运行位置！*/

	/*跳过系统初始化中 EqpID的初始化 并在此处指定EqpID的值
	AtDbug.Bit.Skip_EqpIDSet = 1;
	g_Para.EqpID = EqpID_Template;*/

	/*跳过系统初始化中 项目参数从eep中读取,用于在项目调试中不想覆盖原有系统的eep内容时
	AtDbug.Bit.Skip_EEP2Sys = 1;*/

	return 1;
}

//项目参数设置函数指针初始化
//注意这个函数在项目中要根据自己项目的EqpID进行修改，但是不必吸收到std
bool ParaInit_p2FucInit(ACC_EqpID_EnumDef EqpID)
{
	bool res = 1;

	switch (EqpID)
	{
	case EqpID_MDKSimulator:
		g_Para.p2Fucs.p2PrjParaInit = Para_Template;
		break;
	case EqpID_BareSimulat:
		g_Para.p2Fucs.p2PrjParaInit = Para_Template;
		break;
	case EqpID_HardwareSimulat:
		g_Para.p2Fucs.p2PrjParaInit = Para_Template;
		break;
	case EqpID_Template:
		g_Para.p2Fucs.p2PrjParaInit = Para_Template;
		break;
		//以上4个模式都只用于调试测试
	
	/*项目
	case EqpID_46501:
	case EqpID_46502:
		g_Para.p2Fucs.p2PrjParaInit = Para_465;
		break;*/

	default:
		g_Para.p2Fucs.p2PrjParaInit = Para_Template;
		res = 0;
		break;
	}
	return res;
}

//外部对象声明

//内部方法声明



//***********************************END OF FILE***********************************

