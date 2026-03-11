/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_IO.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_IO_GLOBALS

#include "ObjUpdate_IO.h"
#include "IOConf.h"
#include "drv_IO.h"
#include "ACC_Init.h"
#include "ACC_Project.h"

/***********************************************************
** 函数名称:		ObjUpdate_IOInput
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_IOInput(void)
{
	u16 i = 0;
	IO_Input_TypeDef* p2IO = (IO_Input_TypeDef*)&g_IOInputs;

	for (i = 0; i < g_IOInputs.Number; i++)
	{
		//更新实时值
		if(p2IO[i].ID != ICN_Disable && g_Para.Debug.IOInputOff == 0)//接口配置有效	
			p2IO[i].RealtimeValue = GET_MODBUS_COIL(p2IO[i].ID);
		else {}
		
		//滤波更新
		p2IO[i].Filter.in_RealTimeStatus = p2IO[i].RealtimeValue;
		IOFilter_ButtonScan(&(p2IO[i].Filter), AccData.in_Ms);
		
		if(1)//滤波处理
		{
			if (p2IO[i].Filter.out_Status >= IOFilterButtonStatus_PressMidDown)
				p2IO[i].StableValue = 1;
			else
				p2IO[i].StableValue = 0;
		}
		else {}
	}
}
/***********************************************************
** 函数名称:		ObjUpdate_IOOutput
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_IOOutput(void)
{
	u16 i = 0;
	IO_Output_TypeDef* p2IO = (IO_Output_TypeDef*)&g_IOOutputs;
	//bool *p2IOBit = (bool*)&IO_DHR;//得到的是char类型，需要转换为bool类型

	for (i = 0; i < g_IOOutputs.Number; i++)
	{
		//更新实时值
		//p2IOBit[p2IO[i].ID] = p2IO[i].OutValue;
		if(p2IO[i].ID < OCN_Disable 
		&& p2IO[i].ID >= IOBit_YOUT0)//接口配置有效 防止错误配置而越界
			ASS_MODBUS_COIL(p2IO[i].ID,p2IO[i].OutValue);
		//IO_DHR[((p2IO[i].ID)/16)]=(IO_DHR[((p2IO[i].ID)/16)] &= ~(0x0001 << (p2IO[i].ID&0x0F))) | ((p2IO[i].OutValue << (p2IO[i].ID&0x0F)));
	}
}
/************************************END OF FILE************************************/
