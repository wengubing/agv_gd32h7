/****************************Copyright (c)**********************************************
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_RFIDData.C
** 创 建 人: master
** 描    述: RFID数据处理源文件，包含将通讯得到的原始数据放到对应结构体成员的函数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2025年4月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/  
#define	RFID_DATA_GLOBALS

#include "drv_RFIDData.h"
#include "EQPComm_Init.h"


/***************************************************************************************
** 函数名称: RFID_DataProcess_MCB
** 功能描述: RFID数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RFID_DataProcess_MCB(RFID_TypeDef *RfidData)
{
	RFID_Read_StDef		*p_Read_Output	= &RfidData->RFID_Read;
	EQP_RFIDObj_StDef	*p_Read_Input	= RfidData->InPutData;

	/*数据更新*/
	p_Read_Output->Status 		= p_Read_Input->ReadState;
	p_Read_Output->RSSIStatus 	= p_Read_Input->Strong;
	/*直连的RFID需要进行数据拼接*/
	p_Read_Output->Tag= p_Read_Input->Data1 + (p_Read_Input->Data2<<8);

	/*记录历史tag值*/
	if(	p_Read_Output->Tag != p_Read_Output->LastTag
		&&p_Read_Output->Tag != 0X00)
	{
		p_Read_Output->LastTag = p_Read_Output->Tag;
	}

}
/***************************************************************************************
** 函数名称: RFID_DataProcess_SerialExp
** 功能描述: RFID数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RFID_DataProcess_SerialExp(RFID_TypeDef *RfidData)
{
	RFID_Read_StDef	*p_Read_Output	= &RfidData->RFID_Read;
	RFIDObj_StDef	*p_Read_Input	= RfidData->InPutData;

	/*数据更新*/
	p_Read_Output->Status 		= p_Read_Input->ReadState;
	p_Read_Output->RSSIStatus 	= p_Read_Input->Strong;
	/*导航转接板已经将数据进行了拼接，直接赋值就行*/
	p_Read_Output->Tag= p_Read_Input->Data;

	/*记录历史tag值*/
	if(	p_Read_Output->Tag != p_Read_Output->LastTag
		&&p_Read_Output->Tag != 0X00)
	{
		p_Read_Output->LastTag = p_Read_Output->Tag;
	}
}
/***************************************************************************************
** 函数名称: RFID_DataProcess
** 功能描述: RFID数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RFID_DataProcess(RFID_TypeDef *RfidData)
{
	if(RfidData->InPutData == NULL) {return;}
	switch (RfidData->RFID_Source)
	{
		default:
		case RFID_InPutSource_MCB :
		{
			RFID_DataProcess_MCB(RfidData);
		}break;
		case RFID_InPutSource_SerialExp :
		{
			RFID_DataProcess_SerialExp(RfidData);
		}break;
	}
}

/*****************************************************END OF FILE*********************************************/
