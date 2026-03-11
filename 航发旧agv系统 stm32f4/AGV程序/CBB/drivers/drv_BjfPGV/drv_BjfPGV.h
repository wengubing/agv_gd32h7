/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_BjfPGV.h
** 创 建 人: Master 
** 描    述: 
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年3月29日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef _DRV_BJFPGV_h__
#define _DRV_BJFPGV_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  DRV_BJFPGV_GLOBALS
 #define DRV_BJFPGV_EXT
#else
 #define DRV_BJFPGV_EXT  extern
#endif

//自定义引用
#include <stm32f4xx.h>

#define _PI_DIV180                      0.01745329251994f
#define _180_DIV_PI                     57.2957795130823f


typedef enum
{									/*	位定义*/
	BJF_PGV_NotData		= 0x00,	//0：无效 
	BJF_PGV_FindLine	= 0x01,	//1：识别到色带
	BJF_PGV_NaviCode	= 0x02,	//2：
	BJF_PGV_CtrlCode	= 0x04,	//3：
	BJF_PGV_FindCODE	= 0x08,	//4：识别到二维码
	BJF_PGV_MarkCode	= 0x10,	//5：读取到当前MARK点Code码
}BJF_PGV_Sta_EnumDef;


/***************************************************************/
/***************************************************************/
typedef struct
{
	float		X; 			/*X 方向定位数据，单位1mm*/
	float		Y;			/*Y 方向定位数据，单位1mm*/
	float		Z_Deviation;/*z 和正交轴的偏差值，单位1° -45-45*/	
}EQP_BJF_PGVOffsetObj_StDef;

typedef struct
{
	uint8_t		State;				//扫描当前二维码状态
	uint16_t	ControlCode;  	    //当前MARK点的CODE码

	//原始值 地面坐标系 用于导航
	float		Xpositon;			//PGV相对于MARK点中心X坐标的偏移     
	float		Ypositon;     		//PGV相对于MARK点中心Y坐标的偏移
	float		Zpositon;     		//PGV相对于MARK点中心Y坐标的偏移
	float		Anglevalue;			//PGV相对于MARK点中心角度的偏移  
	EQP_BJF_PGVOffsetObj_StDef	Offset;
}EQP_BJF_PGVObj_StDef;


//倍加福PGV通信数据池 定义
typedef struct
{
	uint8_t 				WD[2];		//私有协议，发送0xC8 37 即可
	uint8_t 				RD[21];		//私有协议，预计接收到21个字节
	EQP_BJF_PGVObj_StDef 	BJF_PGV_Data;
}EQP_BJF_PGV_StDef;

DRV_BJFPGV_EXT uint8_t BjfPGV_DataAnalysis(uint8_t* p_buff,uint16_t buff_num);
DRV_BJFPGV_EXT void BjfPGV_UpDate(uint8_t *BJG_PGV_RD,EQP_BJF_PGVObj_StDef *Data);
#endif

//***********************************END OF FILE***********************************


