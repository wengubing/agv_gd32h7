/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_Mag.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_MAG_GLOBALS

#include "ObjUpdate_Mag.h"
#include "ACC_Project.h"

#include "drv_MagneticNavigationData.h"
/***********************************************************
** 函数名称:		ObjUpdate_Mag
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_Mag_init(void)
{
	/*前磁导航*/
	if(g_Para.SerialExpObjSource.MagF == 1)
	{
		g_MNData[0].MN_Source 	= MN_InPutSource_SerialExp;
		g_MNData[0].InPutData	= EQP_SerialExp.p2Data.F_MagData;
	}
	else
	{
		g_MNData[0].MN_Source 	= MN_InPutSource_MCB;
		g_MNData[0].InPutData	= &EQP_MagF.RD;
	}
	
	/*后磁导航*/
	if(g_Para.SerialExpObjSource.MagB == 1)
	{
		g_MNData[1].MN_Source 	= MN_InPutSource_SerialExp;
		g_MNData[1].InPutData	= EQP_SerialExp.p2Data.B_MagData;
	}
	else
	{
		g_MNData[1].MN_Source 	= MN_InPutSource_MCB;
		g_MNData[1].InPutData	= &EQP_MagB.RD;
	}
	
	/*左磁导航*/
	if(g_Para.SerialExpObjSource.MagL == 1)
	{
		g_MNData[2].MN_Source 	= MN_InPutSource_SerialExp;
		g_MNData[2].InPutData	= EQP_SerialExp.p2Data.L_MagData;
	}
	else
	{
		g_MNData[2].MN_Source 	= MN_InPutSource_MCB;
		g_MNData[2].InPutData	= &EQP_MagL.RD;
	}
	
	/*右磁导航*/
	if(g_Para.SerialExpObjSource.MagR == 1)
	{
		g_MNData[3].MN_Source 	= MN_InPutSource_SerialExp;
		g_MNData[3].InPutData	= EQP_SerialExp.p2Data.R_MagData;
	}
	else
	{
		g_MNData[3].MN_Source 	= MN_InPutSource_MCB;
		g_MNData[3].InPutData	= &EQP_MagR.RD;
	}
}
/***********************************************************
** 函数名称:		ObjUpdate_Mag
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_Mag(void)
{
	static uint8_t STM = 0;
	{
		switch(STM)
		{
			case 0:		/*初始化*/
			{
				ObjUpdate_Mag_init();
				STM = 1;
			}break;
			case 1:		/*正常运行*/
			{
				MN_DataProcess(&g_MNData[0]);
				MN_DataProcess(&g_MNData[1]);
				MN_DataProcess(&g_MNData[2]);
				MN_DataProcess(&g_MNData[3]);
			}break;
		}
	}
}

/************************************END OF FILE************************************/
