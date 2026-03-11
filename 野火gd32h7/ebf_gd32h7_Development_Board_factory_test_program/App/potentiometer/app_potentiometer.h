#ifndef __APP_POTENTIONMETER_H
#define __APP_POTENTIONMETER_H

#include "gd32h7xx.h"

extern uint32_t  potentiometer_task_cycle;
extern uint32_t  potentiometer_task_timer;
extern uint8_t   potentiometer_task_flag;

void Potentiometer_Task_Reset(void);
void Potentiometer_Task_Init(uint32_t potentiometer_cycle);
void Potentiometer_Task(void);

#endif /* __APP_POTENTIONMETER_H  */
