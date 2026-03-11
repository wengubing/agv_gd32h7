/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_KxdBMS.h
** 创 建 人: Master 
** 描    述: 
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年3月29日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define DRV_KXDBMS_GLOBALS
#include "drv_KxdBMS.h"
#include "string.h"

/***********************************************************
** 函数名称:	KxdBMS_DataAnalysis
** 功能描述: 
** 参    数:	None
** 返 回 值:		0-数据核验通过  其他- 不通过，详见注释
***********************************************************/
uint8_t KxdBMS_DataAnalysis(uint8_t* p_buff,uint16_t buff_num)
{
	uint8_t 	res = 0x00;
//	uint8_t 	num = 0;
//	uint16_t	CalculatedSumCheck	= 0;
//	uint16_t	AcceptSumCheck		= 0;

	do{
		/*检验帧头*/
		if(p_buff[0] 		!= 0xDD)			{res = 1;break;}

		/*检验帧尾*/
		if(p_buff[buff_num-1] != 0X77)			{res = 2;break;}
		
		/*检验校验和*/
//		for(num = 2 ;num < buff_num-3 ;num++ )
//		{
//			CalculatedSumCheck += p_buff[num];
//		}
//		AcceptSumCheck = (p_buff[buff_num-3]<<8)+p_buff[buff_num-2];
//		if(AcceptSumCheck != 0)	{res = 3;break;}

	}while(0);

	return res;
}
/***************************************************************************************
** 函数名称: short_xch_hl
** 功能描述: 交换两个字节的结果
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
void KxdBMS_short_xch_hl(uint8_t*p_1,uint8_t*p_2)
{
	uint8_t		temp = 0;
	temp = *p_1;
	*p_1 = *p_2;
	*p_2 = temp;
}
/***********************************************************
** 函数名称:	KxdBMS_UpDate
** 功能描述: 数据解析，在主循环中调用，不涉及底层
** 参    数:	KXD_BMS_RD:原始数据池
** 返 回 值:		
***********************************************************/
EQP_KXD_BMSObj_StDef* KxdBMS_UpDate(uint8_t *KXD_BMS_RD)
{
	static uint8_t temp_buff[45];
	uint8_t		*p_temp = temp_buff;
	memcpy(temp_buff,KXD_BMS_RD,sizeof(temp_buff));
	do{
		p_temp = p_temp+4;
		//电压高低字节交换
		KxdBMS_short_xch_hl(p_temp,p_temp+1);
		//电流高低字节交换
		p_temp = p_temp+2;
		KxdBMS_short_xch_hl(p_temp,p_temp+1);
		//SOC
		p_temp = p_temp+2;
		KxdBMS_short_xch_hl(p_temp,p_temp+1);
		p_temp = p_temp+2;
		KxdBMS_short_xch_hl(p_temp,p_temp+1);
		//循环次数
		p_temp = p_temp+2;
		KxdBMS_short_xch_hl(p_temp,p_temp+1);
		//
		p_temp = p_temp+2;
		KxdBMS_short_xch_hl(p_temp,p_temp+1);
		//
		p_temp = p_temp+10;
		KxdBMS_short_xch_hl(p_temp,p_temp+1);

	}while(0);
	return (EQP_KXD_BMSObj_StDef*)temp_buff;
}
//***********************************END OF FILE***********************************

