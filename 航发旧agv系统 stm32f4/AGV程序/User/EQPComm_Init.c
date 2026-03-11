/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: EQPComm_Init.c
** 创 建 人: Master
** 描    述: ModBus设备通讯的通用初始化，如果没有特殊配置统一使用默认参数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期:  2025年2月10日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define EQPComm_Init_GLOBALS

#include "EQPComm_Init.h"
#include "Para_Sys.h"
#include "ACC_Project.h"
#include "HMI.h"
#include "Custom_UART.h"

//通信接口配置复位
bool EQPComm_PortConfReset(void)
{
	bool res=1;
	char i=0;
	//CCF_StDef *p2CCF = (CCF_StDef*) & CCFs;
	CCFEQP_Master_StDef* p2EQPMst = (CCFEQP_Master_StDef*)&EQPMsts;
	CCFEQP_Slave_StDef* p2EQPSlv = (CCFEQP_Slave_StDef*)&EQPSlvs;

	//数据清零
	memset(p2EQPMst,0,sizeof(CCFEQP_Master_StDef));
	memset(p2EQPSlv,0,sizeof(CCFEQP_Slave_StDef));
	//全部失能
	for (i = 0; i < EQPMsts.Number; i++)
		p2EQPMst[i].CommId = CCFCN_Disable;

	for (i = 0; i < EQPSlvs.Number; i++)
		p2EQPSlv[i].CommId = CCFCN_Disable;

	//填充结构体对象数据大小
	EQPMsts.Number = sizeof(CCFEQP_Masters_StDef) / sizeof(CCFEQP_Master_StDef);
	EQPSlvs.Number = sizeof(CCFEQP_Slaves_StDef) / sizeof(CCFEQP_Slave_StDef);
	return res;
}
/***********************************************************
** 函数名称:	Para_EQPInit_Template
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//供外部调用的初始化接口
void Para_EQPInit(Para_EQPCommSource_EnumDef Id,CCFID_EnumDef Cn,uint8_t Address)
{
	switch(Id)
	{
		default:break;
		case Para_EQP_HMI:					{g_Para.EQPCommSource.EN.HMI 				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.HMI					=Cn;g_Para.EQPCommSource.Address.HMI 					= Address;}break;
		case Para_EQP_DSP:					{g_Para.EQPCommSource.EN.DSP				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.DSP					=Cn;g_Para.EQPCommSource.Address.DSP 					= Address;}break;
		case Para_EQP_Assembly_Slave:		{g_Para.EQPCommSource.EN.Assembly_Slave		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Assembly_Slave		=Cn;g_Para.EQPCommSource.Address.Assembly_Slave			= Address;}break;
		case Para_EQP_API_Slave:			{g_Para.EQPCommSource.EN.API_Slave			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.API_Slave			=Cn;g_Para.EQPCommSource.Address.API_Slave				= Address;}break;

		case Para_EQP_WHA:					{g_Para.EQPCommSource.EN.WHA				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.WHA					=Cn;g_Para.EQPCommSource.Address.WHA 					= Address;}break;
		case Para_EQP_BMS_CommConv:			{g_Para.EQPCommSource.EN.BMS_CommConv		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BMS_CommConv		=Cn;g_Para.EQPCommSource.Address.BMS_CommConv 			= Address;}break;
		case Para_EQP_BMS_MCB:				{g_Para.EQPCommSource.EN.BMS_MCB			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BMS_MCB				=Cn;g_Para.EQPCommSource.Address.BMS_MCB 				= Address;}break;
		case Para_EQP_Voice:				{g_Para.EQPCommSource.EN.Voice				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Voice				=Cn;g_Para.EQPCommSource.Address.Voice 					= Address;}break;
		case Para_EQP_AD:					{g_Para.EQPCommSource.EN.AD					= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.AD					=Cn;g_Para.EQPCommSource.Address.AD 					= Address;}break;
		case Para_EQP_AD1:					{g_Para.EQPCommSource.EN.AD1				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.AD1					=Cn;g_Para.EQPCommSource.Address.AD1 					= Address;}break;
		case Para_EQP_Thermodetector:		{g_Para.EQPCommSource.EN.Thermodetector		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Thermodetector		=Cn;g_Para.EQPCommSource.Address.Thermodetector 		= Address;}break;
		case Para_EQP_Inclinometer_Rion:	{g_Para.EQPCommSource.EN.Inclinometer_Rion	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Inclinometer_Rion	=Cn;g_Para.EQPCommSource.Address.Inclinometer_Rion 		= Address;}break;
		case Para_EQP_MagF:					{g_Para.EQPCommSource.EN.MagF				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.MagF				=Cn;g_Para.EQPCommSource.Address.MagF 					= Address;}break;
		case Para_EQP_MagB:					{g_Para.EQPCommSource.EN.MagB				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.MagB				=Cn;g_Para.EQPCommSource.Address.MagB 					= Address;}break;
		case Para_EQP_MagL:					{g_Para.EQPCommSource.EN.MagL				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.MagL				=Cn;g_Para.EQPCommSource.Address.MagL 					= Address;}break;
		case Para_EQP_MagR:					{g_Para.EQPCommSource.EN.MagR				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.MagR				=Cn;g_Para.EQPCommSource.Address.MagR 					= Address;}break;
		case Para_EQP_RFID:					{g_Para.EQPCommSource.EN.RFID				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.RFID				=Cn;g_Para.EQPCommSource.Address.RFID 					= Address;}break;
		case Para_EQP_RFID_Up:				{g_Para.EQPCommSource.EN.RFID_Up			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.RFID_Up				=Cn;g_Para.EQPCommSource.Address.RFID_Up				= Address;}break;
		case Para_EQP_PGVUpward:			{g_Para.EQPCommSource.EN.PGVUpward			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.PGVUpward			=Cn;g_Para.EQPCommSource.Address.PGVUpward 				= Address;}break;
		case Para_EQP_PGVDownward:			{g_Para.EQPCommSource.EN.PGVDownward		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.PGVDownward			=Cn;g_Para.EQPCommSource.Address.PGVDownward 			= Address;}break;
		case Para_EQP_PGVUpward_F:			{g_Para.EQPCommSource.EN.PGVUpward_F		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.PGVUpward_F			=Cn;g_Para.EQPCommSource.Address.PGVUpward_F 			= Address;}break;
		case Para_EQP_PGVUpward_B:			{g_Para.EQPCommSource.EN.PGVUpward_B		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.PGVUpward_B			=Cn;g_Para.EQPCommSource.Address.PGVUpward_B 			= Address;}break;
		case Para_EQP_PGVDownward_F:		{g_Para.EQPCommSource.EN.PGVDownward_F		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.PGVDownward_F		=Cn;g_Para.EQPCommSource.Address.PGVDownward_F 			= Address;}break;
		case Para_EQP_PGVDownward_B:		{g_Para.EQPCommSource.EN.PGVDownward_B		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.PGVDownward_B		=Cn;g_Para.EQPCommSource.Address.PGVDownward_B 			= Address;}break;
		case Para_EQP_SerialExp:			{g_Para.EQPCommSource.EN.SerialExp			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.SerialExp			=Cn;g_Para.EQPCommSource.Address.SerialExp 				= Address;}break;
		case Para_EQP_SerialExp_2:			{g_Para.EQPCommSource.EN.SerialExp_2		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.SerialExp_2			=Cn;g_Para.EQPCommSource.Address.SerialExp_2			= Address;}break;
		case Para_EQP_CommConv:				{g_Para.EQPCommSource.EN.CommConv			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.CommConv			=Cn;g_Para.EQPCommSource.Address.CommConv 				= Address;}break;
		case Para_EQP_CommConv2:			{g_Para.EQPCommSource.EN.CommConv2			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.CommConv2			=Cn;g_Para.EQPCommSource.Address.CommConv2 				= Address;}break;
		case Para_EQP_IPC:					{g_Para.EQPCommSource.EN.IPC				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.IPC					=Cn;g_Para.EQPCommSource.Address.IPC 					= Address;}break;
		case Para_EQP_Roll_Controller:		{g_Para.EQPCommSource.EN.Roll_Controller	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Roll_Controller		=Cn;g_Para.EQPCommSource.Address.Roll_Controller 		= Address;}break;
		case Para_EQP_WHB:					{g_Para.EQPCommSource.EN.WHB				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.WHB					=Cn;g_Para.EQPCommSource.Address.WHB 					= Address;}break;
		case Para_EQP_LaserSpot:			{g_Para.EQPCommSource.EN.LaserSpot			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.LaserSpot			=Cn;g_Para.EQPCommSource.Address.LaserSpot 				= Address;}break;
		case Para_EQP_Assembly_Master:		{g_Para.EQPCommSource.EN.Assembly_Master	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Assembly_Master		=Cn;g_Para.EQPCommSource.Address.Assembly_Master		= Address;}break;
		case Para_EQP_Hinson_Radar_Front:	{g_Para.EQPCommSource.EN.Hinson_Radar_Front = Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Hinson_Radar_Front	=Cn;g_Para.EQPCommSource.Address.Hinson_Radar_Front 	= Address;}break;
		case Para_EQP_Hinson_Radar_Rear:	{g_Para.EQPCommSource.EN.Hinson_Radar_Rear	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Hinson_Radar_Rear	=Cn;g_Para.EQPCommSource.Address.Hinson_Radar_Rear 		= Address;}break;
		case Para_EQP_Hinson_Radar_Left:	{g_Para.EQPCommSource.EN.Hinson_Radar_Left	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Hinson_Radar_Left	=Cn;g_Para.EQPCommSource.Address.Hinson_Radar_Left 		= Address;}break;
		case Para_EQP_Hinson_Radar_Righ:	{g_Para.EQPCommSource.EN.Hinson_Radar_Righ	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Hinson_Radar_Righ	=Cn;g_Para.EQPCommSource.Address.Hinson_Radar_Righ 		= Address;}break;
		case Para_EQP_Hinson_Radar_FrontUp:	{g_Para.EQPCommSource.EN.Hinson_Radar_FrontUp=Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Hinson_Radar_FrontUp=Cn;g_Para.EQPCommSource.Address.Hinson_Radar_FrontUp 	= Address;}break;
		case Para_EQP_Hinson_Radar_RearUp:	{g_Para.EQPCommSource.EN.Hinson_Radar_RearUp= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.Hinson_Radar_RearUp	=Cn;g_Para.EQPCommSource.Address.Hinson_Radar_RearUp 	= Address;}break;
		case Para_EQP_ME15_Near:			{g_Para.EQPCommSource.EN.ME15_Near			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.ME15_Near			=Cn;g_Para.EQPCommSource.Address.ME15_Near 				= Address;}break;
		case Para_EQP_ME15_Far:				{g_Para.EQPCommSource.EN.ME15_Far			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.ME15_Far			=Cn;g_Para.EQPCommSource.Address.ME15_Far 				= Address;}break;
		case Para_PLC_Estop:				{g_Para.EQPCommSource.EN.PLC_Estop			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.PLC_Estop			=Cn;g_Para.EQPCommSource.Address.PLC_Estop				= Address;}break;
		case Para_EQP_DA:					{g_Para.EQPCommSource.EN.DA					= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.DA					=Cn;g_Para.EQPCommSource.Address.DA						= Address;}break;
		case Para_EQP_API_Master:			{g_Para.EQPCommSource.EN.API_Master			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.API_Master			=Cn;g_Para.EQPCommSource.Address.API_Master				= Address;}break;

		case Para_EQP_BMS_KXD:				{g_Para.EQPCommSource.EN.BMS_KXD			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BMS_KXD				=Cn;g_Para.EQPCommSource.Address.BMS_KXD 				= Address;}break;

		case Para_EQP_BJF_PGV_Upward:		{g_Para.EQPCommSource.EN.BJF_PGV_Upward		= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BJF_PGV_Upward		=Cn;g_Para.EQPCommSource.Address.BJF_PGV_Upward			= Address;}break;
		case Para_EQP_BJF_PGV_Downward:		{g_Para.EQPCommSource.EN.BJF_PGV_Downward	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BJF_PGV_Downward	=Cn;g_Para.EQPCommSource.Address.BJF_PGV_Downward		= Address;}break;
		case Para_EQP_BJF_PGV_Upward_F:		{g_Para.EQPCommSource.EN.BJF_PGV_Upward_F	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BJF_PGV_Upward_F	=Cn;g_Para.EQPCommSource.Address.BJF_PGV_Upward_F		= Address;}break;
		case Para_EQP_BJF_PGV_Upward_B:		{g_Para.EQPCommSource.EN.BJF_PGV_Upward_B	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BJF_PGV_Upward_B	=Cn;g_Para.EQPCommSource.Address.BJF_PGV_Upward_B		= Address;}break;
		case Para_EQP_BJF_PGV_Downward_F:	{g_Para.EQPCommSource.EN.BJF_PGV_Downward_F	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BJF_PGV_Downward_F	=Cn;g_Para.EQPCommSource.Address.BJF_PGV_Downward_F		= Address;}break;
		case Para_EQP_BJF_PGV_Downward_B:	{g_Para.EQPCommSource.EN.BJF_PGV_Downward_B	= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BJF_PGV_Downward_B	=Cn;g_Para.EQPCommSource.Address.BJF_PGV_Downward_B		= Address;}break;

		case Para_EQP_ANC:					{g_Para.EQPCommSource.EN.ANC				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.ANC					=Cn;g_Para.EQPCommSource.Address.ANC					= Address;}break;
		case Para_EQP_DBSET:				{g_Para.EQPCommSource.EN.DBSET				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.DBSET				=Cn;g_Para.EQPCommSource.Address.DBSET					= Address;}break;
		case Para_EQP_BMS_CATL:				{g_Para.EQPCommSource.EN.BMS_CATL			= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.BMS_CATL			=Cn;g_Para.EQPCommSource.Address.BMS_CATL 				= Address;}break;
		case Para_EQP_PLC:					{g_Para.EQPCommSource.EN.PLC				= Cn==CCFCN_Disable?0:1;g_Para.EQPCommSource.CN.PLC					=Cn;g_Para.EQPCommSource.Address.PLC	 				= Address;}break;
	}
	
}
/***********************************************************
** 函数名称:	EQPComm_p2Data_Init
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
void EQPComm_p2Data_Init(void)
{
	EQP_BMS_CommConv.p2Data			= (EQP_BMS_CommConvObj_StDef*)	EQP_BMS_CommConv.RD;
	EQP_BMS_MCB.p2Data				= (EQP_BMS_MCBObj_StDef*)		EQP_BMS_MCB.RD;
	EQP_BMS_CATL.p2Data				= (EQP_BMS_CATLObj_StDef*)		EQP_BMS_CATL.RD;
	EQP_MagF.p2Data 				= (EQP_MagObj_StDef*)	EQP_MagF.RD;
	EQP_MagB.p2Data 				= (EQP_MagObj_StDef*)	EQP_MagB.RD;
	EQP_MagL.p2Data 				= (EQP_MagObj_StDef*)	EQP_MagL.RD;
	EQP_MagR.p2Data 				= (EQP_MagObj_StDef*)	EQP_MagR.RD;
	EQP_RFID.p2Data 				= (EQP_RFIDObj_StDef*)	EQP_RFID.RD;

	EQP_PGVUpward.p2Data 			= (EQP_HFPGVObj_StDef*)	EQP_PGVUpward.RD;
	EQP_PGVDownward.p2Data			= (EQP_HFPGVObj_StDef*)	EQP_PGVDownward.RD;
	EQP_PGVUpward_F.p2Data			= (EQP_HFPGVObj_StDef*)	EQP_PGVUpward_F.RD;
	EQP_PGVUpward_B.p2Data 			= (EQP_HFPGVObj_StDef*)	EQP_PGVUpward_B.RD;
	EQP_PGVDownward_F.p2Data		= (EQP_HFPGVObj_StDef*)	EQP_PGVDownward_F.RD;
	EQP_PGVDownward_B.p2Data 		= (EQP_HFPGVObj_StDef*)	EQP_PGVDownward_B.RD;

	EQP_LaserSpot.p2Data 			= (EQP_LaserSpotObj_StDef*)	EQP_LaserSpot.RD;

	EQP_Hinson_Radar_Front.p2Data 	= (EQP_HinsonRadarObj_StDef*)EQP_Hinson_Radar_Front.RD;
	EQP_Hinson_Radar_Rear.p2Data	= (EQP_HinsonRadarObj_StDef*)EQP_Hinson_Radar_Rear.RD;
	EQP_Hinson_Radar_Left.p2Data	= (EQP_HinsonRadarObj_StDef*)EQP_Hinson_Radar_Left.RD;
	EQP_Hinson_Radar_Right.p2Data 	= (EQP_HinsonRadarObj_StDef*)EQP_Hinson_Radar_Right.RD;
	EQP_Hinson_Radar_FrontUp.p2Data	= (EQP_HinsonRadarObj_StDef*)EQP_Hinson_Radar_FrontUp.RD;
	EQP_Hinson_Radar_RearUp.p2Data 	= (EQP_HinsonRadarObj_StDef*)EQP_Hinson_Radar_RearUp.RD;

	EQP_IPC.R_Data 					= (EQP_IPC_Read_Obj_StDef*)EQP_IPC.RD;
	EQP_IPC.W_Data 					= (EQP_IPC_Write_Obj_StDef*)EQP_IPC.WD;
	
	EQP_PLC_Estop.p2Data			= (EQP_PLC_EstopObj_StDef*)EQP_PLC_Estop.RD;

	//安川m3板卡
	EQP_ANC.p2RData					= (EQP_ANC_RObj_StDef*)EQP_ANC.RD;
	EQP_ANC.p2WData					= (EQP_ANC_WObj_StDef*)EQP_ANC.WD;

	//多倍通配置切换
	EQP_DBSET.p2RData				= (EQP_DBSET_RObj_StDef*)EQP_DBSET.RD;
	EQP_DBSET.p2WData				= (EQP_DBSET_WObj_StDef*)EQP_DBSET.WD;
}

/***********************************************************
** 函数名称:		EQPComm_Universal_Init
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
//通用modbus外设初始化
void EQPComm_Universal_Init(void)
{
	/*根据配置参数使能情况，按照已有经验初始化所有外设*/
	{
		CCFID_EnumDef 	Temp_CCFID = CCFCN_Disable;
		CCFID_EnumDef 	SET_CCFID  = CCFCN_Disable;
		uint8_t			Temp_Address = 0;
		uint8_t			Set_Address = 0;
		//HMI
		if(g_Para.EQPCommSource.EN.HMI == 1)
		{
			/*如果没有设置cn端口，默认使用CN13*/
			SET_CCFID		= g_Para.EQPCommSource.CN.HMI;
			Temp_CCFID		= SET_CCFID==CCFCN_Disable?CCFCN13:SET_CCFID;
			/*主站默认使用0x10作为地址*/
			//Set_Address 	= g_Para.EQPCommSource.Address.HMI;
			//Temp_Address	= Set_Address==0?0x01:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPSlvInit(&EQPSlvs.HMI, Baud_115200, 7, CCFRTU, HMI_Buf, sizeof(HMI_Buf), Temp_CCFID);

		//DSP
		if(g_Para.EQPCommSource.EN.DSP == 1)
		{
			SET_CCFID = g_Para.EQPCommSource.CN.DSP;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;
			/*主站默认使用0x10作为地址*/
			//Set_Address 	= g_Para.EQPCommSource.Address.DSP;
			//Temp_Address	= Set_Address==0?0x01:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPSlvInit(&EQPSlvs.DSP, Baud_115200, 5, CCFTCP, EQP_DSP.Data, sizeof(EQP_DSP.Data), Temp_CCFID);

		//二次开发接口
		if(g_Para.EQPCommSource.EN.API_Slave == 1)
		{
			SET_CCFID = g_Para.EQPCommSource.CN.API_Slave;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;
			/*主站默认使用0x10作为地址*/
			//Set_Address 	= g_Para.EQPCommSource.Address.API_Slave;
			//Temp_Address	= Set_Address==0?0x01:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPSlvInit(&EQPSlvs.API_Slave, Baud_115200, 5, CCFRTU, EQP_API_Slave.Data, sizeof(EQP_API_Slave.Data), Temp_CCFID);

		//联动的从机通讯
		if(g_Para.EQPCommSource.EN.Assembly_Slave == 1)
		{
			SET_CCFID = g_Para.EQPCommSource.CN.Assembly_Slave;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;
			/*主站默认使用0x10作为地址*/
			//Set_Address 	= g_Para.EQPCommSource.Address.HMI;
			//Temp_Address	= Set_Address==0?0x01:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}

		if (g_Para.SysFucBit.Asb_CommDirect == 0)
		{
			EQPSlvInit(&EQPSlvs.Assembly_Slave, Baud_115200, 5, CCFRTU, EQP_Assembly_Slave.Data, sizeof(EQP_Assembly_Slave.Data), Temp_CCFID);
		}
		else
		{
			//测试直接使用对象作为数据池
			EQPSlvInit(&EQPSlvs.Assembly_Slave, Baud_115200, 5, CCFRTU, (u16*)&g_Asb.Comm.SlaveData, sizeof(AsbComm_TypeDef), Temp_CCFID);
		}

		//遥控器A
		if(g_Para.EQPCommSource.EN.WHA == 1)
		{
			/*默认使用CN22*/
			SET_CCFID = g_Para.EQPCommSource.CN.WHA;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;
			/*默认使用126作为地址*/
			Set_Address 	= g_Para.EQPCommSource.Address.WHA;
			Temp_Address	= Set_Address==0?126:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}

		{//根据参数决定，遥控器接收器写的数据个数
			char temp = 0;
			if (g_Para.Asb.Bit.Channel_Write)
				temp = 2;
			else
				temp = 0;

			EQPMstInit(&EQPMsts.WHA, Baud_115200, 3, CCFRTU, 50, 10, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
				EQP_WHA.RD, 194, sizeof(EQP_WHA.RD) / 2, EQP_WHA.WD, 301, temp/*sizeof(EQP_WHA.WD) / 2*/,
				NORMAL_QUE, Temp_CCFID);
			/*注意上面的遥控器通信，默认不写，避免造成遥控器通信异常，
			如为联动车需要实时配置信道写301，或者显示写230，需要特殊修改*/
		}

		//卡式转接板的BMS
		if(g_Para.EQPCommSource.EN.BMS_CommConv == 1)
		{
			/*默认使用CN22*/
			SET_CCFID = g_Para.EQPCommSource.CN.BMS_CommConv;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;
			/*默认使用0x01作为地址*/
			Set_Address 	= g_Para.EQPCommSource.Address.BMS_CommConv;
			Temp_Address	= Set_Address==0?0x01:Set_Address;

			/*卡式转接板 地址从170开始读9位*/
			EQPMstInit(&EQPMsts.BMS_CommConv, Baud_115200, 3, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
			EQP_BMS_CommConv.RD, 168, sizeof(EQP_BMS_CommConv.RD) / 2, EQP_BMS_CommConv.WD, 0, sizeof(EQP_BMS_CommConv.WD) / 2,
			NORMAL_QUE, Temp_CCFID);

		}
		//直接的BMS
		if(g_Para.EQPCommSource.EN.BMS_MCB == 1)
		{
			/*默认使用CN22*/
			SET_CCFID = g_Para.EQPCommSource.CN.BMS_MCB;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;
			/*默认使用0x01作为地址*/
			Set_Address 	= g_Para.EQPCommSource.Address.BMS_CommConv;
			Temp_Address	= Set_Address==0?0x01:Set_Address;
			/*直接开发板从1000地址开始读 3位*/
			EQPMstInit(&EQPMsts.BMS_MCB, Baud_9600, 3, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
			EQP_BMS_MCB.RD, 1000, 3, EQP_BMS_MCB.WD, 0, sizeof(EQP_BMS_MCB.WD) / 2,
			NORMAL_QUE, Temp_CCFID);
		}
		//宁德时代BMS
		if(g_Para.EQPCommSource.EN.BMS_CATL == 1)
		{
			/*默认使用CN22*/
			SET_CCFID = g_Para.EQPCommSource.CN.BMS_CATL;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;
			/*默认使用0x01作为地址*/
			Set_Address 	= g_Para.EQPCommSource.Address.BMS_CATL;
			Temp_Address	= Set_Address==0?0x01:Set_Address;
			/*宁德时代BMS地址从0开始读取*/
			EQPMstInit(&EQPMsts.BMS_CATL, Baud_115200, 3, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
			EQP_BMS_CATL.RD, 3500, sizeof(EQP_BMS_CATL.RD) / 2, EQP_BMS_CATL.WD, 0, sizeof(EQP_BMS_CATL.WD) / 2,
			NORMAL_QUE, Temp_CCFID);
		}


		//语音播报
		if(g_Para.EQPCommSource.EN.Voice == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Voice;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Voice;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Voice, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_SHR, OP_REG,
		EQP_Voice.RD, 3, 1, EQP_Voice.WD, 3, 1,
		NORMAL_QUE, Temp_CCFID);

		//模拟量采集0
		if(g_Para.EQPCommSource.EN.AD == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.AD;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.AD;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.AD, Baud_115200, 6, CCFRTU, 50, 10, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_AD.RD, 0, sizeof(EQP_AD.RD) / 2, EQP_AD.WD, 0, sizeof(EQP_AD.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//模拟量采集1
		if(g_Para.EQPCommSource.EN.AD1 == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.AD1;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.AD1;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.AD1, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_AD1.RD, 0, sizeof(EQP_AD1.RD) / 2, EQP_AD1.WD, 0, sizeof(EQP_AD1.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//温湿度检测模块
		if(g_Para.EQPCommSource.EN.Thermodetector == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Thermodetector;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Thermodetector;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Thermodetector, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_Thermodetector.RD, 0, 2, EQP_Thermodetector.WD, 3, 0,
		NORMAL_QUE, Temp_CCFID);

		//倾角检测模块
		if(g_Para.EQPCommSource.EN.Inclinometer_Rion == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Inclinometer_Rion;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Inclinometer_Rion;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Inclinometer_Rion, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_Inclinometer_Rion.RD, 0, sizeof(EQP_Inclinometer_Rion.RD) / 2, EQP_Inclinometer_Rion.WD, 0, sizeof(EQP_Inclinometer_Rion.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//磁导航前
		if(g_Para.EQPCommSource.EN.MagF == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.MagF;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.MagF;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.MagF, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_MagF.RD, 1000, sizeof(EQP_MagF.RD) / 2, EQP_MagF.WD, 0, sizeof(EQP_MagF.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//磁导航后
		if(g_Para.EQPCommSource.EN.MagB == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.MagB;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.MagB;
			Temp_Address	= Set_Address==0?2:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.MagB, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_MagB.RD, 1000, sizeof(EQP_MagB.RD) / 2, EQP_MagB.WD, 0, sizeof(EQP_MagB.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//磁导航左
		if(g_Para.EQPCommSource.EN.MagL == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.MagL;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.MagL;
			Temp_Address	= Set_Address==0?3:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.MagL, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_MagL.RD, 1000, sizeof(EQP_MagL.RD) / 2, EQP_MagL.WD, 0, sizeof(EQP_MagL.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//磁导航右
		if(g_Para.EQPCommSource.EN.MagR == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.MagR;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.MagR;
			Temp_Address	= Set_Address==0?4:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.MagR, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_MagR.RD, 1000, sizeof(EQP_MagR.RD) / 2, EQP_MagR.WD, 0, sizeof(EQP_MagR.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//RFID
		if(g_Para.EQPCommSource.EN.RFID == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.RFID;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.RFID;
			Temp_Address	= Set_Address==0?5:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.RFID, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_RFID.RD, 1000, sizeof(EQP_RFID.RD) / 2, EQP_RFID.WD, 0, sizeof(EQP_RFID.WD) / 2,
		NORMAL_QUE, Temp_CCFID);
		
		//RFID_Up
		if(g_Para.EQPCommSource.EN.RFID_Up == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.RFID_Up;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.RFID_Up;
			Temp_Address	= Set_Address==0?5:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.RFID_Up, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_RFID_Up.RD, 1000, sizeof(EQP_RFID_Up.RD) / 2, EQP_RFID_Up.WD, 0, sizeof(EQP_RFID_Up.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//向上PGV
		if(g_Para.EQPCommSource.EN.PGVUpward == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.PGVUpward;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.PGVUpward;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.PGVUpward, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_PGVUpward.RD, 6, sizeof(EQP_PGVUpward.RD) / 2, EQP_PGVUpward.WD, 22, sizeof(EQP_PGVUpward.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//向下PGV
		if(g_Para.EQPCommSource.EN.PGVDownward == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.PGVDownward;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.PGVDownward;
			Temp_Address	= Set_Address==0?2:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.PGVDownward, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_PGVDownward.RD, 6, sizeof(EQP_PGVDownward.RD) / 2, EQP_PGVDownward.WD, 22, sizeof(EQP_PGVDownward.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//向上前PGV
		if(g_Para.EQPCommSource.EN.PGVUpward_F == 1)
		{
			SET_CCFID = g_Para.EQPCommSource.CN.PGVUpward_F;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.PGVUpward_F;
			Temp_Address	= Set_Address==0?3:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.PGVUpward_F, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_PGVUpward_F.RD, 6, sizeof(EQP_PGVUpward_F.RD) / 2, EQP_PGVUpward_F.WD, 22, sizeof(EQP_PGVUpward_F.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//向上后PGV
		if(g_Para.EQPCommSource.EN.PGVUpward_B == 1)
		{
			SET_CCFID = g_Para.EQPCommSource.CN.PGVUpward_B;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.PGVUpward_B;
			Temp_Address	= Set_Address==0?4:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.PGVUpward_B, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_PGVUpward_B.RD, 6, sizeof(EQP_PGVUpward_B.RD) / 2, EQP_PGVUpward_B.WD, 22, sizeof(EQP_PGVUpward_B.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//向下前PGV
		if(g_Para.EQPCommSource.EN.PGVDownward_F == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.PGVDownward_F;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.PGVDownward_F;
			Temp_Address	= Set_Address==0?5:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.PGVDownward_F, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_PGVDownward_F.RD, 6, sizeof(EQP_PGVDownward_F.RD) / 2, EQP_PGVDownward_F.WD, 22, sizeof(EQP_PGVDownward_F.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//向下后PGV
		if(g_Para.EQPCommSource.EN.PGVDownward_B == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.PGVDownward_B;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.PGVDownward_B;
			Temp_Address	= Set_Address==0?6:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.PGVDownward_B, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_PGVDownward_B.RD, 6, sizeof(EQP_PGVDownward_B.RD) / 2, EQP_PGVDownward_B.WD, 22, sizeof(EQP_PGVDownward_B.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//串口拓展板
		if(g_Para.EQPCommSource.EN.SerialExp == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.SerialExp;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.SerialExp;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.SerialExp, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
		EQP_SerialExp.RD, 0, sizeof(EQP_SerialExp.RD) / 2, EQP_SerialExp.WD, SerialExpReadMax, sizeof(EQP_SerialExp.WD) / 2,
		NORMAL_QUE, Temp_CCFID);
		
		//串口拓展板
		if(g_Para.EQPCommSource.EN.SerialExp_2 == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.SerialExp_2;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.SerialExp_2;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.SerialExp_2, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
		EQP_SerialExp_2.RD, 0, sizeof(EQP_SerialExp_2.RD) / 2, EQP_SerialExp_2.WD, SerialExpReadMax, sizeof(EQP_SerialExp_2.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//通信转换板 卡式转接板
		if(g_Para.EQPCommSource.EN.CommConv == 1)
		{
			SET_CCFID = g_Para.EQPCommSource.CN.CommConv;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.CommConv;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.CommConv, Baud_115200, 3, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_CommConv.RD, 250, sizeof(EQP_CommConv.RD) / 2, EQP_CommConv.WD, 0, sizeof(EQP_CommConv.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//通信转换板 卡式转接板
		if(g_Para.EQPCommSource.EN.CommConv2 == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.CommConv2;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.CommConv2;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.CommConv2, Baud_115200, 3, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_CommConv2.RD, 250, sizeof(EQP_CommConv2.RD) / 2, EQP_CommConv2.WD, 0, sizeof(EQP_CommConv2.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//工控机 SLAM导航中间件
		if(g_Para.EQPCommSource.EN.IPC == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.IPC;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN15:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.IPC;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.IPC, Baud_115200, 3, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
		EQP_IPC.RD, 6, sizeof(EQP_IPC.RD) / 2, EQP_IPC.WD, 21, sizeof(EQP_IPC.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//辊筒控制器
		if(g_Para.EQPCommSource.EN.Roll_Controller == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Roll_Controller;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Roll_Controller;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Roll_Controller, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
		EQP_Roll_Controller.RD, 0, sizeof(EQP_Roll_Controller.RD) / 2, EQP_Roll_Controller.WD, 0, sizeof(EQP_Roll_Controller.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//遥控器B
		if(g_Para.EQPCommSource.EN.WHB == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.WHB;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.WHB;
			Temp_Address	= Set_Address==0?127:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.WHB, Baud_115200, 3, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
		EQP_WHB.RD, 194, sizeof(EQP_WHB.RD) / 2, EQP_WHB.WD, 230, sizeof(EQP_WHB.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//光斑
		if(g_Para.EQPCommSource.EN.LaserSpot == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.LaserSpot;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.LaserSpot;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.LaserSpot, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_LaserSpot.RD, 0, sizeof(EQP_LaserSpot.RD) / 2, EQP_LaserSpot.WD, LaserSpotReadMax, sizeof(EQP_LaserSpot.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//联动通信
		if(g_Para.EQPCommSource.EN.Assembly_Master == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Assembly_Master;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Assembly_Master;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		if (g_Para.SysFucBit.Asb_CommDirect == 0)
		{
			EQPMstInit(&EQPMsts.Assembly_Master, Baud_115200, 5, CCFRTU, 50, 10, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
			EQP_Assembly_Master.RD, g_Asb.Comm.MasterDataLength, sizeof(EQP_Assembly_Master.RD) / 2, EQP_Assembly_Master.WD, 0, sizeof(EQP_Assembly_Master.WD) / 2,
		NORMAL_QUE, Temp_CCFID);
		}
		else
		{//直接使用对象作为数据池
			EQPMstInit(&EQPMsts.Assembly_Master, Baud_115200, 5, CCFRTU, 50, 20, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
				(u16*)&(g_Asb.Comm.SlaveData[1]),0, sizeof(AsbCommSlave_TypeDef) / 2, (u16*)&g_Asb.Comm.MasterData,g_Asb.Comm.SlaveDataLength*2, sizeof(AsbCommMaster_TypeDef) / 2,
				NORMAL_QUE, Temp_CCFID);
		}

		//兴颂modbus通讯的前雷达 读取指令
		if(g_Para.EQPCommSource.EN.Hinson_Radar_Front == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Hinson_Radar_Front;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Hinson_Radar_Front;
			Temp_Address	= Set_Address==0?1:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Hinson_Radar_Front_R, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_Hinson_Radar_Front.RD, 1000, sizeof(EQP_Hinson_Radar_Front.RD) / 2, EQP_Hinson_Radar_Front.RD, 1000, sizeof(EQP_Hinson_Radar_Front.RD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的前雷达 写指令  由读指令一次性完成控制
		EQPMstInit(&EQPMsts.Hinson_Radar_Front_W, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR, OP_REG,
		EQP_Hinson_Radar_Front.WD, 2000, sizeof(EQP_Hinson_Radar_Front.WD) / 2, EQP_Hinson_Radar_Front.WD, 2000, sizeof(EQP_Hinson_Radar_Front.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的后雷达 读取指令
		if(g_Para.EQPCommSource.EN.Hinson_Radar_Rear == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Hinson_Radar_Rear;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Hinson_Radar_Rear;
			Temp_Address	= Set_Address==0?2:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Hinson_Radar_Rear_R, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_Hinson_Radar_Rear.RD, 1000, sizeof(EQP_Hinson_Radar_Rear.RD) / 2, EQP_Hinson_Radar_Rear.RD, 1000, sizeof(EQP_Hinson_Radar_Rear.RD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的后雷达 写指令
		EQPMstInit(&EQPMsts.Hinson_Radar_Rear_W, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR, OP_REG,
		EQP_Hinson_Radar_Rear.WD, 2000, sizeof(EQP_Hinson_Radar_Rear.WD) / 2, EQP_Hinson_Radar_Rear.WD, 2000, sizeof(EQP_Hinson_Radar_Rear.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的左雷达 读取指令
		if(g_Para.EQPCommSource.EN.Hinson_Radar_Left == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Hinson_Radar_Left;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Hinson_Radar_Left;
			Temp_Address	= Set_Address==0?3:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Hinson_Radar_Left_R, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_Hinson_Radar_Left.RD, 1000, sizeof(EQP_Hinson_Radar_Left.RD) / 2, EQP_Hinson_Radar_Left.RD, 1000, sizeof(EQP_Hinson_Radar_Left.RD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的左雷达 写指令
		EQPMstInit(&EQPMsts.Hinson_Radar_Left_W, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR, OP_REG,
		EQP_Hinson_Radar_Left.WD, 2000, sizeof(EQP_Hinson_Radar_Left.WD) / 2, EQP_Hinson_Radar_Left.WD, 2000, sizeof(EQP_Hinson_Radar_Left.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的右雷达 读取指令
		if(g_Para.EQPCommSource.EN.Hinson_Radar_Righ == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Hinson_Radar_Righ;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Hinson_Radar_Righ;
			Temp_Address	= Set_Address==0?4:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Hinson_Radar_Righ_R, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_Hinson_Radar_Right.RD, 1000, sizeof(EQP_Hinson_Radar_Right.RD) / 2, EQP_Hinson_Radar_Right.RD, 1000, sizeof(EQP_Hinson_Radar_Right.RD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的右雷达 写指令
		EQPMstInit(&EQPMsts.Hinson_Radar_Right_W, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR, OP_REG,
		EQP_Hinson_Radar_Right.WD, 2000, sizeof(EQP_Hinson_Radar_Right.WD) / 2, EQP_Hinson_Radar_Right.WD, 2000, sizeof(EQP_Hinson_Radar_Right.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的前斜面雷达 读取指令
		if(g_Para.EQPCommSource.EN.Hinson_Radar_FrontUp == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Hinson_Radar_FrontUp;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Hinson_Radar_FrontUp;
			Temp_Address	= Set_Address==0?5:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Hinson_Radar_FrontUp_R, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_Hinson_Radar_FrontUp.RD, 1000, sizeof(EQP_Hinson_Radar_FrontUp.RD) / 2, EQP_Hinson_Radar_FrontUp.RD, 1000, sizeof(EQP_Hinson_Radar_FrontUp.RD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的前斜面雷达 写指令
		EQPMstInit(&EQPMsts.Hinson_Radar_FrontUp_W, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR, OP_REG,
		EQP_Hinson_Radar_FrontUp.WD, 2000, sizeof(EQP_Hinson_Radar_FrontUp.WD) / 2, EQP_Hinson_Radar_FrontUp.WD, 2000, sizeof(EQP_Hinson_Radar_FrontUp.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的后斜面雷达 读取指令
		if(g_Para.EQPCommSource.EN.Hinson_Radar_RearUp == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.Hinson_Radar_RearUp;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN14:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.Hinson_Radar_RearUp;
			Temp_Address	= Set_Address==0?6:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.Hinson_Radar_RearUp_R, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_IR, OP_REG,
		EQP_Hinson_Radar_RearUp.RD, 1000, sizeof(EQP_Hinson_Radar_RearUp.RD) / 2, EQP_Hinson_Radar_RearUp.RD, 1000, sizeof(EQP_Hinson_Radar_RearUp.RD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//兴颂modbus通讯的后斜面雷达 写指令
		EQPMstInit(&EQPMsts.Hinson_Radar_RearUp_W, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR, OP_REG,
		EQP_Hinson_Radar_RearUp.WD, 2000, sizeof(EQP_Hinson_Radar_RearUp.WD) / 2, EQP_Hinson_Radar_RearUp.WD, 2000, sizeof(EQP_Hinson_Radar_RearUp.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//易佰特 无线io数据模块  发送 --站号为近端模块站号
		if(g_Para.EQPCommSource.EN.ME15_Near == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.ME15_Near;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.ME15_Near;
			Temp_Address	= Set_Address==0?3:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.ME15_Near_W, Baud_9600, 6, CCFRTU, 900, 300, 1, Temp_Address, MD_FR_MCOIL, OP_COIL,
		&EQP_ME15_Near.RD[0], 0, 8, &EQP_ME15_Near.WD[0], 8, 1,
		NORMAL_QUE, Temp_CCFID);

		//易佰特 无线io数据模块  接收 --站号为近端模块站号
		EQPMstInit(&EQPMsts.ME15_Near_R, Baud_9600, 6, CCFRTU, 900, 500, 1, Temp_Address, MD_RD_DISCRETE, OP_COIL,
		&EQP_ME15_Near.RD[0], 0, 8, &EQP_ME15_Near.WD[0], 8, 1,
		NORMAL_QUE, Temp_CCFID);

		//易佰特 无线io数据模块  发送 --站号为远端模块站号
		if(g_Para.EQPCommSource.EN.ME15_Far == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.ME15_Far;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;

			Set_Address 	= g_Para.EQPCommSource.Address.ME15_Far;
			Temp_Address	= Set_Address==0?5:Set_Address;
		}
		else{Temp_CCFID = CCFCN_Disable;}
		EQPMstInit(&EQPMsts.ME15_Far_W, Baud_9600, 6, CCFRTU, 900, 300, 1, Temp_Address, MD_FR_MCOIL, OP_COIL,
		&EQP_ME15_Far.RD[0], 0, 8, &EQP_ME15_Far.WD[0], 8, 1,
		NORMAL_QUE, Temp_CCFID);

		//易佰特 无线io数据模块  接收 --站号为远端模块站号
		EQPMstInit(&EQPMsts.ME15_Far_R, Baud_9600, 6, CCFRTU, 900, 500, 1, Temp_Address, MD_RD_DISCRETE, OP_COIL,
		&EQP_ME15_Far.RD[0], 0, 8, &EQP_ME15_Far.WD[0], 8, 1,
		NORMAL_QUE, Temp_CCFID);
		
		//PLC安全模块
		if (g_Para.EQPCommSource.EN.PLC_Estop == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.PLC_Estop;
			Temp_CCFID = SET_CCFID == CCFCN_Disable ? CCFCN22 : SET_CCFID;

			Set_Address = g_Para.EQPCommSource.Address.PLC_Estop;
			Temp_Address = Set_Address == 0 ? 1 : Set_Address;
		}
		else { Temp_CCFID = CCFCN_Disable; }
		EQPMstInit(&EQPMsts.PLC_Estop, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
		EQP_PLC_Estop.RD, 0, sizeof(EQP_PLC_Estop.RD) / 2, EQP_PLC_Estop.WD, 0, sizeof(EQP_PLC_Estop.WD) / 2,
		NORMAL_QUE, Temp_CCFID);

		//DA模块  此模块出厂默认是9600 无校验；需要配置成115200 偶校验；
		if (g_Para.EQPCommSource.EN.DA == 1)
		{
			SET_CCFID = g_Para.EQPCommSource.CN.DA;
			Temp_CCFID = SET_CCFID == CCFCN_Disable ? CCFCN22 : SET_CCFID;

			Set_Address = g_Para.EQPCommSource.Address.DA;
			Temp_Address = Set_Address == 0 ? 1 : Set_Address;
		}
		else { Temp_CCFID = CCFCN_Disable; }
		EQPMstInit(&EQPMsts.DA, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR, OP_REG,
		EQP_DA.RD, 0, sizeof(EQP_DA.RD) / 2, EQP_DA.WD, 0, sizeof(EQP_DA.WD) / 2,
		NORMAL_QUE, Temp_CCFID);
		//二次开发 主机通讯
		if (g_Para.EQPCommSource.EN.API_Master == 1)
		{
			SET_CCFID = g_Para.EQPCommSource.CN.API_Master;
			Temp_CCFID = SET_CCFID == CCFCN_Disable ? CCFCN21 : SET_CCFID;

			Set_Address = g_Para.EQPCommSource.Address.API_Master;
			Temp_Address = Set_Address == 0 ? 1 : Set_Address;
		}
		else { Temp_CCFID = CCFCN_Disable; }
		EQPMstInit(&EQPMsts.API_Master, Baud_115200, 5, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
			EQP_API_Master.RD, 0, sizeof(EQP_API_Master.RD) / 2, EQP_API_Master.WD, API_Master_ReadMax, sizeof(EQP_API_Master.WD) / 2,
			NORMAL_QUE, Temp_CCFID);
		//安川m3板卡
		if(g_Para.EQPCommSource.EN.ANC == 1)
		{
			/*默认使用CN22*/
			SET_CCFID = g_Para.EQPCommSource.CN.ANC;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN15:SET_CCFID;
			/*默认使用0x0A作为地址*/
			Set_Address 	= g_Para.EQPCommSource.Address.ANC;
			Temp_Address	= Set_Address==0?0x0A:Set_Address;
			/*从572开始读取*/
			EQPMstInit(&EQPMsts.ANC, Baud_115200, 3, CCFRTU, 50, 20, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
			EQP_ANC.RD, 572, sizeof(EQP_ANC.RD) / 2, EQP_ANC.WD, 440, sizeof(EQP_ANC.WD) / 2,
			NORMAL_QUE, Temp_CCFID);
		}

		//多倍通配置切换
		if(g_Para.EQPCommSource.EN.DBSET == 1)
		{
			/*默认使用CN22*/
			SET_CCFID = g_Para.EQPCommSource.CN.DBSET;
			Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN22:SET_CCFID;
			/*默认使用0x01作为地址*/
			Set_Address 	= g_Para.EQPCommSource.Address.DBSET;
			Temp_Address	= Set_Address==0?0x01:Set_Address;
			/*自定义 注意CCFCustomUART协议的读写长度参数单位 为char*/
			EQPMstInit(&EQPMsts.DBSET, Baud_115200, 3, CCFCustomUART, 2000, 1000, 1, Temp_Address, MD_FR_MHR_RDHR, OP_REG,
			EQP_DBSET.RD, 0, sizeof(EQP_DBSET.RD), EQP_DBSET.WD, 0, sizeof(EQP_DBSET.WD),
			NORMAL_QUE, Temp_CCFID);
		}

        //PLC
		if (g_Para.EQPCommSource.EN.PLC == 1)
		{

			SET_CCFID = g_Para.EQPCommSource.CN.PLC;
			Temp_CCFID = SET_CCFID == CCFCN_Disable ? CCFCN21 : SET_CCFID;

			Set_Address = g_Para.EQPCommSource.Address.PLC;
			Temp_Address = Set_Address == 0 ? 1 : Set_Address;
		}
		else { Temp_CCFID = CCFCN_Disable; }
		EQPMstInit(&EQPMsts.PLC, Baud_115200, 6, CCFRTU, 100, 50, 1, Temp_Address, MD_FR_MHR, OP_REG,
		EQP_PLC.WD, 3, sizeof(EQP_PLC.WD) / 2, EQP_PLC.RD, 10, sizeof(EQP_PLC.RD) / 2,
		NORMAL_QUE, Temp_CCFID);
	}
}

/***********************************************************
** 函数名称:		EQPComm_Custom_Init
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/

//通用modbus外设初始化
void EQPComm_Custom_Init(void)
{
	CCFID_EnumDef 	Temp_CCFID = CCFCN_Disable;
	CCFID_EnumDef 	SET_CCFID  = CCFCN_Disable;
	uint8_t			Temp_Address = 0;
	uint8_t			Set_Address = 0;

	//凯信达 BMS 私有协议通讯 发送0xDD A5 03 00 FF FD 77 即可
	if(g_Para.EQPCommSource.EN.BMS_KXD == 1)
	{
		uint8_t Temp[7] = {0xDD,0xA5,0x03,0x00,0xFF,0xFD,0x77};
		memcpy(EQP_KXD_BMS.WD,Temp,sizeof(Temp));

		SET_CCFID = g_Para.EQPCommSource.CN.BMS_KXD;
		Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

		Set_Address 	= g_Para.EQPCommSource.Address.BMS_KXD;
		Temp_Address	= Set_Address==0?5:Set_Address;
		//注册数据解析函数
		Custom_UART_Receive_DataAnalysis = KxdBMS_DataAnalysis;
	}
	else{Temp_CCFID = CCFCN_Disable;}

	EQPMstInit(&EQPMsts.KXD_BMS, Baud_9600, 3, CCFCustomUART, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
	(uint16_t*)EQP_KXD_BMS.RD, 0, sizeof(EQP_KXD_BMS.RD), (uint16_t*)EQP_KXD_BMS.WD, 0, sizeof(EQP_KXD_BMS.WD),
	NORMAL_QUE, Temp_CCFID);
	
	//倍加福PGV 私有协议通讯
	/*上*/
	if(g_Para.EQPCommSource.EN.BJF_PGV_Upward == 1)
	{
		uint8_t Temp[2] = {0xC8,0x37};
		
		SET_CCFID = g_Para.EQPCommSource.CN.BJF_PGV_Upward;
		Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

		Set_Address 	= g_Para.EQPCommSource.Address.BJF_PGV_Upward;
		Temp_Address	= Set_Address==0?5:Set_Address;

		if(Set_Address<=4)
		{
			Temp[0] = Temp[0] + Set_Address-1;
			Temp[1] = Temp[1] - Set_Address+1;
		}
		memcpy(EQP_BJF_PGV_Upward.WD,Temp,sizeof(Temp));
		//注册数据解析函数
		if(Custom_UART_Receive_DataAnalysis == NULL)
		{
			Custom_UART_Receive_DataAnalysis = BjfPGV_DataAnalysis;
		}
	}
	else{Temp_CCFID = CCFCN_Disable;}

	EQPMstInit(&EQPMsts.BJF_PGV_Upward, Baud_115200, 3, CCFCustomUART, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
	(uint16_t*)EQP_BJF_PGV_Upward.RD, 0, sizeof(EQP_BJF_PGV_Upward.RD), (uint16_t*)EQP_BJF_PGV_Upward.WD, 0, sizeof(EQP_BJF_PGV_Upward.WD),
	NORMAL_QUE, Temp_CCFID);

	/*下*/
	if(g_Para.EQPCommSource.EN.BJF_PGV_Downward == 1)
	{
		uint8_t Temp[2] = {0xC8,0x37};
		
		SET_CCFID = g_Para.EQPCommSource.CN.BJF_PGV_Downward;
		Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

		Set_Address 	= g_Para.EQPCommSource.Address.BJF_PGV_Downward;
		Temp_Address	= Set_Address==0?5:Set_Address;

		if(Temp_Address<=4)
		{
			Temp[0] = Temp[0] + Temp_Address-1;
			Temp[1] = Temp[1] - Temp_Address+1;
		}
		memcpy(EQP_BJF_PGV_Downward.WD,Temp,sizeof(Temp));
		//注册数据解析函数
		if(Custom_UART_Receive_DataAnalysis == NULL)
		{
			Custom_UART_Receive_DataAnalysis = BjfPGV_DataAnalysis;
		}
	}
	else{Temp_CCFID = CCFCN_Disable;}

	EQPMstInit(&EQPMsts.BJF_PGV_Downward, Baud_115200, 3, CCFCustomUART, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
	(uint16_t*)EQP_BJF_PGV_Downward.RD, 0, sizeof(EQP_BJF_PGV_Downward.RD), (uint16_t*)EQP_BJF_PGV_Downward.WD, 0, sizeof(EQP_BJF_PGV_Downward.WD),
	NORMAL_QUE, Temp_CCFID);

	/*上前*/
	if(g_Para.EQPCommSource.EN.BJF_PGV_Upward_F == 1)
	{
		uint8_t Temp[2] = {0xC8,0x37};
		
		SET_CCFID = g_Para.EQPCommSource.CN.BJF_PGV_Upward_F;
		Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

		Set_Address 	= g_Para.EQPCommSource.Address.BJF_PGV_Upward_F;
		Temp_Address	= Set_Address==0?5:Set_Address;

		if(Set_Address<=4)
		{
			Temp[0] = Temp[0] + Set_Address-1;
			Temp[1] = Temp[1] - Set_Address+1;
		}
		memcpy(EQP_BJF_PGV_Upward_F.WD,Temp,sizeof(Temp));
		//注册数据解析函数
		if(Custom_UART_Receive_DataAnalysis == NULL)
		{
			Custom_UART_Receive_DataAnalysis = BjfPGV_DataAnalysis;
		}
	}
	else{Temp_CCFID = CCFCN_Disable;}

	EQPMstInit(&EQPMsts.BJF_PGV_Upward_F, Baud_115200, 3, CCFCustomUART, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
	(uint16_t*)EQP_BJF_PGV_Upward_F.RD, 0, sizeof(EQP_BJF_PGV_Upward_F.RD), (uint16_t*)EQP_BJF_PGV_Upward_F.WD, 0, sizeof(EQP_BJF_PGV_Upward_F.WD),
	NORMAL_QUE, Temp_CCFID);

	/*上后*/
	if(g_Para.EQPCommSource.EN.BJF_PGV_Upward_B == 1)
	{
		uint8_t Temp[2] = {0xC8,0x37};
		SET_CCFID = g_Para.EQPCommSource.CN.BJF_PGV_Upward_B;
		Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

		Set_Address 	= g_Para.EQPCommSource.Address.BJF_PGV_Upward_B;
		Temp_Address	= Set_Address==0?5:Set_Address;
		
		if(Set_Address<=4)
		{
			Temp[0] = Temp[0] + Set_Address-1;
			Temp[1] = Temp[1] - Set_Address+1;
		}
		memcpy(EQP_BJF_PGV_Upward_B.WD,Temp,sizeof(Temp));
		//注册数据解析函数
		if(Custom_UART_Receive_DataAnalysis == NULL)
		{
			Custom_UART_Receive_DataAnalysis = BjfPGV_DataAnalysis;
		}
	}
	else{Temp_CCFID = CCFCN_Disable;}

	EQPMstInit(&EQPMsts.BJF_PGV_Upward_B, Baud_115200, 3, CCFCustomUART, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
	(uint16_t*)EQP_BJF_PGV_Upward_B.RD, 0, sizeof(EQP_BJF_PGV_Upward_B.RD), (uint16_t*)EQP_BJF_PGV_Upward_B.WD, 0, sizeof(EQP_BJF_PGV_Upward_B.WD),
	NORMAL_QUE, Temp_CCFID);

	/*下前*/
	if(g_Para.EQPCommSource.EN.BJF_PGV_Downward_F == 1)
	{
		uint8_t Temp[2] = {0xC8,0x37};

		SET_CCFID = g_Para.EQPCommSource.CN.BJF_PGV_Downward_F;
		Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

		Set_Address 	= g_Para.EQPCommSource.Address.BJF_PGV_Downward_F;
		Temp_Address	= Set_Address==0?5:Set_Address;
		if(Set_Address<=4)
		{
			Temp[0] = Temp[0] + Set_Address-1;
			Temp[1] = Temp[1] - Set_Address+1;
		}
		memcpy(EQP_BJF_PGV_Downward_F.WD,Temp,sizeof(Temp));
		//注册数据解析函数
		if(Custom_UART_Receive_DataAnalysis == NULL)
		{
			Custom_UART_Receive_DataAnalysis = BjfPGV_DataAnalysis;
		}
	}
	else{Temp_CCFID = CCFCN_Disable;}

	EQPMstInit(&EQPMsts.BJF_PGV_Downward_F, Baud_115200, 3, CCFCustomUART, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
	(uint16_t*)EQP_BJF_PGV_Downward_F.RD, 0, sizeof(EQP_BJF_PGV_Downward_F.RD), (uint16_t*)EQP_BJF_PGV_Downward_F.WD, 0, sizeof(EQP_BJF_PGV_Downward_F.WD),
	NORMAL_QUE, Temp_CCFID);

	if(g_Para.EQPCommSource.EN.BJF_PGV_Downward_B == 1)
	{
		uint8_t Temp[2] = {0xC8,0x37};

		SET_CCFID = g_Para.EQPCommSource.CN.BJF_PGV_Downward_B;
		Temp_CCFID = SET_CCFID==CCFCN_Disable?CCFCN21:SET_CCFID;

		Set_Address 	= g_Para.EQPCommSource.Address.BJF_PGV_Downward_B;
		Temp_Address	= Set_Address==0?5:Set_Address;

		if(Set_Address<=4)
		{
			Temp[0] = Temp[0] + Set_Address-1;
			Temp[1] = Temp[1] - Set_Address+1;
		}
		memcpy(EQP_BJF_PGV_Downward_B.WD,Temp,sizeof(Temp));
		//注册数据解析函数
		if(Custom_UART_Receive_DataAnalysis == NULL)
		{
			Custom_UART_Receive_DataAnalysis = BjfPGV_DataAnalysis;
		}
	}
	else{Temp_CCFID = CCFCN_Disable;}

	//115200 N 8 1
	EQPMstInit(&EQPMsts.BJF_PGV_Downward_B, Baud_115200, 3, CCFCustomUART, 100, 50, 1, Temp_Address, MD_RD_HR, OP_REG,
	(uint16_t*)EQP_BJF_PGV_Downward_B.RD, 0, sizeof(EQP_BJF_PGV_Downward_B.RD), (uint16_t*)EQP_BJF_PGV_Downward_B.WD, 0, sizeof(EQP_BJF_PGV_Downward_B.WD),
	NORMAL_QUE, Temp_CCFID);
}

/***********************************************************
** 函数名称:		EQPComm_Init
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/

void EQPComm_Init(void)
{
	EQPComm_PortConfReset();

	EQPComm_p2Data_Init();
	
	EQPComm_Universal_Init();
	
	EQPComm_Custom_Init();
}

//***********************************END OF FILE***********************************

