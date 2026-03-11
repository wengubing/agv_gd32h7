#ifndef __APP_SDRAM_H
#define __APP_SDRAM_H

#include "gd32h7xx.h"

/* SDRAM */
#define BUFFER_SIZE_COUT            ((uint32_t)0x10000)    //((uint32_t)0x0FFFF)
#define BUFFER_SIZE                ((uint32_t)0x200)
#define WRITE_READ_ADDR            ((uint32_t)0x0000)

extern uint8_t sdram_task_flag;

void Sdram_Task_Reset(void);
void Sdram_Task_Init(void);
void Sdram_Task(void);

#endif /* __APP_SDRAM_H  */
