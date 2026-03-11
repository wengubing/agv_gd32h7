/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_Color_Band.c
	* Version    :	v1.0
	* Author     :	MT
	* Date       : 2025-06-17
	* Description:
	* 说    明：目前只是适配的448转接板，倍加福PGV数据；还需持续吸收其他项目完善
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define OBJUPDATE_COLOR_BAND_GLOBALS


#include "ObjUpdate_Color_Band.h"
#include "ACC_Project.h"

#include "drv_ColorBand_PGV.h"
/***********************************************************
** 函数名称:		ObjUpdate_AllAround_Color_Band_init
** 功能描述: 	四周PGV色带巡线方式的数据管道初始化；
				没有传感器共用到横向/正向的情况
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_AllAround_Color_Band_init(void)
{
	/*前色带PGV*/
	{
		ColorBandType_EnumDef 	*p_Color_Sensor = & g_ColorBand[0].Color_Sensor;
		ColorBand_TypeDef		*p_InPutData	= &(g_ColorBand[0]);
		if(g_Para.SerialExpObjSource.PGVDownward_F == 1)
		{
			/*连接导航板*/
			*p_Color_Sensor			= NCS_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_SerialExp.p2Data.PGVDownward_FData;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -1.0f;}
		}
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Downward_F == 1)
		{
			/*倍加福直连*/
			*p_Color_Sensor			= BJF_F_B_Sensor;
			(*p_InPutData).InPutData= &EQP_BJF_PGV_Downward_F.BJF_PGV_Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	=  1.0f;}
		}
		else
		{
			/*HF直连*/
			*p_Color_Sensor			= HF_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_PGVDownward_F.p2Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -0.1f;}
		}
	}
	/*后色带PGV*/
	{
		ColorBandType_EnumDef 	*p_Color_Sensor = & g_ColorBand[1].Color_Sensor;
		ColorBand_TypeDef		*p_InPutData	= &(g_ColorBand[1]);
		if(g_Para.SerialExpObjSource.PGVDownward_B == 1)
		{
			/*连接导航板*/
			*p_Color_Sensor			= NCS_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_SerialExp.p2Data.PGVDownward_BData;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -1.0f;}
		}
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Downward_B == 1)
		{
			/*倍加福直连*/
			*p_Color_Sensor			= BJF_F_B_Sensor;
			(*p_InPutData).InPutData= &EQP_BJF_PGV_Downward_B.BJF_PGV_Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	=  1.0f;}
		}
		else
		{
			/*HF直连*/
			*p_Color_Sensor			= HF_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_PGVDownward_B.p2Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -0.1f;}
		}
	}
	/*左色带PGV*/
	{
		ColorBandType_EnumDef 	*p_Color_Sensor = & g_ColorBand[2].Color_Sensor;
		ColorBand_TypeDef		*p_InPutData	= &(g_ColorBand[2]);
		if(g_Para.SerialExpObjSource.PGVUpward_F == 1)
		{
			/*连接导航板*/
			*p_Color_Sensor			= NCS_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_SerialExp.p2Data.PGVUpward_FData;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -1.0f;}
			
		}
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Upward_F == 1)
		{
			/*倍加福直连*/
			*p_Color_Sensor			= BJF_F_B_Sensor;
			(*p_InPutData).InPutData= &EQP_BJF_PGV_Upward_F.BJF_PGV_Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	=  1.0f;}
		}
		else
		{
			/*HF直连*/
			*p_Color_Sensor			= HF_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_PGVUpward_F.p2Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -0.1f;}
		}
	}
	/*右色带PGV*/
	{
		ColorBandType_EnumDef 	*p_Color_Sensor = & g_ColorBand[3].Color_Sensor;
		ColorBand_TypeDef		*p_InPutData	= &(g_ColorBand[3]);
		if(g_Para.SerialExpObjSource.PGVUpward_B == 1)
		{
			/*连接导航板*/
			*p_Color_Sensor			= NCS_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_SerialExp.p2Data.PGVUpward_BData;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -1.0f;}
		}
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Upward_B == 1)
		{
			/*倍加福直连*/
			*p_Color_Sensor			= BJF_F_B_Sensor;
			(*p_InPutData).InPutData= &EQP_BJF_PGV_Upward_B.BJF_PGV_Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	=  1.0f;}
		}
		else
		{
			/*HF直连*/
			*p_Color_Sensor			= HF_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_PGVUpward_B.p2Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -0.1f;}
		}
	}
	
}
/***********************************************************
** 函数名称:		ObjUpdate_AllAround_Color_Band_init
** 功能描述: 	经济PGV色带巡线方式的数据管道初始化；
				AGV中心有传感器，即是横向的右侧也是正向的后侧
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_Economics_Color_Band_init(void)
{
	/*前色带PGV*/
	{
		ColorBandType_EnumDef 	*p_Color_Sensor = & g_ColorBand[0].Color_Sensor;
		ColorBand_TypeDef		*p_InPutData	= &(g_ColorBand[0]);
		if(g_Para.SerialExpObjSource.PGVDownward_F == 1)
		{
			/*连接导航板*/
			*p_Color_Sensor			= NCS_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_SerialExp.p2Data.PGVDownward_FData;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -1.0f;}
		}
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Downward_F == 1)
		{
			/*倍加福直连*/
			*p_Color_Sensor			= BJF_F_B_Sensor;
			(*p_InPutData).InPutData= &EQP_BJF_PGV_Downward_F.BJF_PGV_Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	=  1.0f;}
		}
		else
		{
			/*HF直连*/
			*p_Color_Sensor			= HF_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_PGVDownward_F.p2Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -0.1f;}
		}
	}
	/*后色带PGV*/
	{
		ColorBandType_EnumDef 	*p_Color_Sensor = & g_ColorBand[1].Color_Sensor;
		ColorBand_TypeDef		*p_InPutData	= &(g_ColorBand[1]);
		if(g_Para.SerialExpObjSource.PGVDownward == 1)
		{
			/*连接导航板*/
			*p_Color_Sensor			= NCS_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_SerialExp.p2Data.PGVDownwardData;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -1.0f;}
		}
		else
		{
			/*HF直连*/
			*p_Color_Sensor			= HF_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_PGVDownward_B.p2Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -0.1f;}
		}
	}
	/*左色带PGV*/
	{
		ColorBandType_EnumDef 	*p_Color_Sensor = & g_ColorBand[2].Color_Sensor;
		ColorBand_TypeDef		*p_InPutData	= &(g_ColorBand[2]);
		if(g_Para.SerialExpObjSource.PGVUpward_F == 1)
		{
			/*连接导航板*/
			*p_Color_Sensor			= NCS_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_SerialExp.p2Data.PGVUpward_FData;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -1.0f;}
		}
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Upward_F == 1)
		{
			/*倍加福直连*/
			*p_Color_Sensor			= BJF_F_B_Sensor;
			(*p_InPutData).InPutData= &EQP_BJF_PGV_Upward_F.BJF_PGV_Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	=  1.0f;}
		}
		else
		{
			/*HF直连*/
			*p_Color_Sensor			= HF_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_PGVUpward_F.p2Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -0.1f;}
		}
	}
	/*右色带PGV*/
	{
		ColorBandType_EnumDef 	*p_Color_Sensor = & g_ColorBand[3].Color_Sensor;
		ColorBand_TypeDef		*p_InPutData	= &(g_ColorBand[3]);
		if(g_Para.SerialExpObjSource.PGVDownward == 1)
		{
			/*连接导航板*/
			*p_Color_Sensor			= NCS_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_SerialExp.p2Data.PGVDownwardData;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -1.0f;}
		}
		else
		{
			/*HF直连*/
			*p_Color_Sensor			= HF_F_B_Sensor;
			(*p_InPutData).InPutData= EQP_PGVDownward.p2Data;
			if(p_InPutData->Color_diff == 0.0f)	{p_InPutData->Color_diff	= -0.1f;}
		}
	}
	
}
/***********************************************************
** 函数名称:		ObjUpdate_Color_Band
** 功能描述: 	色带数据更新
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_Color_Band(void)
{
	static uint8_t STM = 0;
	//
	{
		switch(STM)
		{
			case 0:		/*初始化*/
			{
				//暂时没想好怎么自动区分 4个PGV还是3个PGV巡线
				if(1)
				{
					ObjUpdate_AllAround_Color_Band_init();
				}
				else
				{
					ObjUpdate_Economics_Color_Band_init();
				}
				STM = 1;
			}break;
			case 1:		/*正常运行*/
			{
				DataSourceSelection(&g_ColorBand[0]);
				DataSourceSelection(&g_ColorBand[1]);
				DataSourceSelection(&g_ColorBand[2]);
				DataSourceSelection(&g_ColorBand[3]);
			}break;
		}
	}

}

/************************************END OF FILE************************************/


