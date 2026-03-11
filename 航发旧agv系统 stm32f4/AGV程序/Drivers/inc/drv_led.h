

#ifndef __DRVLED_H
#define __DRVLED_H

#ifdef  LED_GLOBALS
	#define LED_EXT
#else
	#define LED_EXT  extern
#endif


/* Private define ------------------------------------------------------------*/

#define		LEDMAXCNT			3


#define RCC_ALL_LED 		(RCC_AHB1Periph_GPIOE)

#define GPIO_PORT_LED1  GPIOE
#define GPIO_PIN_LED1		GPIO_Pin_2

#define GPIO_PORT_LED2  GPIOE
#define GPIO_PIN_LED2		GPIO_Pin_3

#define GPIO_PORT_LED3  GPIOE
#define GPIO_PIN_LED3		GPIO_Pin_4



#define b_led1		HWREGBITW(GPIOE_ODR,2)		//led1闪烁操作位映射方式
#define b_led2		HWREGBITW(GPIOE_ODR,3)		//led2
#define b_led3		HWREGBITW(GPIOE_ODR,4)		//led3











/* Private typedef -----------------------------------------------------------*/
typedef enum
{
 LED1=0,
 LED2,
 LED3
}LED_ID;


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* 供外部调用的函数声明 */
extern void Led_InitHard(void);
extern void bsp_LedOn(u8 _no);
extern void bsp_LedOff(u8 _no);
extern void bsp_LedToggle(u8 _no);
extern u8 bsp_IsLedOn(LED_ID LEDx);











#endif


