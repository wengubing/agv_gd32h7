/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_API_Slave.c
	* Version    :	v1.0
	* Author     :	MT
	* Date       : 2025-12-08
	* Description:
	* 说    明	：
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define ObjUpdate_API_Slave_GLOBALS
#include <stm32f4xx.h>


#include "ACC_Project.h"
#include "EQPComm_Init.h"
#include "ObjUpdate_API_Slave.h"


/***********************************************************
** 函数名称:	API_Slave_InPut
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
__weak void API_Slave_InPut(void)
{
	//EQP_API_Slave.Data[0] = 0; /*预留*/

	EQP_API_Slave.Data[1] = g_Para.EqpID;		 	/*设备序列号*/
	EQP_API_Slave.Data[2] = 1000;					/*硬件版本*/
	EQP_API_Slave.Data[3] = g_HostData.SoftVerSion;	/*软件版本*/

	//EQP_API_Slave.Data[4] = 0; /*预留*/

}

/***********************************************************
** 函数名称:	API_Slave_OutPut
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
__weak void API_Slave_OutPut(void)
{

	
}
/***********************************************************
** 函数名称:		ObjUpdate_API_Slave
** 功能描述: 	色带数据更新
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_API_Slave(void)
{
	static uint8_t STM = 0;
	//
	{
		switch(STM)
		{
			case 0:		/*初始化*/
			{
				if(0)
				{
					
				}
				else
				{
					STM = 1;
				}
				
			}break;
			case 1:		/*正常运行*/
			{
				API_Slave_InPut();
				API_Slave_OutPut();
			}break;
		}
	}

}

/************************************END OF FILE************************************/


