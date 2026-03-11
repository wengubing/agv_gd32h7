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

//保证以下所有内容只被同一源文件引用一次
#ifndef _DRV_KXDBMS_h__
#define _DRV_KXDBMS_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  DRV_KXDBMS_GLOBALS
 #define DRV_KXDBMS_EXT
#else
 #define DRV_KXDBMS_EXT  extern
#endif

//自定义引用
#include <stm32f4xx.h>
 
 //BMS 结构化通信内容定义
typedef struct
{
	uint8_t		HeaderCode;			//起始码 		必定是0xDD
	uint8_t		FunctionCode;		//功能码			0x03 或者 0x05
	uint8_t		ErrCode;			//通讯故障码		0x00无故障 其他通讯错误
	uint8_t		DataLength;			//数据长度
	uint16_t	Voltage;			//电压 单位10mV，高字节在前
	int16_t		Current;			//电流 单位10mA,正为充电，负为放电
	uint16_t	SOC_Current;		//剩余容量	SOC 的分子	单位10mAh
	uint16_t	SOC_Default;		//剩余容量	SOC 的母子	单位10mAh
	uint16_t	Cycle_Index;		//循环次数
	uint16_t	ManufactureDate;	//生产日期
	uint32_t	EquilibriumState;	//均衡状态	每一个bit 则表示每一串均衡，0 为关闭，1 为打开表示 1~16 串
	uint16_t 	ProtectionState;	//保护状态	每一个bit 表示一种保护状态，0 为未保护，1 发生保护
									/*
									//保护状态说明
									//bit0 单体过压保护
									//bit1 单体欠压保护
									//bit2 整组过压保护
									//bit3 整组欠压保护
									//bit4 充电过温保护
									//bit5 充电低温保护
									//bit6 放电过温保护
									//bit7 放电低温保护
									//bit8 充电过流保护
									//bit9 放电过流保护
									//bit10 短路保护
									//bit11 前端检测IC 错误
									//bit12 软件锁定MOS
									//bit13~bit15 预留
									*/
	uint8_t		SoftwareVersion;	//软件版本
	uint8_t		SOC;				//SOC 单位1%
	uint8_t		FETState;			//MOS 指示状态，bit0 表示充电，bit1 表示放电，0 表示MOS 关闭，1 表示打开
	uint8_t		BatteryString;		//电池串数
	uint8_t		NTCNum;				//NTC 个数N
	uint16_t	NTC[4];
	uint16_t	SumCheckCode;		//校验和
	uint8_t		EpilogCode;			//结束码		必定0x77
}EQP_KXD_BMSObj_StDef;

//BMS通信数据池 定义
typedef struct
{
	uint8_t WD[7];		//私有协议，发送0xDD A5 03 00 FF FD 77 即可
	uint8_t RD[45];		//私有协议，不定长数据解析，常规是45个字节；不确定有多长的数据
}EQP_KXD_BMS_StDef;

DRV_KXDBMS_EXT uint8_t KxdBMS_DataAnalysis(uint8_t* p_buff,uint16_t buff_num);
DRV_KXDBMS_EXT EQP_KXD_BMSObj_StDef* KxdBMS_UpDate(uint8_t *KXD_BMS_RD);
#endif

//***********************************END OF FILE***********************************


