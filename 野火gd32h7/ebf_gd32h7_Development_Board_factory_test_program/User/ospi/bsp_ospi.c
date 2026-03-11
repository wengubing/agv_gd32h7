/**
  ******************************************************************************
  * @file     bsp_ospi.c
  * @author   embedfire
  * @version  V1.0
  * @date     2023
  * @brief    ospi应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDH759IMK6-GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */ 
  
#include "ospi/bsp_ospi.h" 
/*!
    \brief      initialize OSPI/OSPIM and GPIO
    \param[in]  ospi_periph		: OSPIx(x=0,1)
    \param[in]  ospi_struct		: OSPI参数初始化结构的成员和成员值如下所示:
                prescaler		: between 0 and 255
                fifo_threshold	: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift	: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size		: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime		: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type		: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
								  OSPI_MACRONIX_RAM_MODE,
                wrap_size		: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
								  OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \retval     none
*/
void OSPI_FLASH_INIT(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct)
{
    /* 设置OSPI和OSPIM外设 */
    ospi_deinit(ospi_periph);
    ospim_deinit();
    
    /* 使能OSPIM和GPIO时钟 */
    rcu_periph_clock_enable(RCU_OSPIM);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure OSPIM GPIO pin:
           OSPIM_P0_IO0(PF8)
           OSPIM_P0_IO1(PF9)
           OSPIM_P0_IO2(PE2)
           OSPIM_P0_IO3(PA6)
           OSPIM_P0_CLK(PA3)
           OSPIM_P0_NCS(PB10) */

    /* OSPIM_P0_CLK(PA3)*/
    gpio_af_set(GPIOA, GPIO_AF_12, GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_3);

    /* OSPIM_P0_NCS(PB10)*/
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_10);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_10);

    /* OSPIM_P0_IO0(PF8) OSPIM_P0_IO1(PF9)*/
    gpio_af_set(GPIOF, GPIO_AF_10, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_8 | GPIO_PIN_9);
    
    /* OSPIM_P0_IO2(PE2) */
    gpio_af_set(GPIOE, GPIO_AF_9, GPIO_PIN_2);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_2);
    
    /* OSPIM_P0_IO3(PA6) */
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_6);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_6);
    
    ospim_port_sck_config(OSPIM_PORT0, OSPIM_PORT_SCK_ENABLE);
    ospim_port_csn_config(OSPIM_PORT0, OSPIM_PORT_CSN_ENABLE);
    
    ospim_port_io3_0_config(OSPIM_PORT0, OSPIM_IO_LOW_ENABLE);
    ospim_port_io7_4_config(OSPIM_PORT0, OSPIM_IO_HIGH_DISABLE);

    switch(ospi_periph) {
    case OSPI0:
        rcu_periph_clock_enable(RCU_OSPI0);
        /* 配置 OSPIM port0 */
        ospim_port_sck_source_select(OSPIM_PORT0, OSPIM_SCK_SOURCE_OSPI0_SCK);
        ospim_port_csn_source_select(OSPIM_PORT0, OSPIM_CSN_SOURCE_OSPI0_CSN);
        ospim_port_io3_0_source_select(OSPIM_PORT0, OSPIM_SRCPLIO_OSPI0_IO_LOW);
        break;
	
    case OSPI1:
        rcu_periph_clock_enable(RCU_OSPI1);
        /* 配置 OSPIM port0 */
        ospim_port_sck_source_select(OSPIM_PORT0, OSPIM_SCK_SOURCE_OSPI1_SCK);
        ospim_port_csn_source_select(OSPIM_PORT0, OSPIM_CSN_SOURCE_OSPI1_CSN);
        ospim_port_io3_0_source_select(OSPIM_PORT0, OSPIM_SRCPLIO_OSPI1_IO_LOW);
        break;
    default:
        break;
    }

    /* 初始化OSPI结构的参数 */
    ospi_struct_init(ospi_struct);

    ospi_struct->prescaler        = 9U;                          /*内核时钟分频产生OSPI时钟的分频因子*/
    ospi_struct->sample_shift     = OSPI_SAMPLE_SHIFTING_NONE;   /*指定采样移位*/
    ospi_struct->fifo_threshold   = OSPI_FIFO_THRESHOLD_5;       /*FIFO中的阈值字节数*/
    ospi_struct->device_size      = OSPI_MESZ_8_MBS;             /*指定设备大小*/
    ospi_struct->wrap_size        = OSPI_DIRECT;                 /*指定外部设备回卷大小*/
    ospi_struct->cs_hightime      = OSPI_CS_HIGH_TIME_3_CYCLE;   /*片选高电平时间*/
    ospi_struct->memory_type      = OSPI_MICRON_MODE;            /*外部设备类型*/
    ospi_struct->delay_hold_cycle = OSPI_DELAY_HOLD_NONE;        /*延迟保持1/4周期*/

    /* 初始化OSPI参数 */
    ospi_init(ospi_periph, ospi_struct);
    /* 使能 OSPI */
    ospi_enable(ospi_periph);
}



/*********************************************END OF FILE**********************/
