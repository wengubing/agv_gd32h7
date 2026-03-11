#ifndef __APP_FLASH_H
#define __APP_FLASH_H

#include "gd32h7xx.h"



extern uint8_t flash_task_flag;

void Flash_Task_Reset(void);
void Flash_Task_Init(void);
void Flash_Task(void);

#endif /* __APP_FLASH_H  */
