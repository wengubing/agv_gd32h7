#ifndef __BSP_DATE_H
#define __BSP_DATE_H

#include "gd32h7xx.h"

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
};
    
void GregorianDay(rtc_parameter_struct *tm);
uint32_t mktimev(rtc_parameter_struct *tm);
void to_tm(uint32_t tim, rtc_parameter_struct *tm);
#endif /* __BSP_DATE_H */
