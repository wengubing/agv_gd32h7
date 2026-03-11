
#ifndef _TIMER_H
#define _TIMER_H

#ifdef  TIMER_GLOBALS
	#define TIMER_EXT
#else
	#define TIMER_EXT  extern
#endif


#include "..\..\User\bsp.h"




/*已用TIMER资源:2015.10.30添加
1.高速输出：TIM8、TIM9、TIM10、TIM11;
2.编码器检测：TIM3、TIM4;
3.系统定时器：TIM2
4.第5路低速PWM输出：TIM5
5.CanFestival调度定时器：TIM1
*/
	

#define CPU_IDLE()/* 目前是空操作，用户可以定义让CPU进入IDLE状态的函数和喂狗函数 */

#define  TIM_OC1		1				// tim_OC通道号1
#define  TIM_OC2		2				// tim_OC通道号2
#define  TIM_OC3		3				// tim_OC通道号3
#define  TIM_OC4		4				// tim_OC通道号4
	
/*系统定时器配置宏定义*/
#define SysTimer_Base                   TIM2
#define SysTimer_CLK                    RCC_APB1Periph_TIM2
#define SysTimer_IRQn                   TIM2_IRQn 
#define SysTimer_IrqHandler             TIM2_IRQHandler
#define SysTimer_dbg_stop             	DBGMCU_TIM2_STOP
#define	SysTimerSet											SysTimer_Base,SysTimer_CLK,SysTimer_IRQn,SysTimer_dbg_stop
	
#define sys_timer_irq                   SysTimer_IrqHandler//SysTick_Handler	//dy修改:

	
	
/*高速输入(正交编码)定义宏*/
#define ENCODERn                        2                       // 脉冲编码器数量


#define ENCODER1_TIMER_BASE             TIM3                    // 脉冲编码器1所使用的定时器基址
#define ENCODER1_TIMER_CLK              RCC_APB1Periph_TIM3     // 脉冲编码器1定时器外设时钟
#define ENCODER1_RCC_CMD                RCC_APB1                // 脉冲编码器1定时器外设时钟使能命令
#define ENCODER1_GPIO_PORT              GPIOB                   // 脉冲编码器1端口
#define ENCODER1_GPIO_CLK               RCC_AHB1Periph_GPIOB    // 脉冲编码器1端口外设时钟
#define ENCODER1A_GPIO_PIN              GPIO_Pin_4              // 脉冲编码器1A相输入
#define ENCODER1B_GPIO_PIN              GPIO_Pin_5              // 脉冲编码器1B相输入
#define ENCODER1_IRQn                   TIM3_IRQn               // 脉冲编码器1中断向量
#define ENCODER1A_PIN_SOURCE            GPIO_PinSource4
#define ENCODER1B_PIN_SOURCE            GPIO_PinSource5
#define ENCODER1_GPIO_AF_TIM            GPIO_AF_TIM3

#define ENCODER2_TIMER_BASE             TIM4                    // 脉冲编码器2所使用的定时器基址
#define ENCODER2_TIMER_CLK              RCC_APB1Periph_TIM4     // 脉冲编码器2定时器外设时钟
#define ENCODER2_RCC_CMD                RCC_APB1                // 脉冲编码器2定时器外设时钟使能命令
#define ENCODER2_GPIO_PORT              GPIOD                   // 脉冲编码器2端口
#define ENCODER2_GPIO_CLK               RCC_AHB1Periph_GPIOD    // 脉冲编码器2端口外设时钟
#define ENCODER2A_GPIO_PIN              GPIO_Pin_12             // 脉冲编码器2A相输入
#define ENCODER2B_GPIO_PIN              GPIO_Pin_13             // 脉冲编码器2B相输入
#define ENCODER2_IRQn                   TIM4_IRQn               // 脉冲编码器2中断向量
#define ENCODER2A_PIN_SOURCE            GPIO_PinSource12
#define ENCODER2B_PIN_SOURCE            GPIO_PinSource13
#define ENCODER2_GPIO_AF_TIM            GPIO_AF_TIM4


	

#define	ENCODER_ARR_INI_DATA						0x7FFF
#define	ENCODER_ARR_16U_FULL						0xFFFF



// 编码器编号
typedef enum
{
    ENCODER1=0,
    ENCODER2,
}EncoderID;


/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct TIMER_typedef
{
	volatile u32 count;	/* 计数器 */
	volatile u8 flag;		/* 定时到达标志  */
}SOFT_TMR;

TIMER_EXT		TIM_TypeDef		*ENCODER_TIMER_BASE[ENCODERn];
TIMER_EXT		vu32					g_Sys_50us_Counter, g_Sys_1ms_Counter;


/* 供外部调用的函数声明 */

TIMER_EXT void  Timer_init(TIM_TypeDef *Timx, u32 Rcc, u8 NvicIrq, u32 dbg, u32 toc, s8 pi, u32 *pArr, u32 *pPsc, FunctionalState NewState, u8 tim_OCx, u8 config);
TIMER_EXT void  SysTimer(TIM_TypeDef* TIMx, FunctionalState NewState);
TIMER_EXT void	Encoder_Init(u32 EncoderLines);
TIMER_EXT void	Sys_1ms_ISR(void);
TIMER_EXT void	SysTick_Ini(u32 ticks);
TIMER_EXT void	bsp_DelayMS(uint32_t n);
TIMER_EXT void	bsp_StartTimer(uint8_t _id, uint32_t _period);
TIMER_EXT u8		bsp_CheckTimer(uint8_t _id);
TIMER_EXT u32		bsp_GetRunTime(void);
TIMER_EXT void	Delay_1us(vu32 Nus);


#endif
