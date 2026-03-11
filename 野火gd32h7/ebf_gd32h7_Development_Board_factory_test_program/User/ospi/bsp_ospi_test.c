/**
  ******************************************************************************
  * @file        bsp_ospi_test.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief      OSPI对GD25Q64的测试接口
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

#include "ospi/bsp_ospi_gd25q64.h"
#include "ospi/bsp_ospi_test.h"
#include "led/bsp_gpio_led.h"
#include "usart/bsp_usart.h"
#include "systick/systick.h"

/*!
    \brief      内存比较功能
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus MEMORY_COMPARE(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length --) 
    {
        if(*src++ != *dst++)
        {
            return ERROR;
        }
    }
    return SUCCESS;
}

/*********************************************END OF FILE**********************/
