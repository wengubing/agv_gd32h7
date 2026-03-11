/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_PGV.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_PGV_GLOBALS

#include "ObjUpdate_PGV.h"
#include "ACC_Project.h"
#include "ACC_AssemlyCtrl.h"


//单一PGV数据更新
void PGV_SolePaparDataInit(void)
{
	if(g_Para.PGVInputType.HF_Tool_Sole == 1)
	{
		g_ToolPGV.PGV_Source  		= PGV_InPutSource_MCB;
		g_ToolPGV.PGV_Sensor		= PGV_Sensor_HF;
		g_ToolPGV.InPutData			= &EQP_PGVUpward.RD;
	}
	else if(g_Para.PGVInputType.DH_Tool_Sole == 1)
	{
		//暂无实现
	}
	else if(g_Para.PGVInputType.BJF_Tool_Sole == 1)
	{
		g_ToolPGV.PGV_Source 		= PGV_InPutSource_MCB;
		g_ToolPGV.PGV_Sensor 		= PGV_Sensor_BJF;
		g_ToolPGV.InPutData			= &EQP_BJF_PGV_Upward.BJF_PGV_Data;
	}
	else if(g_Para.PGVInputType.SE_Tool_Sole == 1)
	{
		g_ToolPGV.PGV_Source 		= PGV_InPutSource_SerialExp;
		g_ToolPGV.PGV_Sensor 		= PGV_Sensor_HF;	/*单个PGV的时候，导航转接板下不关心具体传感器是什么*/
		g_ToolPGV.InPutData			= EQP_SerialExp.p2Data.PGVUpwardData;
	}

	if(g_Para.PGVInputType.HF_Groud_Sole == 1)
	{
		g_GroundPGV.PGV_Source		= PGV_InPutSource_MCB;
		g_GroundPGV.PGV_Sensor		= PGV_Sensor_HF;
		g_GroundPGV.InPutData		= &EQP_PGVDownward.RD;
	}
	else if(g_Para.PGVInputType.DH_Groud_Sole == 1)
	{
		//暂无实现
	}
	else if(g_Para.PGVInputType.BJF_Groud_Sole == 1)
	{
		g_GroundPGV.PGV_Source 		= PGV_InPutSource_MCB;
		g_GroundPGV.PGV_Sensor 		= PGV_Sensor_BJF;
		g_GroundPGV.InPutData		= &EQP_BJF_PGV_Downward.BJF_PGV_Data;
	}
	else if(g_Para.PGVInputType.SE_Groud_Sole == 1)
	{
		g_GroundPGV.PGV_Source 		= PGV_InPutSource_SerialExp;
		g_GroundPGV.PGV_Sensor 		= PGV_Sensor_HF;	/*单个PGV的时候，导航转接板下不关心具体传感器是什么*/
		g_GroundPGV.InPutData		= EQP_SerialExp.p2Data.PGVDownwardData;
	}

	//511项目特殊处理，将中间PGV的数据给到自动框架的二维码停车对象中
	{
		g_NavigationPGV.PGV_Source 		= PGV_InPutSource_MCB;
		g_NavigationPGV.PGV_Sensor 		= PGV_Sensor_BJF;
		g_NavigationPGV.InPutData		= &EQP_BJF_PGV_Downward.BJF_PGV_Data;
	}
}


void PGV_MergePaparDataInit(void)
{
	//单动下的PGV数据融合
	{
		if(g_Para.PGVInputType.HF_Tool_Merge == 1)
		{
			g_MergeToPGV.PGVFitType				= MergeToPGVType_FB_BisHF;
			
			/*PGV数据抽象调用*/
			g_ToolPGV.PGV_Source  				= PGV_InPutSource_MCB;
			g_ToolPGV.PGV_Sensor				= PGV_Sensor_HF_Merge;
			g_ToolPGV.InPutData					= &(g_MergeToPGV.PGVOut);
		}
		else if(g_Para.PGVInputType.DH_Tool_Merge == 1)
		{
			//暂无实现
		}
		else if(g_Para.PGVInputType.BJF_Tool_Merge == 1)
		{
			g_MergeToPGV.PGVFitType				= MergeToPGVType_FB_BisHF;

			/*PGV数据抽象调用*/
			g_ToolPGV.PGV_Source  				= PGV_InPutSource_MCB;
			g_ToolPGV.PGV_Sensor				= PGV_Sensor_BJF;
			g_ToolPGV.InPutData					= &(g_MergeToPGV.PGVOut);
		}
		else if(g_Para.PGVInputType.SE_Tool_Merge == 1)
		{
			g_MergeToPGV.PGVFitType				= MergeToPGVType_FB_BisHF;

			/*PGV数据抽象调用*/
			g_ToolPGV.PGV_Source  				= PGV_InPutSource_SerialExp;
			g_ToolPGV.PGV_Sensor				= PGV_Sensor_HF_Merge;
			g_ToolPGV.InPutData					= &(g_MergeToPGV.PGVOut);
		}

		if(g_Para.PGVInputType.HF_Groud_Merge == 1)
		{
			g_MergeToPGV.PGVFitType				= MergeToPGVType_FB_BisHF;

			/*PGV数据抽象调用*/
			g_GroundPGV.PGV_Source  			= PGV_InPutSource_MCB;
			g_GroundPGV.PGV_Sensor				= PGV_Sensor_HF_Merge;
			g_GroundPGV.InPutData				= &(g_MergeToPGV.PGVOut);
		}
		else if(g_Para.PGVInputType.DH_Groud_Merge == 1)
		{
			//暂无实现
		}
		else if(g_Para.PGVInputType.BJF_Groud_Merge == 1)
		{
			g_MergeToPGV.PGVFitType				= MergeToPGVType_FB_BisHF;

			/*PGV数据抽象调用*/
			g_GroundPGV.PGV_Source  			= PGV_InPutSource_MCB;
			g_GroundPGV.PGV_Sensor				= PGV_Sensor_HF_Merge;
			g_GroundPGV.InPutData				= &(g_MergeToPGV.PGVOut);
		}
		else if(g_Para.PGVInputType.SE_Groud_Merge == 1)
		{
			g_MergeToPGV.PGVFitType				= MergeToPGVType_FB_BisHF;

			/*PGV数据抽象调用*/
			g_GroundPGV.PGV_Source  				= PGV_InPutSource_SerialExp;
			g_GroundPGV.PGV_Sensor					= PGV_Sensor_HF_Merge;
			g_GroundPGV.InPutData					= &(g_MergeToPGV.PGVOut);
		}
	}
	//联动下的PGV数据融合
	{
		//覆盖单动下的PGV融合，当联动下出现
		if(g_Para.PGVInputType.SE_Groud_Merge == 1)
		{
			g_MergeToPGV.PGVFitType				= MergeToPGVType_Logical;

			/*PGV数据抽象调用*/
			g_GroundPGV.PGV_Source	= PGV_InPutSource_SerialExp;
			g_GroundPGV.PGV_Sensor	= PGV_Sensor_HF_Merge;
			g_GroundPGV.InPutData	= &(g_MergeToPGV.PGVOut);
		}
		
		//主车才需要跨车的PGV数据融合作为定位结果
		//if(	  ACC_Assemly_OutData.Assemly_OutRole == ACC_AssemlyOutRole_Master)
		if(0)
		{
			if(g_Para.PGVInputType.SE_Groud_Merge == 1)
			{
				g_AssemlyMergeToPGV.PGVFitType				= MergeToPGVType_FB_BisHF;

				/*PGV数据抽象调用*/
				g_AssemlyGroundPGV.PGV_Source	= PGV_InPutSource_Assemly;
				g_AssemlyGroundPGV.PGV_Sensor	= PGV_Sensor_HF_Merge;
				g_AssemlyGroundPGV.InPutData	= &(g_AssemlyMergeToPGV.PGVOut);
			}
		}
		//从车数据需要将本车的PGV结果做逻辑与发给主车
		else
		{
			g_AssemlyMergeToPGV.PGVFitType	= MergeToPGVType_Logical;

			g_AssemlyGroundPGV.PGV_Source	= PGV_InPutSource_Assemly;
			g_AssemlyGroundPGV.PGV_Sensor	= PGV_Sensor_HF_Merge;
			g_AssemlyGroundPGV.InPutData	= &(g_AssemlyMergeToPGV.PGVOut);
			
		}
		
	}

}

/***********************************************************
** 函数名称:	PGV_MergeUpData
** 功能描述: PGV数据融合
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//PGV数据融合
void PGV_MergeUpData(void)
{
	//单动下的PGV数据融合
	{
		if(g_Para.PGVInputType.HF_Tool_Merge == 1)
		{
			/*数据融合调用*/
			EQP_HFPGVObj_StDef* 		p_f 	= EQP_PGVUpward_F.p2Data;
			EQP_HFPGVObj_StDef* 		p_b 	= EQP_PGVUpward_B.p2Data;
			_MergeToPGV_BisHF_TypeDef 	*p_Data = &(g_MergeToPGV.BisHFData);
			
			p_Data->F_PGV_Sta 	= p_f->PGV_IdentifyStatus;
			p_Data->F_PGV_X 	= p_f->PGV_Xpositon;
			p_Data->F_PGV_Y 	= p_f->PGV_Ypositon;

			p_Data->B_PGV_Sta 	= p_b->PGV_IdentifyStatus;
			p_Data->B_PGV_X 	= p_b->PGV_Xpositon;
			p_Data->B_PGV_Y 	= p_b->PGV_Ypositon;

		}
		else if(g_Para.PGVInputType.DH_Tool_Merge == 1)
		{
			//暂无实现
		}
		else if(g_Para.PGVInputType.BJF_Tool_Merge == 1)
		{
			EQP_BJF_PGVObj_StDef* 		p_f		= &EQP_BJF_PGV_Upward_F.BJF_PGV_Data;
			EQP_BJF_PGVObj_StDef* 		p_b		= &EQP_BJF_PGV_Upward_B.BJF_PGV_Data;
			_MergeToPGV_BisHF_TypeDef 	*p_Data = &(g_MergeToPGV.BisHFData);

			p_Data->F_PGV_Sta 	= p_f->State;
			p_Data->F_PGV_X 	= p_f->Xpositon;
			p_Data->F_PGV_Y 	= p_f->Ypositon;

			p_Data->B_PGV_Sta 	= p_b->State;
			p_Data->B_PGV_X 	= p_b->Xpositon;
			p_Data->B_PGV_Y 	= p_b->Ypositon;
		}
		else if(g_Para.PGVInputType.SE_Tool_Merge == 1)
		{
			PgvObjDef* 		p_f 				= EQP_SerialExp.p2Data.PGVUpward_FData;
			PgvObjDef* 		p_b 				= EQP_SerialExp.p2Data.PGVUpward_BData;
			_MergeToPGV_BisHF_TypeDef 	*p_Data = &(g_MergeToPGV.BisHFData);

			p_Data->F_PGV_Sta 	= p_f->ReadSta;
			p_Data->F_PGV_X 	= p_f->X_Value;
			p_Data->F_PGV_Y 	= p_f->Y_Value;

			p_Data->B_PGV_Sta 	= p_b->ReadSta;
			p_Data->B_PGV_X 	= p_b->X_Value;
			p_Data->B_PGV_Y 	= p_b->Y_Value;
		}

		if(g_Para.PGVInputType.HF_Groud_Merge == 1)
		{
			EQP_HFPGVObj_StDef* 		p_f 	= EQP_PGVDownward_F.p2Data;
			EQP_HFPGVObj_StDef* 		p_b 	= EQP_PGVDownward_B.p2Data;
			_MergeToPGV_BisHF_TypeDef 	*p_Data = &(g_MergeToPGV.BisHFData);

			p_Data->F_PGV_Sta 	= p_f->PGV_IdentifyStatus;
			p_Data->F_PGV_X 	= p_f->PGV_Xpositon;
			p_Data->F_PGV_Y 	= p_f->PGV_Ypositon;

			p_Data->B_PGV_Sta 	= p_b->PGV_IdentifyStatus;
			p_Data->B_PGV_X 	= p_b->PGV_Xpositon;
			p_Data->B_PGV_Y 	= p_b->PGV_Ypositon;
		}
		else if(g_Para.PGVInputType.DH_Groud_Merge == 1)
		{
			//暂无实现
		}
		else if(g_Para.PGVInputType.BJF_Groud_Merge == 1)
		{
			EQP_BJF_PGVObj_StDef* 		p_f		= &EQP_BJF_PGV_Downward_F.BJF_PGV_Data;
			EQP_BJF_PGVObj_StDef* 		p_b		= &EQP_BJF_PGV_Downward_B.BJF_PGV_Data;
			_MergeToPGV_BisHF_TypeDef 	*p_Data = &(g_MergeToPGV.BisHFData);

			if(p_f->State == 8 || p_f->State == 9)
			{
				p_Data->F_PGV_Sta 	= 1;
			}
			else
			{
				p_Data->F_PGV_Sta 	= 0;
			}
			p_Data->F_PGV_X 	= p_f->Xpositon;
			p_Data->F_PGV_Y 	= p_f->Ypositon;

			if(p_b->State == 8 || p_b->State == 9)
			{
				p_Data->B_PGV_Sta 	= 1;
			}
			else
			{
				p_Data->B_PGV_Sta 	= 0;
			}
			p_Data->B_PGV_X 	= p_b->Xpositon;
			p_Data->B_PGV_Y 	= p_b->Ypositon;

		}
		else if(g_Para.PGVInputType.SE_Groud_Merge == 1)
		{
			PgvObjDef					*p_f	= EQP_SerialExp.p2Data.PGVDownward_FData;
			PgvObjDef					*p_b	= EQP_SerialExp.p2Data.PGVDownward_BData;
			_MergeToPGV_BisHF_TypeDef 	*p_Data = &(g_MergeToPGV.BisHFData);

			p_Data->F_PGV_Sta 	= p_f->ReadSta==1?1:0;
			p_Data->F_PGV_X 	= p_f->X_Value;
			p_Data->F_PGV_Y 	= p_f->Y_Value;
			p_Data->F_PGV_Z		= p_f->Z_Value*10;

			p_Data->B_PGV_Sta 	= p_b->ReadSta==1?1:0;
			p_Data->B_PGV_X 	= p_b->X_Value;
			p_Data->B_PGV_Y 	= p_b->Y_Value;
			p_Data->B_PGV_Z		= p_b->Z_Value*10;
		}
		Drv_MergeToPGV_Main(&g_MergeToPGV);
	}
	//联动下的PGV数据融合
	{
		//主车PGV数据融合
		//if(	  ACC_Assemly_OutData.Assemly_OutRole == ACC_AssemlyOutRole_Master)
		if(0)
		{
			if(g_Para.PGVInputType.SE_Groud_Merge == 1)
			{
				//PgvObjDef					*p_f	= EQP_SerialExp.p2Data.PGVDownward_FData;
				_MergeToPGV_OutPut_TypeDef	*p_f	= &(g_MergeToPGV.PGVOut);
				ACC_Assemly_OutPGV_StDef	*p_b	= &(ACC_Assemly_OutData.Assemly_OutPGV);
				_MergeToPGV_BisHF_TypeDef 	*p_Data = &(g_AssemlyMergeToPGV.BisHFData);

				g_AssemlyMergeToPGV.PGVFitType		= MergeToPGVType_FB_BisHF;

				if(p_f->PGV_Sta == 1 || p_f->PGV_Sta == 2)
				{
					p_Data->F_PGV_Sta 	= 1;
					p_Data->F_PGV_X		= p_f->PGV_X;
					p_Data->F_PGV_Y		= p_f->PGV_Y;
				}
				else
				{
					p_Data->F_PGV_Sta	= 0;
					p_Data->F_PGV_X		= 0;
					p_Data->F_PGV_Y		= 0;
				}

				if(p_b->Status == 1)
				{
					p_Data->B_PGV_Sta 	= 1;
					p_Data->B_PGV_X		= p_b->X;
					p_Data->B_PGV_Y		= p_b->Y;
				}
				else
				{
					p_Data->B_PGV_Sta 	= 0;
					p_Data->B_PGV_X		= 0;
					p_Data->B_PGV_Y		= 0;
				}
				

			}
		}
		//从车数据PGV数据逻辑与
		else
		{
			if(g_Para.PGVInputType.SE_Groud_Merge == 1)
			{
				PgvObjDef					*p_f	= EQP_SerialExp.p2Data.PGVDownward_FData;
				PgvObjDef					*p_b	= EQP_SerialExp.p2Data.PGVDownward_BData;
				_MergeToPGV_BisHF_TypeDef 	*p_Data = &(g_AssemlyMergeToPGV.BisHFData);

				g_AssemlyMergeToPGV.PGVFitType	= MergeToPGVType_Logical;

				p_Data->F_PGV_Sta 	= p_f->ReadSta==1?1:0;
				p_Data->F_PGV_X		= p_f->X_Value;
				p_Data->F_PGV_Y		= p_f->Y_Value;
				p_Data->F_PGV_Z		= p_f->Z_Value*10;


				p_Data->B_PGV_Sta 	= p_b->ReadSta==1?1:0;
				p_Data->B_PGV_X		= p_b->X_Value;
				p_Data->B_PGV_Y		= p_b->Y_Value;
				p_Data->B_PGV_Y		= p_b->Z_Value*10;
			}
		}
		//融合数据更新
		Drv_MergeToPGV_Main(&g_AssemlyMergeToPGV);
		
	}

}


void BJF_PGV_DataProcess(void)
{
	if(EQPMsts.BJF_PGV_Upward.CommId != CCFCN_Disable)
	{

		BjfPGV_UpDate(EQP_BJF_PGV_Upward.RD,&EQP_BJF_PGV_Upward.BJF_PGV_Data);
		{
			static uint8_t Flag = 0;
			if(	EQPMsts.BJF_PGV_Upward.CommOkCnt >= 1
				&&Flag == 0)
			{
				uint8_t Temp[2] = {0xC8,0x37};
				Flag = 1;
				if(g_Para.EQPCommSource.Address.BJF_PGV_Upward <= 4)
				{
					Temp[0] = Temp[0] + g_Para.EQPCommSource.Address.BJF_PGV_Upward-1;
					Temp[1] = Temp[1] - g_Para.EQPCommSource.Address.BJF_PGV_Upward+1;
				}
				memcpy(EQP_BJF_PGV_Upward.WD,Temp,sizeof(Temp));
			}
			
		}
	}

	if(EQPMsts.BJF_PGV_Downward.CommId != CCFCN_Disable)
	{

		BjfPGV_UpDate(EQP_BJF_PGV_Downward.RD,&EQP_BJF_PGV_Downward.BJF_PGV_Data);
		{
			static uint8_t Flag = 0;
			if(	EQPMsts.BJF_PGV_Downward.CommOkCnt >= 1
				&&Flag == 0)
			{
				uint8_t Temp[2] = {0xC8,0x37};
				Flag = 1;
				if(g_Para.EQPCommSource.Address.BJF_PGV_Upward <= 4)
				{
					Temp[0] = Temp[0] + g_Para.EQPCommSource.Address.BJF_PGV_Downward-1;
					Temp[1] = Temp[1] - g_Para.EQPCommSource.Address.BJF_PGV_Downward+1;
				}
				memcpy(EQP_BJF_PGV_Downward.WD,Temp,sizeof(Temp));
			}
			
		}
	}
	
	if(EQPMsts.BJF_PGV_Upward_F.CommId != CCFCN_Disable)
	{

		BjfPGV_UpDate(EQP_BJF_PGV_Upward_F.RD,&EQP_BJF_PGV_Upward_F.BJF_PGV_Data);
		{
			static uint8_t Flag = 0;
			if(	EQPMsts.BJF_PGV_Upward_F.CommOkCnt >= 1
				&&Flag == 0)
			{
				uint8_t Temp[2] = {0xC8,0x37};
				Flag = 1;
				if(g_Para.EQPCommSource.Address.BJF_PGV_Upward <= 4)
				{
					Temp[0] = Temp[0] + g_Para.EQPCommSource.Address.BJF_PGV_Upward_F-1;
					Temp[1] = Temp[1] - g_Para.EQPCommSource.Address.BJF_PGV_Upward_F+1;
				}
				memcpy(EQP_BJF_PGV_Upward_F.WD,Temp,sizeof(Temp));
			}
			
		}
	}
	if(EQPMsts.BJF_PGV_Upward_B.CommId != CCFCN_Disable)
	{

		BjfPGV_UpDate(EQP_BJF_PGV_Upward_B.RD,&EQP_BJF_PGV_Upward_B.BJF_PGV_Data);
		
		{
			static uint8_t Flag = 0;
			if(	EQPMsts.BJF_PGV_Upward_B.CommOkCnt >= 1
				&&Flag == 0)
			{
				uint8_t Temp[2] = {0xC8,0x37};
				Flag = 1;
				if(g_Para.EQPCommSource.Address.BJF_PGV_Upward <= 4)
				{
					Temp[0] = Temp[0] + g_Para.EQPCommSource.Address.BJF_PGV_Upward_B-1;
					Temp[1] = Temp[1] - g_Para.EQPCommSource.Address.BJF_PGV_Upward_B+1;
				}
				memcpy(EQP_BJF_PGV_Upward_B.WD,Temp,sizeof(Temp));
			}
			
		}
	}

	if(EQPMsts.BJF_PGV_Downward_F.CommId != CCFCN_Disable)
	{

		BjfPGV_UpDate(EQP_BJF_PGV_Downward_F.RD,&EQP_BJF_PGV_Downward_F.BJF_PGV_Data);

		{
			static uint8_t Flag = 0;
			if(	EQPMsts.BJF_PGV_Downward_F.CommOkCnt >= 1
				&&Flag == 0)
			{
				uint8_t Temp[2] = {0xC8,0x37};
				Flag = 1;
				if(g_Para.EQPCommSource.Address.BJF_PGV_Upward <= 4)
				{
					Temp[0] = Temp[0] + g_Para.EQPCommSource.Address.BJF_PGV_Downward_F-1;
					Temp[1] = Temp[1] - g_Para.EQPCommSource.Address.BJF_PGV_Downward_F+1;
				}
				memcpy(EQP_BJF_PGV_Downward_F.WD,Temp,sizeof(Temp));
			}
			
		}
	}

	if(EQPMsts.BJF_PGV_Downward_B.CommId != CCFCN_Disable)
	{

		BjfPGV_UpDate(EQP_BJF_PGV_Downward_B.RD,&EQP_BJF_PGV_Downward_B.BJF_PGV_Data);
		
		{
			static uint8_t Flag = 0;
			if(	EQPMsts.BJF_PGV_Downward_B.CommOkCnt >=  1
				&&Flag == 0)
			{
				uint8_t Temp[2] = {0xC8,0x37};
				Flag = 1;
				if(g_Para.EQPCommSource.Address.BJF_PGV_Upward <= 4)
				{
					Temp[0] = Temp[0] + g_Para.EQPCommSource.Address.BJF_PGV_Downward_B-1;
					Temp[1] = Temp[1] - g_Para.EQPCommSource.Address.BJF_PGV_Downward_B+1;
				}
				memcpy(EQP_BJF_PGV_Downward_B.WD,Temp,sizeof(Temp));
			}
			
		}
	}
}
/***********************************************************
** 函数名称:		ObjUpdate_PGV
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_PGV(void)
{
	static uint8_t init_flag = 0;
	if(g_Sys_1ms_Counter <= 10000)
	{
		//无需执行任何代码，等到系统启动时间超过10秒
	}
	else if(init_flag == 0)
	{
		init_flag  = 1;
		//融合PGV参数 配置
		PGV_MergePaparDataInit();
		//单个PGV参数 配置
		PGV_SolePaparDataInit();
	}
	else
	{
		//PGV融合数据更新
		PGV_MergeUpData();

		/*函数调用*/
		PGV_DataProcess(&g_ToolPGV);
		PGV_DataProcess(&g_GroundPGV);
		PGV_DataProcess(&g_NavigationPGV);
		
		PGV_DataProcess(&g_AssemlyGroundPGV);
	}
	//倍加福PGV数据更新
	{
		BJF_PGV_DataProcess();
	}
}

/************************************END OF FILE************************************/
