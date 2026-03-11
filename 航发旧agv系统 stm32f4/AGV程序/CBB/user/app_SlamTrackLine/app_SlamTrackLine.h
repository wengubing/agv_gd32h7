/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_SlamTrackLine.h   
* Version    : V1.0		
* Author     : master		
* Date       : 2025.09.29
* Description: slam巡线功能支持，包含Slam遥控巡线 自动巡线 
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_SlamTrackLine_h__
#define __app_SlamTrackLine_h__

#ifdef  app_SlamTrackLinee_GLOBALS
 #define app_SlamTrackLinee_GLOBALS_EXT
#else
 #define app_SlamTrackLinee_GLOBALS_EXT  extern
#endif 

/*包含以下头文件*/
#include <stdbool.h>

#include <stm32f4xx.h>

#define MaxSlamTrackLine			16//最大的手动巡线轨迹

 
//工位区域，4个点 确定一个区域
typedef struct
{
	float UpperLeft_X;
	float UpperLeft_Y;
	float BottomRight_X;
	float BottomRight_Y;
}WorkstationArea_TypeDef;

//虚拟slam路线，2个点 确定一条循迹轨迹线
typedef struct
{
	float org_X;
	float org_Y;
	float des_X;
	float des_Y;
}TrackLine_TypeDef;

typedef struct
{
	//初始化参数
	WorkstationArea_TypeDef WorkstationArea	[MaxSlamTrackLine];
	TrackLine_TypeDef 		TrackLine		[MaxSlamTrackLine];
	//输入参数
	float 					Rtx;
	float 					Rty;
	//中间参数
	uint8_t 				WorkstationArea_Index;
	//输出参数
	uint8_t 				SlamTrackLine_STA;	//0-遥控slam轨迹生成正确 1-遥控slam轨迹生成生效
	TrackLine_TypeDef		Remote_SlamTrackLine;

}SlamTrackLine_TypeDef;

app_SlamTrackLinee_GLOBALS_EXT void app_Remote_SlamTrackLine(SlamTrackLine_TypeDef *p);
/*外部方法声明*/


#endif

/************************************END OF FILE************************************/

