
#ifndef _DRVSTM32F4RTC_H
#define _DRVSTM32F4RTC_H


#ifdef		STM32RTC_GLOBALS
	#define STM32RTC_EXT
#else
	#define STM32RTC_EXT  extern
#endif


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/


#define		stm32_rtc_enable			1								

#define		stm32_rtc_miao				0					/* 实时时钟:秒        */
#define		stm32_rtc_fen					1					/* 实时时钟:分        */ 
#define		stm32_rtc_hour				2					/* 实时时钟:时        */
#define		stm32_rtc_day					3					/* 实时时钟:日        */
#define		stm32_rtc_month				4					/* 实时时钟:月        */
#define		stm32_rtc_year				5					/* 实时时钟:年        */ 
#define		stm32_rtc_week				6					/* 实时时钟:星期      */


/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

STM32RTC_EXT u8 STM32F4_RTC_Init(void); 
STM32RTC_EXT ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);
STM32RTC_EXT ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);
STM32RTC_EXT void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec);
STM32RTC_EXT void RTC_Set_WakeUp(u32 wksel,u16 cnt);

STM32RTC_EXT u8 Get_RTC(u16 *pData);





#endif	/* _DRVSTM32F4RTC_H */ 

/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/
