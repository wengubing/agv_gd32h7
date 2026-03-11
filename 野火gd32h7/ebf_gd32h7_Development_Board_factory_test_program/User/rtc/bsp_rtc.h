#ifndef __BSP_RTC_H
#define	__BSP_RTC_H

#include "gd32h7xx.h"
#include "rtc/bsp_calendar.h"
#include "rtc/bsp_date.h"

//使用LXTAL外部时钟 或 IRC32K内部时钟
#define RTC_CLOCK_SOURCE_LXTAL      
//#define RTC_CLOCK_SOURCE_IRC32K

#define RTC_BKP_DRX          BKP_DR1

// 写入到备份寄存器的数据宏定义
#define RTC_BKP_DATA         0xA5

#if defined (RTC_CLOCK_SOURCE_IRC32K)

#define PRESCALER_S 0x13F
#define PRESCALER_A 0x63

#elif defined (RTC_CLOCK_SOURCE_LXTAL)

#define PRESCALER_S 0xFF
#define PRESCALER_A 0x7F

#endif


#define  DEBUG_RTC_IRQ                TAMPER_STAMP_LXTAL_IRQn
#define  DEBUG_RTC_IRQHANDLER         TAMPER_STAMP_LXTAL_IRQHandler

//北京时间的时区秒数差
#define TIME_ZOOM						(8*60*60)


/* 如果定义了下面这个宏的话,PC13就会输出频率为RTC Clock/64的时钟 */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

void RTC_Configuration(void);
void Time_Regulate_Set(void);
void RTC_SHOW_TIMESTAMP(void);
void RTC_SHOW_TIME(void);
void RTC_CheckAndConfig(void);

#endif /* __BSP_RTC_H */
