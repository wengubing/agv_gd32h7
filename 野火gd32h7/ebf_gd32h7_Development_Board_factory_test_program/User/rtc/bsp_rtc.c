/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief   GD32 RTC 驱动
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
#include "usart/bsp_usart.h"
#include "rtc/bsp_rtc.h"

/* 秒中断标志，进入秒中断时置1，当时间被刷新之后清0 */
__IO uint32_t time_display = 0;
__IO uint16_t prescaler_a = PRESCALER_A, prescaler_s = PRESCALER_S;

rtc_timestamp_struct rtc_timestamp;
rtc_tamper_struct rtc_tamper;
rtc_parameter_struct  rtc_parameter;

uint32_t RTCSRC_FLAG = 0;

/**
 * @brief 检查并配置RTC
 * @param tm：用于读取RTC时间的结构体指针 
 * @note  无
 * @retval 无
 */
void RTC_CheckAndConfig(void)
{
    uint8_t a;
   	/*在启动时检查备份寄存器RCU_BDCTL，如果内容不是0xA5,
	  则需重新配置时间并询问用户调整时间*/    
    a = RTC_BKP0;
    printf("RTC_BKP0为：%x\n\r",a);
    a = RTCSRC_FLAG;
    printf("RTCSRC_FLAG：%x\n\r",a);
    if((RTC_BKP_DATA != RTC_BKP0) || (0x00 == RTCSRC_FLAG))
	{
        
        printf("\r\n\r\nRTC尚未配置,RTC配置....\n\r");

		/* 使用initpara_temp的时间配置RTC寄存器 */
        Time_Regulate_Set();
		
		/*向RTC_BKP0寄存器写入标志，说明RTC已在运行*/
        RTC_BKP0 = RTC_BKP_DATA;
	}
	else
	{
		/*检查是否掉电重启*/
        if(RESET != rcu_flag_get(RCU_FLAG_PORRST)) 
		{
		    printf("\r\n\r\n上电复位....\n\r");
		}
		/*检查是否Reset复位*/
		else if(RESET != rcu_flag_get(RCU_FLAG_EPRST))
		{
			printf("\r\n\r\n外部复位....\n\r");
		}
	
		printf("\r\n不需要配置RTC....\n\r");
	}

    /* 清除复位标志 flags */
    rcu_all_reset_flag_clear();
    
    /* RTC tamper 中断配置 */
    exti_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    nvic_irq_enable(DEBUG_RTC_IRQ, 0U, 0U);
//    pmu_backup_write_disable();
}

uint8_t rcu_osci_state = 0;

/**
 * @brief 配置RTC时钟
 * @param 无
 * @note  无
 * @retval 无
 */
void RTC_Configuration(void)
{
    uint32_t a = 0U;
	/* 使能 PWR 和 Backup 时钟 */
    rcu_periph_clock_enable(RCU_PMU);
	
	/* 允许访问 Backup 区域 */
    pmu_backup_write_enable();
    rtc_tamper_disable(RTC_TAMPER0);
    
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);
    
//使用外部时钟还是内部时钟（在bsp_rtc.h文件定义）	
//使用外部时钟时，在有些情况下晶振不起振
//批量产品的时候，很容易出现外部晶振不起振的情况，不太可靠	
#if defined (RTC_CLOCK_SOURCE_IRC32K)
    
    /* 打开内部32k振荡器 */
    rcu_osci_on(RCU_IRC32K);
    
    /* 等待内部32k振荡器 准备好 */
    rcu_osci_stab_wait(RCU_IRC32K);
    
    /* 选择内部32k振荡器作为 RTC 时钟源 */
    rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
	
#elif defined (RTC_CLOCK_SOURCE_LXTAL)
    
    /* 打开外部32.768k振荡器 */
    rcu_osci_on(RCU_LXTAL);
    rcu_osci_off(RCU_IRC32K);//关闭后,若没有外部晶振不能自动跳转内部时钟
    
    /* 等待外部32.768k振荡器准备好 */
    if(rcu_osci_stab_wait(RCU_LXTAL))
    {
        rcu_osci_state = 1; 
    }
    else
    {
        rcu_osci_state = 0;
    }
    
    /* 选择外部32.768k振荡器作为 RTC 时钟源 */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
	
#else
    #error RTC clock source should be defined.
#endif

    /* 使能RTC时钟 */
    rcu_periph_clock_enable(RCU_RTC);
    
    /* 等待直到RTC_TIME和RTC_DATE寄存器与APB时钟同步，并且阴影寄存器被更新 */
    rtc_register_sync_wait();
    a = RTCSRC_FLAG;
    switch(a)
    {
        case 0:
            printf("\n\r没有时钟源\n\r");
            break;
        case 1:
            printf("\n\r选择CK_LXTAL时钟作为RTC 的时钟源\n\r");
            break;
        case 2:
            printf("\n\r选择CK_IRC32K 时钟作为RTC 的时钟源\n\r");
            break;
        case 3:
            printf("\n\r选择CK_HXTAL / RTCDIV 时钟作为RTC 的时钟源\n\r");
            break;
        default:
            break;
    }
}

/**
 * @brief 保存用户使用串口设置的时间，以便后面转化成时间戳存储到RTC 计数寄存器中。
 * @param *initpara_temp：用于读取RTC时间的结构体指针 
 * @note  在串口调试助手输入时，输入完数字要加回车
 * @retval 无
 */
void Time_Regulate_Set(void)
{
    uint32_t temp_num = 0;
    uint8_t day_max=0 ;

    rtc_parameter.factor_asyn = prescaler_a;
    rtc_parameter.factor_syn = prescaler_s;
    rtc_parameter.year = 0x23;
    rtc_parameter.day_of_week = RTC_WEDNESDAY;
    rtc_parameter.month = RTC_NOV;
    rtc_parameter.date = 0x11;
    rtc_parameter.display_format = RTC_24HOUR;
    rtc_parameter.am_pm = RTC_AM;

    /* 当前时间输入 */
    printf("\r\n=========================设置时间==================");
    #if 0
    printf("\r\nplease input hour:\n\r");

    do 
    {
        printf("\r\n  请输入年份(Please Set Years),范围[1970~2038]，输入字符后请加回车:");
        scanf("%d",&temp_num);
        if(temp_num <00 || temp_num >99)
        {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
                  
        }
        else
        {	  
            printf("\n\r  年份被设置为: %d\n\r", temp_num);

            parameter_temp->year = temp_num;
            break;
        }
    }while(1);
    
    do 
    {
        printf("\r\n  请输入月份(Please Set Months):范围[1~12]，输入字符后请加回车:");
        scanf("%d",&temp_num);
        if(temp_num <1 || temp_num >12)
        {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
                  
        }
        else
        {	  
            printf("\n\r  月份被设置为: %d\n\r", temp_num);

            parameter_temp->month = temp_num;
            break;
        }
    }while(1);
    
    /*根据月份计算最大日期*/
    switch(initpara_temp->month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:					
            day_max = 31;
            break;
        
        case 4:
        case 6:
        case 9:
        case 11:
            day_max = 30;
            break;
        
        case 2:					
            /*计算闰年*/
            if((parameter_temp->year%4==0) &&
            ((parameter_temp->year%100!=0) || (initpara_temp->year%400==0)) &&
            (parameter_temp->year>2)) 
            {
                day_max = 29;
            } 
            else 
            {
                day_max = 28;
            }
            break;			
    }
    
    do 
    {				
        printf("\r\n  请输入日期(Please Set Months),范围[1~%d]，输入字符后请加回车:",day_max);
        scanf("%d",&temp_num);

        if(temp_num <1 || temp_num >day_max)
        {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
        }
        else
        {
            printf("\n\r  日期被设置为: %d\n\r", temp_num);

            parameter_temp->date = temp_num;
            break;
        }
    }while(1);
    
    do 
    {				
        printf("\r\n  请输入时钟制,范围[12或24]，输入字符后请加回车:");
        scanf("%d",&temp_num);
        
        if( temp_num !=12 && temp_num !=24)
        {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
        }
        else
        {
            if(temp_num ==12)
            {
                parameter_temp->display_format = RTC_12HOUR;
            }
            else
            {
                parameter_temp->display_format = RTC_24HOUR;
            }
            printf("\n\r  时钟制被设置为: %d小时制\n\r", temp_num);
            break;
        }
    }while(1);
    
    do 
    {				
        printf("\r\n  请输入当前时间段(AM/PM),范围[0或1]，输入字符后请加回车:");
        scanf("%d",&temp_num);
        
        if( temp_num !=0 && temp_num !=1)
        {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
        }
        else
        {
            if(temp_num ==0)
            {
                parameter_temp->am_pm = RTC_AM;
            }
            else
            {
                parameter_temp->am_pm = RTC_PM;
            }
            printf("\n\r  当前时间段被设置为: %d(AM/PM=0/1])\n\r", temp_num);
            break;
        }
    }while(1);
    
    do 
    {				
        printf("\r\n  请输入时钟(Please Set Hours),范围[0~23]，输入字符后请加回车:");
        scanf("%d",&temp_num);

        if( temp_num >23)
        {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
        }
        else
        {
            if(parameter_temp->display_format == RTC_12HOUR)
            {
                temp_num = temp_num%12;
            
            }
            printf("\n\r  时钟被设置为: %d\n\r", temp_num);
            parameter_temp->hour = temp_num;
            break;
        }
    }while(1);
    
    do 
    {				
        printf("\r\n  请输入分钟(Please Set Minutes),范围[0~59]，输入字符后请加回车:");
        scanf("%d",&temp_num);
        
        if( temp_num >59)
        {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
        }
        else
        {
            printf("\n\r  分钟被设置为: %d\n\r", temp_num);

            parameter_temp->minute = temp_num;
            break;
        }
    }while(1);
    
    do 
    {				
        printf("\r\n  请输入秒钟(Please Set Seconds),范围[0~59]，输入字符后请加回车:");
        scanf("%d",&temp_num);

        if( temp_num >59)
        {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
        }
        else
        {
            printf("\n\r  秒钟被设置为: %d\n\r", temp_num);

            parameter_temp->second = temp_num;
            break;
        }
    }while(1);
    #endif
    if(ERROR == rtc_init(&rtc_parameter)) 
    {
        printf("\n\r** 配置RTC时间失败! **\n\r");
    } 
    else 
    {
        printf("\n\r** RTC时间配置成功! **\n\r");
    }
    
}

/**
 * @brief 显示时间戳时间
 * @param 无
 * @note  无
 * @retval 无
 */
void RTC_SHOW_TIMESTAMP()
{
    uint32_t ts_subsecond = 0;
    uint8_t ts_subsecond_ss, ts_subsecond_ts, ts_subsecond_hs ;

    rtc_timestamp_get(&rtc_timestamp);
    /* get the subsecond value of timestamp time, and convert it into fractional format */
    ts_subsecond = rtc_timestamp_subsecond_get();
    ts_subsecond_ss = (1000 - (ts_subsecond * 1000 + 1000) / 400) / 100;
    ts_subsecond_ts = (1000 - (ts_subsecond * 1000 + 1000) / 400) % 100 / 10;
    ts_subsecond_hs = (1000 - (ts_subsecond * 1000 + 1000) / 400) % 10;

    printf("Get the time-stamp time: %0.2x:%0.2x:%0.2x .%x%x%x \n\r", \
           rtc_timestamp.timestamp_hour, rtc_timestamp.timestamp_minute, rtc_timestamp.timestamp_second, \
           ts_subsecond_ss, ts_subsecond_ts, ts_subsecond_hs);
			
}

/**
 * @brief 显示当前时间
 * @param 无
 * @note  无
 * @retval 无
 */
void RTC_SHOW_TIME(void)
{
    uint32_t time_subsecond = 0;
    uint8_t subsecond_ss = 0, subsecond_ts = 0, subsecond_hs = 0;

    rtc_current_time_get(&rtc_parameter);

    /* get the subsecond value of current time, and convert it into fractional format */
    time_subsecond = rtc_subsecond_get();
    subsecond_ss = (1000 - (time_subsecond * 1000 + 1000) / 400) / 100;
    subsecond_ts = (1000 - (time_subsecond * 1000 + 1000) / 400) % 100 / 10;
    subsecond_hs = (1000 - (time_subsecond * 1000 + 1000) / 400) % 10;

    printf("当前时间: 20%0.2x-%0.2x-%0.2x %0.2x:%0.2x:%0.2x .%x%x%x \n\r", \
          rtc_parameter.year,rtc_parameter.month ,rtc_parameter.date,rtc_parameter.hour, rtc_parameter.minute, rtc_parameter.second, \
           subsecond_ss, subsecond_ts, subsecond_hs);
			
}



/*!
    \brief      获取输入字符串并检查它是否有效
    \param[in]  none
    \param[out] none
    \retval     input value in BCD mode
*/
uint8_t usart_input_threshold(uint32_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};

    while(index < 2) {
        while(RESET == usart_flag_get(USART0, USART_FLAG_RBNE));
        tmp[index++] = usart_data_receive(USART0);
        if((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39)) {
            printf("\n\r please input a valid number between 0 and 9 \n\r");
            index--;
        }
    }

    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    if(index > value) {
        printf("\n\r please input a valid number between 0 and %d \n\r", value);
        return 0xFF;
    }

    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) << 4);
    return index;
}

/************************END OF FILE***************************************/
