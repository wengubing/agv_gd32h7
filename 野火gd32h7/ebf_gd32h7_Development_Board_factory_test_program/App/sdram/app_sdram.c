/**
  ******************************************************************************
  * @file       app_sdram.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      sdram应用层功能接口
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDH759IMK6 GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */ 
#include "App/sdram/app_sdram.h"
#include "sdram/bsp_exmc_sdram.h"
#include "usart/bsp_usart.h"
#include "dwt/bsp_dwt_delay.h"  

uint8_t   sdram_task_flag;

__ALIGNED(32) uint32_t txbuffer32[BUFFER_SIZE/4] = {0};
__ALIGNED(32) uint32_t rxbuffer32[BUFFER_SIZE/4] = {0};

/**
 * @brief  sdram计数复位
 * @param  无
 * @retval 无
 */
void Sdram_Task_Reset(void)
{
    sdram_task_flag    = 0;
}

/**
 * @brief  sdram 任务初始化
 * @param  potentiometer_cycle: 任务轮询周期 单位ms(可修改系统节拍定时器)
 * @retval 无
 */
void Sdram_Task_Init(void)
{
     exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
}
extern uint8_t test_success_num;
extern uint8_t test_error_num;
/**
 * @brief  POTENTIOMETER_ADC任务  
 * @param  无
 * @retval 无
 */
void Sdram_Task(void)
{
    if(sdram_task_flag == 1)
    {
        uint32_t writereadstatus = 0;
        uint32_t write_addr = 0;
        uint32_t read_addr = 0;
        
        fill_buffer32(txbuffer32, BUFFER_SIZE/4, 0x0000);

        write_addr = WRITE_READ_ADDR;
        for(uint32_t i = 0; i < BUFFER_SIZE_COUT; i++) 
        {
            sdram_writebuffer_32(EXMC_SDRAM_DEVICE0, txbuffer32, write_addr, BUFFER_SIZE/4);
            write_addr = write_addr+BUFFER_SIZE;
        }
        
        read_addr = WRITE_READ_ADDR;
        for(uint32_t i = 0; i < BUFFER_SIZE_COUT; i++) 
        {
            sdram_readbuffer_32(EXMC_SDRAM_DEVICE0, rxbuffer32, read_addr, BUFFER_SIZE/4);
            read_addr = read_addr+BUFFER_SIZE;
        }
        
        for(uint32_t i = 0; i < BUFFER_SIZE/4; i++) 
        {
            if(rxbuffer32[i] != txbuffer32[i])     
            {
                writereadstatus ++;
                break;
            }
        }
        
        if(writereadstatus) 
        {
            printf("\r\n SDRAM测试未通过 !\r\n");
            test_error_num +=1;
        } 
        else 
        {
            printf("\r\n SDRAM测试通过 !\r\n");
            test_success_num +=1;
        }
        
        Sdram_Task_Reset();
    }
}
 
 /*****************************END OF FILE***************************************/
