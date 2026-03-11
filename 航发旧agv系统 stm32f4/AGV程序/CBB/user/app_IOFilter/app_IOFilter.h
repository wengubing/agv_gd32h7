/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_IOFilter.h   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.2.1        
* Description: 输入实时IO值，输出滤波值
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_IOFilter_h__
#define __app_IOFilter_h__

/*包含以下头文件*/
#include <stdbool.h>

//MDK、VS从当前文件所在目录计算位置
#include "..\..\..\CBB\user\basecalfunc\common_def.h"

/*按钮操作状态类型定义*/
typedef enum
{
    IOFilterButtonStatus_None=0,       						/*按钮无操作(上电默认值)*/
    IOFilterButtonStatus_ReleasePowerUp,   					/*上电前一直处于释放状态*/
    IOFilterButtonStatus_Release,      						/*释放有效*/
	/*具体时间由参数决定*/
    IOFilterButtonStatus_PressShortDown,   					/*短按 0.1s有效*/
    IOFilterButtonStatus_PressMidDown,     					/*短按 0.5s有效*/
    IOFilterButtonStatus_PressNormalDown,  					/*中按 1s有效*/
    IOFilterButtonStatus_PressShortLongDown,				/*短长按 2s有效*/
    IOFilterButtonStatus_PressLongDown,   					/*长按 3s有效*/
}IOFilterButtonStatus_TypeDef;

typedef struct
{
	unsigned short PressShortDown;   						/*短按*/
	unsigned short PressMidDown;     						/*中按*/
	unsigned short PressNormalDown;  						/*更长中按*/
	unsigned short PressShortLongDown;						/*中长按*/
	unsigned short PressLongDown;   						/*长按 此参数需要比其它参数都长 以避免短按无法判断*/
}IOFilterPressTime_TypeDef;

typedef struct
{
	/*初始化*/
	IOFilterPressTime_TypeDef PressTimePara;

	/*调用输入*/
	bool in_RealTimeStatus;									/*实时数据输入*/
	u32 in_Ms;												/*ms输入*/

	/*内部成员*/
	u32 ButtonPressCnt;										/**/
	u32 ButtonReleaseCnt;
	

	/*输出*/
	IOFilterButtonStatus_TypeDef out_Status;				/*按钮状态值*/
		
}IOFilter_TypeDef;



/*外部方法声明*/
#ifdef __cplusplus
extern "C" {
#endif

	void IOFilter_ButtonScan(IOFilter_TypeDef* p, u32 in_Ms);
	bool IOFilter_InitExample(IOFilter_TypeDef* p);
	void CppCallC(void);

#ifdef __cplusplus
}
#endif

#endif

/************************************END OF FILE************************************/

