
#define   DRV_PID_EXT_GLOBALS


/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "drv_pid.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

//PID 参数初始化
void PIDInit(PID_Type *sptr)
{
	sptr->Proportion = 0; //比例常数 Proportional Const
	sptr->Integral = 0; //积分常数 Integral Const
	sptr->Derivative = 0; //微分常数 Derivative Const	
	sptr->SumError = 0;
	sptr->LastError = 0; //Error[-1]
	sptr->PrevError = 0; //Error[-2]
	sptr->SetPoint = 0;
	sptr->OutVale = 0;
}

//增量式 PID 控制设计
float IncPIDCalc(PID_Type *sptr)
{
	float iError, iIncpid;
	
	//当前误差
	iError = sptr->SetPoint - sptr->NextPoint;
	//增量计算
	iIncpid = sptr->Proportion * iError //E[k]项
					- sptr->Integral * sptr->LastError //E[k－1]项
					+ sptr->Derivative * sptr->PrevError; //E[k－2]项
	//存储误差，用于下次计算
	sptr->PrevError = sptr->LastError;
	sptr->LastError = iError;
	
	//返回增量值
	return(iIncpid);
}

//位置式 PID 控制设计
float LocPIDCalc(PID_Type *sptr)
{
	float iError,dError,iLocpid;
	
	iError = sptr->SetPoint - sptr->NextPoint; //偏差
	sptr->SumError += iError; //积分
	dError = iError - sptr->LastError; //微分
	sptr->LastError = iError;
	iLocpid = sptr->Proportion * iError //比例项
					+ sptr->Integral * sptr->SumError //积分项
					+ sptr->Derivative * dError; //微分项
	
	//返回位置值
	return(iLocpid);
}

/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/
