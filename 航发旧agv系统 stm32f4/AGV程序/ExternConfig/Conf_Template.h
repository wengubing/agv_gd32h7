/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Conf_Template.h
** 创 建 人: Master 
** 描    述: 项目配置文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.1
** 创建日期: 
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __Conf_Template_h__
#define __Conf_Template_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  Conf_Template_GLOBALS
    #define Conf_Template_EXT
#else
    #define Conf_Template_EXT  extern
#endif

//自定义引用
#include <stdbool.h>
#include "app_MotorResolving.h"
//#include "Fun_Template.h"

//项目自定义对象 注意该.h中的定义仅限在项目配置文件中使用

/*差速轮轮系属性*/
typedef enum
{
    Conf_ModuleType_Null = 0,
    Conf_ModuleType_General,						        //一般：+45 至 -95；顺时针旋转90°
    Conf_ModuleType_Special,							    //特殊：+95 至 -45；逆时针旋转90°
    Conf_ModuleType_End = 65535,							//u16
}Conf_ModuleType_TypeDef_Template;

typedef struct
{
    Conf_ModuleType_TypeDef_Template ModuleType[MR_ModuleNumMxa];    //轮系类型
}Conf_TypeDef_Template;

//对象实例化
Conf_Template_EXT Conf_TypeDef_Template g_Conf_Template;

//方法外部声明
Conf_Template_EXT bool Para_Template(void);                 // Template设备参数初始化（通过代码设定）

#endif

//***********************************END OF FILE***********************************


