
#define TIMER_GLOBALS

#include "includes.h"
#include "..\..\Drivers\inc\drv_timer.h"



TIM_TypeDef  *ENCODER_TIMER_BASE[ENCODERn]	= { ENCODER1_TIMER_BASE, ENCODER2_TIMER_BASE };
GPIO_TypeDef *ENCODER_GPIO_PORT[ENCODERn]		= { ENCODER1_GPIO_PORT, ENCODER2_GPIO_PORT };
//const u8   ENCODER_RCC_CMD[ENCODERn] = { ENCODER1_RCC_CMD, ENCODER2_RCC_CMD };
const u32  ENCODER_TIMER_CLK[ENCODERn]			= { ENCODER1_TIMER_CLK, ENCODER2_TIMER_CLK };
const u32  ENCODER_GPIO_CLK[ENCODERn]				= { ENCODER1_GPIO_CLK, ENCODER2_GPIO_CLK };
const u32  ENCODERA_GPIO_PIN[ENCODERn]			= { ENCODER1A_GPIO_PIN, ENCODER2A_GPIO_PIN };
const u32  ENCODERB_GPIO_PIN[ENCODERn]			= { ENCODER1B_GPIO_PIN, ENCODER2B_GPIO_PIN };
const s8   ENCODER_IRQn[ENCODERn]						= { ENCODER1_IRQn, ENCODER2_IRQn };
const u8   ENCODERA_PIN_SOURCE[ENCODERn]		= { ENCODER1A_PIN_SOURCE, ENCODER2A_PIN_SOURCE };
const u8   ENCODERB_PIN_SOURCE[ENCODERn]		= { ENCODER1B_PIN_SOURCE, ENCODER2B_PIN_SOURCE };
const u8   ENCODER_GPIO_AF_TIM[ENCODERn]		= { ENCODER1_GPIO_AF_TIM, ENCODER2_GPIO_AF_TIM };


/****装载SysTick->LOAD的值,进SysTick_Handler的时间:SysTick_reload_val/AHB_clk(160MHZ)****/
#define		SysTick_reload_val					8400		// 50us


#define		TMR_COUNT										4				//软件定时器的个数，第1个保留给bsp_DelayMS()使用

SOFT_TMR		g_Tmr[TMR_COUNT];


		
//=============================================================================
// 名称：Delay_1us
// 功能：延时函数_us
// 参数：None
// 返回：None
// 说明：
//=============================================================================     
void Delay_1us(vu32 Nus)
{ 
	u16 i=0, j=0;
		
	if(Nus)
	{
		if(SystemCoreClock==72000000)
		{
			j=5;
		}
		else if(SystemCoreClock==160000000)
		{
			j=22;
		}		
		else if(SystemCoreClock==168000000)
		{
			j=35;
		}
		while(Nus --)
		{
			i = j;
			while(i --) ;    
		}	
	}
}

/***************************************************************************************
** 函数名称: Encoder_Config
** 功能描述: 配置单路编码器通道
** 参    数: 	
							Ex:编码器通道编号
							EncodePulseNumber:编码器输出的脉冲数
** 返 回 值: None       
****************************************************************************************/
void Encoder_Config(EncoderID Ex, u32 EncodePulseNumber)
{
    GPIO_InitTypeDef 					GPIO_IS;
    TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    TIM_ICInitTypeDef 				TIM_IS;

    // 使能定时器外设时钟
		if(	 ENCODER_TIMER_BASE[Ex]==TIM1 
			|| ENCODER_TIMER_BASE[Ex]==TIM8 
			|| ENCODER_TIMER_BASE[Ex]==TIM9 
			|| ENCODER_TIMER_BASE[Ex]==TIM10
			|| ENCODER_TIMER_BASE[Ex]==TIM11
		  )
		{//AHB2
			RCC_APB2PeriphClockCmd( ENCODER_TIMER_CLK[Ex], ENABLE );
		}
		else
		{//AHB1
			RCC_APB1PeriphClockCmd( ENCODER_TIMER_CLK[Ex], ENABLE );
		}

    // 使能端口时钟
    RCC_AHB1PeriphClockCmd( ENCODER_GPIO_CLK[Ex], ENABLE );

    // GPIO参数设置为默认值
    GPIO_StructInit( &GPIO_IS );

    // 口线配置为输入
    GPIO_IS.GPIO_Pin = ENCODERA_GPIO_PIN[Ex] | ENCODERB_GPIO_PIN[Ex]; // | ENCODERZ_GPIO_PIN[Ex];
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_IS.GPIO_Mode = GPIO_Mode_AF;
    GPIO_IS.GPIO_OType = GPIO_OType_OD;
    GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init( ENCODER_GPIO_PORT[Ex], &GPIO_IS );

    // 引脚连接到定时器
    GPIO_PinAFConfig( ENCODER_GPIO_PORT[Ex], ENCODERA_PIN_SOURCE[Ex], ENCODER_GPIO_AF_TIM[Ex] );
    GPIO_PinAFConfig( ENCODER_GPIO_PORT[Ex], ENCODERB_PIN_SOURCE[Ex], ENCODER_GPIO_AF_TIM[Ex] );

    // 配置定时器为编码器模式
    TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );

    TIM_TimeBaseStructure.TIM_Prescaler = 0;  // 不分频
    TIM_TimeBaseStructure.TIM_Period = ENCODER_ARR_16U_FULL; //内部做硬件4倍频处理(加载最大装载值)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( ENCODER_TIMER_BASE[Ex], &TIM_TimeBaseStructure );

    TIM_EncoderInterfaceConfig( ENCODER_TIMER_BASE[Ex], TIM_EncoderMode_TI12,
                                TIM_ICPolarity_Rising, TIM_ICPolarity_Rising );
    TIM_ICStructInit( &TIM_IS );

    TIM_IS.TIM_ICFilter = 0;
    TIM_ICInit( ENCODER_TIMER_BASE[Ex], &TIM_IS );

    // 清除挂起的中断
    TIM_ClearFlag( ENCODER_TIMER_BASE[Ex], TIM_FLAG_Update );
    TIM_ITConfig( ENCODER_TIMER_BASE[Ex], TIM_IT_Update, ENABLE );

    // 复位计数器值
    ENCODER_TIMER_BASE[Ex]->CNT = ENCODER_ARR_INI_DATA;// 初始化为中间值，使计数值可以有效加减

    // 使能定时器
    TIM_Cmd( ENCODER_TIMER_BASE[Ex], ENABLE );

}

/***************************************************************************************
** 函数名称: Encoder_Init
** 功能描述: 初始化板载所有编码器检测接口
** 参    数: 	
							EncoderLines:编码器输出的脉冲数(即线数)
** 返 回 值: None       
****************************************************************************************/
void Encoder_Init(u32 EncoderLines)
{
	u8 i;

	for (i=0; i<ENCODERn; i++)
	{
		Encoder_Config( (EncoderID )i, EncoderLines);
	}
}

/***************************************************************************************
** 函数名称: TIMx_OC_Config
** 功能描述: 定时器OC相关配置
** 参    数: 	tim	/ 定时器的寄存器组指针
							tim_OCx		/ tim的OC输出通道
							Pulse		/ 设置脉宽值,unit:us
** 返 回 值: None       
****************************************************************************************/
void TIMx_OC_Config(TIM_TypeDef* Timx, u8 tim_OCx, u32 Pulse, FunctionalState NewState)
{
	TIM_OCInitTypeDef 	TIM_OCIS;
//	TIM_BDTRInitTypeDef TIM_BDTRIS;

	/*根据TIM_OCInitStruct中指定的参数初始化外设TIMx,TIMx_OCx模块设置（设置1通道占空比）*/ 
	TIM_OCIS.TIM_OCMode = TIM_OCMode_PWM1;//PWM1为正常占空比模式,PWM2为反极性模式
	TIM_OCIS.TIM_OutputState = TIM_OutputState_Enable;// 使能TIMx_CHx通道
	TIM_OCIS.TIM_OCPolarity = TIM_OCPolarity_High;//High为占空比高极性,此时占空比为20%；Low则为反极性，占空比为80%
	TIM_OCIS.TIM_Pulse = Pulse;	// 待装入TIMX->CCRX,启用CCR1寄存器的影子寄存器（直到产生更新事件才更改设置）	
	TIM_OCIS.TIM_OCIdleState = TIM_OCIdleState_Reset;
	if( (Timx == TIM1) || (Timx == TIM8))
	{//高级定时器
		TIM_OCIS.TIM_OutputNState = TIM_OutputNState_Disable;
		TIM_OCIS.TIM_OCNPolarity = TIM_OCNPolarity_Low;
		TIM_OCIS.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	}
	switch(tim_OCx)
	{										 
		case TIM_OC1:	  
	    				TIM_OC1Init(Timx, &TIM_OCIS);
	    				TIM_OC1PreloadConfig(Timx, TIM_OCPreload_Enable);//Timx->CCMR1,使能TIMx在CCR1上的预装载寄存器
			break;		   
		case TIM_OC2:
	    				TIM_OC2Init(Timx, &TIM_OCIS);
	    				TIM_OC2PreloadConfig(Timx, TIM_OCPreload_Enable);
			break;		   		   
		case TIM_OC3:  	
	    				TIM_OC3Init(Timx, &TIM_OCIS);
	    				TIM_OC3PreloadConfig(Timx, TIM_OCPreload_Enable);
			break;
		case TIM_OC4:  	
	    				TIM_OC4Init(Timx, &TIM_OCIS);
	    				TIM_OC4PreloadConfig(Timx, TIM_OCPreload_Enable);
			break;		   	   		   
		default:
			return;
	}

	if( (Timx == TIM1) || (Timx == TIM8))
	{
		// 死区设置
//		TIM_BDTRIS.TIM_OSSRState = TIM_OSSRState_Enable;
//		TIM_BDTRIS.TIM_OSSIState = TIM_OSSIState_Enable;
//		TIM_BDTRIS.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
//		TIM_BDTRIS.TIM_DeadTime = 0x90;			//调整死区大小0x00-0xff
//		TIM_BDTRIS.TIM_Break = TIM_Break_Disable;
//		TIM_BDTRIS.TIM_BreakPolarity = TIM_BreakPolarity_High;
//		TIM_BDTRIS.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
//		TIM_BDTRConfig(Timx, &TIM_BDTRIS);	
		
		TIM_CtrlPWMOutputs(Timx, NewState);// 开启Timx的主输出			
	}
}

//系统定时器控制
void SysTimer(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    TIM_Cmd( TIMx, NewState );
}

/***************************************************************************************
** 函数名称: Timer_init
** 功能描述: STM32设置中断时间函数,仅开启中断,不对引脚进行操作
** 参    数: Timx	/ 定时器的寄存器组指针
** 参    数: tcc  / TIMx的计数时钟频率:unit:hz
** 参    数: toc	/ TIMx output clock:TIMx的输出时钟周期t:unit:us
** 参    数: pi		/ 中断优先级配置
** 参    数: NewState		是否使能当前设定时器
** 返 回 值: None      
***************************************************************************************/
void Timer_init(TIM_TypeDef *Timx, u32 Rcc, u8 NvicIrq, u32 dbg, u32 toc, s8 pi, u32 *pArr, u32 *pPsc, FunctionalState NewState, u8 tim_OCx, u8 config)
{
    NVIC_InitTypeDef					NVIC_IS;
    TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
		u32	TIMAhbxClk,//为TIMx提供的外设频率,unit:Hz
				tcc,//tcc:TIMx counter clock,unit:hz
				toc2,
				AHBx_PPREx,
				psc,arr;
//		f32	tcc2=0;//unit:Mhz

	if(Timx!=0)
	{//配置STM32外设定时器
		if(Timx==0)
			return;
		
		AHBx_PPREx=RCC->CFGR;
		
    // 使能定时器外设时钟
		if(Timx==TIM1 || Timx==TIM8 || Timx==TIM9 || Timx==TIM10 || Timx==TIM11)
		{//AHB2
			RCC_APB2PeriphClockCmd( Rcc | RCC_APB2Periph_SYSCFG, ENABLE );
			if(tim_OCx!=0)
			{
				TIMAhbxClk=RCC_ClocksStatus.PCLK2_Frequency;
			}
			else
			{
				if((AHBx_PPREx & RCC_CFGR_PPRE2) == RCC_CFGR_PPRE2_DIV1)
				{//AHB2预分频系数等于1
					TIMAhbxClk=RCC_ClocksStatus.PCLK2_Frequency;			
				}
				else
				{
					TIMAhbxClk=2*RCC_ClocksStatus.PCLK2_Frequency;		
				}
			}
		}
		else
		{//AHB1
			RCC_APB1PeriphClockCmd( Rcc | RCC_APB2Periph_SYSCFG, ENABLE );
			if(tim_OCx!=0)
			{
				TIMAhbxClk=RCC_ClocksStatus.PCLK1_Frequency;
			}
			else
			{
				if((AHBx_PPREx & RCC_CFGR_PPRE2) == RCC_CFGR_PPRE1_DIV1)
				{//AHB1预分频系数等于1
					TIMAhbxClk=RCC_ClocksStatus.PCLK1_Frequency;			
				}
				else
				{
					TIMAhbxClk=2*RCC_ClocksStatus.PCLK1_Frequency;
				}
			}
		}

		if(TIMAhbxClk>80000000)
			TIMAhbxClk=80000000;

		if(config==0)
		{//脉宽输出
			tcc=TIMAhbxClk;//默认tcc和外设输入频率一致(此处,tcc可以由参数toc来进行计算)

			if(tim_OCx)
				toc2=2*toc;//此时,toc表示输出pwm的周期数
			else
				toc2=toc;//此时,toc表示TIMx更新ARR的周期
			
			psc = TIMAhbxClk/tcc;//预分频器值PSC,默认不分频		
			arr = toc2*(tcc/1000000);// 自动重装载值ARR(此值为0时,计数器不工作)
			
			if(psc==0 || psc>=0xFFFF || arr==0 || arr>=0xFFFF)
				return;
			
			if(pArr!=0 && pPsc!=0)
			{
				*pArr=arr;
				*pPsc=psc;		
			}
			
		}
		else if(config==1)
		{//频率输出
			psc = 1;
			arr = 16000;//10k
		}

    // 定时器基本参数配置
    TIM_DeInit( Timx );
    TIM_TimeBaseStructure.TIM_Prescaler = psc-1;
    TIM_TimeBaseStructure.TIM_Period = arr-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;// 设置时钟分割: 无
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;// 选择计数模式:向上计数模式
		if( (Timx == TIM1) || (Timx == TIM8))
		{
			TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;// 重复的次数为0,也即是只计算一次	
		}
    TIM_TimeBaseInit( Timx, &TIM_TimeBaseStructure );
		TIM_ARRPreloadConfig(Timx, ENABLE);// Timx->ARR,允许ARR自动重装载缓冲器

    // 调试时定时器停止计数
		if(dbg)
			DBGMCU_APB1PeriphConfig( dbg, ENABLE );
		
		//判断是否有timx输出配置
		if(tim_OCx)
		{
			TIMx_OC_Config(Timx, tim_OCx, 0, ENABLE);//默认:占空比值:1:1,arr >> 1
		}
		
    /*定时器中断向量配置*/ 
		if(pi>=0)
		{
			NVIC_IS.NVIC_IRQChannel = NvicIrq;
			NVIC_IS.NVIC_IRQChannelPreemptionPriority = pi;
			NVIC_IS.NVIC_IRQChannelSubPriority = 0;
			NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init( &NVIC_IS );
			TIM_ClearFlag( Timx, TIM_IT_Update );// 清除TIMx溢出中断标志
			TIM_ITConfig( Timx, TIM_IT_Update, ENABLE );// 定时器溢出中断使能			
		}
//		else
//		{
//			TIM_ITConfig( Timx, TIM_IT_Update, ENABLE );	
//		}			
		
    /*使能定时器*/
		TIM_SetCounter(Timx, 0);// tim计数器清零
		SysTimer(Timx, NewState);
	}
	else
	{//配置cotex内核SysTick
		SysTick_Ini(toc);
	}
}

/**********************************************************************************************************
*	函 数 名: bsp_SoftTimerDec
*	功能说明: 每隔1ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
*	形    参：_tmr : 定时器变量指针
*	返 回 值: 无
**********************************************************************************************************/
void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->flag == 0)
	{
		if (_tmr->count > 0)
		{
			/* 如果定时器变量减到1则设置定时器到达标志 */
			if (--_tmr->count == 0)
			{
				_tmr->flag = 1;
			}
		}
	}
}

/**********************************************************************************************************
*	函 数 名: SysTick_Ini
*	功能说明: 初始化systick定时器，并启动定时中断
*	形    参：t:进中断周期数,unit:us
*	返 回 值: 无
**********************************************************************************************************/
void SysTick_Ini(u32 t)
{
	u8 i;
	u32 ticks;
	
	//ticks = SystemCoreClock / t;
	ticks = t*(SystemCoreClock/1000000);
	
	/* 清零所有的软件定时器 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		g_Tmr[i].count = 0;
		g_Tmr[i].flag = 0;
	}

	SysTick_Config(ticks);
}


/**********************************************************************************************************
*	函 数 名: bsp_DelayMS
*	功能说明: ms级延迟，延迟精度为正负1ms
*	形    参：n : 延迟长度，单位1 ms。 n 应大于2
*	返 回 值: 无
**********************************************************************************************************/
void bsp_DelayMS(uint32_t n)
{
	/* 避免 n = 1 出现主程序死锁 */
	if (n <= 1)
	{
		n = 2;
	}
	
	__set_PRIMASK(1);  		/* 关中断 */
	g_Tmr[0].count = n;
	g_Tmr[0].flag = 0;
	__set_PRIMASK(0);  		/* 开中断 */

	while (1)
	{
		CPU_IDLE();	/* 此处是空操作。用户可以定义，让CPU进入IDLE状态，以降低功耗；或实现喂狗 */

		/* 等待延迟时间到 */
		if (g_Tmr[0].flag == 1)
		{
			break;
		}
	}
}

/**********************************************************************************************************
*	函 数 名: bsp_StartTimer
*	功能说明: 启动一个定时器，并设置定时周期。
*	形    参：	_id     : 定时器ID，值域【1,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*						  定时器ID = 0 已用于bsp_DelayMS()函数
*				_period : 定时周期，单位1ms
*	返 回 值: 无
**********************************************************************************************************/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		/* while(1); 死机 */
		return;
	}

	__set_PRIMASK(1);  		/* 关中断 */
	g_Tmr[_id].count = _period;
	g_Tmr[_id].flag = 0;
	__set_PRIMASK(0);  		/* 开中断 */
}

/**********************************************************************************************************
*	函 数 名: bsp_CheckTimer
*	功能说明: 检测定时器是否超时
*	形    参：	_id     : 定时器ID，值域【1,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*						  0 用于
*				_period : 定时周期，单位1ms
*	返 回 值: 返回 0 表示定时未到， 1表示定时到
**********************************************************************************************************/
uint8_t bsp_CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (g_Tmr[_id].flag == 1)
	{
		g_Tmr[_id].flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

/**********************************************************************************************************
*	函 数 名: bsp_GetRunTime
*	功能说明: 获取CPU运行时间，单位1ms
*	形    参：无
*	返 回 值: CPU运行时间，单位1ms
**********************************************************************************************************/
u32 bsp_GetRunTime(void)
{
	u32 runtime; 

	__set_PRIMASK(1);  		/* 关中断 */
	
	runtime = g_Sys_1ms_Counter;	/* 由于在Systick中断被改写，因此关中断进行保护 */
		
	__set_PRIMASK(0);  		/* 开中断 */

	return runtime;
}





