/**
  ******************************************************************************
  * @file       app_flash.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      flash 应用层功能接口
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
#include "App/flash/app_flash.h"
#include "ospi/bsp_ospi.h" 
#include "ospi/bsp_ospi_gd25q64.h"
#include "usart/bsp_usart.h"
#include "dwt/bsp_dwt_delay.h"  

uint8_t   flash_task_flag;

uint8_t tx_buffer_ospi[] = "qspi13dfgadasgsdgsdgqrrt";

/**
 * @brief  sdram计数复位
 * @param  无
 * @retval 无
 */
void Flash_Task_Reset(void)
{
    flash_task_flag    = 0;
}

/**
 * @brief  sdram 任务初始化
 * @param  potentiometer_cycle: 任务轮询周期 单位ms(可修改系统节拍定时器)
 * @retval 无
 */
void Flash_Task_Init(void)
{
    ospi_parameter_struct ospi_struct = {0};
    
    /* 初始化 OSPI/OSPIM  and GPIO */
    OSPI_FLASH_INIT(OSPI_INTERFACE, &ospi_struct);

    /* 复位 ospi flash */
    OSPI_FLASH_RESET_ENABLE(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
    OSPI_FLASH_RESET_MEMORY(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
    OSPI_FLASH_RESET_ENABLE(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);
    OSPI_FLASH_RESET_MEMORY(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);
}
extern uint8_t test_success_num;
extern uint8_t test_error_num;
/**
 * @brief  POTENTIOMETER_ADC任务  
 * @param  无
 * @retval 无
 */
void Flash_Task(void)
{
    if(flash_task_flag == 1)
    {
        uint32_t writereadstatus = 0;
        uint32_t flash_wr_lenth =  sizeof(tx_buffer_ospi) / sizeof(*(tx_buffer_ospi))-1;
        uint32_t flashid = 0;
        ospi_parameter_struct ospi_struct = {0};
        uint8_t rx_buffer_ospi[(sizeof(tx_buffer_ospi) / sizeof(*(tx_buffer_ospi)))-1] = {0};
        
        /* 读 flash ID */
        flashid = OSPI_FLASH_READ_ID(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
 
        /* 擦除指定地址 */
        /* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
        // 这里擦除4K，即一个扇区，擦除的最小单位是扇区
        OSPI_FLASH_AUTOPOLLING_MEM_READY(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
        OSPI_FLASH_WRITE_ENBALE(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
        OSPI_FLASH_BLOCK_ERASE(OSPI_INTERFACE, &ospi_struct, SPI_MODE, FLASH_WRITE_ADDRESS, GD25Q64CSIGR_ERASE_4K);
        OSPI_FLASH_AUTOPOLLING_MEM_READY(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
        
        /* 将tx_buffer的数据写入flash */
        OSPI_FLASH_AUTOPOLLING_MEM_READY(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
        OSPI_FLASH_WRITE_ENBALE(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
        OSPI_FLASH_PAGE_PROGRAM(OSPI_INTERFACE, &ospi_struct, SPI_MODE, GD25Q64CSIGR_3BYTES_SIZE, tx_buffer_ospi, FLASH_WRITE_ADDRESS,flash_wr_lenth);
        OSPI_FLASH_AUTOPOLLING_MEM_READY(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
        
        /* 从flash读取数据 */
        OSPI_FLASH_READ(OSPI_INTERFACE, &ospi_struct, SPI_MODE, GD25Q64CSIGR_3BYTES_SIZE, rx_buffer_ospi, FLASH_WRITE_ADDRESS,flash_wr_lenth);    
        
        if(flash_wr_lenth<10)
        {
            writereadstatus++;
        }
        else
        {
            while(flash_wr_lenth--) 
            {
                if(tx_buffer_ospi[flash_wr_lenth] != rx_buffer_ospi[flash_wr_lenth])
                {
                    writereadstatus++;
                }
            }
        }
          
        if(writereadstatus) 
        {
            printf("\r\n FLASH测试未通过 !\r\n");
            test_error_num +=1;
            printf("\n\r%s\n\r", rx_buffer_ospi);
        } 
        else 
        {
            if(GD25Q64CSIGR_FLASH_ID == flashid) 
            {
                printf("\r\n GD25Q64CSIGR");
            } 
            else if(W25Q64CSIGR_FLASH_ID == flashid) 
            {
                printf("\r\n W25Q64CSIGR");
            } 
            printf(" FLASH测试通过 !\r\n");
            test_success_num +=1;
        }

        Flash_Task_Reset();
    }
}
 
 /*****************************END OF FILE***************************************/
