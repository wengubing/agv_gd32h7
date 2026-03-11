/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	SwitchboardData.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  SwitchboardData_GLOBALS

#if defined(Navigation)
#include "DataAssembly.h"
#else
#include "ACC_Project.h"
#endif
#include "Para_Sys.h"


#if defined(Navigation)
void SerialExp_p_Init(	Para_EQPCommSource_BitDef	*p_En,
			DataAssemblyObj_StDef 		*p_Hal,
			SerialExpObj_StDef			*SerialExp_p)
{
	static 	uint8_t		DataAssembly_NULL[100];
	uint8_t				* p_NULL		= DataAssembly_NULL;
#else
void SerialExp_p_Init(	DataPool_BitDef				*p_En,
			SerialExp_StDef 			*p_Hal,
			SerialExpObj_StDef			*SerialExp_p)
{
	static uint8_t		EQP_SerialExp_NULL[40];
	uint8_t				* p_NULL		= EQP_SerialExp_NULL;
#endif
	uint8_t				* p				= NULL;
	uint8_t				* p_Stat		= NULL;
	p = (uint8_t*)SerialExp_p;
	//记录只读数据的起始地址，才能计算得出只读数据长度
	p_Stat = (uint8_t*)SerialExp_p;
	/*中间封装和开发板通讯结构体不同的地方，需要初始时候加上*/
	p	= p + (sizeof(SerialExp_p->RODataSize)+sizeof(SerialExp_p->RWDataSize));

	/*一定存在的超时结构体*/
	p_Hal->LinkSta = (DataPool_BitDef*)p;
	p = p + (sizeof(DataPool_BitDef) );
	/*一定存在的故障结构体*/
	p_Hal->Err 	= (DataPool_BitDef*)p;
	p = p + (sizeof(DataPool_BitDef) );
#if defined(Navigation)
	if(p_En->BMS_CommConv	== 1||p_En->BMS_MCB	== 1 || g_Para.CanFun == ACC_Para_CanFun_BMS)
#else
	if(p_En->BMS == 1)
#endif
	{
		p_Hal->BMSData = (BMSDataDef*)p;
		p = p + (sizeof(BMSDataDef) );
	}else{p_Hal->BMSData = (BMSDataDef*)p_NULL;}

	if(p_En->AD	== 1)
	{
		p_Hal->ADDAData = (ADDAObj_StDef*)p;
		p = p + (sizeof(ADDAObj_StDef));
	}else{p_Hal->ADDAData = (ADDAObj_StDef*)p_NULL;}
	
		if(p_En->AD1	== 1)
	{
		p_Hal->ADDA1Data = (ADDAObj_StDef*)p;
		p = p + (sizeof(ADDAObj_StDef));
	}else{p_Hal->ADDA1Data = (ADDAObj_StDef*)p_NULL;}

	if(p_En->MagF	== 1)
	{
		p_Hal->F_MagData = (MagObj_StDef*)p;
		p = p + (sizeof(MagObj_StDef));
	}else{p_Hal->F_MagData = (MagObj_StDef*)p_NULL;}

	if(p_En->MagB	== 1)
	{
		p_Hal->B_MagData = (MagObj_StDef*)p;
		p = p + (sizeof(MagObj_StDef));
	}else{p_Hal->B_MagData = (MagObj_StDef*)p_NULL;}

	if(p_En->MagL	== 1)
	{
		p_Hal->L_MagData = (MagObj_StDef*)p;
		p = p + (sizeof(MagObj_StDef));
	}else{p_Hal->L_MagData = (MagObj_StDef*)p_NULL;}

	if(p_En->MagR	== 1)
	{
		p_Hal->R_MagData = (MagObj_StDef*)p;
		p = p + (sizeof(MagObj_StDef));
	}else{p_Hal->R_MagData = (MagObj_StDef*)p_NULL;}

	if(p_En->RFID	== 1)
	{
		p_Hal->RFIDData = (RFIDObj_StDef*)p;
		p = p + (sizeof(RFIDObj_StDef));
	}else{p_Hal->RFIDData = (RFIDObj_StDef*)p_NULL;}
	

#if defined(Navigation)
	if(	  p_En->PGVUpward		== 1
		||p_En->BJF_PGV_Upward	== 1)
	{
		p_Hal->PGVUpwardData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVUpwardData = (PgvObjDef*)p_NULL;}
	
	if(   p_En->PGVDownward			== 1
		||p_En->BJF_PGV_Downward	== 1)
	{
		p_Hal->PGVDownwardData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVDownwardData = (PgvObjDef*)p_NULL;}

	if(	  p_En->PGVUpward_F			== 1
		||p_En->BJF_PGV_Upward_F	== 1)
	{
		p_Hal->PGVUpward_FData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVUpward_FData = (PgvObjDef*)p_NULL;}

	if(	  p_En->PGVUpward_B			== 1
		||p_En->BJF_PGV_Upward_B	== 1)
	{
		p_Hal->PGVUpward_BData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVUpward_BData = (PgvObjDef*)p_NULL;}
	
	if(	  p_En->PGVDownward_F		== 1
		||p_En->BJF_PGV_Downward_F	== 1)
	{
		p_Hal->PGVDownward_FData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVDownward_FData = (PgvObjDef*)p_NULL;}

	if(	  p_En->PGVDownward_B	== 1
		||p_En->BJF_PGV_Downward_F	== 1)
	{
		p_Hal->PGVDownward_BData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVDownward_BData = (PgvObjDef*)p_NULL;}
#else
	if(p_En->PGVUpward	== 1)
	{
		p_Hal->PGVUpwardData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVUpwardData = (PgvObjDef*)p_NULL;}
	
	if(p_En->PGVDownward	== 1)
	{
		p_Hal->PGVDownwardData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVDownwardData = (PgvObjDef*)p_NULL;}

	if(p_En->PGVUpward_F	== 1)
	{
		p_Hal->PGVUpward_FData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVUpward_FData = (PgvObjDef*)p_NULL;}

	if(p_En->PGVUpward_B	== 1)
	{
		p_Hal->PGVUpward_BData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVUpward_BData = (PgvObjDef*)p_NULL;}
	
	if(p_En->PGVDownward_F	== 1)
	{
		p_Hal->PGVDownward_FData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVDownward_FData = (PgvObjDef*)p_NULL;}

	if(p_En->PGVDownward_B	== 1)
	{
		p_Hal->PGVDownward_BData = (PgvObjDef*)p;
		p = p + (sizeof(PgvObjDef));
	}else{p_Hal->PGVDownward_BData = (PgvObjDef*)p_NULL;}

#endif
	if(p_En->IPC	== 1)
	{
		p_Hal->IPC_ReadData = (IPC_Read_StDef*)p;
		p = p + (sizeof(IPC_Read_StDef));
	}else{p_Hal->IPC_ReadData = (IPC_Read_StDef*)p_NULL;}

	if(p_En->LaserSpot	== 1)
	{
		p_Hal->LaserSpotData = (LaserSpot_StDef*)p;
		p = p + (sizeof(LaserSpot_StDef));
	}else{p_Hal->LaserSpotData = (LaserSpot_StDef*)p_NULL;}
	
	if(p_En->LsLiftModule_F	== 1 || p_En->LsLiftModule_B == 1 
	|| p_En->LsLiftModule_L == 1 || p_En->LsLiftModule_R == 1) 
	{
		p_Hal->LS_ReadData = (LS_Read_StDef*)p;
		p = p + (sizeof(LS_Read_StDef));
	}else{p_Hal->LS_ReadData = (LS_Read_StDef*)p_NULL;}
	
	if(p_En->SM45BL_MB_1 == 1 || p_En->SM45BL_MB_2 == 1 
	|| p_En->SM45BL_MB_3 == 1 || p_En->SM45BL_MB_4 == 1) 
	{
		p_Hal->SM_ReadData = (SM2924BLMB_Read_StDef*)p;
		p = p + (sizeof(SM2924BLMB_Read_StDef));
	}else{p_Hal->SM_ReadData = (SM2924BLMB_Read_StDef*)p_NULL;}
	//计算只读对象大小
	SerialExp_p->RODataSize = p-p_Stat;
	//手动对齐只读区域 确保其是u16对齐的
	if(SerialExp_p->RODataSize%2 != 0)
	{
		p=p+1;
		SerialExp_p->RODataSize = (SerialExp_p->RODataSize+1)/2;
	}
	else
	{
		SerialExp_p->RODataSize = (SerialExp_p->RODataSize)/2;
	}
	//重置指针
	p_Stat = p;

	//写的对象
	if(p_En->Voice	== 1)
	{
		p_Hal->VoiceData = (VoiceObj_StDef*)p;
		p = p + (sizeof(VoiceObj_StDef));
	}else{p_Hal->VoiceData = (VoiceObj_StDef*)p_NULL;}
	
	if(p_En->IPC	== 1)
	{
		p_Hal->IPC_WriteData = (IPC_Write_StDef*)p;
		p = p + (sizeof(IPC_Write_StDef));
	}else{p_Hal->IPC_WriteData = (IPC_Write_StDef*)p_NULL;}
	
	if(p_En->PLC	== 1)
	{
		p_Hal->PLCData = (PLCObj_StDef*)p;
		p = p + (sizeof(PLCObj_StDef));
	}else{p_Hal->PLCData = (PLCObj_StDef*)p_NULL;}
	
	if(p_En->LsLiftModule_F	== 1 || p_En->LsLiftModule_B == 1 
	|| p_En->LsLiftModule_L == 1 || p_En->LsLiftModule_R == 1) 
	{
		p_Hal->LS_WriteData = (LS_Write_StDef*)p;
		p = p + (sizeof(LS_Write_StDef));
	}else{p_Hal->LS_WriteData = (LS_Write_StDef*)p_NULL;}

	if (   p_En->SM45BL_MB_1 == 1 || p_En->SM45BL_MB_2 == 1
		|| p_En->SM45BL_MB_3 == 1 || p_En->SM45BL_MB_4 == 1)
	{
		p_Hal->SM_WriteData = (SM2924BLMB_Write_StDef*)p;
		p = p + (sizeof(SM2924BLMB_Write_StDef));
	}
	else { p_Hal->SM_WriteData = (SM2924BLMB_Write_StDef*)p_NULL; }

	if(p_En->Roller	== 1)
	{
		p_Hal->Roller_WriteData = (Roller_Write_StDef*)p;
		p = p + (sizeof(Roller_Write_StDef));
	}else{p_Hal->Roller_WriteData = (Roller_Write_StDef*)p_NULL;}

	//计算读/写对象大小
	SerialExp_p->RWDataSize = (p-p_Stat+1)/2;
}


void SwitchboardData_Init(void)
{
#if defined(Navigation)

	//导航转接板 从站 动态指针初始化
	SerialExp_p_Init(&g_Para.EQPCommSource.EN		,&g_DataAssembly			,(SerialExpObj_StDef*)&EQP_SerialExp_Slave);
#else
	//开发板 主站 动态指针初始化
	SerialExp_p_Init(&g_Para.SerialExpObjSource		,&(EQP_SerialExp.p2Data)	,(SerialExpObj_StDef*)&EQP_SerialExp.RD);
	//开发板 主站 第二导航转接板 动态指针初始化
	SerialExp_p_Init(&g_Para.SerialExp_2_ObjSource	,&(EQP_SerialExp_2.p2Data)	,(SerialExpObj_StDef*)&EQP_SerialExp_2.RD);
#endif

	
}




/************************************END OF FILE************************************/
