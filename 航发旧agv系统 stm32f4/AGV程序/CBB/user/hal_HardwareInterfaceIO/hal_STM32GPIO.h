/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : hal_STM32GPIO.h   
  * Version    : V1.0		
  * Author     : Master			
  * Date       : 2024.02.07
  * Description: 硬件抽象层IO接口模块依赖头文件
  *******************************************************************************/

#ifndef _HAL_STM32GPIO_H_
#define _HAL_STM32GPIO_H_

#include "STM32f4xx.h"
/*Gpio对象 管脚连接在开发板的位置*/
typedef enum
{
	/*全部的输入IO*/
		/*普通输入IO，无源输入，共计16个*/
		Cn56,
		Cn72,
		Cn57,
		Cn73,

		Cn58,
		Cn74,
		Cn61,
		Cn77,

		Cn62,
		Cn78,
		Cn63,
		Cn79,

		Cn64,
		Cn80,
		Cn65,
		Cn81,

		/*特殊输入IO，有源输入，共计4个*/
		Cn66,
		Cn67,
		Cn68,
		Cn69,

		/*4PIN端子；1脚电源 2脚GND 3、4脚 2路 输入IO*/
		Cn60_3,
		Cn60_4,
		Cn75_3,
		Cn75_4,
		Cn59_3,
		Cn59_4,
		Cn76_3,
		Cn76_4,

		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		Cn40_9,
		Cn40_10,
		Cn40_11,
		Cn40_12,

		Cn24_9,
		Cn24_10,
		Cn24_11,
		Cn24_12,

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		Cn70_10,
		Cn70_11,

		Cn82_10,
		Cn82_11,

		Cn83_10,
		Cn83_11,

		Cn84_10,
		Cn84_11,
	
	/*单片机GPIO直接输出*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		Cn40_5,
		Cn40_6,
		Cn40_7,

		Cn24_3,
		Cn24_4,
		Cn24_5,
		Cn24_6,
		Cn24_7,

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		Cn70_5,
		Cn70_6,
		Cn70_7,
		Cn70_8,
		Cn70_9,

		Cn82_5,
		Cn82_6,
		Cn82_7,
		Cn82_8,
		Cn82_9,

		Cn83_5,
		Cn83_6,
		Cn83_7,
		Cn83_8,
		Cn83_9,

		Cn84_5,
		Cn84_6,
		Cn84_7,
		Cn84_8,
		Cn84_9,

		/*15PIN端子；6路 输出IO、2路 输入IO*/
		Cn70_3,

		Cn82_3,

		Cn83_3,

		Cn84_3,

	/*HC595扩展IO的GPIO，共计68个*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/
		Cn40_4,	//Y32
		Cn40_3,	//Y33

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		Cn31_2,	//Y34
		Cn31_3,	//Y35

		
		/*普通输出IO，有源输出，共计28个*/
		Cn26,	//Y36
		Cn42,	//Y37
		Cn27,	//Y38
		Cn43,	//Y39

		Cn28,	//Y40
		Cn44,	//Y41
		Cn32,	//Y42
		Cn47,	//Y43

		Cn25,	//Y44
		Cn41,	//Y45
		Cn33,	//Y46
		Cn48,	//Y47

		Cn34,	//Y48
		Cn49,	//Y49
		Cn35,	//Y50
		Cn50,	//Y51
		
		Cn36,	//Y52
		Cn51,	//Y53
		Cn37,	//Y54
		Cn52,	//Y55

		Cn38,	//Y56
		Cn53,	//Y57
		Cn87,	//Y58
		Cn89,	//Y59

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		Cn46_2,	//Y60
		Cn46_3,	//Y61
		Cn46_4,	//Y62

		Cn31_4,	//Y63

		/*普通输出IO，有源输出，共计28个*/
		Cn45,	//Y64
		Cn29,	//Y65
		Cn90,	//Y66
		Cn88,	//Y67

		/*7PIN端子；4路 输出IO*/
		Cn30_3,	//Y68
		Cn30_4,	//Y69
		Cn30_5,	//Y70
		Cn30_6,	//Y71
		
		CnEnd,
}
_HalGpioNum_TypeDef;

/*Gpio对象芯片管脚到CN端子的映射关系 共计44管脚*/
/*x0-x7 暂不可用*/
#define		GPIO_CLK_CN24_12		RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_CN24_12		GPIOE
#define		GPIO_PIN_CN24_12		GPIO_Pin_14
#define		GPIO_SOURCE_CN24_12		GPIO_PinSource14
#define		EXTI_PortSource_CN24_12	EXTI_PortSourceGPIOE
#define		EXTI_PinSource_CN24_12	EXTI_PinSource14

#define		GPIO_CLK_CN24_11		RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN24_11		GPIOI
#define		GPIO_PIN_CN24_11		GPIO_Pin_11
#define		GPIO_SOURCE_CN24_11		GPIO_PinSource11
#define		EXTI_PortSource_CN24_11	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN24_11	EXTI_PinSource11

#define		GPIO_CLK_CN24_10		RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN24_10		GPIOD
#define		GPIO_PIN_CN24_10		GPIO_Pin_8
#define		GPIO_SOURCE_CN24_10		GPIO_PinSource8
#define		EXTI_PortSource_CN24_10	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN24_10	EXTI_PinSource8

#define		GPIO_CLK_CN24_9			RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN24_9		GPIOD
#define		GPIO_PIN_CN24_9			GPIO_Pin_11
#define		GPIO_SOURCE_CN24_9		GPIO_PinSource11
#define		EXTI_PortSource_CN24_9	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN24_9	EXTI_PinSource11

#define		GPIO_CLK_CN40_9			RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN40_9		GPIOD
#define		GPIO_PIN_CN40_9			GPIO_Pin_10
#define		GPIO_SOURCE_CN40_9		GPIO_PinSource10
#define		EXTI_PortSource_CN40_9	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN40_9	EXTI_PinSource10

#define		GPIO_CLK_CN40_10		RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN40_10		GPIOD
#define		GPIO_PIN_CN40_10		GPIO_Pin_9
#define		GPIO_SOURCE_CN40_10		GPIO_PinSource9
#define		EXTI_PortSource_CN40_10	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN40_10	EXTI_PinSource9

#define		GPIO_CLK_CN40_11		RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN40_11		GPIOG
#define		GPIO_PIN_CN40_11		GPIO_Pin_12
#define		GPIO_SOURCE_CN40_11		GPIO_PinSource12
#define		EXTI_PortSource_CN40_11	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN40_11	EXTI_PinSource12

#define		GPIO_CLK_CN40_12		RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN40_12		GPIOI
#define		GPIO_PIN_CN40_12		GPIO_Pin_3
#define		GPIO_SOURCE_CN40_12		GPIO_PinSource3
#define		EXTI_PortSource_CN40_12	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN40_12	EXTI_PinSource3

#define		GPIO_CLK_CN56			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN56			GPIOG
#define		GPIO_PIN_CN56			GPIO_Pin_9
#define		GPIO_SOURCE_CN56		GPIO_PinSource9
#define		EXTI_PortSource_CN56	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN56		EXTI_PinSource9

#define		GPIO_CLK_CN72			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN72			GPIOG
#define		GPIO_PIN_CN72			GPIO_Pin_10
#define		GPIO_SOURCE_CN72		GPIO_PinSource10
#define		EXTI_PortSource_CN72	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN72		EXTI_PinSource10

#define		GPIO_CLK_CN57			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN57			GPIOG
#define		GPIO_PIN_CN57			GPIO_Pin_11
#define		GPIO_SOURCE_CN57		GPIO_PinSource11
#define		EXTI_PortSource_CN57	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN57		EXTI_PinSource11

#define		GPIO_CLK_CN73			RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN73			GPIOD
#define		GPIO_PIN_CN73			GPIO_Pin_7
#define		GPIO_SOURCE_CN73		GPIO_PinSource7
#define		EXTI_PortSource_CN73	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN73		EXTI_PinSource7

#define		GPIO_CLK_CN59_3			RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN59_3		GPIOI
#define		GPIO_PIN_CN59_3			GPIO_Pin_4
#define		GPIO_SOURCE_CN59_3		GPIO_PinSource4
#define		EXTI_PortSource_CN59_3	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN59_3	EXTI_PinSource4

#define		GPIO_CLK_CN59_4			RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN59_4		GPIOI
#define		GPIO_PIN_CN59_4			GPIO_Pin_5
#define		GPIO_SOURCE_CN59_4		GPIO_PinSource5
#define		EXTI_PortSource_CN59_4	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN59_4	EXTI_PinSource5

#define		GPIO_CLK_CN75_3			RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN75_3		GPIOI
#define		GPIO_PIN_CN75_3			GPIO_Pin_6
#define		GPIO_SOURCE_CN75_3		GPIO_PinSource6
#define		EXTI_PortSource_CN75_3	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN75_3	EXTI_PinSource6

#define		GPIO_CLK_CN75_4			RCC_AHB1Periph_GPIOB
#define		GPIO_PORT_CN75_4		GPIOB
#define		GPIO_PIN_CN75_4			GPIO_Pin_7
#define		GPIO_SOURCE_CN75_4		GPIO_PinSource7
#define		EXTI_PortSource_CN75_4	EXTI_PortSourceGPIOB
#define		EXTI_PinSource_CN75_4	EXTI_PinSource7

#define		GPIO_CLK_CN60_3			RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN60_3		GPIOD
#define		GPIO_PIN_CN60_3			GPIO_Pin_4
#define		GPIO_SOURCE_CN60_3		GPIO_PinSource4
#define		EXTI_PortSource_CN60_3	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN60_3	EXTI_PinSource4

#define		GPIO_CLK_CN60_4			RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN60_4		GPIOD
#define		GPIO_PIN_CN60_4			GPIO_Pin_5
#define		GPIO_SOURCE_CN60_4		GPIO_PinSource5
#define		EXTI_PortSource_CN60_4	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN60_4	EXTI_PinSource5

#define		GPIO_CLK_CN76_3			RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN76_3		GPIOD
#define		GPIO_PIN_CN76_3			GPIO_Pin_6
#define		GPIO_SOURCE_CN76_3		GPIO_PinSource6
#define		EXTI_PortSource_CN76_3	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN76_3	EXTI_PinSource6

#define		GPIO_CLK_CN76_4			RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN76_4		GPIOD
#define		GPIO_PIN_CN76_4			GPIO_Pin_3
#define		GPIO_SOURCE_CN76_4		GPIO_PinSource3
#define		EXTI_PortSource_CN76_4	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN76_4	EXTI_PinSource3

#define		GPIO_CLK_CN58			RCC_AHB1Periph_GPIOD
#define		GPIO_PORT_CN58			GPIOD
#define		GPIO_PIN_CN58			GPIO_Pin_1
#define		GPIO_SOURCE_CN58		GPIO_PinSource1
#define		EXTI_PortSource_CN58	EXTI_PortSourceGPIOD
#define		EXTI_PinSource_CN58		EXTI_PinSource1

#define		GPIO_CLK_CN74			RCC_AHB1Periph_GPIOA
#define		GPIO_PORT_CN74			GPIOA
#define		GPIO_PIN_CN74			GPIO_Pin_15
#define		GPIO_SOURCE_CN74		GPIO_PinSource15
#define		EXTI_PortSource_CN74	EXTI_PortSourceGPIOA
#define		EXTI_PinSource_CN74		EXTI_PinSource15

#define		GPIO_CLK_CN61			RCC_AHB1Periph_GPIOH
#define		GPIO_PORT_CN61			GPIOH
#define		GPIO_PIN_CN61			GPIO_Pin_14
#define		GPIO_SOURCE_CN61		GPIO_PinSource14
#define		EXTI_PortSource_CN61	EXTI_PortSourceGPIOH
#define		EXTI_PinSource_CN61		EXTI_PinSource14

#define		GPIO_CLK_CN77			RCC_AHB1Periph_GPIOH
#define		GPIO_PORT_CN77			GPIOH
#define		GPIO_PIN_CN77			GPIO_Pin_13
#define		GPIO_SOURCE_CN77		GPIO_PinSource13
#define		EXTI_PortSource_CN77	EXTI_PortSourceGPIOH
#define		EXTI_PinSource_CN77		EXTI_PinSource13

#define		GPIO_CLK_CN62			RCC_AHB1Periph_GPIOA
#define		GPIO_PORT_CN62			GPIOA
#define		GPIO_PIN_CN62			GPIO_Pin_12
#define		GPIO_SOURCE_CN62		GPIO_PinSource12
#define		EXTI_PortSource_CN62	EXTI_PortSourceGPIOA
#define		EXTI_PinSource_CN62		EXTI_PinSource12

#define		GPIO_CLK_CN78			RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN78			GPIOI
#define		GPIO_PIN_CN78			GPIO_Pin_1
#define		GPIO_SOURCE_CN78		GPIO_PinSource1
#define		EXTI_PortSource_CN78	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN78		EXTI_PinSource1

#define		GPIO_CLK_CN63			RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN63			GPIOI
#define		GPIO_PIN_CN63			GPIO_Pin_0
#define		GPIO_SOURCE_CN63		GPIO_PinSource0
#define		EXTI_PortSource_CN63	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN63		EXTI_PinSource0

#define		GPIO_CLK_CN79			RCC_AHB1Periph_GPIOH
#define		GPIO_PORT_CN79			GPIOH
#define		GPIO_PIN_CN79			GPIO_Pin_15
#define		GPIO_SOURCE_CN79		GPIO_PinSource15
#define		EXTI_PortSource_CN79	EXTI_PortSourceGPIOH
#define		EXTI_PinSource_CN79		EXTI_PinSource15

#define		GPIO_CLK_CN64			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN64			GPIOG
#define		GPIO_PIN_CN64			GPIO_Pin_5
#define		GPIO_SOURCE_CN64		GPIO_PinSource5
#define		EXTI_PortSource_CN64	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN64		EXTI_PinSource5

#define		GPIO_CLK_CN80			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN80			GPIOG
#define		GPIO_PIN_CN80			GPIO_Pin_4
#define		GPIO_SOURCE_CN80		GPIO_PinSource4
#define		EXTI_PortSource_CN80	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN80		EXTI_PinSource4

#define		GPIO_CLK_CN65			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN65			GPIOG
#define		GPIO_PIN_CN65			GPIO_Pin_3
#define		GPIO_SOURCE_CN65		GPIO_PinSource3
#define		EXTI_PortSource_CN65	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN65		EXTI_PinSource3

#define		GPIO_CLK_CN81			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN81			GPIOG
#define		GPIO_PIN_CN81			GPIO_Pin_8
#define		GPIO_SOURCE_CN81		GPIO_PinSource8
#define		EXTI_PortSource_CN81	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN81		EXTI_PinSource8

#define		GPIO_CLK_CN66			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN66			GPIOG
#define		GPIO_PIN_CN66			GPIO_Pin_7
#define		GPIO_SOURCE_CN66		GPIO_PinSource7
#define		EXTI_PortSource_CN66	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN66		EXTI_PinSource7

#define		GPIO_CLK_CN67			RCC_AHB1Periph_GPIOG
#define		GPIO_PORT_CN67			GPIOG
#define		GPIO_PIN_CN67			GPIO_Pin_6
#define		GPIO_SOURCE_CN67		GPIO_PinSource6
#define		EXTI_PortSource_CN67	EXTI_PortSourceGPIOG
#define		EXTI_PinSource_CN67		EXTI_PinSource6

#define		GPIO_CLK_CN68			RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_CN68			GPIOE
#define		GPIO_PIN_CN68			GPIO_Pin_0
#define		GPIO_SOURCE_CN68		GPIO_PinSource0
#define		EXTI_PortSource_CN68	EXTI_PortSourceGPIOE
#define		EXTI_PinSource_CN68		EXTI_PinSource0

#define		GPIO_CLK_CN69			RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_CN69			GPIOE
#define		GPIO_PIN_CN69			GPIO_Pin_1
#define		GPIO_SOURCE_CN69		GPIO_PinSource1
#define		EXTI_PortSource_CN69	EXTI_PortSourceGPIOE
#define		EXTI_PinSource_CN69		EXTI_PinSource1

#define		GPIO_CLK_CN70_10		RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_CN70_10		GPIOE
#define		GPIO_PIN_CN70_10		GPIO_Pin_11
#define		GPIO_SOURCE_CN70_10		GPIO_PinSource11
#define		EXTI_PortSource_CN70_10	EXTI_PortSourceGPIOE
#define		EXTI_PinSource_CN70_10	EXTI_PinSource11

#define		GPIO_CLK_CN70_11		RCC_AHB1Periph_GPIOH
#define		GPIO_PORT_CN70_11		GPIOH
#define		GPIO_PIN_CN70_11		GPIO_Pin_6
#define		GPIO_SOURCE_CN70_11		GPIO_PinSource6
#define		EXTI_PortSource_CN70_11	EXTI_PortSourceGPIOH
#define		EXTI_PinSource_CN70_11	EXTI_PinSource6

#define		GPIO_CLK_CN82_10		RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_CN82_10		GPIOE
#define		GPIO_PIN_CN82_10		GPIO_Pin_15	
#define		GPIO_SOURCE_CN82_10		GPIO_PinSource15
#define		EXTI_PortSource_CN82_10	EXTI_PortSourceGPIOE
#define		EXTI_PinSource_CN82_10	EXTI_PinSource15

#define		GPIO_CLK_CN82_11		RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_CN82_11		GPIOE
#define		GPIO_PIN_CN82_11		GPIO_Pin_13
#define		GPIO_SOURCE_CN82_11		GPIO_PinSource13
#define		EXTI_PortSource_CN82_11	EXTI_PortSourceGPIOE
#define		EXTI_PinSource_CN82_11	EXTI_PinSource13

#define		GPIO_CLK_CN83_10		RCC_AHB1Periph_GPIOE
#define		GPIO_PORT_CN83_10		GPIOE
#define		GPIO_PIN_CN83_10		GPIO_Pin_12
#define		GPIO_SOURCE_CN83_10		GPIO_PinSource12
#define		EXTI_PortSource_CN83_10	EXTI_PortSourceGPIOE
#define		EXTI_PinSource_CN83_10	EXTI_PinSource12

#define		GPIO_CLK_CN83_11		RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN83_11		GPIOI
#define		GPIO_PIN_CN83_11		GPIO_Pin_8
#define		GPIO_SOURCE_CN83_11		GPIO_PinSource8
#define		EXTI_PortSource_CN83_11	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN83_11	EXTI_PinSource8

#define		GPIO_CLK_CN84_10		RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN84_10		GPIOI
#define		GPIO_PIN_CN84_10		GPIO_Pin_9
#define		GPIO_SOURCE_CN84_10		GPIO_PinSource9
#define		EXTI_PortSource_CN84_10	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN84_10	EXTI_PinSource9

#define		GPIO_CLK_CN84_11		RCC_AHB1Periph_GPIOI
#define		GPIO_PORT_CN84_11		GPIOI
#define		GPIO_PIN_CN84_11		GPIO_Pin_10
#define		GPIO_SOURCE_CN84_11		GPIO_PinSource10
#define		EXTI_PortSource_CN84_11	EXTI_PortSourceGPIOI
#define		EXTI_PinSource_CN84_11	EXTI_PinSource10

//输出引脚定义
#define		GPIO_CLK_CN70_5					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_CN70_5					GPIOF	
#define		GPIO_PIN_CN70_5					GPIO_Pin_11	
#define		GPIO_SOURCE_CN70_5				GPIO_PinSource11		
								
#define		GPIO_CLK_CN70_6					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_CN70_6					GPIOF	
#define		GPIO_PIN_CN70_6					GPIO_Pin_12	
#define		GPIO_SOURCE_CN70_6				GPIO_PinSource12		
								
#define		GPIO_CLK_CN70_7					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_CN70_7					GPIOG	
#define		GPIO_PIN_CN70_7					GPIO_Pin_14	
#define		GPIO_SOURCE_CN70_7				GPIO_PinSource14		
								
#define		GPIO_CLK_CN70_8					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_CN70_8					GPIOF	
#define		GPIO_PIN_CN70_8					GPIO_Pin_3
#define		GPIO_SOURCE_CN70_8				GPIO_PinSource3	
								
#define		GPIO_CLK_CN70_9					RCC_AHB1Periph_GPIOA	
#define		GPIO_PORT_CN70_9					GPIOA	
#define		GPIO_PIN_CN70_9					GPIO_Pin_4	
#define		GPIO_SOURCE_CN70_9				GPIO_PinSource4		
								
#define		GPIO_CLK_CN82_5					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_CN82_5					GPIOH	
#define		GPIO_PIN_CN82_5					GPIO_Pin_4	
#define		GPIO_SOURCE_CN82_5				GPIO_PinSource4		
								
#define		GPIO_CLK_CN82_6					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_CN82_6				GPIOH		
#define		GPIO_PIN_CN82_6					GPIO_Pin_10	
#define		GPIO_SOURCE_CN82_6				GPIO_PinSource10			
								
#define		GPIO_CLK_CN82_7					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_CN82_7				GPIOG		
#define		GPIO_PIN_CN82_7					GPIO_Pin_13	
#define		GPIO_SOURCE_CN82_7				GPIO_PinSource13			
								
#define		GPIO_CLK_CN82_8					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_CN82_8				GPIOG		
#define		GPIO_PIN_CN82_8					GPIO_Pin_0	
#define		GPIO_SOURCE_CN82_8				GPIO_PinSource0			
								
#define		GPIO_CLK_CN82_9					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_CN82_9				GPIOF		
#define		GPIO_PIN_CN82_9					GPIO_Pin_15	
#define		GPIO_SOURCE_CN82_9				GPIO_PinSource15			
								
#define		GPIO_CLK_CN83_5					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_CN83_5				GPIOF		
#define		GPIO_PIN_CN83_5					GPIO_Pin_14	
#define		GPIO_SOURCE_CN83_5				GPIO_PinSource14			
								
#define		GPIO_CLK_CN83_6					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_CN83_6				GPIOF		
#define		GPIO_PIN_CN83_6					GPIO_Pin_13	
#define		GPIO_SOURCE_CN83_6				GPIO_PinSource13			
								
#define		GPIO_CLK_CN83_7					RCC_AHB1Periph_GPIOI	
#define		GPIO_PORT_CN83_7				GPIOI		
#define		GPIO_PIN_CN83_7					GPIO_Pin_2	
#define		GPIO_SOURCE_CN83_7				GPIO_PinSource2			
								
#define		GPIO_CLK_CN83_8					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_CN83_8				GPIOG		
#define		GPIO_PIN_CN83_8					GPIO_Pin_1	
#define		GPIO_SOURCE_CN83_8				GPIO_PinSource1			
								
#define		GPIO_CLK_CN83_9					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_CN83_9				GPIOE		
#define		GPIO_PIN_CN83_9					GPIO_Pin_7	
#define		GPIO_SOURCE_CN83_9				GPIO_PinSource7			
								
#define		GPIO_CLK_CN84_5					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_CN84_5				GPIOH		
#define		GPIO_PIN_CN84_5					GPIO_Pin_5	
#define		GPIO_SOURCE_CN84_5				GPIO_PinSource5			
								
#define		GPIO_CLK_CN84_6					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_CN84_6				GPIOE		
#define		GPIO_PIN_CN84_6					GPIO_Pin_8	
#define		GPIO_SOURCE_CN84_6				GPIO_PinSource8			
								
#define		GPIO_CLK_CN84_7					RCC_AHB1Periph_GPIOF	
#define		GPIO_PORT_CN84_7				GPIOF		
#define		GPIO_PIN_CN84_7					GPIO_Pin_2
#define		GPIO_SOURCE_CN84_7				GPIO_PinSource2
								
#define		GPIO_CLK_CN84_8					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_CN84_8				GPIOE		
#define		GPIO_PIN_CN84_8					GPIO_Pin_9	
#define		GPIO_SOURCE_CN84_8				GPIO_PinSource9			
								
#define		GPIO_CLK_CN84_9					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_CN84_9				GPIOH		
#define		GPIO_PIN_CN84_9					GPIO_Pin_2	
#define		GPIO_SOURCE_CN84_9				GPIO_PinSource2			
								
#define		GPIO_CLK_CN24_6					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_CN24_6				GPIOH		
#define		GPIO_PIN_CN24_6					GPIO_Pin_3	
#define		GPIO_SOURCE_CN24_6				GPIO_PinSource3			
								
#define		GPIO_CLK_CN24_5					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_CN24_5				GPIOH		
#define		GPIO_PIN_CN24_5					GPIO_Pin_9	
#define		GPIO_SOURCE_CN24_5				GPIO_PinSource9			
								
#define		GPIO_CLK_CN24_4					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_CN24_4				GPIOH		
#define		GPIO_PIN_CN24_4					GPIO_Pin_8	
#define		GPIO_SOURCE_CN24_4				GPIO_PinSource8			
								
#define		GPIO_CLK_CN24_3					RCC_AHB1Periph_GPIOH	
#define		GPIO_PORT_CN24_3				GPIOH		
#define		GPIO_PIN_CN24_3					GPIO_Pin_7	
#define		GPIO_SOURCE_CN24_3				GPIO_PinSource7			
								
#define		GPIO_CLK_CN24_7					RCC_AHB1Periph_GPIOG	
#define		GPIO_PORT_CN24_7				GPIOG		
#define		GPIO_PIN_CN24_7					GPIO_Pin_2	
#define		GPIO_SOURCE_CN24_7				GPIO_PinSource2			
								
#define		GPIO_CLK_CN40_7					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_CN40_7				GPIOD		
#define		GPIO_PIN_CN40_7					GPIO_Pin_15	
#define		GPIO_SOURCE_CN40_7				GPIO_PinSource15			
								
#define		GPIO_CLK_CN40_6					RCC_AHB1Periph_GPIOD	
#define		GPIO_PORT_CN40_6				GPIOD		
#define		GPIO_PIN_CN40_6					GPIO_Pin_14	
#define		GPIO_SOURCE_CN40_6				GPIO_PinSource14			
								
#define		GPIO_CLK_CN40_5					RCC_AHB1Periph_GPIOE	
#define		GPIO_PORT_CN40_5				GPIOE		
#define		GPIO_PIN_CN40_5					GPIO_Pin_10	
#define		GPIO_SOURCE_CN40_5				GPIO_PinSource10	

/*HC595扩展IO占位使用*/
	#define		GPIO_CLK_CN28					RCC_AHB1Periph_GPIOE	
	#define		GPIO_PORT_CN28					GPIOE		
	#define		GPIO_PIN_CN28					GPIO_Pin_10	
	#define		GPIO_SOURCE_CN28				GPIO_PinSource10	

	#define		GPIO_CLK_CN44					RCC_AHB1Periph_GPIOE	
	#define		GPIO_PORT_CN44					GPIOE		
	#define		GPIO_PIN_CN44					GPIO_Pin_10	
	#define		GPIO_SOURCE_CN44				GPIO_PinSource10	

	#define		GPIO_CLK_CN32					RCC_AHB1Periph_GPIOE	
	#define		GPIO_PORT_CN32					GPIOE		
	#define		GPIO_PIN_CN32					GPIO_Pin_10	
	#define		GPIO_SOURCE_CN32				GPIO_PinSource10	

	#define		GPIO_CLK_CN47					RCC_AHB1Periph_GPIOE	
	#define		GPIO_PORT_CN47					GPIOE
	#define		GPIO_PIN_CN47					GPIO_Pin_10
	#define		GPIO_SOURCE_CN47				GPIO_PinSource10

	#define		GPIO_CLK_CN43					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN43					GPIOE
	#define		GPIO_PIN_CN43					GPIO_Pin_10
	#define		GPIO_SOURCE_CN43				GPIO_PinSource10

	#define		GPIO_CLK_CN27					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN27					GPIOE
	#define		GPIO_PIN_CN27					GPIO_Pin_10
	#define		GPIO_SOURCE_CN27				GPIO_PinSource10

	#define		GPIO_CLK_CN42					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN42					GPIOE
	#define		GPIO_PIN_CN42					GPIO_Pin_10
	#define		GPIO_SOURCE_CN42				GPIO_PinSource10

	#define		GPIO_CLK_CN26					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN26					GPIOE
	#define		GPIO_PIN_CN26					GPIO_Pin_10
	#define		GPIO_SOURCE_CN26				GPIO_PinSource10

	#define		GPIO_CLK_CN34					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN34					GPIOE
	#define		GPIO_PIN_CN34					GPIO_Pin_10
	#define		GPIO_SOURCE_CN34				GPIO_PinSource10

	#define		GPIO_CLK_CN49					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN49					GPIOE
	#define		GPIO_PIN_CN49					GPIO_Pin_10
	#define		GPIO_SOURCE_CN49				GPIO_PinSource10

	#define		GPIO_CLK_CN35					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN35					GPIOE
	#define		GPIO_PIN_CN35					GPIO_Pin_10
	#define		GPIO_SOURCE_CN35				GPIO_PinSource10

	#define		GPIO_CLK_CN50					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN50					GPIOE
	#define		GPIO_PIN_CN50					GPIO_Pin_10
	#define		GPIO_SOURCE_CN50				GPIO_PinSource10

	#define		GPIO_CLK_CN48					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN48					GPIOE
	#define		GPIO_PIN_CN48					GPIO_Pin_10
	#define		GPIO_SOURCE_CN48				GPIO_PinSource10

	#define		GPIO_CLK_CN33					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN33					GPIOE
	#define		GPIO_PIN_CN33					GPIO_Pin_10
	#define		GPIO_SOURCE_CN33				GPIO_PinSource10

	#define		GPIO_CLK_CN41					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN41					GPIOE
	#define		GPIO_PIN_CN41					GPIO_Pin_10
	#define		GPIO_SOURCE_CN41				GPIO_PinSource10

	#define		GPIO_CLK_CN25					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN25					GPIOE
	#define		GPIO_PIN_CN25					GPIO_Pin_10
	#define		GPIO_SOURCE_CN25				GPIO_PinSource10

	#define		GPIO_CLK_CN38					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN38					GPIOE
	#define		GPIO_PIN_CN38					GPIO_Pin_10
	#define		GPIO_SOURCE_CN38				GPIO_PinSource10

	#define		GPIO_CLK_CN53					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN53					GPIOE
	#define		GPIO_PIN_CN53					GPIO_Pin_10
	#define		GPIO_SOURCE_CN53				GPIO_PinSource10

	#define		GPIO_CLK_CN87					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN87					GPIOE
	#define		GPIO_PIN_CN87					GPIO_Pin_10
	#define		GPIO_SOURCE_CN87				GPIO_PinSource10

	#define		GPIO_CLK_CN89					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN89					GPIOE
	#define		GPIO_PIN_CN89					GPIO_Pin_10
	#define		GPIO_SOURCE_CN89				GPIO_PinSource10

	#define		GPIO_CLK_CN52					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN52					GPIOE
	#define		GPIO_PIN_CN52					GPIO_Pin_10
	#define		GPIO_SOURCE_CN52				GPIO_PinSource10

	#define		GPIO_CLK_CN37					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN37					GPIOE
	#define		GPIO_PIN_CN37					GPIO_Pin_10
	#define		GPIO_SOURCE_CN37				GPIO_PinSource10

	#define		GPIO_CLK_CN51					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN51					GPIOE
	#define		GPIO_PIN_CN51					GPIO_Pin_10
	#define		GPIO_SOURCE_CN51				GPIO_PinSource10

	#define		GPIO_CLK_CN36					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN36					GPIOE
	#define		GPIO_PIN_CN36					GPIO_Pin_10
	#define		GPIO_SOURCE_CN36				GPIO_PinSource10

	#define		GPIO_CLK_CN88					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN88					GPIOE
	#define		GPIO_PIN_CN88					GPIO_Pin_10
	#define		GPIO_SOURCE_CN88				GPIO_PinSource10

	#define		GPIO_CLK_CN90					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN90					GPIOE
	#define		GPIO_PIN_CN90					GPIO_Pin_10
	#define		GPIO_SOURCE_CN90				GPIO_PinSource10

	#define		GPIO_CLK_CN29					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN29					GPIOE
	#define		GPIO_PIN_CN29					GPIO_Pin_10
	#define		GPIO_SOURCE_CN29				GPIO_PinSource10

	#define		GPIO_CLK_CN45					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN45					GPIOE
	#define		GPIO_PIN_CN45					GPIO_Pin_10
	#define		GPIO_SOURCE_CN45				GPIO_PinSource10

	#define		GPIO_CLK_CN30_3					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN30_3				GPIOE
	#define		GPIO_PIN_CN30_3					GPIO_Pin_10
	#define		GPIO_SOURCE_CN30_3				GPIO_PinSource10

	#define		GPIO_CLK_CN30_4					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN30_4				GPIOE
	#define		GPIO_PIN_CN30_4					GPIO_Pin_10
	#define		GPIO_SOURCE_CN30_4				GPIO_PinSource10

	#define		GPIO_CLK_CN30_5					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN30_5				GPIOE
	#define		GPIO_PIN_CN30_5					GPIO_Pin_10
	#define		GPIO_SOURCE_CN30_5				GPIO_PinSource10

	#define		GPIO_CLK_CN30_6					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN30_6				GPIOE
	#define		GPIO_PIN_CN30_6					GPIO_Pin_10
	#define		GPIO_SOURCE_CN30_6				GPIO_PinSource10

	#define		GPIO_CLK_CN31_2					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN31_2				GPIOE
	#define		GPIO_PIN_CN31_2					GPIO_Pin_10
	#define		GPIO_SOURCE_CN31_2				GPIO_PinSource10

	#define		GPIO_CLK_CN31_3					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN31_3				GPIOE
	#define		GPIO_PIN_CN31_3					GPIO_Pin_10
	#define		GPIO_SOURCE_CN31_3				GPIO_PinSource10

	#define		GPIO_CLK_CN31_4					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN31_4				GPIOE
	#define		GPIO_PIN_CN31_4					GPIO_Pin_10
	#define		GPIO_SOURCE_CN31_4				GPIO_PinSource10

	#define		GPIO_CLK_CN46_2					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN46_2				GPIOE
	#define		GPIO_PIN_CN46_2					GPIO_Pin_10
	#define		GPIO_SOURCE_CN46_2				GPIO_PinSource10

	#define		GPIO_CLK_CN46_3					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN46_3				GPIOE
	#define		GPIO_PIN_CN46_3					GPIO_Pin_10
	#define		GPIO_SOURCE_CN46_3				GPIO_PinSource10

	#define		GPIO_CLK_CN46_4					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN46_4				GPIOE
	#define		GPIO_PIN_CN46_4					GPIO_Pin_10
	#define		GPIO_SOURCE_CN46_4				GPIO_PinSource10

	#define		GPIO_CLK_CN40_3					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN40_3				GPIOE
	#define		GPIO_PIN_CN40_3					GPIO_Pin_10
	#define		GPIO_SOURCE_CN40_3				GPIO_PinSource10

	#define		GPIO_CLK_CN40_4					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN40_4				GPIOE
	#define		GPIO_PIN_CN40_4					GPIO_Pin_10
	#define		GPIO_SOURCE_CN40_4				GPIO_PinSource10

	#define		GPIO_CLK_CN70_3					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN70_3				GPIOE
	#define		GPIO_PIN_CN70_3					GPIO_Pin_10
	#define		GPIO_SOURCE_CN70_3				GPIO_PinSource10

	#define		GPIO_CLK_CN82_3					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN82_3				GPIOE
	#define		GPIO_PIN_CN82_3					GPIO_Pin_10
	#define		GPIO_SOURCE_CN82_3				GPIO_PinSource10

	#define		GPIO_CLK_CN83_3					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN83_3				GPIOE
	#define		GPIO_PIN_CN83_3					GPIO_Pin_10
	#define		GPIO_SOURCE_CN83_3				GPIO_PinSource10

	#define		GPIO_CLK_CN84_3					RCC_AHB1Periph_GPIOE
	#define		GPIO_PORT_CN84_3				GPIOE
	#define		GPIO_PIN_CN84_3					GPIO_Pin_10
	#define		GPIO_SOURCE_CN84_3				GPIO_PinSource10
/*HC595扩展IO占位结束*/

/*HC595扩展IO的GPIO相关定义*/
#ifndef  EXT595_NUM
	//扩展IO输出的数量
	#define		EXTIO_CNT							40
	#define		EXT595_NUM							5
	//595串行输入引脚
	#define		EXTIO_CLK_SI						RCC_AHB1Periph_GPIOG
	#define		EXTIO_PORT_SI						GPIOG		
	#define		EXTIO_PIN_SI						GPIO_Pin_15	
	#define		EXTIO_SOURCE_SI						GPIO_PinSource15		
	//595锁存存储器时钟输入引脚
	#define		EXTIO_CLK_RCK						RCC_AHB1Periph_GPIOB
	#define		EXTIO_PORT_RCK						GPIOB
	#define		EXTIO_PIN_RCK						GPIO_Pin_3
	#define		EXTIO_SOURCE_RCK					GPIO_PinSource3
	//595数据移位时钟输入引脚
	#define		EXTIO_CLK_SCK						RCC_AHB1Periph_GPIOB
	#define		EXTIO_PORT_SCK						GPIOB		
	#define		EXTIO_PIN_SCK						GPIO_Pin_6
	#define		EXTIO_SOURCE_SCK					GPIO_PinSource6
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
#endif
/***********************************************************************************/


/*按位输出的GPIO相关定义，HC595扩展IO时会用上*/
#ifndef  YOUT_LS_BIT_NUM
	/*输出状态定义*/
	#define		RW_BIT_YOUT_STATUS					3 	//Y4-Y71,占用68bit,给5个uint16_t缓存即可
	#define		YOUT_LS_BIT_NUM						68
	#define		YOUT_DHR_NUM_LS						5
	//高速输出(可配置位PWM脉冲或数字频率输出)
	#define		B48												48	 //Y0
	#define		B49												49	 //Y1
	#define		B50												50	 //Y2
	#define		B51												51	 //Y3	
	//驱动电机使用专用位(Y4-Y23)
	#define		B52												52	 //Y4  //使能信号1:启动/停止输入-0:启动,1:停止
	#define		B53												53	 //Y5
	#define		B54												54	 //Y6  //制动刹车信号1:运行/急停brake输入-0:运行，1:瞬时停止
	#define		B55												55	 //Y7  //旋转方向切信号1-1:顺CW,0:逆CCW
	#define		B56												56	 //Y8
	#define		B57												57	 //Y9  //使能信号2:启动/停止输入-0:启动,1:停止
	#define		B58												58	 //Y10
	#define		B59												59	 //Y11 //制动刹车信号2:运行/急停brake输入-0:运行，1:瞬时停止
	#define		B60												60	 //Y12 //旋转方向切信号2-1:顺CW,0:逆CCW
	#define		B61												61	 //Y13
	#define		B62												62	 //Y14 //使能信号3:启动/停止输入-0:启动,1:停止
	#define		B63												63	 //Y15
	#define		B64												64	 //Y16 //制动刹车信号3:运行/急停brake输入-0:运行，1:瞬时停止
	#define		B65												65	 //Y17 //旋转方向切信号3-1:顺CW,0:逆CCW
	#define		B66												66	 //Y18
	#define		B67												67	 //Y19 //使能信号4:启动/停止输入-0:启动,1:停止
	#define		B68												68	 //Y20
	#define		B69												69	 //Y21 //制动刹车信号4:运行/急停brake输入-0:运行，1:瞬时停止
	#define		B70												70	 //Y22 //旋转方向切信号4-1:顺CW,0:逆CCW
	#define		B71												71	 //Y23
	//普通Yx输出位
	#define		B72												72	 //Y24
	#define		B73												73	 //Y25
	#define		B74												74	 //Y26
	#define		B75												75	 //Y27
	#define		B76												76	 //Y28
	#define		B77												77	 //Y29
	#define		B78												78	 //Y30
	#define		B79												79	 //Y31
	#define		B80												80	 //Y32		----595
	#define		B81												81	 //Y33
	#define		B82												82	 //Y34
	#define		B83												83	 //Y35
	#define		B84												84	 //Y36
	#define		B85												85	 //Y37
	#define		B86												86	 //Y38
	#define		B87												87	 //Y39
	#define		B88												88	 //Y40 CN28 第一个垃圾控制
	#define		B89												89	 //Y41
	#define		B90												90	 //Y42
	#define		B91												91	 //Y43
	#define		B92												92	 //Y44
	#define		B93												93	 //Y45
	#define		B94												94	 //Y46
	#define		B95												95	 //Y47
	#define		B96												96	 //Y48
	#define		B97												97	 //Y49
	#define		B98												98	 //Y50
	#define		B99												99	 //Y51
	#define		B100											100	 //Y52
	#define		B101											101	 //Y53
	#define		B102											102	 //Y54
	#define		B103											103	 //Y55
	#define		B104											104	 //Y56
	#define		B105											105	 //Y57
	#define		B106											106	 //Y58
	#define		B107											107	 //Y59
	#define		B108											108	 //Y60
	#define		B109											109	 //Y61
	#define		B110											110	 //Y62
	#define		B111											111	 //Y63
	#define		B112											112	 //Y64
	#define		B113											113	 //Y65
	#define		B114											114	 //Y66
	#define		B115											115	 //Y67
	#define		B116											116	 //Y68
	#define		B117											117	 //Y69
	#define		B118											118	 //Y70
	#define		B119											119	 //Y71 CN30-6备用垃圾控制
	#define		B120											120	 //全开
	#define		B121											121	 //全关
#endif
/***********************************************************************************/

/***********************************************************************************/
/*全部的输入IO*/
const uint32_t	 CN_CLK_IN	= 
		GPIO_PIN_CN56|
		GPIO_PIN_CN72|
		GPIO_PIN_CN57|
		GPIO_PIN_CN73|

		GPIO_PIN_CN58|
		GPIO_PIN_CN74|
		GPIO_PIN_CN61|
		GPIO_PIN_CN77|

		GPIO_PIN_CN62|
		GPIO_PIN_CN78|
		GPIO_PIN_CN63|
		GPIO_PIN_CN79|

		GPIO_PIN_CN64|
		GPIO_PIN_CN80|
		GPIO_PIN_CN65|
		GPIO_PIN_CN81|

		/*特殊输入IO，有源输入，共计4个*/
		GPIO_PIN_CN66|
		GPIO_PIN_CN67|
		GPIO_PIN_CN68|
		GPIO_PIN_CN69|

		/*4PIN端子；1脚电源 2脚GND 3、4脚 2路 输入IO*/
		GPIO_PIN_CN60_3|
		GPIO_PIN_CN60_4|
		GPIO_PIN_CN75_3|
		GPIO_PIN_CN75_4|
		GPIO_PIN_CN59_3|
		GPIO_PIN_CN59_4|
		GPIO_PIN_CN76_3|
		GPIO_PIN_CN76_4|

		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		GPIO_PIN_CN40_9|
		GPIO_PIN_CN40_10|
		GPIO_PIN_CN40_11|
		GPIO_PIN_CN40_12|

		GPIO_PIN_CN24_9|
		GPIO_PIN_CN24_10|
		GPIO_PIN_CN24_11|
		GPIO_PIN_CN24_12|

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		GPIO_PIN_CN70_10|
		GPIO_PIN_CN70_11|

		GPIO_PIN_CN82_10|
		GPIO_PIN_CN82_11|

		GPIO_PIN_CN83_10|
		GPIO_PIN_CN83_11|

		GPIO_PIN_CN84_10|
		GPIO_PIN_CN84_11;
/***********************************************************************************/

/*单片机GPIO直接输出*/
const uint32_t	 CN_CLK_OUT	= 
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/
		GPIO_PIN_CN40_5|
		GPIO_PIN_CN40_6|
		GPIO_PIN_CN40_7|

		GPIO_PIN_CN24_3|
		GPIO_PIN_CN24_4|
		GPIO_PIN_CN24_5|
		GPIO_PIN_CN24_6|
		GPIO_PIN_CN24_7|

		/*15PIN端子；6路 输出IO、2路 输入IO*/
		GPIO_PIN_CN70_5|
		GPIO_PIN_CN70_6|
		GPIO_PIN_CN70_7|
		GPIO_PIN_CN70_8|
		GPIO_PIN_CN70_9|

		GPIO_PIN_CN82_5|
		GPIO_PIN_CN82_6|
		GPIO_PIN_CN82_7|
		GPIO_PIN_CN82_8|
		GPIO_PIN_CN82_9|

		GPIO_PIN_CN83_5|
		GPIO_PIN_CN83_6|
		GPIO_PIN_CN83_7|
		GPIO_PIN_CN83_8|
		GPIO_PIN_CN83_9|

		GPIO_PIN_CN84_5|
		GPIO_PIN_CN84_6|
		GPIO_PIN_CN84_7|
		GPIO_PIN_CN84_8|
		GPIO_PIN_CN84_9;
/***********************************************************************************/

/*全部的CN的Port列表*/
GPIO_TypeDef    *CN_PORT[CnEnd]	= {
	/*全部的输入IO*/
		/*普通输入IO，无源输入，共计16个*/
		GPIO_PORT_CN56,
		GPIO_PORT_CN72,
		GPIO_PORT_CN57,
		GPIO_PORT_CN73,

		GPIO_PORT_CN58,
		GPIO_PORT_CN74,
		GPIO_PORT_CN61,
		GPIO_PORT_CN77,

		GPIO_PORT_CN62,
		GPIO_PORT_CN78,
		GPIO_PORT_CN63,
		GPIO_PORT_CN79,

		GPIO_PORT_CN64,
		GPIO_PORT_CN80,
		GPIO_PORT_CN65,
		GPIO_PORT_CN81,

		/*特殊输入IO，有源输入，共计4个*/
		GPIO_PORT_CN66,
		GPIO_PORT_CN67,
		GPIO_PORT_CN68,
		GPIO_PORT_CN69,

		/*4PIN端子；1脚电源 2脚GND 3、4脚 2路 输入IO*/
		GPIO_PORT_CN60_3,
		GPIO_PORT_CN60_4,
		GPIO_PORT_CN75_3,
		GPIO_PORT_CN75_4,
		GPIO_PORT_CN59_3,
		GPIO_PORT_CN59_4,
		GPIO_PORT_CN76_3,
		GPIO_PORT_CN76_4,

		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		GPIO_PORT_CN40_9,
		GPIO_PORT_CN40_10,
		GPIO_PORT_CN40_11,
		GPIO_PORT_CN40_12,

		GPIO_PORT_CN24_9,
		GPIO_PORT_CN24_10,
		GPIO_PORT_CN24_11,
		GPIO_PORT_CN24_12,

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		GPIO_PORT_CN70_10,
		GPIO_PORT_CN70_11,

		GPIO_PORT_CN82_10,
		GPIO_PORT_CN82_11,

		GPIO_PORT_CN83_10,
		GPIO_PORT_CN83_11,

		GPIO_PORT_CN84_10,
		GPIO_PORT_CN84_11,
	
	/*单片机GPIO直接输出*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		GPIO_PORT_CN40_5,
		GPIO_PORT_CN40_6,
		GPIO_PORT_CN40_7,

		GPIO_PORT_CN24_3,
		GPIO_PORT_CN24_4,
		GPIO_PORT_CN24_5,
		GPIO_PORT_CN24_6,
		GPIO_PORT_CN24_7,

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		GPIO_PORT_CN70_5,
		GPIO_PORT_CN70_6,
		GPIO_PORT_CN70_7,
		GPIO_PORT_CN70_8,
		GPIO_PORT_CN70_9,

		GPIO_PORT_CN82_5,
		GPIO_PORT_CN82_6,
		GPIO_PORT_CN82_7,
		GPIO_PORT_CN82_8,
		GPIO_PORT_CN82_9,

		GPIO_PORT_CN83_5,
		GPIO_PORT_CN83_6,
		GPIO_PORT_CN83_7,
		GPIO_PORT_CN83_8,
		GPIO_PORT_CN83_9,

		GPIO_PORT_CN84_5,
		GPIO_PORT_CN84_6,
		GPIO_PORT_CN84_7,
		GPIO_PORT_CN84_8,
		GPIO_PORT_CN84_9,

		/*15PIN端子；6路 输出IO、2路 输入IO*/
		GPIO_PORT_CN70_3,

		GPIO_PORT_CN82_3,

		GPIO_PORT_CN83_3,

		GPIO_PORT_CN84_3,

	/*HC595扩展IO的GPIO，共计68个*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/
		GPIO_PORT_CN40_4,	//Y32
		GPIO_PORT_CN40_3,	//Y33

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		GPIO_PORT_CN31_2,	//Y34
		GPIO_PORT_CN31_3,	//Y35

		
		/*普通输出IO，有源输出，共计28个*/
		GPIO_PORT_CN26,	//Y36
		GPIO_PORT_CN42,	//Y37
		GPIO_PORT_CN27,	//Y38
		GPIO_PORT_CN43,	//Y39

		GPIO_PORT_CN28,	//Y40
		GPIO_PORT_CN44,	//Y41
		GPIO_PORT_CN32,	//Y42
		GPIO_PORT_CN47,	//Y43

		GPIO_PORT_CN25,	//Y44
		GPIO_PORT_CN41,	//Y45
		GPIO_PORT_CN33,	//Y46
		GPIO_PORT_CN48,	//Y47

		GPIO_PORT_CN34,	//Y48
		GPIO_PORT_CN49,	//Y49
		GPIO_PORT_CN35,	//Y50
		GPIO_PORT_CN50,	//Y51
		
		GPIO_PORT_CN36,	//Y52
		GPIO_PORT_CN51,	//Y53
		GPIO_PORT_CN37,	//Y54
		GPIO_PORT_CN52,	//Y55

		GPIO_PORT_CN38,	//Y56
		GPIO_PORT_CN53,	//Y57
		GPIO_PORT_CN87,	//Y58
		GPIO_PORT_CN89,	//Y59

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		GPIO_PORT_CN46_2,	//Y60
		GPIO_PORT_CN46_3,	//Y61
		GPIO_PORT_CN46_4,	//Y62

		GPIO_PORT_CN31_4,	//Y63

		/*普通输出IO，有源输出，共计28个*/
		GPIO_PORT_CN45,	//Y64
		GPIO_PORT_CN29,	//Y65
		GPIO_PORT_CN90,	//Y66
		GPIO_PORT_CN88,	//Y67

		/*7PIN端子；4路 输出IO*/
		GPIO_PORT_CN30_3,	//Y68
		GPIO_PORT_CN30_4,	//Y69
		GPIO_PORT_CN30_5,	//Y70
		GPIO_PORT_CN30_6,	//Y71
};

/*全部的CN的Pin列表*/
const uint32_t	 CN_PIN[CnEnd]	= {
	/*全部的输入IO*/
		/*普通输入IO，无源输入，共计16个*/
		GPIO_PIN_CN56,
		GPIO_PIN_CN72,
		GPIO_PIN_CN57,
		GPIO_PIN_CN73,

		GPIO_PIN_CN58,
		GPIO_PIN_CN74,
		GPIO_PIN_CN61,
		GPIO_PIN_CN77,

		GPIO_PIN_CN62,
		GPIO_PIN_CN78,
		GPIO_PIN_CN63,
		GPIO_PIN_CN79,

		GPIO_PIN_CN64,
		GPIO_PIN_CN80,
		GPIO_PIN_CN65,
		GPIO_PIN_CN81,

		/*特殊输入IO，有源输入，共计4个*/
		GPIO_PIN_CN66,
		GPIO_PIN_CN67,
		GPIO_PIN_CN68,
		GPIO_PIN_CN69,

		/*4PIN端子；1脚电源 2脚GND 3、4脚 2路 输入IO*/
		GPIO_PIN_CN60_3,
		GPIO_PIN_CN60_4,
		GPIO_PIN_CN75_3,
		GPIO_PIN_CN75_4,
		GPIO_PIN_CN59_3,
		GPIO_PIN_CN59_4,
		GPIO_PIN_CN76_3,
		GPIO_PIN_CN76_4,

		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		GPIO_PIN_CN40_9,
		GPIO_PIN_CN40_10,
		GPIO_PIN_CN40_11,
		GPIO_PIN_CN40_12,

		GPIO_PIN_CN24_9,
		GPIO_PIN_CN24_10,
		GPIO_PIN_CN24_11,
		GPIO_PIN_CN24_12,

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		GPIO_PIN_CN70_10,
		GPIO_PIN_CN70_11,

		GPIO_PIN_CN82_10,
		GPIO_PIN_CN82_11,

		GPIO_PIN_CN83_10,
		GPIO_PIN_CN83_11,

		GPIO_PIN_CN84_10,
		GPIO_PIN_CN84_11,

	/*单片机GPIO直接输出*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		GPIO_PIN_CN40_5,
		GPIO_PIN_CN40_6,
		GPIO_PIN_CN40_7,

		GPIO_PIN_CN24_3,
		GPIO_PIN_CN24_4,
		GPIO_PIN_CN24_5,
		GPIO_PIN_CN24_6,
		GPIO_PIN_CN24_7,

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		GPIO_PIN_CN70_5,
		GPIO_PIN_CN70_6,
		GPIO_PIN_CN70_7,
		GPIO_PIN_CN70_8,
		GPIO_PIN_CN70_9,

		GPIO_PIN_CN82_5,
		GPIO_PIN_CN82_6,
		GPIO_PIN_CN82_7,
		GPIO_PIN_CN82_8,
		GPIO_PIN_CN82_9,

		GPIO_PIN_CN83_5,
		GPIO_PIN_CN83_6,
		GPIO_PIN_CN83_7,
		GPIO_PIN_CN83_8,
		GPIO_PIN_CN83_9,

		GPIO_PIN_CN84_5,
		GPIO_PIN_CN84_6,
		GPIO_PIN_CN84_7,
		GPIO_PIN_CN84_8,
		GPIO_PIN_CN84_9,

		/*15PIN端子；6路 输出IO、2路 输入IO*/
		GPIO_PIN_CN70_3,

		GPIO_PIN_CN82_3,

		GPIO_PIN_CN83_3,

		GPIO_PIN_CN84_3,

	/*HC595扩展IO的GPIO，共计68个*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/
		GPIO_PIN_CN40_4,	//Y32
		GPIO_PIN_CN40_3,	//Y33

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		GPIO_PIN_CN31_2,	//Y34
		GPIO_PIN_CN31_3,	//Y35

		
		/*普通输出IO，有源输出，共计28个*/
		GPIO_PIN_CN26,	//Y36
		GPIO_PIN_CN42,	//Y37
		GPIO_PIN_CN27,	//Y38
		GPIO_PIN_CN43,	//Y39

		GPIO_PIN_CN28,	//Y40
		GPIO_PIN_CN44,	//Y41
		GPIO_PIN_CN32,	//Y42
		GPIO_PIN_CN47,	//Y43

		GPIO_PIN_CN25,	//Y44
		GPIO_PIN_CN41,	//Y45
		GPIO_PIN_CN33,	//Y46
		GPIO_PIN_CN48,	//Y47

		GPIO_PIN_CN34,	//Y48
		GPIO_PIN_CN49,	//Y49
		GPIO_PIN_CN35,	//Y50
		GPIO_PIN_CN50,	//Y51
		
		GPIO_PIN_CN36,	//Y52
		GPIO_PIN_CN51,	//Y53
		GPIO_PIN_CN37,	//Y54
		GPIO_PIN_CN52,	//Y55

		GPIO_PIN_CN38,	//Y56
		GPIO_PIN_CN53,	//Y57
		GPIO_PIN_CN87,	//Y58
		GPIO_PIN_CN89,	//Y59

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		GPIO_PIN_CN46_2,	//Y60
		GPIO_PIN_CN46_3,	//Y61
		GPIO_PIN_CN46_4,	//Y62

		GPIO_PIN_CN31_4,	//Y63

		/*普通输出IO，有源输出，共计28个*/
		GPIO_PIN_CN45,	//Y64
		GPIO_PIN_CN29,	//Y65
		GPIO_PIN_CN90,	//Y66
		GPIO_PIN_CN88,	//Y67

		/*7PIN端子；4路 输出IO*/
		GPIO_PIN_CN30_3,	//Y68
		GPIO_PIN_CN30_4,	//Y69
		GPIO_PIN_CN30_5,	//Y70
		GPIO_PIN_CN30_6,	//Y71

};




/*HC595扩展IO的GPIO 对应的硬件编号，共计68个*/
const uint32_t	 HC595_PIN[]	= {
		/*HC595扩展IO的GPIO，共计68个*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/
		B88,		//Cn40_4,	//Y32
		B88,		//Cn40_3,	//Y33

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		B88,		//Cn31_2,	//Y34
		B88,		//Cn31_3,	//Y35

		
		/*普通输出IO，有源输出，共计28个*/
		B88,		//Cn26,	//Y36
		B88,		//Cn42,	//Y37
		B88,		//Cn27,	//Y38
		B88,		//Cn43,	//Y39

		B88,		//Cn28,	//Y40
		B88,		//Cn44,	//Y41
		B88,		//Cn32,	//Y42
		B88,		//Cn47,	//Y43

		B88,		//Cn25,	//Y44
		B88,		//Cn41,	//Y45
		B88,		//Cn33,	//Y46
		B88,		//Cn48,	//Y47

		B88,		//Cn34,	//Y48
		B88,		//Cn49,	//Y49
		B88,		//Cn35,	//Y50
		B88,		//Cn50,	//Y51
		
		B88,		//Cn36,	//Y52
		B88,		//Cn51,	//Y53
		B88,		//Cn37,	//Y54
		B88,		//Cn52,	//Y55

		B88,		//Cn38,	//Y56
		B88,		//Cn53,	//Y57
		B88,		//Cn87,	//Y58
		B88,		//Cn89,	//Y59

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		B88,		//Cn46_2,	//Y60
		B88,		//Cn46_3,	//Y61
		B88,		//Cn46_4,	//Y62

		B88,		//Cn31_4,	//Y63

		/*普通输出IO，有源输出，共计28个*/
		B88,		//Cn45,	//Y64
		B88,		//Cn29,	//Y65
		B88,		//Cn90,	//Y66
		B88,		//Cn88,	//Y67

		/*7PIN端子；4路 输出IO*/
		B88,		//Cn30_3,	//Y68
		B88,		//Cn30_4,	//Y69
		B88,		//Cn30_5,	//Y70
		B88,		//Cn30_6,	//Y71
};


#endif

/************************************END OF FILE************************************/
