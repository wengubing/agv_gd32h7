
#ifndef _DRV_PID_H
#define _DRV_PID_H


#ifdef  DRV_PID_EXT_GLOBALS
 #define DRV_PID_EXT
#else
 #define DRV_PID_EXT  extern
#endif






/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
 
//pid数据结构
typedef struct PID
{
	float SetPoint, //设定目标 Desired Value
		    NextPoint;//输入值
	float  SumError; //误差累计
	float Proportion, //比例常数 Proportional Const
				Integral,   //积分常数 Integral Const
				Derivative; //微分常数 Derivative Const
	float LastError; //Error[-1]
	float PrevError; //Error[-2]
	float OutVale;//输出值
}PID_Type;


/* Exported functions ------------------------------------------------------- */
DRV_PID_EXT void  PIDInit(PID_Type *sptr);
DRV_PID_EXT float IncPIDCalc(PID_Type *sptr);
DRV_PID_EXT float LocPIDCalc(PID_Type *sptr);

#endif

/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/

