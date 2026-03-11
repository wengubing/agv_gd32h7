

#ifndef __DRVIO_H
#define __DRVIO_H


#ifdef		IO_GLOBALS
	#define IO_EXT
#else
	#define IO_EXT  extern
#endif

/* Includes ------------------------------------------------------------------*/
#include "..\..\Drivers\inc\drv_timer.h"
#include "..\..\Drivers\inc\drv_ee_para.h"
#include "..\..\Drivers\inc\drv_analog.h"
#include "drv_IODef.h"

	
/* Private define ------------------------------------------------------------*/
//ADR,代表线圈中的bit(0-15)
/* -----------------------------------------------------------------------------------*/ 
#define GET_MODBUS_COIL(ADR) (IO_DHR[((ADR)/16)] &   (0x0001 << (ADR&0x0F))) /* 返回COIL   */
#define SET_MODBUS_COIL(ADR) (IO_DHR[((ADR)/16)] |=  (0x0001 << (ADR&0x0F))) /* 置位COIL   */
#define CLR_MODBUS_COIL(ADR) (IO_DHR[((ADR)/16)] &= ~(0x0001 << (ADR&0x0F))) /* 清零COIL   */
#define CPL_MODBUS_COIL(ADR) (IO_DHR[((ADR)/16)] ^=  (0x0001 << (ADR&0x0F))) /* 翻转COIL   */ 
#define ASS_MODBUS_COIL(ADR,BOOLIN) (IO_DHR[((ADR)/16)] = ((IO_DHR[((ADR)/16)] &= ~(0x0001 << (ADR&0x0F))) | ((BOOLIN << (ADR&0x0F))))) /* 赋值COIL   */ 
/* -----------------------------------------------------------------------------------*/ 

//拓展任意连续内存区域里的位操作
/* -----------------------------------------------------------------------------------*/ 
#define GET_ASSIGN_COIL(p2U16,ADR) ((p2U16)[((ADR)/16)] &   (0x0001 << (ADR&0x0F))) /* 返回COIL   */
#define SET_ASSIGN_COIL(p2U16,ADR) ((p2U16)[((ADR)/16)] |=  (0x0001 << (ADR&0x0F))) /* 置位COIL   */
#define CLR_ASSIGN_COIL(p2U16,ADR) ((p2U16)[((ADR)/16)] &= ~(0x0001 << (ADR&0x0F))) /* 清零COIL   */
#define CPL_ASSIGN_COIL(p2U16,ADR) ((p2U16)[((ADR)/16)] ^=  (0x0001 << (ADR&0x0F))) /* 翻转COIL   */ 
#define ASS_ASSIGN_COIL(p2U16,ADR,BOOLIN) ((p2U16)[((ADR)/16)] = (((p2U16)[((ADR)/16)] &= ~(0x0001 << (ADR&0x0F))) | ((BOOLIN << (ADR&0x0F))))) /* 赋值COIL   */ 
/* -----------------------------------------------------------------------------------*/ 

/*HC595扩展IO的GPIO相关定义*/
//扩展IO输出的数量
#define		EXTIO_CNT							40
#define		EXT595_NUM							5
//595串行输入引脚
#define		EXTIO_CLK_SI						RCC_AHB1Periph_GPIOG
#define		EXTIO_PORT_SI						GPIOG		
#define		EXTIO_PIN_SI						GPIO_Pin_15	
#define		EXTIO_SOURCE_SI					    GPIO_PinSource15		
//595锁存存储器时钟输入引脚
#define		EXTIO_CLK_RCK						RCC_AHB1Periph_GPIOB
#define		EXTIO_PORT_RCK					    GPIOB
#define		EXTIO_PIN_RCK						GPIO_Pin_3
#define		EXTIO_SOURCE_RCK				    GPIO_PinSource3
//595数据移位时钟输入引脚
#define		EXTIO_CLK_SCK						RCC_AHB1Periph_GPIOB
#define		EXTIO_PORT_SCK					    GPIOB		
#define		EXTIO_PIN_SCK						GPIO_Pin_6
#define		EXTIO_SOURCE_SCK				    GPIO_PinSource6
//595数据使能输出引脚                                                                  
#define		EXTIO_CLK_OE    					RCC_AHB1Periph_GPIOE
#define		EXTIO_PORT_OE   					GPIOE		
#define		EXTIO_PIN_OE					    GPIO_Pin_5
#define		EXTIO_SOURCE_OE				        GPIO_PinSource5
//EXTGPIO状态控制宏定义
#define		SI_L										(EXTIO_PORT_SI->BSRRH=EXTIO_PIN_SI)	//置"0"
#define		SI_H										(EXTIO_PORT_SI->BSRRL=EXTIO_PIN_SI)	//置"1"

#define		OE_L										(EXTIO_PORT_OE->BSRRH=EXTIO_PIN_OE)
#define		OE_H										(EXTIO_PORT_OE->BSRRL=EXTIO_PIN_OE)

#define		RCK_L										(EXTIO_PORT_RCK->BSRRH=EXTIO_PIN_RCK)
#define		RCK_H										(EXTIO_PORT_RCK->BSRRL=EXTIO_PIN_RCK)

#define		SCK_L										(EXTIO_PORT_SCK->BSRRH=EXTIO_PIN_SCK)
#define		SCK_H										(EXTIO_PORT_SCK->BSRRL=EXTIO_PIN_SCK)



/*高速输出(PWM输出)定义宏*/
//********************************************************************Y0
#define		GPIO_CLK_YOUT0					RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_YOUT0					GPIOE	
#define		GPIO_PIN_YOUT0					GPIO_Pin_6	
#define		GPIO_SOURCE_YOUT0				GPIO_PinSource6		
#define		GPIO_AF_YOUT0					GPIO_AF_TIM9

#define Y0Timer_Base						TIM9
#define Y0Timer_CLK							RCC_APB2Periph_TIM9
#define Y0Timer_IRQn						TIM1_BRK_TIM9_IRQn
#define Y0Timer_IrqHandler					TIM9_IRQHandler
#define Y0Timer_dbg_stop					DBGMCU_TIM9_STOP
#define Y0Timer_pwm_CHx						TIM_OC2
#define	Y0TimerSet							Y0Timer_Base,Y0Timer_CLK,Y0Timer_IRQn,Y0Timer_dbg_stop

//********************************************************************Y1
#define		GPIO_CLK_YOUT1					RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_YOUT1					GPIOI	
#define		GPIO_PIN_YOUT1					GPIO_Pin_7	
#define		GPIO_SOURCE_YOUT1				GPIO_PinSource7		
#define		GPIO_AF_YOUT1					GPIO_AF_TIM8

#define Y1Timer_Base						TIM8
#define Y1Timer_CLK							RCC_APB2Periph_TIM8
#define Y1Timer_IRQn						TIM8_UP_TIM13_IRQn
#define Y1Timer_IrqHandler					TIM8_IRQHandler
#define Y1Timer_dbg_stop					DBGMCU_TIM8_STOP
#define Y1Timer_pwm_CHx						TIM_OC3
#define	Y1TimerSet							Y1Timer_Base,Y1Timer_CLK,Y1Timer_IRQn,Y1Timer_dbg_stop

//********************************************************************Y2
#define		GPIO_CLK_YOUT2					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT2					GPIOF	
#define		GPIO_PIN_YOUT2					GPIO_Pin_6
#define		GPIO_SOURCE_YOUT2				GPIO_PinSource6
#define		GPIO_AF_YOUT2  					GPIO_AF_TIM10

#define Y2Timer_Base						TIM10
#define Y2Timer_CLK							RCC_APB2Periph_TIM10
#define Y2Timer_IRQn						TIM1_UP_TIM10_IRQn
#define Y2Timer_IrqHandler					TIM10_IRQHandler
#define Y2Timer_dbg_stop					DBGMCU_TIM10_STOP
#define Y2Timer_pwm_CHx						TIM_OC1
#define	Y2TimerSet							Y2Timer_Base,Y2Timer_CLK,Y2Timer_IRQn,Y2Timer_dbg_stop

//********************************************************************Y3
#define		GPIO_CLK_YOUT3					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT3					GPIOF	
#define		GPIO_PIN_YOUT3					GPIO_Pin_7
#define		GPIO_SOURCE_YOUT3				GPIO_PinSource7
#define		GPIO_AF_YOUT3					GPIO_AF_TIM11

#define Y3Timer_Base						TIM11
#define Y3Timer_CLK							RCC_APB2Periph_TIM11
#define Y3Timer_IRQn						TIM1_TRG_COM_TIM11_IRQn
#define Y3Timer_IrqHandler					TIM11_IRQHandler
#define Y3Timer_dbg_stop					DBGMCU_TIM11_STOP
#define Y3Timer_pwm_CHx						TIM_OC1
#define	Y3TimerSet							Y3Timer_Base,Y3Timer_CLK,Y3Timer_IRQn,Y3Timer_dbg_stop


/*输入输出GPIO相关定义*/
//输入输出使用的GPIO最大数量定义
#define		YOMAXCNT						32
#define		XINMAXCNT						44	

//输入引脚定义
#define		GPIO_CLK_XIN0					RCC_AHB1Periph_GPIOB	
#define		GPIO_PORT_XIN0					GPIOB		
#define		GPIO_PIN_XIN0					GPIO_Pin_4	
#define		GPIO_SOURCE_XIN0				GPIO_PinSource4			
								
#define		GPIO_CLK_XIN2					RCC_AHB1Periph_GPIOB	
#define		GPIO_PORT_XIN2					GPIOB		
#define		GPIO_PIN_XIN2					GPIO_Pin_5	
#define		GPIO_SOURCE_XIN2				GPIO_PinSource5			
								
#define		GPIO_CLK_XIN4					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN4					GPIOD		
#define		GPIO_PIN_XIN4					GPIO_Pin_12	
#define		GPIO_SOURCE_XIN4				GPIO_PinSource12			
								
#define		GPIO_CLK_XIN6					RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_XIN6					GPIOD	
#define		GPIO_PIN_XIN6					GPIO_Pin_13
#define		GPIO_SOURCE_XIN6				GPIO_PinSource13		
								
#define		GPIO_CLK_XIN8					RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_XIN8					GPIOE	
#define		GPIO_PIN_XIN8					GPIO_Pin_14
#define		GPIO_SOURCE_XIN8				GPIO_PinSource14		
#define		EXTI_PortSource_XIN8			EXTI_PortSourceGPIOE
#define		EXTI_PinSource_XIN8				EXTI_PinSource14

#define		GPIO_CLK_XIN9					RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_XIN9					GPIOI
#define		GPIO_PIN_XIN9					GPIO_Pin_11
#define		GPIO_SOURCE_XIN9				GPIO_PinSource11
#define		EXTI_PortSource_XIN9			EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN9				EXTI_PinSource11

#define		GPIO_CLK_XIN10					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN10					GPIOD	
#define		GPIO_PIN_XIN10					GPIO_Pin_8	
#define		GPIO_SOURCE_XIN10				GPIO_PinSource8		
#define		EXTI_PortSource_XIN10			EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN10			EXTI_PinSource8

#define		GPIO_CLK_XIN11					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN11					GPIOD	
#define		GPIO_PIN_XIN11					GPIO_Pin_11	
#define		GPIO_SOURCE_XIN11				GPIO_PinSource11		
#define		EXTI_PortSource_XIN11			EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN11			EXTI_PinSource11

#define		GPIO_CLK_XIN12					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN12					GPIOD	
#define		GPIO_PIN_XIN12					GPIO_Pin_10	
#define		GPIO_SOURCE_XIN12				GPIO_PinSource10		
#define		EXTI_PortSource_XIN12			EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN12			EXTI_PinSource10

#define		GPIO_CLK_XIN13					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN13					GPIOD	
#define		GPIO_PIN_XIN13					GPIO_Pin_9	
#define		GPIO_SOURCE_XIN13				GPIO_PinSource9		
#define		EXTI_PortSource_XIN13			EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN13			EXTI_PinSource9

#define		GPIO_CLK_XIN14					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN14					GPIOG	
#define		GPIO_PIN_XIN14					GPIO_Pin_12	
#define		GPIO_SOURCE_XIN14				GPIO_PinSource12		
#define		EXTI_PortSource_XIN14			EXTI_PortSourceGPIOG
#define		EXTI_PinSource_XIN14			EXTI_PinSource12

#define		GPIO_CLK_XIN15					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_XIN15					GPIOI	
#define		GPIO_PIN_XIN15					GPIO_Pin_3	
#define		GPIO_SOURCE_XIN15				GPIO_PinSource3		
#define		EXTI_PortSource_XIN15			EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN15			EXTI_PinSource3

#define		GPIO_CLK_XIN16					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN16					GPIOG	
#define		GPIO_PIN_XIN16					GPIO_Pin_9	
#define		GPIO_SOURCE_XIN16				GPIO_PinSource9		
#define		EXTI_PortSource_XIN16			EXTI_PortSourceGPIOG		//st1
#define		EXTI_PinSource_XIN16			EXTI_PinSource9

#define		GPIO_CLK_XIN17					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN17					GPIOG	
#define		GPIO_PIN_XIN17					GPIO_Pin_10	
#define		GPIO_SOURCE_XIN17				GPIO_PinSource10		
#define		EXTI_PortSource_XIN17			EXTI_PortSourceGPIOG		//st2
#define		EXTI_PinSource_XIN17			EXTI_PinSource10

#define		GPIO_CLK_XIN18					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN18					GPIOG	
#define		GPIO_PIN_XIN18					GPIO_Pin_11	
#define		GPIO_SOURCE_XIN18				GPIO_PinSource11		
#define		EXTI_PortSource_XIN18		EXTI_PortSourceGPIOG
#define		EXTI_PinSource_XIN18		EXTI_PinSource11

#define		GPIO_CLK_XIN19					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN19					GPIOD	
#define		GPIO_PIN_XIN19					GPIO_Pin_7	
#define		GPIO_SOURCE_XIN19				GPIO_PinSource7		
#define		EXTI_PortSource_XIN19		EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN19		EXTI_PinSource7

#define		GPIO_CLK_XIN20					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_XIN20					GPIOI	
#define		GPIO_PIN_XIN20					GPIO_Pin_4
#define		GPIO_SOURCE_XIN20				GPIO_PinSource4
#define		EXTI_PortSource_XIN20		EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN20		EXTI_PinSource4

#define		GPIO_CLK_XIN21					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_XIN21					GPIOI	
#define		GPIO_PIN_XIN21					GPIO_Pin_5
#define		GPIO_SOURCE_XIN21				GPIO_PinSource5
#define		EXTI_PortSource_XIN21		EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN21		EXTI_PinSource5

#define		GPIO_CLK_XIN22					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_XIN22					GPIOI	
#define		GPIO_PIN_XIN22					GPIO_Pin_6
#define		GPIO_SOURCE_XIN22				GPIO_PinSource6	
#define		EXTI_PortSource_XIN22		EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN22		EXTI_PinSource6

#define		GPIO_CLK_XIN23					RCC_AHB1Periph_GPIOB
#define		GPIO_PORT_XIN23					GPIOB
#define		GPIO_PIN_XIN23					GPIO_Pin_7
#define		GPIO_SOURCE_XIN23				GPIO_PinSource7
#define		EXTI_PortSource_XIN23		EXTI_PortSourceGPIOB
#define		EXTI_PinSource_XIN23		EXTI_PinSource7

#define		GPIO_CLK_XIN24					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN24					GPIOD	
#define		GPIO_PIN_XIN24					GPIO_Pin_4	
#define		GPIO_SOURCE_XIN24				GPIO_PinSource4		
#define		EXTI_PortSource_XIN24		EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN24		EXTI_PinSource4

#define		GPIO_CLK_XIN25					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN25					GPIOD	
#define		GPIO_PIN_XIN25					GPIO_Pin_5	
#define		GPIO_SOURCE_XIN25				GPIO_PinSource5		
#define		EXTI_PortSource_XIN25		EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN25		EXTI_PinSource5

#define		GPIO_CLK_XIN26					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN26					GPIOD	
#define		GPIO_PIN_XIN26					GPIO_Pin_6	
#define		GPIO_SOURCE_XIN26				GPIO_PinSource6		
#define		EXTI_PortSource_XIN26		EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN26		EXTI_PinSource6

#define		GPIO_CLK_XIN27					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN27					GPIOD	
#define		GPIO_PIN_XIN27					GPIO_Pin_3	
#define		GPIO_SOURCE_XIN27				GPIO_PinSource3		
#define		EXTI_PortSource_XIN27		EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN27		EXTI_PinSource3

#define		GPIO_CLK_XIN28					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_XIN28					GPIOD	
#define		GPIO_PIN_XIN28					GPIO_Pin_1	
#define		GPIO_SOURCE_XIN28				GPIO_PinSource1		
#define		EXTI_PortSource_XIN28		EXTI_PortSourceGPIOD
#define		EXTI_PinSource_XIN28		EXTI_PinSource1

#define		GPIO_CLK_XIN29					RCC_AHB1Periph_GPIOA	
#define		GPIO_PORT_XIN29					GPIOA	
#define		GPIO_PIN_XIN29					GPIO_Pin_15	
#define		GPIO_SOURCE_XIN29				GPIO_PinSource15		
#define		EXTI_PortSource_XIN29		EXTI_PortSourceGPIOA
#define		EXTI_PinSource_XIN29		EXTI_PinSource15

#define		GPIO_CLK_XIN30					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_XIN30					GPIOH	
#define		GPIO_PIN_XIN30					GPIO_Pin_14	
#define		GPIO_SOURCE_XIN30				GPIO_PinSource14		
#define		EXTI_PortSource_XIN30		EXTI_PortSourceGPIOH
#define		EXTI_PinSource_XIN30		EXTI_PinSource14

#define		GPIO_CLK_XIN31					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_XIN31					GPIOH	
#define		GPIO_PIN_XIN31					GPIO_Pin_13	
#define		GPIO_SOURCE_XIN31				GPIO_PinSource13		
#define		EXTI_PortSource_XIN31		EXTI_PortSourceGPIOH
#define		EXTI_PinSource_XIN31		EXTI_PinSource13

#define		GPIO_CLK_XIN32					RCC_AHB1Periph_GPIOA	
#define		GPIO_PORT_XIN32					GPIOA	
#define		GPIO_PIN_XIN32					GPIO_Pin_12	
#define		GPIO_SOURCE_XIN32				GPIO_PinSource12		
#define		EXTI_PortSource_XIN32		EXTI_PortSourceGPIOA
#define		EXTI_PinSource_XIN32		EXTI_PinSource12

#define		GPIO_CLK_XIN33					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_XIN33					GPIOI	
#define		GPIO_PIN_XIN33					GPIO_Pin_1	
#define		GPIO_SOURCE_XIN33				GPIO_PinSource1		
#define		EXTI_PortSource_XIN33		EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN33		EXTI_PinSource1

#define		GPIO_CLK_XIN34					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_XIN34					GPIOI	
#define		GPIO_PIN_XIN34					GPIO_Pin_0	
#define		GPIO_SOURCE_XIN34				GPIO_PinSource0		
#define		EXTI_PortSource_XIN34		EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN34		EXTI_PinSource0

#define		GPIO_CLK_XIN35					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_XIN35					GPIOH	
#define		GPIO_PIN_XIN35					GPIO_Pin_15	
#define		GPIO_SOURCE_XIN35				GPIO_PinSource15		
#define		EXTI_PortSource_XIN35		EXTI_PortSourceGPIOH
#define		EXTI_PinSource_XIN35		EXTI_PinSource15

#define		GPIO_CLK_XIN36					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN36					GPIOG	
#define		GPIO_PIN_XIN36					GPIO_Pin_5	
#define		GPIO_SOURCE_XIN36				GPIO_PinSource5		
#define		EXTI_PortSource_XIN36		EXTI_PortSourceGPIOG
#define		EXTI_PinSource_XIN36		EXTI_PinSource5

#define		GPIO_CLK_XIN37					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN37					GPIOG	
#define		GPIO_PIN_XIN37					GPIO_Pin_4	
#define		GPIO_SOURCE_XIN37				GPIO_PinSource4		
#define		EXTI_PortSource_XIN37		EXTI_PortSourceGPIOG
#define		EXTI_PinSource_XIN37		EXTI_PinSource4

#define		GPIO_CLK_XIN38					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN38					GPIOG	
#define		GPIO_PIN_XIN38					GPIO_Pin_3	
#define		GPIO_SOURCE_XIN38				GPIO_PinSource3		
#define		EXTI_PortSource_XIN38		EXTI_PortSourceGPIOG
#define		EXTI_PinSource_XIN38		EXTI_PinSource3

#define		GPIO_CLK_XIN39					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN39					GPIOG	
#define		GPIO_PIN_XIN39					GPIO_Pin_8	
#define		GPIO_SOURCE_XIN39				GPIO_PinSource8		
#define		EXTI_PortSource_XIN39		EXTI_PortSourceGPIOG
#define		EXTI_PinSource_XIN39		EXTI_PinSource8

#define		GPIO_CLK_XIN40					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN40					GPIOG	
#define		GPIO_PIN_XIN40					GPIO_Pin_7	
#define		GPIO_SOURCE_XIN40				GPIO_PinSource7		
#define		EXTI_PortSource_XIN40		EXTI_PortSourceGPIOG
#define		EXTI_PinSource_XIN40		EXTI_PinSource7

#define		GPIO_CLK_XIN41					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_XIN41					GPIOG	
#define		GPIO_PIN_XIN41					GPIO_Pin_6	
#define		GPIO_SOURCE_XIN41				GPIO_PinSource6	
#define		EXTI_PortSource_XIN41		EXTI_PortSourceGPIOG
#define		EXTI_PinSource_XIN41		EXTI_PinSource6

#define		GPIO_CLK_XIN42					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_XIN42					GPIOE	
#define		GPIO_PIN_XIN42					GPIO_Pin_0
#define		GPIO_SOURCE_XIN42				GPIO_PinSource0
#define		EXTI_PortSource_XIN42		EXTI_PortSourceGPIOE
#define		EXTI_PinSource_XIN42		EXTI_PinSource0

#define		GPIO_CLK_XIN43					RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_XIN43					GPIOE
#define		GPIO_PIN_XIN43					GPIO_Pin_1
#define		GPIO_SOURCE_XIN43				GPIO_PinSource1
#define		EXTI_PortSource_XIN43		EXTI_PortSourceGPIOE
#define		EXTI_PinSource_XIN43		EXTI_PinSource1

#define		GPIO_CLK_XIN44					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_XIN44					GPIOE	
#define		GPIO_PIN_XIN44					GPIO_Pin_11	
#define		GPIO_SOURCE_XIN44				GPIO_PinSource11		
#define		EXTI_PortSource_XIN44		EXTI_PortSourceGPIOE
#define		EXTI_PinSource_XIN44		EXTI_PinSource11

#define		GPIO_CLK_XIN45					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_XIN45					GPIOH	
#define		GPIO_PIN_XIN45					GPIO_Pin_6	
#define		GPIO_SOURCE_XIN45				GPIO_PinSource6		
#define		EXTI_PortSource_XIN45		EXTI_PortSourceGPIOH
#define		EXTI_PinSource_XIN45		EXTI_PinSource6

#define		GPIO_CLK_XIN46					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_XIN46					GPIOE	
#define		GPIO_PIN_XIN46					GPIO_Pin_15	
#define		GPIO_SOURCE_XIN46				GPIO_PinSource15		
#define		EXTI_PortSource_XIN46		EXTI_PortSourceGPIOE
#define		EXTI_PinSource_XIN46		EXTI_PinSource15

#define		GPIO_CLK_XIN47					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_XIN47					GPIOE	
#define		GPIO_PIN_XIN47					GPIO_Pin_13	
#define		GPIO_SOURCE_XIN47				GPIO_PinSource13		
#define		EXTI_PortSource_XIN47		EXTI_PortSourceGPIOE
#define		EXTI_PinSource_XIN47		EXTI_PinSource13

#define		GPIO_CLK_XIN48					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_XIN48					GPIOE	
#define		GPIO_PIN_XIN48					GPIO_Pin_12	
#define		GPIO_SOURCE_XIN48				GPIO_PinSource12		
#define		EXTI_PortSource_XIN48		EXTI_PortSourceGPIOE
#define		EXTI_PinSource_XIN48		EXTI_PinSource12

#define		GPIO_CLK_XIN49					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_XIN49					GPIOI	
#define		GPIO_PIN_XIN49					GPIO_Pin_8
#define		GPIO_SOURCE_XIN49				GPIO_PinSource8	
#define		EXTI_PortSource_XIN49		EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN49		EXTI_PinSource8

#define		GPIO_CLK_XIN50					RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_XIN50					GPIOI	
#define		GPIO_PIN_XIN50					GPIO_Pin_9
#define		GPIO_SOURCE_XIN50				GPIO_PinSource9
#define		EXTI_PortSource_XIN50		EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN50		EXTI_PinSource9

#define		GPIO_CLK_XIN51					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_XIN51					GPIOI
#define		GPIO_PIN_XIN51					GPIO_Pin_10
#define		GPIO_SOURCE_XIN51				GPIO_PinSource10		
#define		EXTI_PortSource_XIN51		EXTI_PortSourceGPIOI
#define		EXTI_PinSource_XIN51		EXTI_PinSource10


//输出引脚定义
#define		GPIO_CLK_YOUT4					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT4					GPIOF	
#define		GPIO_PIN_YOUT4					GPIO_Pin_11	
#define		GPIO_SOURCE_YOUT4				GPIO_PinSource11		
								
#define		GPIO_CLK_YOUT5					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT5					GPIOF	
#define		GPIO_PIN_YOUT5					GPIO_Pin_12	
#define		GPIO_SOURCE_YOUT5				GPIO_PinSource12		
								
#define		GPIO_CLK_YOUT6					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_YOUT6					GPIOG	
#define		GPIO_PIN_YOUT6					GPIO_Pin_14	
#define		GPIO_SOURCE_YOUT6				GPIO_PinSource14		
								
#define		GPIO_CLK_YOUT7					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT7					GPIOF	
#define		GPIO_PIN_YOUT7					GPIO_Pin_3
#define		GPIO_SOURCE_YOUT7				GPIO_PinSource3	
								
#define		GPIO_CLK_YOUT8					RCC_AHB1Periph_GPIOA	
#define		GPIO_PORT_YOUT8					GPIOA	
#define		GPIO_PIN_YOUT8					GPIO_Pin_4	
#define		GPIO_SOURCE_YOUT8				GPIO_PinSource4		
								
#define		GPIO_CLK_YOUT9					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_YOUT9					GPIOH	
#define		GPIO_PIN_YOUT9					GPIO_Pin_4	
#define		GPIO_SOURCE_YOUT9				GPIO_PinSource4		
								
#define		GPIO_CLK_YOUT10					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_YOUT10				GPIOH		
#define		GPIO_PIN_YOUT10					GPIO_Pin_10	
#define		GPIO_SOURCE_YOUT10			GPIO_PinSource10			
								
#define		GPIO_CLK_YOUT11					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_YOUT11				GPIOG		
#define		GPIO_PIN_YOUT11					GPIO_Pin_13	
#define		GPIO_SOURCE_YOUT11			GPIO_PinSource13			
								
#define		GPIO_CLK_YOUT12					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_YOUT12				GPIOG		
#define		GPIO_PIN_YOUT12					GPIO_Pin_0	
#define		GPIO_SOURCE_YOUT12			GPIO_PinSource0			
								
#define		GPIO_CLK_YOUT13					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT13				GPIOF		
#define		GPIO_PIN_YOUT13					GPIO_Pin_15	
#define		GPIO_SOURCE_YOUT13			GPIO_PinSource15			
								
#define		GPIO_CLK_YOUT14					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT14				GPIOF		
#define		GPIO_PIN_YOUT14					GPIO_Pin_14	
#define		GPIO_SOURCE_YOUT14			GPIO_PinSource14			
								
#define		GPIO_CLK_YOUT15					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT15				GPIOF		
#define		GPIO_PIN_YOUT15					GPIO_Pin_13	
#define		GPIO_SOURCE_YOUT15			GPIO_PinSource13			
								
#define		GPIO_CLK_YOUT16					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_YOUT16				GPIOI		
#define		GPIO_PIN_YOUT16					GPIO_Pin_2	
#define		GPIO_SOURCE_YOUT16			GPIO_PinSource2			
								
#define		GPIO_CLK_YOUT17					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_YOUT17				GPIOG		
#define		GPIO_PIN_YOUT17					GPIO_Pin_1	
#define		GPIO_SOURCE_YOUT17			GPIO_PinSource1			
								
#define		GPIO_CLK_YOUT18					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_YOUT18				GPIOE		
#define		GPIO_PIN_YOUT18					GPIO_Pin_7	
#define		GPIO_SOURCE_YOUT18			GPIO_PinSource7			
								
#define		GPIO_CLK_YOUT19					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_YOUT19				GPIOH		
#define		GPIO_PIN_YOUT19					GPIO_Pin_5	
#define		GPIO_SOURCE_YOUT19			GPIO_PinSource5			
								
#define		GPIO_CLK_YOUT20					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_YOUT20				GPIOE		
#define		GPIO_PIN_YOUT20					GPIO_Pin_8	
#define		GPIO_SOURCE_YOUT20			GPIO_PinSource8			
								
#define		GPIO_CLK_YOUT21					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_YOUT21				GPIOF		
#define		GPIO_PIN_YOUT21					GPIO_Pin_2
#define		GPIO_SOURCE_YOUT21			GPIO_PinSource2
								
#define		GPIO_CLK_YOUT22					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_YOUT22				GPIOE		
#define		GPIO_PIN_YOUT22					GPIO_Pin_9	
#define		GPIO_SOURCE_YOUT22			GPIO_PinSource9			
								
#define		GPIO_CLK_YOUT23					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_YOUT23				GPIOH		
#define		GPIO_PIN_YOUT23					GPIO_Pin_2	
#define		GPIO_SOURCE_YOUT23			GPIO_PinSource2			
								
#define		GPIO_CLK_YOUT24					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_YOUT24				GPIOH		
#define		GPIO_PIN_YOUT24					GPIO_Pin_3	
#define		GPIO_SOURCE_YOUT24			GPIO_PinSource3			
								
#define		GPIO_CLK_YOUT25					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_YOUT25				GPIOH		
#define		GPIO_PIN_YOUT25					GPIO_Pin_9	
#define		GPIO_SOURCE_YOUT25			GPIO_PinSource9			
								
#define		GPIO_CLK_YOUT26					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_YOUT26				GPIOH		
#define		GPIO_PIN_YOUT26					GPIO_Pin_8	
#define		GPIO_SOURCE_YOUT26			GPIO_PinSource8			
								
#define		GPIO_CLK_YOUT27					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_YOUT27				GPIOH		
#define		GPIO_PIN_YOUT27					GPIO_Pin_7	
#define		GPIO_SOURCE_YOUT27			GPIO_PinSource7			
								
#define		GPIO_CLK_YOUT28					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_YOUT28				GPIOG		
#define		GPIO_PIN_YOUT28					GPIO_Pin_2	
#define		GPIO_SOURCE_YOUT28			GPIO_PinSource2			
								
#define		GPIO_CLK_YOUT29					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_YOUT29				GPIOD		
#define		GPIO_PIN_YOUT29					GPIO_Pin_15	
#define		GPIO_SOURCE_YOUT29			GPIO_PinSource15			
								
#define		GPIO_CLK_YOUT30					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_YOUT30				GPIOD		
#define		GPIO_PIN_YOUT30					GPIO_Pin_14	
#define		GPIO_SOURCE_YOUT30			GPIO_PinSource14			
								
#define		GPIO_CLK_YOUT31					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_YOUT31				GPIOE		
#define		GPIO_PIN_YOUT31					GPIO_Pin_10	
#define		GPIO_SOURCE_YOUT31			GPIO_PinSource10			
								
								
//EN_232_SEL串口通道选择控制端引脚定义
#define RCC_EN_232_SEL						(RCC_AHB1Periph_GPIOA)
#define GPIO_PORT_EN_232_SEL  				GPIOA
#define GPIO_PIN_EN_232_SEL					GPIO_Pin_8

								
//2015.10.30新增加,pwm输出端口
//********************************************************************Y4
#define	GPIO_Y10_FUNC							0		//0:普通GPIO;1:高速GPIO

#define GPIO_AF_YOUT10  						GPIO_AF_TIM5

#define Y10Timer_Base							TIM5
#define Y10Timer_CLK							RCC_APB1Periph_TIM5
#define Y10Timer_IRQn							UART5_IRQn
#define Y10Timer_IrqHandler					TIM5_IRQHandler
#define Y10Timer_dbg_stop					DBGMCU_TIM5_STOP
#define Y10Timer_pwm_CHx					TIM_OC1
#define	Y10TimerSet							Y10Timer_Base,Y10Timer_CLK,Y10Timer_IRQn,Y10Timer_dbg_stop



#if (GPIO_Y10_FUNC==0)
#define		YOUT_DHR_NUM_HS							4
#elif (GPIO_Y10_FUNC==1)
#define		YOUT_DHR_NUM_HS							5
#endif
	
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	XIN8=1,
	XIN9,
	XIN10,
	XIN11,
	XIN12,
	XIN13,
	XIN14,
	XIN15,
	XIN16,
	XIN17,
	XIN18,
	XIN19,
	XIN20,
	XIN21,
	XIN22,
	XIN23,
	XIN24,
	XIN25,
	XIN26,
	XIN27,
	XIN28,
	XIN29,
	XIN30,
	XIN31,
	XIN32,
	XIN33,
	XIN34,
	XIN35,
	XIN36,
	XIN37,
	XIN38,
	XIN39,
	XIN40,
	XIN41,
	XIN42,
	XIN43,
	XIN44,
	XIN45,
	XIN46,
	XIN47,
	XIN48,
	XIN49,
	XIN50,
	XIN51,
}XIN_ID;

typedef enum
{
	YO0=0,	
	YO1,
	YO2,
	YO3,
	YO4,
	YO5,
	YO6,
	YO7,
	YO8,
	YO9,
	YO10,
	YO11,
	YO12,
	YO13,
	YO14,
	YO15,
	YO16,
	YO17,
	YO18,
	YO19,
	YO20,
	YO21,
	YO22,
	YO23,
	YO24,
	YO25,
	YO26,
	YO27,
	YO28,
	YO29,
	YO30,
	YO31,
}YOUT_ID;

typedef struct structEncoderValueType
{
		u8				EncoderUpdateFlag;//编码器数据更新标识
    s32       NewCounter[ENCODERn];// 编码器新的计数值
    s32       DeltaCounter[ENCODERn];// 编码器计数值变化量
		s32				g_EncPulseNumber[ENCODERn]; //编码器累计脉冲数
		f32				WalkLen[ENCODERn];
}EncoderValueType, *PEncoderValueType;

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


IO_EXT 	EncoderValueType     g_EncoderValue;// 编码器计数值变化的影响参数
IO_EXT	float								 g_WalkLenPerPulse, //每个脉冲对应轮子的行走距离:unit:mm
														 g_WheelPerimeter, //轮子周长:unit:mm
														 g_RGR_Factor;	//传动轴和行走轴的联动减速因子数:


IO_EXT		u32 	Prescaler_PSC[YOUT_DHR_NUM_HS],//预分频器值PSC
								Period_ARR[YOUT_DHR_NUM_HS];// 自动重装载值ARR(此值为0时,计数器不工作)


IO_EXT	void YOUTx_Turn(YOUT_ID YOx);//2015.11.20添加

IO_EXT	void Tsk_GpioOutputControl(void);
IO_EXT	void Tsk_GpioInputScan(void);
IO_EXT	void IO_InitHard(void);
IO_EXT	void IO_Para_Init(void);
IO_EXT  void Fuc_All_FreshIO(u16 *pDat, u8 ClrAll);

IO_EXT	u8 encoder_get_wheel_walk_length(EncoderValueType *pEncoderValue);
IO_EXT	u8 XIN_CHK(XIN_ID XINx);
IO_EXT	void UpadrTimerData(void);

IO_EXT	u8 SetPluseFreq(YOUT_ID Yx, u32 freq);

IO_EXT u16 IO_DHR[10];											/*仅用于存IO状态*/

#endif
/***************************** hangfa (END OF FILE) *********************************/

