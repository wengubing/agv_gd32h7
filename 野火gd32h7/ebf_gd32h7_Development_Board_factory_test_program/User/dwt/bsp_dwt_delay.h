#ifndef __BSP_DWT_DELAY_H
#define __BSP_DWT_DELAY_H

#include "gd32h7xx.h"
/*
 在Cortex-M里面有一个外设叫DWT(Data Watchpoint and Trace)，该外设有一个32位的寄存器叫CYCCNT，它是一个向上的计数器，记录的是内核时钟运行的个数，最长能记录的时间为：
 10.74s=2的32次方/400000000(假设内核频率为400M，内核跳一次的时间大概为1/400M=2.5ns)当CYCCNT溢出之后，会清0重新开始向上计数。使能CYCCNT计数的操作步骤：
 1、先使能DWT外设，这个由另外内核调试寄存器DEMCR的位24控制，写1使能
 2、使能CYCCNT寄存器之前，先清0
 3、使能CYCCNT寄存器，这个由DWT_CTRL(代码上宏定义为DWT_CR)的位0控制，写1使能
 */
 
/* 时间戳相关寄存器定义 */

#define  DEMCR       *(__IO uint32_t *)0xE000EDFC
#define  DWT_CTRL    *(__IO uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004

#define  DEMCR_TRCENA           (1 << 24)
#define  DWT_CTRL_CYCCNTENA     (1 <<  0)

/* 获取内核时钟频率 */
#define GET_CPU_CLKFREQ()   SystemCoreClock

/* 为方便使用，在延时函数内部调用DWT_InitTick函数初始化时间戳寄存器，
   这样每次调用函数都会初始化一遍。
   把本宏值设置为0，然后在main函数刚运行时初始化可避免每次都初始化 */  

#define DWT_INIT_IN_DELAY_FUNCTION   1  

void DWT_InitTick(void);
uint32_t DWT_GetTick(void);
void DWT_Delay_Us(uint32_t time);
void DWT_Delay_Ms(uint32_t time);
void DWT_Delay_S(uint32_t time);

#endif /* __BSP_DWT_DELAY_H  */
