#ifndef __DRV_EEPARA_H__
#define __DRV_EEPARA_H__


#ifdef		EEPARA_GLOBALS
	#define EEPARA_EXT
#else
	#define	EEPARA_EXT  extern
#endif

	
#include "..\..\Drivers\spi_eeprom\spi_eeprom.h"
#include "DeviceDefine.h"
#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"

#define EEPARA_U16NUM										300/*eeprom参数数量*/


typedef struct
{
	u32	WriteCnt;											//EE写入次数 0~1
	u32	WriteDate;											//最后一次写入时间:1512312359,15年12月31日23时59分
	u32	ProductSerial;										//产品序列号 4~5
	u16 User[EEPARA_U16NUM-7];								//用户数据占位
	u16	CRCSum;												//CRC校验和 299
}EEP_ParaInf_TypeDef;
	
//==========================================================================================
//                                        版本规则
//
//                              XX      -      XX      -     XXXX
//                              版本           中块级          小版本
//
//==========================================================================================
//软件版本：
//大版本号：重大的革命性的全局变更，如要求变更、原理性改变或方案变更，一次性变更很多内容。从1开始，占一个字节
//模块级版本号：功能模块发生变化的时候变更。从0开始，占一个字节
//小版本号：有修改并提交后要加1，占用两个字节，从0开始
//
//
//硬件版本：
//大版本：同软件
//小版本：小范围布局布线变更
//
//版本号使用宏定义方式，不能用变量保存在存储器中。传输版本号时高位字节在前。
//
//==========================================================================================
//                                      序列号规则
//
//码段： [年]  [月]  [日]  [编号]
//位长：  12    4     5      11
//
//年  ：自然年号，能到4095
//月  ：自然月
//日  ：自然日
//编号：0~2047，一批产品的编号
//==========================================================================================
//定义序列号掩码
#define SN_YEAR_MASK                                    20
#define SN_MONTH_MASK                                   16
#define SN_DATE_MASK                                    11
#define SN_ID_MASK                                      0

//硬件版本宏定义
#define HARDWAREVER_BIG                                 ( 2 )
#define HARDWAREVER_SMALL                               ( 2 )
//软件版本宏定义
#define SOFTWAREVER_BIG                                 ( 1 )
#define SOFTWAREVER_MID                                 ( 0 )
#define SOFTWAREVER_SMALL                               ( 3 )

//软硬件版本号定义
#define HARDVERSION                                     ((HARDWAREVER_BIG << 8) + \
                                                        HARDWAREVER_SMALL)

#define SOFTVERSION                                     ((SOFTWAREVER_BIG << 24) + \
                                                        (SOFTWAREVER_MID << 16) + \
                                                        SOFTWAREVER_SMALL)


/*
1.规定EEPROM的4个扇区:
0-1:作为用户参数区;(地址:0-65535)
	2:出厂参数区;(地址:65536-98303)
	3.默认参数区;(地址:98304-131071)
1.出厂参数起始地址：
2.用户参数起始地址：
*/
#define PARA_ADDRESS_USER               0     		// 用户参数区起始地址
#define PARA_ADDRESS_FACT               65536     // 出厂参数区起始地址
#define PARA_ADDRESS_DEFAULT						98304     // 默认参数区起始地址


#define BOARD_PARA_BYTE_SIZE						(EEPARA_U16NUM<<1)

#define	EEPROM_POWER_WODWN_TIMES				300 //ms

// 参数索引
typedef enum
{
    PARA_TAG_DATA = 1,              // TAG_UID参数
    PARA_STATION_SET_DATA,					//站点设置参数
		PARA_DETAIL_SET_DATA,

}UartParaCmdType;



//EEPROM操作结果定义
typedef enum 
{
    EE_OK = 0,                           // 读/写ok
    EE_ErrorParameter,                   //输入参数错误
    EE_ErrorSetProtect,                  //设置保护错误
    EE_ErrorSaveProtect,                 //保存保护设置错误
    EE_ErrorEraseBlock,                  //擦除块错误
    EE_ErrorEraseData,                   //擦除后数据错误
    EE_ErrorProgramBlock,                //块写入错误
    EE_ErrorProgramData                  //写入后数据错误
}EE_OperateResult_Type;

//参数类型定义
typedef enum 
{
	NONE_PARA=0,	//无参数类型
	USER_PARA,		//用户参数
	FACT_PARA,		//厂家参数
	DEFAULT_PARA,	//默认参数
}PARA_Type;

//EEPROM工作流程定义
typedef enum
{
	EE_CMD_NULL = 0,	
	EE_CMD_INI,
	EE_CMD_PWR_UP_GET_DATA,		
	EE_CMD_SAVE_USER_PARA,		//用户自己设定的一套参数
	EE_CMD_SAVE_FACT_PARA,		//出厂设定的一套参数
	EE_CMD_SAVE_DEFAULT_PARA,	//默认的理想参数
	EE_CMD_LOADING_DEFAULT_PARA,//导入默认参数到用户区
}EE_CMD;



//外部变量
EEPARA_EXT 	u32			g_SerialNumber;
EEPARA_EXT 	u16			*g_pBoardpara;	//EEPROM映射到DHR的指针

//外部接口函数
EEPARA_EXT	u8 TSK_EE_PARA(EE_CMD cmd);







#endif
