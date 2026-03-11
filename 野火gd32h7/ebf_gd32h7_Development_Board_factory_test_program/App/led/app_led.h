#ifndef __APP_LED_H
#define __APP_LED_H

#include "gd32h7xx.h"

extern uint32_t  heartbeat1_led_task_cycle;
extern uint32_t  heartbeat1_led_task_timer;
extern uint8_t   heartbeat1_led_task_flag;

void Heartbeat1_LED_Task_Reset(void);
void Heartbeat1_LED_Task_Init(uint32_t heartbeat1_led_cycle);
void Heartbeat1_LED_Task(void);

#endif /* __APP_LED_H  */
