/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ADDA.h
** 创 建 人: 文小兵  
** 描    述: ADDA对象实例化、初始化，整理汇总到此文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月17日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __ADDA_h__
#define __ADDA_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  ADDA_GLOBALS
    #define ADDA_EXT
#else
    #define ADDA_EXT  extern
#endif

//自定义引用	

#include "app_DataFilter.h"

//----------------------------------AD相关定义----------------------------------------------
//AD接口 数据对象定义
typedef struct
{
	u16 OrgVal;                                             //原始值
    u16 mV;                                                 //电压值 mV
    bool Inited;                                            //已被分配
}ADDA_ADCN_StDef;

//AD接口组 数据对象定义
typedef struct
{
	ADDA_ADCN_StDef CN1;//通道1
    ADDA_ADCN_StDef CN2;//通道2
    ADDA_ADCN_StDef CN9;//通道9
    ADDA_ADCN_StDef CN5;//通道5
    ADDA_ADCN_StDef CNREF;//
}ADDA_ADCNs_StDef;

/*AD接口 位置定义*/
typedef enum
{
    ADCN1=0,
    ADCN2=1,
    ADCN9=2,
    ADCN5=3,
    ADREF=4,
    ADCN_Number=5,
	ADCN_Disable=127,
}ADDA_ADCN_EnumDef;

/*对象初始化状态定义*/
typedef enum
{
    ObjInit_Null=0,//未初始化
    ObjInit_OK,//初始化成功
    ObjInit_Fail,//初始化失败
}ObjInitStt_EnumDef;

//DA接口功能 数据对象定义
typedef struct
{
    ADDA_ADCN_EnumDef Id;//通道ID
    ADDA_ADCN_StDef* p2ADCN;//AD接口 指针
    f32 RatioOrgA,RatioOrgB;//标定参数 原始值
    f32 RatioActA,RatioActB;//标定参数 实际值
    f32 ActVal;//标定结果
    //DataSmoothing_TypeDef Smooth;//数据平滑
    DataFilter_TypeDef Fiter;//数据滤波
}ADDA_ADFuc_StDef;

//DA接口功能组 数据对象定义
typedef struct
{
    ADDA_ADFuc_StDef SysCtrlVoltage;        //系统控制电压
	ADDA_ADFuc_StDef FL_Pressure;			//前左压力
	ADDA_ADFuc_StDef FR_Pressure;			//前右压力
	ADDA_ADFuc_StDef B_Pressure;			//后侧压力
	ADDA_ADFuc_StDef Lift_Cable;			//举升拉线
	ADDA_ADFuc_StDef Lift_Cable1;			//举升拉线1 当平台拉线
	ADDA_ADFuc_StDef Lead_Screw_Wire;		//丝杠拉线
	ADDA_ADFuc_StDef Leg_Pressure;			//支腿压力

    u8 Number;//功能数量
    ObjInitStt_EnumDef InitStt;//初始化状态
}ADDA_ADFucs_StDef;

//----------------------------------DA相关定义----------------------------------------------
//DA接口 数据对象定义
typedef struct
{
	u16 mV;//电压命令 mV
    u16 DigVal;//数字量命令
    bool Inited;//已被分配
}ADDA_DACN_StDef;

//DA接口组 数据对象定义
typedef struct
{
	ADDA_DACN_StDef CN11;
    ADDA_DACN_StDef CN7;
    ADDA_DACN_StDef CN4;
    ADDA_DACN_StDef CN3;
    ADDA_DACN_StDef CN10;
    ADDA_DACN_StDef CN6;
    ADDA_DACN_StDef CN12;
    ADDA_DACN_StDef CN8;
}ADDA_DACNs_StDef;

/*DA接口 位置定义*/
typedef enum
{
    DACN11		= 0,
    DACN7		= 1,
    DACN4		= 2,
    DACN3		= 3,
    DACN10		= 4,
    DACN6		= 5,
    DACN12		= 6,
    DACN8		= 7,
	DACN_Disable=127,
    DACN_Number,                                            //DA接口数量，同时被配置为该值等同功能接口悬空
}ADDA_DACN_EnumDef;

//DA接口功能 数据对象定义
typedef struct
{
    ADDA_DACN_EnumDef Id;                                   //通道ID
    ADDA_DACN_StDef* p2DACN;                                //接口指针
    f32 RatioOrgA,RatioOrgB;                                //标定参数 原始值
    f32 RatioActA,RatioActB;                                //标定参数 实际值
    f32 OrgCmd;                                             //原始命令
}ADDA_DAFuc_StDef;

//DA接口功能组 数据对象定义
typedef struct
{
    //ADDA_DAFuc_StDef Fuc1;                                //功能1
    ADDA_DAFuc_StDef KoRunCtrl;                             //科润控制
	ADDA_DAFuc_StDef KoRunCtrl_2;                           //第二个科润
    u8 Number;                                              //功能数量
    ObjInitStt_EnumDef InitStt;                             //初始化状态
}ADDA_DAFucs_StDef;

//对象外部声明
ADDA_EXT ADDA_ADCNs_StDef g_ADCNs;                          //AD接口组
ADDA_EXT ADDA_ADFucs_StDef g_ADFucs;                        //AD功能组
ADDA_EXT ADDA_DACNs_StDef g_DACNs;                          //DA接口组
ADDA_EXT ADDA_DAFucs_StDef g_DAFucs;                        //DA功能组

//方法外部声明
ADDA_EXT bool ADDA_Init(void);                              //ADDA初始化
ADDA_EXT bool ADDA_Update(void);                            //ADDA更新

#endif

//***********************************END OF FILE***********************************


