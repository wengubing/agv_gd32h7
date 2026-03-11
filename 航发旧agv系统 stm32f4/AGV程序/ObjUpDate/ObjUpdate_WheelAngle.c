/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_WheelAngle.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_WheelAngle_GLOBALS

#include "ObjUpdate_WheelAngle.h"
#include "LinearCal.h"
#include "ACC_Project.h"

/***********************************************************
** 函数名称:		ObjUpdate_WheelAngle
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_WheelAngle(void)
{
	u8 i = 0;
	u8 Num = g_Para.WheelAngle.Num;
	u32 temp = 0;

	//输入
	for (i = 0; i < Num; i++)
	{
		temp = EQP_CommConv.RD[2 + i * 2] + (EQP_CommConv.RD[2 + i * 2 + 1] << 16);
		g_WheelAngle[i].Org = (s32)temp;
	}

	//模拟器角度反馈到实际角度
	if (g_Para.Debug.MRSimEn)
	//if(0)//测试标定
	{
		for (i = 0; i < Num; i++)
			g_WheelAngle[i].Res = g_MotorResolving.VirAng.VirAng[i];
	}
	else
	{
		AngleCal(g_Para.WheelAngle.Num);//标定计算
	}

}

/************************************END OF FILE************************************/
