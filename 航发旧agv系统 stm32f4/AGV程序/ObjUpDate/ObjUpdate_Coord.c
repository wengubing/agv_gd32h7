/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_Coord.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_COORD_GLOBALS

#include "ObjUpdate_Coord.h"
#include "ACC_Project.h"

void ObjUpdate_Coord_init(void)
{
	//SLAM导航的数据更新
	if(g_Para.SerialExpObjSource.IPC == 1)
	{
		g_ROKITData.ROKIT_Source 	= ROKIT_InPutSource_SerialExp;
		g_ROKITData.InPutReadData	= EQP_SerialExp.p2Data.IPC_ReadData;
		g_ROKITData.OutPutWriteData = EQP_SerialExp.p2Data.IPC_WriteData;
	}
	else
	{
		g_ROKITData.ROKIT_Source 	= ROKIT_InPutSource_MCB;
		g_ROKITData.InPutReadData	= &EQP_IPC.RD;
		g_ROKITData.OutPutWriteData = &EQP_IPC.WD;
	}
	
}

/***********************************************************
** 函数名称:		ObjUpdate_Coord
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_Coord(void)
{
	
	static uint8_t STM = 0;
	//
	{
		switch(STM)
		{
			case 0:		/*初始化*/
			{
				ObjUpdate_Coord_init();
				STM = 1;
			}break;
			case 1:		/*正常运行*/
			{
				//GPS导航的数据更新

				//Slam数据更新
				ROKIT_DataProcess(&g_ROKITData);
			}break;
		}
	}
	
	
}

/************************************END OF FILE************************************/
