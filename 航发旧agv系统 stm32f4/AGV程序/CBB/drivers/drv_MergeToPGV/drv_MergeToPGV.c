/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : drv_MergeToPGV.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: 将对设定个数传感器的数据，融合成AGV中心偏移的PGV数据
  *******************************************************************************/
#define  DRV_MERGETOPGV_GLOBALS
#include "drv_MergeToPGV.h"
#include <string.h>
/*模块主要的函数方法调用*/
void Drv_MergeToPGV_Main(_MergeToPGV_TypeDef *pPGVData)
{
	switch (pPGVData->PGVFitType)
	{
		case MergeToPGVType_NULL: //
		default:
		break;
		case MergeToPGVType_Logical: /*1：前后两个航发PGV传入数据*/
		{
			_MergeToPGV_Offset_TypeDef	*p_Offset 	= &(pPGVData->PGVOffset);
			_MergeToPGV_BisHF_TypeDef	*p_InData 	= &(pPGVData->BisHFData);
			_MergeToPGV_OutPut_TypeDef	*p_OutPut	= &(pPGVData->PGVOut);

			/*前后都识别到，取平均值*/
			if(((p_InData->F_PGV_Sta== 1) && (p_InData->B_PGV_Sta == 1)))
			{
				p_OutPut->PGV_Sta = 0X01;
				p_OutPut->PGV_X = (p_InData->F_PGV_X+p_Offset->F_Offset_X + p_InData->B_PGV_X+p_Offset->B_Offset_X);
				p_OutPut->PGV_Y = (p_InData->F_PGV_Y+p_Offset->F_Offset_Y + p_InData->B_PGV_Y+p_Offset->B_Offset_Y);
				p_OutPut->PGV_Z_Projector = (p_InData->F_PGV_Z + p_InData->B_PGV_Z)+(p_Offset->F_Offset_Z+p_Offset->B_Offset_Z);
			}
			else if(p_InData->F_PGV_Sta == 1)
			{
				p_OutPut->PGV_Sta = 0X01;
				p_OutPut->PGV_X = (p_InData->F_PGV_X+p_Offset->F_Offset_X);
				p_OutPut->PGV_Y = (p_InData->F_PGV_Y+p_Offset->F_Offset_Y);
				p_OutPut->PGV_Z_Projector = (p_InData->F_PGV_Z+p_Offset->F_Offset_Z);
			}
			else if(p_InData->B_PGV_Sta == 1)
			{
				p_OutPut->PGV_Sta = 0X01;
				p_OutPut->PGV_X = (p_InData->B_PGV_X+p_Offset->B_Offset_X);
				p_OutPut->PGV_Y = (p_InData->B_PGV_Y+p_Offset->B_Offset_Y);
				p_OutPut->PGV_Z_Projector = (p_InData->B_PGV_Z+p_Offset->B_Offset_Z);
			}
			else
			{
				p_OutPut->PGV_Sta = 0;
				p_OutPut->PGV_X = 0;
				p_OutPut->PGV_Y = 0;
				p_OutPut->PGV_Z_Projector = 0;
			}

		}
		break;
		
		
		case MergeToPGVType_FB_BisHF: /*11：前后两个航发PGV传入数据*/
		{
			_MergeToPGV_Offset_TypeDef	*p_Offset 	= &(pPGVData->PGVOffset);
			_MergeToPGV_BisHF_TypeDef	*p_InData 	= &(pPGVData->BisHFData);
			_MergeToPGV_OutPut_TypeDef	*p_OutPut	= &(pPGVData->PGVOut);
			/*更新PGV状态，当两个PGV同时识别时，状态为1，否则为0*/
			p_OutPut->PGV_Sta = (p_InData->F_PGV_Sta) & (p_InData->B_PGV_Sta);
			if(p_OutPut->PGV_Sta == 1)
			{
				/*更新PGV数据*/
				p_OutPut->PGV_X = (p_InData->F_PGV_X+p_Offset->F_Offset_X + p_InData->B_PGV_X+p_Offset->B_Offset_X);
				p_OutPut->PGV_Y = (p_InData->F_PGV_Y+p_Offset->F_Offset_Y + p_InData->B_PGV_Y+p_Offset->B_Offset_Y);
				p_OutPut->PGV_Z_Projector = (p_InData->F_PGV_X - p_InData->B_PGV_X)+p_Offset->Offset_Z;
			}
			else
			{
				p_OutPut->PGV_X = 0;
				p_OutPut->PGV_Y = 0;
				p_OutPut->PGV_Z_Projector = 0;
			}

		}
		break;
		case MergeToPGVType_LR_BisHF: /*12：左右两个航发PGV传入数据*/
		{
			_MergeToPGV_BisHF_TypeDef	BisHF_Data;
			_MergeToPGV_OutPut_TypeDef	*p_OutPut	= &(pPGVData->PGVOut);
			memcpy(&BisHF_Data, &pPGVData->BisHFData, sizeof(_MergeToPGV_BisHF_TypeDef));
			/*更新PGV状态，当两个PGV同时识别时，状态为1，否则为0*/
			p_OutPut->PGV_Sta = (BisHF_Data.F_PGV_Sta) & (BisHF_Data.B_PGV_Sta);
			/*更新PGV数据*/
			p_OutPut->PGV_X = (BisHF_Data.F_PGV_X + BisHF_Data.B_PGV_X) / 2;
			p_OutPut->PGV_Y = (BisHF_Data.F_PGV_Y + BisHF_Data.B_PGV_Y) / 2;
			p_OutPut->PGV_Z_Projector = (BisHF_Data.F_PGV_Y - BisHF_Data.B_PGV_Y);
		}
		break;
	}
}
/************************************END OF FILE************************************/
