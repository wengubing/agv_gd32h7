#ifndef __BSP_ENET_H
#define	__BSP_ENET_H


#include "gd32h7xx.h"

void mpu_config(void);
void PrintfPhyReg(void);
void HardwareResetPhy(void);
void Enet_Task_Init(void);


#endif /* __BSP_ENET_H */

