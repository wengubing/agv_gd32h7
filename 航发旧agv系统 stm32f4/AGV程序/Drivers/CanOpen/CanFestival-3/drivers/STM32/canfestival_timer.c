
#include "includes.h"

#include "..\..\Drivers\CanOpen\CanFestival-3\example\LinkCanopenMaster.h"
#include "canfestival.h"
#include "timer.h"

/************************** Modul variables **********************************/
// Store the last timer value to calculate the elapsed time
static TIMEVAL last_time_set = TIMEVAL_MAX;//上一次的时间计数


/*2015-12-22：新增,by dengyang */
#define		CanFestivalTimer_CCRX_Val		1000						
#define		CanFestivalTimer_ARR_Val		TIMEVAL_MAX
#define		CanFestivalTimer_PSC_Val		80							// 80M/1000000 --1us  (定时器外设频率为80M)




/*用来定时的,时间到了就需要调用一下函数TimeDispatch()
value:Set time value 0x0000-0xffff
*/
void setTimer(TIMEVAL value)
{
	UNS32 capture = 0;
	
	capture = TIM_GetCapture1(CanFestivalTimer_Base);
	TIM_SetCompare1(CanFestivalTimer_Base, capture + value);

}

/*查询距离下一个定时触发还有多少时间
TIMEVAL:Return current timer value
*/
TIMEVAL getElapsedTime(void)
{
	UNS16 timer = TIM_GetCounter(CanFestivalTimer_Base);	//Copy the value of the running timer
	
	timer = timer >= last_time_set ? timer - last_time_set : last_time_set - timer;
	
	return timer;
}

/*canfestival定时器配置*/
void CanFestivalTimer_Config(TIM_TypeDef *Timx, UNS32 Rcc, UNS8 NvicIrq, INTEGER8 pi, FunctionalState NewState)
{
    NVIC_InitTypeDef					NVIC_IS;
  	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
	
	
    /*使能定时器外设时钟*/
		if(Timx==TIM1 || Timx==TIM8 || Timx==TIM9 || Timx==TIM10 || Timx==TIM11)
		{//AHB2
			RCC_APB2PeriphClockCmd( Rcc | RCC_APB2Periph_SYSCFG, ENABLE );
		}
		else
		{//AHB1
			RCC_APB1PeriphClockCmd( Rcc | RCC_APB2Periph_SYSCFG, ENABLE );
		}
		
		/* 时钟及分频设置 */
		TIM_DeInit( Timx );
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//计数模式:向上计数
		if( (Timx == TIM1) || (Timx == TIM8))
		{
			TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//重新计数的起始值
		}
    TIM_TimeBaseStructure.TIM_Prescaler = (CanFestivalTimer_PSC_Val - 1);//psc  ---影响计数频率
    TIM_TimeBaseStructure.TIM_Period = CanFestivalTimer_ARR_Val;//arr
    TIM_TimeBaseInit(Timx, &TIM_TimeBaseStructure);
		
//		TIM_ARRPreloadConfig(Timx, DISABLE);// Timx->ARR,禁止ARR自动重装载缓冲器
		
		{
			TIM_OCInitTypeDef  				TIM_OCInitStructure;
			/* Output Compare Timing Mode configuration: Channel1 */
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;//输出比较模式：timing
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_Pulse = CanFestivalTimer_CCRX_Val;//比较输出值
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
			TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
			TIM_OC1Init(Timx, &TIM_OCInitStructure);
			
			TIM_OC1PreloadConfig(Timx, TIM_OCPreload_Disable);//禁用预装载,写入到ARR中的值立即有效!!!
		}
		
    /*定时器中断向量配置*/ 
		if(pi>=0)
		{
			NVIC_IS.NVIC_IRQChannel = NvicIrq;
			NVIC_IS.NVIC_IRQChannelPreemptionPriority = pi;
			NVIC_IS.NVIC_IRQChannelSubPriority = 0;
			NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init( &NVIC_IS );
		}
		
		TIM_ClearFlag( Timx, CanFestivalTimer_FLAG );// 清除TIMx溢出中断标志
		TIM_ITConfig( Timx, CanFestivalTimer_IT, ENABLE );//TIM IT enable
		
		TIM_SetCounter(Timx, 0);// tim计数器清零
		TIM_Cmd(Timx, NewState);//TIM enable counter
}

void CanFestivalTimer_Enable(FunctionalState NewState)
{
	if(NewState)		TIM_Cmd(CanFestivalTimer_Base, ENABLE);
	else						TIM_Cmd(CanFestivalTimer_Base, DISABLE);
}


void CanFestivalTimer_Ctrl(FunctionalState NewState)
{
	TIM_ITConfig(CanFestivalTimer_Base, CanFestivalTimer_IT, NewState);
}

void CanFestivalTimer_Dispatch(void)
{
	last_time_set = TIM_GetCounter(CanFestivalTimer_Base);
	TimeDispatch();//TimeDispatch is called on each timer expiration
}

//===========================================
// 名称：CanFestivalTimer_IRQHandler
// 功能：CanFestival定时器调度中断服务程序
// 参数：无
// 返回：无
// 说明：无
//============================================================================
void CanFestivalTimer_IRQHandler(void)
{
	if (TIM_GetITStatus(CanFestivalTimer_Base, CanFestivalTimer_IT) != RESET)
	{
		TIM_ClearITPendingBit(CanFestivalTimer_Base, CanFestivalTimer_IT);

//		isr_sem_send (&sem_CanOpenDispatch);//传递调度任务信号事件,又RTOS去调度执行
		CanFestivalTimer_Dispatch();
	}
}


