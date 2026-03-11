/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: LinearCal.h
** 创 建 人: 文小兵  
** 描    述: 线性量处理
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __LinearCal_h__
#define __LinearCal_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  LinearCal_GLOBALS
    #define LinearCal_EXT
#else
    #define LinearCal_EXT  extern
#endif

//自定义引用	
#include "common_def.h"
#include <stdbool.h>


//宏定义
#define WheelAngleNum_MAX									12//32//最大角度数量

//对象外部声明


//方法外部声明

//角度传感器类型定义
typedef enum
{
	AngleType_Null = 0,									    //无效
	AngleType_Single,										//单圈
	AngleType_Multi,										//多圈
    AngleType_Slide,									    //电位器
}AngleType_EnumDef;

//角度对象定义
typedef struct
{
	f32 Org;									            //原始数据
    AngleType_EnumDef Type;                                 //角度类型
	f32 OrgSetA;                                            //原始标定数据A
    f32 OrgSetB;							                //原始标定数据B
	f32 SetA;                                               //标定数据A
    f32 SetB;									            //标定数据B
    f32 Prop;                                               //比例系数
	f32 SetRes;									            //标定结果

    f32 Offset;									            //零点偏移参数
    f32 OffsetRes;									        //零点偏移结果
    f32 OrgRes;									            //原始结果 没有经过零点偏移的，但是经过360转180的，因为单圈传感器原始为0~360，仅用于软件内标定
    
    f32 Res;                                                //最终结果 0.01°
}AngleCal_StDef;

//外部对象声明
LinearCal_EXT AngleCal_StDef g_WheelAngle[WheelAngleNum_MAX]; //车轮角度

//外部方法声明
LinearCal_EXT bool AngleCal(u8 Num);                        //角度计算

#endif

//***********************************END OF FILE***********************************


