/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_IOFlashCtrl.h  
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.1         
* Description: IO不同闪烁方式实现
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_IOFlashCtrl_h__
#define __app_IOFlashCtrl_h__

#include <stdbool.h>
#include "common_def.h"

/**/ 
typedef enum
{
	IOFlashCtrlMode_Off=0,									//关闭
	IOFlashCtrlMode_On,										//打开
	IOFlashCtrlMode_flash,									//闪烁
	IOFlashCtrlMode_FlashLimit,								//有限次闪烁
	IOFlashCtrlMode_FlashLoop,								//循环闪烁
}IOFlashCtrlMode_TypeDef;


typedef struct
{
	u32 in_Ms;
	u32 CtrlMsRecord;										//
	IOFlashCtrlMode_TypeDef in_Mode,ModePre;				//工作模式
	u16 t_on;												//
	u16 t_off;												//
	u16 t_loop;												//循环闪烁灭灯间隔
	u16 n;													//闪烁次数
	u16 n_record;											//已闪烁测试
	bool out_Ctrl;											//控制结果
	bool out_CtrlPre;										//上一状态
}IOFlashCtrl_TypeDef;


/*外部方法声明*/
bool IOFlashCtrl_Main(IOFlashCtrl_TypeDef *p, u32 ms, u16 t_on, u16 t_off, u16 t_loop, u16 n, IOFlashCtrlMode_TypeDef mode);


#endif

