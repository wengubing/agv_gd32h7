#ifndef __APP_RTC_H
#define __APP_RTC_H

#include "gd32h7xx.h"

extern uint32_t  rtc_task_cycle;
extern uint32_t  rtc_task_timer;
extern uint8_t   rtc_task_flag;

void Rtc_Task_Reset(void);
void Rtc_Task_Init(uint32_t rtc_cycle);
void Rtc_Task(void);

#endif /* __APP_RTC_H  */
