/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : drv_MergeToPGV.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: 将对设定个数传感器的数据，融合成AGV中心偏移的PGV数据
  *******************************************************************************/

#ifndef _DRV_MERGETOPGV_H_
#define _DRV_MERGETOPGV_H_

#ifdef		DRV_MERGETOPGV_GLOBALS
 #define	DRV_MERGETOPGV_EXT
#else
 #define	DRV_MERGETOPGV_EXT  extern
#endif

/*自定义引用*/
#include <stdbool.h>
#include "STM32f4xx.h"
/*PGV类型定义*/
typedef enum 
{
    MergeToPGVType_NULL		= 0,
	
	/*逻辑PGV 两个PGV源头 那个识别到二维码 输出哪个；都识别到了输出平均值*/
	MergeToPGVType_Logical  = 1,
	/*11：前后两个航发PGV传入数据*/
	MergeToPGVType_FB_BisHF	= 11,
	/*		安装方式		*/	/*		输出x=10mm		*/	/*		输出y=10mm		*/
	/*		  车头			*/	/*		  车头			*/	/*		  车头			*/
	/*		|------|y		*/	/*		|------|		*/	/*		|------|		*/
	/*	|	  |HF|		|	*/	/*	|	  |HF|		|	*/	/*	|	  |HF|		|	*/
	/* 	|				|	*/	/* 	|				|	*/	/* 	|	  靶标		|	*/
	/* 	|				|x	*/	/* 	|			靶标|	*/	/* 	|				|	*/
	/* 	|				|	*/	/* 	|				|	*/	/* 	|				|	*/
	/*	|	  |HF|		|	*/	/*	|	  |HF|		|	*/	/*	|	  |HF|		|	*/
	/*		|------|		*/	/*		|------|		*/	/*		|------|		*/
	/*		  车尾			*/	/*		  车尾			*/	/*		  车尾			*/
	/*12：左右两个航发PGV传入数据*/
	MergeToPGVType_LR_BisHF	= 12,
	/*		安装方式		*/	/*		输出x=10mm		*/	/*		输出y=10mm		*/
	/*		  车头			*/	/*		  车头			*/	/*		  车头			*/
	/*		|------|y		*/	/*		|------|		*/	/*		|------|		*/
	/*	|	  			|	*/	/*	|	  			|	*/	/*	|	  			|	*/
	/* 	|				|	*/	/* 	|				|	*/	/* 	|	  		靶标|	*/
	/* 	||HF|		|HF||x	*/	/* 	||HF|		|HF||靶标*/	/* 	||HF|		|HF||	*/
	/* 	|				|	*/	/* 	|				|	*/	/* 	|				|	*/
	/*	|	  			|	*/	/*	|	  			|	*/	/*	|	  			|	*/
	/*		|------|		*/	/*		|------|		*/	/*		|------|		*/
	/*		  车尾			*/	/*		  车尾			*/	/*		  车尾			*/
}
MergeToPGVType_TypeDef;


/*数据融合 对像定义*/
typedef struct
{
	int16_t F_PGV_X;			/*前PGV的x偏移 单位:0.1mm*/
	int16_t F_PGV_Y;			/*前PGV的y偏移 单位:0.1mm*/
	int16_t F_PGV_Z;			/*前PGV的z偏移 单位:0.1°*/
	uint8_t F_PGV_Sta;			/*前PGV的状态 0-不可靠 其他-可靠*/
	int16_t B_PGV_X;			/*后PGV的x偏移 单位:0.1mm*/
	int16_t B_PGV_Y;			/*后PGV的y偏移 单位:0.1mm*/
	int16_t B_PGV_Z;			/*后PGV的z偏移 单位:0.1°*/
	uint8_t B_PGV_Sta;			/*后PGV的状态 0-不可靠 其他-可靠*/

}_MergeToPGV_BisHF_TypeDef;

/*数据零偏  对像定义*/
typedef struct
{
	int16_t F_Offset_X;			/*前PGV的x零点偏移 单位:0.1mm*/
	int16_t F_Offset_Y;			/*前PGV的y零点偏移 单位:0.1mm*/
	int16_t F_Offset_Z;			/*融合后PGV的z零点偏移*/

	int16_t B_Offset_X;			/*后PGV的x零点偏移 单位:0.1mm*/
	int16_t B_Offset_Y;			/*后PGV的y零点偏移 单位:0.1mm*/
	int16_t B_Offset_Z;			/*融合后PGV的z零点偏移*/

	int16_t Offset_Z;			/*融合后PGV的z零点偏移*/

}_MergeToPGV_Offset_TypeDef;


/*数据输出 对像定义*/
typedef struct
{
	uint8_t	PGV_Sta;		/*数据可靠性 0-不可靠 其他-可靠*/
	int16_t PGV_Tag;		/*融合后的TAG 一般就是前或左的PGV tag*/
	int16_t PGV_X;			/*AGV横向偏移 单位:0.1mm*/
	int16_t PGV_Y;			/*AGV正向偏移 单位:0.1mm*/
	//float	PGV_Z;			/*AGV Z轴偏移 单位:1°	*/
	int16_t PGV_Z_Projector;/*AGV z轴偏移的投影值，当z轴偏移为0时，此结果同样为0
								值为负的时候需要顺时针旋转，值为正的时候需要逆时针旋转 单位:0.1mm*/

}_MergeToPGV_OutPut_TypeDef;
/*数据零偏  对像定义*/
//typedef struct
//{
//	int16_t	Temp_X;			/*x偏移 单位:0.1mm*/
//	int16_t	Temp_Y;			/*y偏移 单位:0.1mm*/
//	int16_t	Temp_Z;			/*z偏移 投射*/
//}_MergeToPGV_Temp_TypeDef;
/*数据融合 对像定义*/
typedef struct MergeToPGV
{
	/*初始化输入*/
	MergeToPGVType_TypeDef		PGVFitType;			/*做数据融合的方式*/
	_MergeToPGV_Offset_TypeDef	PGVOffset;

	/*实时调用输入*/
	_MergeToPGV_BisHF_TypeDef	BisHFData;		/*航发双PGV用法*/

	
	//_MergeToPGV_Temp_TypeDef	PGVTemp;
	/*查询与控制输出*/
	_MergeToPGV_OutPut_TypeDef	PGVOut;

}_MergeToPGV_TypeDef;

DRV_MERGETOPGV_EXT void Drv_MergeToPGV_Main(_MergeToPGV_TypeDef *pPGVData);
#endif


/************************************END OF FILE************************************/
