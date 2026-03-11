/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: main.c
** 创 建 人: hangfa  
** 描    述: 标准程序入口
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2016.04.09
** 标准化重构日期：2024.04.11
**--------------------------------------------------------------------------------------
***************************************************************************************/  

//包含对象头文件
#include "includes.h"
#include "bsp.h"
#include "AppTsk.h"
#include "ACC_Project.h"
#include "Debug.h"
#include "main.h"

/*重定向fputc函数，如果不重定向，硬件仿真时会卡在printf,放在这个位置为了在SSMLT的作用范围*/
int fputc(int ch, FILE* f)
{
#ifdef SSMLT
	/*使用Keil的ITM / SWO输出，硬件仿真时要初始化ITM和使能Trace，否则会卡在printf*/
	ITM_SendChar(ch);
	return ch;
#else
	/*重定义为空的实现，硬件仿真时不会卡在printf，也没有地方输出*/
	return 0;
#endif
}
/**************************************************************************
* 函数名:main
* 功  能:标准c程序入口
* 参  数:
* 返回值:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
**************************************************************************/
int main (void) 
{
	//初始化代码运行方式
	g_Para.RunningMode = ACC_RunningMode_Release;
#ifdef SSMLT
	g_Para.RunningMode = ACC_RunningMode_SoftwareSimulat;
#elif defined BareSSMLT
	g_Para.RunningMode = ACC_RunningMode_BareSimulat;
#endif

	/*printf测试*/
	printf("------------------Hello ACC!------------------\n");
	
	if(bsp_Init()!=INIT_OK)//初始化外设
	{
		g_CodeErr.Bit.InitErr=1;
#ifdef SSMLT
		printf("bsp_Init：异常!\n");
#endif
		return 0;
	}
	else/*创建启动任务*/
	{
#ifdef SSMLT
		printf("RTX：开始!\n");
#endif
		os_sys_init_user (AppTaskStart,						//任务函数
								APP_TASK_START_PRIO,		//任务优先级
								&AppTaskStartStk,			//任务栈
								sizeof(AppTaskStartStk));	//任务栈大小，单位字节数
	}

	while (1)
	{
		
	}
}

//***********************************END OF FILE***********************************
