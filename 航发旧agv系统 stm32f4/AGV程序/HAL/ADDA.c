/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ADDA.c
** 创 建 人: 文小兵  
** 描    述: ADDA对象实例化、初始化，整理汇总到此文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月17日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define ADDA_GLOBALS

//自定义引用			

#include "ADDA.h"
 
#include "ACC_Project.h"

//外部对象声明

//内部方法声明

/***********************************************************
** 函数名称:		ADDA_ADInit_IDfunCheck
** 功能描述: 	AD端口根据业务功能的逻辑检查
** 参    数:	None
** 返 回 值:		None
***********************************************************/
///*AD端口根据业务功能的逻辑检查，*/
__weak void ADDA_ADInit_IDfunCheck(void)
{

}
/***********************************************************
** 函数名称:		ADDA_DAInit_IDfunCheck
** 功能描述: 	DA端口根据业务功能的逻辑检查
** 参    数:	None
** 返 回 值:		None
***********************************************************/
///*AD端口根据业务功能的逻辑检查，*/
__weak void ADDA_DAInit_IDfunCheck(void)
{

}

//AD接口初始化
bool ADDA_ADInit(void)
{
	char i=0;
	ADDA_ADCN_StDef* p2ADCN = (ADDA_ADCN_StDef*)&g_ADCNs;
	ADDA_ADFuc_StDef* p2ADFuc = (ADDA_ADFuc_StDef*)&g_ADFucs;

	g_ADFucs.Number = sizeof(ADDA_ADFucs_StDef) / sizeof(ADDA_ADFuc_StDef);//获取AD接口组中 功能对象数量
	
	ADDA_ADInit_IDfunCheck();
	
	for (i = 0; i < g_ADFucs.Number; i++)
	{
		if(p2ADFuc[i].Id < ADCN_Number)//判断AD接口组中 功能对象ID是否合法
		{
			if(p2ADCN[p2ADFuc[i].Id].Inited == false)//判断AD接口组中 功能对象是否已被分配
			{
				//标定参数初始化
				if(p2ADFuc[i].RatioOrgA == p2ADFuc[i].RatioOrgB)
				{
					p2ADFuc[i].RatioOrgA = 0;
					p2ADFuc[i].RatioOrgB = 50000;
				}
				else	{}
				//标定参数初始化
				if(p2ADFuc[i].RatioActA == p2ADFuc[i].RatioActB)
				{
					p2ADFuc[i].RatioActA = 0;
					p2ADFuc[i].RatioActB = 50000;
				}
				else	{}
				//平滑参数初始化
				//DataSmooth_Init(&p2ADFuc[i].Smooth,0.1,0.3,20,0.5,1.5);

				//滤波参数初始化
				if(p2ADFuc[i].Fiter.in_FifoLength == 0)
				{
					p2ADFuc[i].Fiter.in_FifoLength=10;
					p2ADFuc[i].Fiter.in_FilterMode=DataFilter_Mode_Average;
				}
				p2ADFuc[i].p2ADCN = &p2ADCN[p2ADFuc[i].Id];
				p2ADCN[p2ADFuc[i].Id].Inited = true;
			}
			else
			{
				g_ADFucs.InitStt = ObjInit_Fail;
				g_CodeErr.Bit.InitErr = 1;//初始化错误
				return 0;
			}
		}
		else if(p2ADFuc[i].Id == ADCN_Disable)
		{
			p2ADFuc[i].RatioOrgA = 0;
			p2ADFuc[i].RatioOrgB = 50000;
			p2ADFuc[i].RatioActA = 0;
			p2ADFuc[i].RatioActB = 50000;
		}
		else
		{
			/*输入越界，应该需要返回初始化错误*/
			//g_ADFucs.InitStt = ObjInit_Fail;
			//g_CodeErr.Bit.InitErr = 1;//初始化错误
			//return 0;
		}
	}
	g_ADFucs.InitStt = ObjInit_OK;
	return 1;
}

//DA接口初始化
bool ADDA_DAInit(void)
{
	char i=0;
	ADDA_DACN_StDef* p2DACN = (ADDA_DACN_StDef*)&g_DACNs;
	ADDA_DAFuc_StDef* p2DAFuc = (ADDA_DAFuc_StDef*)&g_DAFucs;

	g_DAFucs.Number = sizeof(ADDA_DAFucs_StDef) / sizeof(ADDA_DAFuc_StDef);//获取DA接口组中 功能对象数量

	ADDA_DAInit_IDfunCheck();

	for (i = 0; i < g_DAFucs.Number; i++)
	{
		if(p2DAFuc[i].Id < ADCN_Number)//判断DA接口组中 功能对象ID是否合法
		{
			if(p2DACN[p2DAFuc[i].Id].Inited == false)//判断DA口组中 功能对象是否已被分配
			{
				//标定参数 默认值初始化
				if(p2DAFuc[i].RatioOrgA == p2DAFuc[i].RatioOrgB)
				{
					p2DAFuc[i].RatioOrgA = 0;
					p2DAFuc[i].RatioOrgB = 50000;
				}
				if(p2DAFuc[i].RatioActA == p2DAFuc[i].RatioActB)
				{
					p2DAFuc[i].RatioActA = 0;
					p2DAFuc[i].RatioActB = 50000;
				}
				p2DAFuc[i].p2DACN = &p2DACN[p2DAFuc[i].Id];
				p2DACN[p2DAFuc[i].Id].Inited = true;
			}
			else
			{
				g_DAFucs.InitStt = ObjInit_Fail;
				g_CodeErr.Bit.InitErr = 1;//初始化错误
				return 0;
			}
		}
		else if(p2DAFuc[i].Id == DACN_Disable)
		{
			p2DAFuc[i].RatioOrgA = 0;
			p2DAFuc[i].RatioOrgB = 50000;
			p2DAFuc[i].RatioActA = 0;
			p2DAFuc[i].RatioActB = 50000;
		}
		else
		{
			/*输入越界，应该需要返回初始化错误*/
			//g_ADFucs.InitStt = ObjInit_Fail;
			//g_CodeErr.Bit.InitErr = 1;//初始化错误
			//return 0;
		}
	}

	g_DAFucs.InitStt = ObjInit_OK;
	return 1;
}

//ADDA 初始化
bool ADDA_Init(void)
{
	if(ADDA_ADInit() && ADDA_DAInit())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//AD 更新
bool ADUpdate(void)
{
	char i = 0;
	//ADDA_ADCN_StDef* p2ADCN = (ADDA_ADCN_StDef*)&g_ADCNs;
	ADDA_ADFuc_StDef* p2ADFuc = (ADDA_ADFuc_StDef*)&g_ADFucs;

	for (i = 0; i < g_ADFucs.Number; i++)
	{
		//计算更新
		if(p2ADFuc[i].RatioOrgA != p2ADFuc[i].RatioOrgB)//判断是否会造成除零错误
		{
			if(p2ADFuc[i].Id != ADCN_Disable)
			{
				p2ADFuc[i].ActVal = RatioStand(p2ADFuc[i].p2ADCN->mV,p2ADFuc[i].RatioOrgA,p2ADFuc[i].RatioOrgB,
					p2ADFuc[i].RatioActA,p2ADFuc[i].RatioActB);

				//数据滤波
				p2ADFuc[i].Fiter.in_RealTimeValue=p2ADFuc[i].ActVal;
				p2ADFuc[i].Fiter.out_FilterValue = DataFilter_Main(&p2ADFuc[i].Fiter);
			}
			else{}/*预留的无需操作*/
		}
		else
		{
			g_CodeErr.Bit.ZeroDenominator=1;//除零错误
			return 0;
		}
	}

	return 1;
}

//DA 更新
bool DAUpdate(void)
{
	char i = 0;
	//ADDA_DACN_StDef* p2DACN = (ADDA_DACN_StDef*)&g_DACNs;
	ADDA_DAFuc_StDef* p2DAFuc = (ADDA_DAFuc_StDef*)&g_DAFucs;

	for(i = 0; i < g_DAFucs.Number; i++)
	{
		//计算更新
		if(p2DAFuc[i].RatioOrgA != p2DAFuc[i].RatioOrgB)//判断是否会造成除零错误
		{
			if(p2DAFuc[i].Id != DACN_Disable)
			{
				p2DAFuc[i].p2DACN->mV = RatioStand(p2DAFuc[i].OrgCmd,p2DAFuc[i].RatioOrgA,p2DAFuc[i].RatioOrgB,
					p2DAFuc[i].RatioActA,p2DAFuc[i].RatioActB);

				p2DAFuc[i].p2DACN->DigVal = p2DAFuc[i].p2DACN->mV;
			}
		}
		else
		{
			g_CodeErr.Bit.ZeroDenominator=1;//除零错误
			return 0;
		}
	}

	return 1;
}

//ADDA 更新
bool ADDA_Update(void)
{
	char ErrCnt = 0;

	//AD硬件抽象层 更新到 功能组
	if (g_Para.Debug.ADOff == 0)
	{
		if (ADUpdate() == 0)
			ErrCnt++;
		else {}
	}
	else {}

	//DA
	if (g_Para.Debug.DAOff == 0)
	{
		if (DAUpdate() == 0)
			ErrCnt++;
		else {}
	}
	else {}

	
	if(ErrCnt)
		return 0; //更新失败
	else
		return 1; //更新成功
}

//***********************************END OF FILE***********************************

