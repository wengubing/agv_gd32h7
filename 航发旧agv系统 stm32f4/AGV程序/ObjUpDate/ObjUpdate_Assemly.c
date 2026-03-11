/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_Assemly.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-06-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  ObjUpdate_Assemly_GLOBALS

#include "ObjUpdate_Assemly.h"
#include "ACC_Project.h"
#include "Debug.h"

Acc_AssemlySensorData_TypeDef 	g_AssemlySensorData;

/***********************************************************
** 函数名称:		ObjUpdate_Assemly
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_Assemly(void)
{	
	Acc_AssemlySensorData_TypeDef 	*p = &g_AssemlySensorData;
	
	//联动传感器输入
	{
		/*438两个两个测距
		p->LaserDis_L.Org 	= EQP_AD1.RD[6];
		p->LaserDis_R.Org 	= EQP_AD1.RD[5];*/

		/*465一个激光测距*/
		p->LaserDis_L.Org = EQP_AD1.RD[7];
		p->LaserDis_R.Org = EQP_AD1.RD[7];

		p->SpotDis.Org 		= EQP_LaserSpot.p2Data->MaxPoint;
		//p->SpotDis.Cal 		= EQP_SerialExp.p2Data.LaserSpotData->Data[6]-95;
	}
	
	{//非联动状态下可以选择从车类型，已适应不同的车长联动坐标偏移
		if(g_IOInputs.Assembly.StableValue == 0)//面板非联动模式下可以修改从车半长参数
		{
			if(g_Asb.Para.SlaveCarType > 0 && g_Asb.Para.SlaveCarType <= 8)//从车类型选择合理则采用选择车型的半长参数
			{
				g_Asb.Para.HalfCar_Edge_Slave.Front = g_Asb.Para.HalfCar_Edge_Slave_List[g_Asb.Para.SlaveCarType - 1].Front;
				g_Asb.Para.HalfCar_Edge_Slave.Back  = g_Asb.Para.HalfCar_Edge_Slave_List[g_Asb.Para.SlaveCarType - 1].Back;
				g_Asb.Para.HalfCar_Edge_Slave.Left  = g_Asb.Para.HalfCar_Edge_Slave_List[g_Asb.Para.SlaveCarType - 1].Left;
				g_Asb.Para.HalfCar_Edge_Slave.Right = g_Asb.Para.HalfCar_Edge_Slave_List[g_Asb.Para.SlaveCarType - 1].Right;
			}
			else//否则使用自身半长参数
			{
				g_Asb.Para.HalfCar_Edge_Slave.Front = g_Asb.Para.HalfCar_Edge.Front;
				g_Asb.Para.HalfCar_Edge_Slave.Back  = g_Asb.Para.HalfCar_Edge.Back;
				g_Asb.Para.HalfCar_Edge_Slave.Left  = g_Asb.Para.HalfCar_Edge.Left;
				g_Asb.Para.HalfCar_Edge_Slave.Right = g_Asb.Para.HalfCar_Edge.Right;
			}
		}
	}

	//线性标定
	{
		RatioStandPara_TypeDef *p_D1 = &g_AssemlySensorData.LaserDis_L.Para;
		RatioStandPara_TypeDef *p_D2 = &g_AssemlySensorData.LaserDis_R.Para;
		RatioStandPara_TypeDef* p_SpotDis = &g_AssemlySensorData.SpotDis.Para;
		p->LaserDis_L.Cal = RatioStand(p->LaserDis_L.Org, p_D1->Org1, p_D1->Org2, p_D1->Act1, p_D1->Act2) + p_D1->Offset;
		p->LaserDis_R.Cal = RatioStand(p->LaserDis_R.Org, p_D2->Org1, p_D2->Org2, p_D2->Act1, p_D2->Act2) + p_D2->Offset;
		p->SpotDis.Cal = RatioStand(p->SpotDis.Org, p_SpotDis->Org1, p_SpotDis->Org2, p_SpotDis->Act1, p_SpotDis->Act2);
	}

	//联动数据和EQP的互通
	if (g_Para.SysFucBit.Asb_CommDirect == 0) 
	{
		static uint16_t MasterDataLen	= 0;
		static uint16_t SlaveDataLen 	= 0;
		if(MasterDataLen == 0)
		{
			uint16_t Temp = sizeof(EQP_Assembly_Master.WD);
			MasterDataLen = g_Asb.Comm.MasterDataLength*2;
			if(MasterDataLen > Temp)
			{
				MasterDataLen = Temp;
			}
		}
		if(SlaveDataLen == 0)
		{
			uint16_t Temp = sizeof(EQP_Assembly_Master.RD);
			SlaveDataLen = g_Asb.Comm.SlaveDataLength*2;
			if(SlaveDataLen > Temp)
			{
				SlaveDataLen = Temp;
			}
		}
		//联动数据 从EQP到cbb
		{
			if(g_Asb.Para.Role == AsbRole_Master)
			{
				//对于主车而言，几号从机 放在SlaveData数组下标几号
				memcpy(&(g_Asb.Comm.SlaveData[1]), EQP_Assembly_Master.RD, SlaveDataLen);
			}
			else if(g_Asb.Para.Role == AsbRole_Slave)
			{
				//对于从车而言，主车写过来的数据 需要放在MasterData中
				memcpy(&g_Asb.Comm.MasterData, EQP_Assembly_Slave.Data,MasterDataLen);
			}
			
		}
		//联动数据 从cbb到EQP
		{
			//主车是 
			if(g_Asb.Para.Role == AsbRole_Master)
			{
				//对于主车而言，需要将MasterData中的内容写到所有从从车中
				memcpy(EQP_Assembly_Master.WD, &g_Asb.Comm.MasterData, MasterDataLen);
			}
			//从车是 
			else if(g_Asb.Para.Role == AsbRole_Slave)
			{
				//对于从车而言，所有的从车都是将SlaveData[0]地址的内容 供主车读取
				memcpy(&(EQP_Assembly_Slave.Data[MasterDataLen/2]),&(g_Asb.Comm.SlaveData[0]),SlaveDataLen);
			}
		}
	}
	
}

/************************************END OF FILE************************************/
