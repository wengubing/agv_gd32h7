/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_PairPusher.h   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2025.5.17         
* Description: 联动推送控制模块
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_PairPusher_h__
#define __app_PairPusher_h__

/*包含以下头文件*/
#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"

//联动通信IO信号
typedef struct
{
	u16 Connect : 1;											//通信握手位
	u16 Control : 1;											//控制使能位
	u16 Fault : 1;												//故障停止位
	u16 Push : 1;												//推出使能位
	u16 Pull : 1;												//拉回使能位
	u16 SpeedGearBit_Low : 1;									//速度档位 低位
	u16 SpeedGearBit_High : 1;									//速度档位 高位
	u16 NC : 1;													//保留位
}PpCommIO_TypeDef;

//输入
typedef struct
{
	//选择联动
	bool PairSel;												//选择联动
    bool ModeReady;												//模式准备好
    PpCommIO_TypeDef CommIO;									//通信IO信号
    u8 SpeedGear;												//速度档位 1~3
	bool Push;													//推出使能命令
	bool Pull;													//拉回使能命令
    bool Limited;												//限位信号
    bool FaultStop;												//故障停止信号
}PpInput_TypeDef;

//输出
typedef struct
{
    PpCommIO_TypeDef CommIO;									//通信IO信号
    u8 SpeedGear;												//速度档位 1~3
	bool Push;													//推出使能命令
	bool Pull;													//拉回使能命令
}PpOutput_TypeDef;

typedef struct
{
	PpInput_TypeDef Input;										//输入
	PpOutput_TypeDef Output;									//输出
}PairPusher_TypeDef;


/*外部方法声明*/
bool PairPusher_Main(PairPusher_TypeDef *p);

#endif

/************************************END OF FILE************************************/

