/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: BaseApi.h
** 创 建 人: 文小兵
** 描    述: 全局基础api
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2024年8月26日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __BaseApi_h__
#define __BaseApi_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  BaseApi_GLOBALS
    #define BaseApi_EXT
#else
    #define BaseApi_EXT  extern
#endif

//自定义引用
#include <stdbool.h>
#include "AgvChassisController.h"



//对象外部声明
//ACC_INIT_EXT ACC_DATA_TypeDef AccData;


//方法外部声明
BaseApi_EXT bool CheckWalk(ACC_DATA_TypeDef* p);		    //行走检查
BaseApi_EXT bool CheckMotion(ACC_DATA_TypeDef* p);		    //动作检查
BaseApi_EXT bool CheckMotionArrive(ACC_DATA_TypeDef* p);	//动作到位检查
BaseApi_EXT SLN_ObsState_TypeDef GetObsState(void);		    //获取障碍物状态
BaseApi_EXT bool ACC_IsOnPoint(void);					    //是否在点上
BaseApi_EXT	bool Switch_AsbSelect_ResetCnt(void);
#endif

//***********************************END OF FILE***********************************


