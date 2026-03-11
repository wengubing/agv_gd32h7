/**
  ******************************************************************************
  * @file        bsp_usart.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief   重定向c库printf函数到usart端口
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
	
#include "usart/bsp_usart.h"


/**
  * @brief  配置USARTX中断配置
  * @param  无
  * @retval 无
  */
void USARTX_NVIC_Config(void)
{
	/* 配置中断源：USART 、配置抢占优先级、 配置响应优先级、使能中断通道*/
#if 1
    nvic_irq_enable(DEBUG_IRQ, 1U, 1U);
#endif
}

/**
 * @brief  配置USARTX模式
 * @param  无
 * @retval 无
 */
void USARTX_Mode_Config(void)
{
  
#if 1  
    /* 开启USART相关的GPIO外设/端口时钟 */
    DEBUG_APBXCLKCMD(DEBUG_USARTX_CLK);

    /* 配置串口的工作参数 */
    usart_deinit(DEBUG_USARTX);
    
    usart_baudrate_set(DEBUG_USARTX, DEBUG_BAUDRATE);               //配置波特率
    usart_hardware_flow_rts_config(DEBUG_USARTX,USART_RTS_DISABLE); // 配置硬件流控制
    usart_hardware_flow_cts_config(DEBUG_USARTX,USART_CTS_DISABLE);
    usart_receive_config(DEBUG_USARTX, USART_RECEIVE_ENABLE);       //配置工作模式，收发一起，使能串口接收中断
    usart_transmit_config(DEBUG_USARTX, USART_TRANSMIT_ENABLE);    
    usart_parity_config(DEBUG_USARTX,USART_PM_NONE);                //配置校验位
    usart_stop_bit_set(DEBUG_USARTX,USART_STB_1BIT);                //配置停止位
    usart_word_length_set(DEBUG_USARTX,USART_WL_8BIT);              //配置帧数据字长

    usart_receive_config(DEBUG_USARTX, USART_RECEIVE_ENABLE);       //使能串口接收中断
    usart_transmit_config(DEBUG_USARTX, USART_TRANSMIT_ENABLE);     //使能串口发送中断
#endif
    
}

/**
 * @brief  USARTX初始化
 * @param  无
 * @retval 无
 */
void USARTX_Init(void)
{
    /* 配置USARTX中断配置 */
    USARTX_NVIC_Config();
    
    /* 配置USARTX模式 */
    USARTX_Mode_Config();

    /* 对应的GPIO的配置 */
    DEBUG_GPIO_Config();
    
    /* 使能串口 */
    usart_enable(DEBUG_USARTX);	 
}

/**
 * @brief 发送一个字节函数
 * @param usart_periph：USARTx(x=0,1,2)/UARTx(x=3,4) 
 * @param ch:要发送的数据
 * @note  无
 * @retval 无
 */
void USARTX_SendByte(uint32_t usart_periph, uint8_t ch)/*****************  发送一个字节 **********************/
{
	/* 发送一个字节数据到USART */
	usart_data_transmit(usart_periph,ch);
		
	/* 等待发送数据寄存器为空 */
	while(RESET == usart_flag_get(usart_periph, USART_FLAG_TBE));
}

/**
 * @brief 发送8位的数组函数
 * @param usart_periph：USARTx(x=0,1,2)/UARTx(x=3,4) 
 * @param array:要发送的数组
 * @param num:数组大小
 * @note  无
 * @retval 无
 */
void USARTX_SendArray( uint32_t usart_periph, uint8_t *array, uint16_t num)/****************** 发送8位的数组 ************************/
{
  uint8_t i;
	
	for(i=0; i<num; i++)
    {
        /* 发送一个字节数据到USART */
        USARTX_SendByte(usart_periph,array[i]);	

    }
	/* 等待发送完成 */
    while(RESET == usart_flag_get(usart_periph, USART_FLAG_TC));
}

/**
 * @brief 发送字符串函数
 * @param usart_periph：USARTx(x=0,1,2)/UARTx(x=3,4) 
 * @param str:要发送的数据
 * @note  无
 * @retval 无
 */
void USARTX_SendString(uint32_t usart_periph, uint8_t *str)/*****************  发送字符串 **********************/
{
    unsigned int k=0;
    do 
    {
        USARTX_SendByte( usart_periph, *(str + k) );
        k++;
    } while(*(str + k)!='\0');

    /* 等待发送完成 */
    while(RESET == usart_flag_get(usart_periph, USART_FLAG_TC));
}
/**
 * @brief 发送指定长度字符串函数
 * @param pusartx：USARTx(x=1,2,3)/UARTx(x=4,5) 
 * @param str:要发送的数据
 * @param num:大小
 * @note  无
 * @retval 无
 */
void USARTX_SendString_Lenth(uint32_t usart_periph, uint8_t *str,uint32_t num)
{
    unsigned int k = 0;
    do
    {
        USARTX_SendByte(usart_periph, *(str + k));
        k++;
    } while (k<num);

    /* 等待发送完成 */
    while(RESET == usart_flag_get(usart_periph, USART_FLAG_TC));
}

/**
 * @brief 发送一个16位数函数
 * @param usart_periph：USARTx(x=0,1,2)/UARTx(x=3,4) 
 * @param data:要发送的数据
 * @note  无
 * @retval 无
 */
void USARTX_SendHalfWord(uint32_t usart_periph, uint16_t data)/*****************  发送一个16位数 **********************/
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (data&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = data&0XFF;
	
	/* 发送高八位 */
	usart_data_transmit(usart_periph,temp_h);	
	while(RESET == usart_flag_get(DEBUG_USARTX, USART_FLAG_TBE));
	
	/* 发送低八位 */
	usart_data_transmit(usart_periph,temp_l);	
    while(RESET == usart_flag_get(DEBUG_USARTX, USART_FLAG_TBE));
}

/**
 * @brief 将一个字符写入到文件中,重定向c库函数printf到串口，重定向后可使用printf函数
 * @param ch: 要写入的字符
 * @param f: 指向FILE结构的指针
 * @note  无
 * @retval 成功，返回该字符
 */
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口 */
    usart_data_transmit(DEBUG_USARTX, (uint8_t)ch);
		
	/* 等待发送完毕 */
    while(RESET == usart_flag_get(DEBUG_USARTX, USART_FLAG_TBE));
    return ch;
}

/**
 * @brief 重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
 * @param f: 指向FILE结构的指针
 * @note  无
 * @retval 成功，返回该字符
 */
int fgetc(FILE *f)
{
    /* 等待串口输入数据 */
    while(RESET == usart_flag_get(DEBUG_USARTX, USART_FLAG_RBNE));
    return (int)usart_data_receive(DEBUG_USARTX);
}

/*********************************************END OF FILE**********************/
