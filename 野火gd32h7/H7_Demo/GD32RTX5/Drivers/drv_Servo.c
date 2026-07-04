/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_Servo.c
** 创 建 人: 文小兵
** 描    述: 伺服抽象定义
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0
** 创建日期: 2024年5月9日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define drv_Servo_GLOBALS

//自定义引用		
#include "drv_Servo.h"
//#include "ACC_Project.h"
#include "..\..\CBB\user\basecalfunc\basecalfunc.h"
#include "..\..\CBB\user\basecalfunc\common_def.h"


//外部对象声明

//内部方法声明

//内部对象声明




//伺服使能控制
bool SVO_EnableOperation(u32 InMs)
{
    u8 i=0;
    for(i=0;i<SVO_MAXNUM;i++)
    {
        if(SVO[i].En != SVO[i].EnLast)
        {
            SVO[i].EnLast = SVO[i].En;
            SVO[i].CMDCtrl = 0;
        }

        if(SVO[i].En && SVO[i].CMDCtrl!=15)
        {//使能
			//需要依据驱动器类型 用不同的指令顺序进行使能
			switch(SVO[i].SvoType)
			{
				/*Copley驱动器，按照 F指令顺序使能*/
				case SvoType_Copley:
				/*科亚驱动器，按照 F指令顺序使能*/
				case SvoType_Keya:
				{
					if(SVO[i].CMDCtrl == 0)
					{
						SVO[i].CMDCtrl = 15;
					}
				}break;
				/*步科驱动器，按照 6 F指令顺序使能*/
				case SvoType_Kinco:
				case SvoType_Kinco_5:
				default:	
				{
					if(SVO[i].CMDCtrl == 0)
					{
						SVO[i].CMDCtrl = 6;
						SVO[i].MsRecord = InMs;
					}
					else if(SVO[i].CMDCtrl == 6)
					{
						if(InMs - SVO[i].MsRecord > 1000)
							SVO[i].CMDCtrl = 15;
					}
				}break;
				/*台达、风得控、惠斯通驱动器，按照 6 7 F指令顺序使能*/
				case SvoType_Delta:
				case SvoType_Fdk:
				case SvoType_Hst:
				{
					if(SVO[i].CMDCtrl == 0)
					{
						SVO[i].CMDCtrl = 6;
						SVO[i].MsRecord = InMs;
					}
					else if(SVO[i].CMDCtrl == 6)
					{						 
						if(InMs - SVO[i].MsRecord > 1000)
						{
							SVO[i].CMDCtrl = 7;
						}
					}
					else if(SVO[i].CMDCtrl == 7)
					{
						if(InMs - SVO[i].MsRecord > 2000)
							SVO[i].CMDCtrl = 15;
					}
				}break;
			}
        }
    }
    return true;
}

//伺服使能控制
bool SVOs_Analysis(void)
{
    u8 i=0;
	f32 tempdata=0;
	Extreme_StDef *p = &SVOs.LoadCnt;

    SVOs.Temp.Max = 0;
    SVOs.Temp.Min = 0;
    SVOs.Temp.MaxID = 0;
    SVOs.Temp.MinID = 0;

	//处理负载计数 历史记录
	p = &SVOs.LoadCnt;
	
    for(i=0;i<SVO_MAXNUM;i++)
    {
        if(SVO[i].Temperature < SVOs.Temp.Min)
        {
            SVOs.Temp.Min = SVO[i].Temperature;
            SVOs.Temp.MinID = i;
        }

        if(SVO[i].Temperature > SVOs.Temp.Max)
        {
            SVOs.Temp.Max = SVO[i].Temperature;
            SVOs.Temp.MaxID = i;
        }

		
		tempdata = SVO[i].LoadCnt;
		if(tempdata < p->Min)
		{
			p->Min = tempdata;
			p->MinID = i;
		}
		else {}

		if(tempdata > p->Max)
		{
			p->Max = tempdata;
			p->MaxID = i;
		}
		else {}
		
    }

    return 1;
}

//***********************************END OF FILE***********************************

