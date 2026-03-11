/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: LinearCal.c
** 创 建 人: 文小兵  
** 描    述: 线性量处理
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define LinearCal_GLOBALS

//自定义引用			
#include "basecalfunc.h"
#include "LinearCal.h"

//外部对象声明

//内部方法声明

//[0,360)转换到[-180,180)，只处理一次
f32 AngleCal_360To180(f32 In)
{
    if(In >= 180)
        return In - 360;
    //2025年10月13日:master 489项目验证， 需要单圈角度传感器存在-180°的情况
    else if(In < -180)
        return In + 360;
    else
        return In;
}

//角度计算函数
bool AngleCal(u8 Num)
{
    char i=0;

    if(Num>WheelAngleNum_MAX)
        return false;
    
    for(i=0;i<Num;i++)
    {
        //线性标定
        if(g_WheelAngle[i].Type == AngleType_Single || g_WheelAngle[i].Type == AngleType_Multi)
            g_WheelAngle[i].SetRes = g_WheelAngle[i].Org * g_WheelAngle[i].Prop;
        else if(g_WheelAngle[i].Type == AngleType_Slide)
            g_WheelAngle[i].SetRes = RatioStand(g_WheelAngle[i].Org,g_WheelAngle[i].OrgSetA,g_WheelAngle[i].OrgSetB,g_WheelAngle[i].SetA,g_WheelAngle[i].SetB); 

        //零点偏移 放在单圈转换之前，避免最终结果出现超过±180°的情况
        g_WheelAngle[i].OffsetRes = g_WheelAngle[i].SetRes + g_WheelAngle[i].Offset;

        //[0,360)转换到[-180,180)
        if(g_WheelAngle[i].Type == AngleType_Single)
        {
            g_WheelAngle[i].Res = AngleCal_360To180(g_WheelAngle[i].OffsetRes);
            g_WheelAngle[i].OrgRes = AngleCal_360To180(g_WheelAngle[i].SetRes);//原始结果 没有经过零点偏移的，仅用于软件内标定
        }
        else
        {
            g_WheelAngle[i].Res = g_WheelAngle[i].OffsetRes;
            g_WheelAngle[i].OrgRes = g_WheelAngle[i].SetRes;//原始结果 没有经过零点偏移的，仅用于软件内标定
        }
    }
    return true;
}


//***********************************END OF FILE***********************************

