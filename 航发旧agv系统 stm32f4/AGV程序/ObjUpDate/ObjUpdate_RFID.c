/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_RFID.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_RFID_GLOBALS

#include "ObjUpdate_RFID.h"
#include "ACC_Project.h"
/***********************************************************
** 函数名称:		ObjUpdate_RFID_init
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_RFID_init(void)
{
	if(g_Para.SerialExpObjSource.RFID == 1)
	{
		g_RFIDData.RFID_Source 	= RFID_InPutSource_SerialExp;
		g_RFIDData.InPutData	= EQP_SerialExp.p2Data.RFIDData;
	}
	else
	{
		g_RFIDData.RFID_Source  = RFID_InPutSource_MCB;
		g_RFIDData.InPutData	= &EQP_RFID.RD;
	}
}
/***********************************************************
** 函数名称:		ObjUpdate_RFID
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_RFID(void)
{
	static uint8_t STM = 0;
	//
	{
		switch(STM)
		{
			case 0:		/*初始化*/
			{
				ObjUpdate_RFID_init();
				STM = 1;

			}break;
			case 1:		/*正常运行*/
			{
				RFID_DataProcess(&g_RFIDData);
			}break;
		}
	}	
}

/************************************END OF FILE************************************/
