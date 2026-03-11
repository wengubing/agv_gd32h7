
#define		STM32RTC_GLOBALS

/* Includes ------------------------------------------------------------------*/

#include "includes.h"
#include "..\..\Drivers\inc\drv_stm32f4rtc.h"
#include "..\..\Drivers\inc\drv_timer.h"
#include "ACC_Project.h"

/* Private define ------------------------------------------------------------*/
#define		RTC_CONFIG_FLAG				0x5A5A
#define		STM32_RTC_TIMEOUT			100

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/



const u8 pWeek_tab[] = {0,1,4,4,0,2,5,0,3,6,1,4,6};									// 星期数表
		   
const u16 mon_tab1[] = {0,31,59,90,120,151,181,212,243,273,304,334,365};			// 普通月天数表 
 
const u16 mon_tab2[] = {0,31,60,91,121,152,182,213,244,274,305,335,366};			// 闰年月天数表	

const u16 year_tab[] = {// 年份的天数表(32位计数器可支持到2132年,基数为2000)
0x0000,0x016e,0x02db,0x0448,0x05b5,0x0723,0x0890,0x09fd,0x0b6a,0x0cd8,0x0e45,0x0fb2, // 12
0x111f,0x128d,0x13fa,0x1567,0x16d4,0x1842,0x19af,0x1b1c,0x1c89,0x1df7,0x1f64,0x20d1, // 24
0x223e,0x23ac,0x2519,0x2686,0x27f3,0x2961,0x2ace,0x2c3b,0x2da8,0x2f16,0x3083,0x31f0, // 36
0x335d,0x34cb,0x3638,0x37a5,0x3912,0x3a80,0x3bed,0x3d5a,0x3ec7,0x4035,0x41a2,0x430f, // 48
0x447c,0x45ea,0x4757,0x48c4,0x4a31,0x4b9f,0x4d0c,0x4e79,0x4fe6,0x5154,0x52c1,0x542e, // 60
0x559b,0x5709,0x5876,0x59e3,0x5b50,0x5cbe,0x5e2b,0x5f98,0x6105,0x6273,0x63e0,0x654d, // 72
0x66ba,0x6828,0x6995,0x6b02,0x6c6f,0x6ddd,0x6f4a,0x70b7,0x7224,0x7392,0x74ff,0x766c, // 84
0x77d9,0x7947,0x7ab4,0x7c21,0x7d8e,0x7efc,0x8069,0x81d6,0x8343,0x84b1,0x861e,0x878b, // 96
0x88f8,0x8a66,0x8bd3,0x8d40,0x8ead,0x901b,0x9188,0x92f5,0x9462,0x95d0,0x973d,0x98aa, // 108
0x9a17,0x9b85,0x9cf2,0x9e5f,0x9fcc,0xa13a,0xa2a7,0xa414,0xa581,0xa6ef,0xa85c,0xa9c9, // 120
0xab36,0xaca4,0xae11,0xaf7e,0xb0eb,0xb259,0xb3c6,0xb533,0xb6a0,0xb80e,0xb97b,0xbae8  // 132
};

/***************************************************************************************
** 函数名称: GetWeekDay
** 功能描述: 实时时钟获得星期函数
** 参    数: 年月日
** 返 回 值: 星期值       
***************************************************************************************/
u32 GetWeekDay(u16 Year, u16 Month, u16 Date)
{
	u8 num;
	
	if((Month<3) && (!(Year&0x03) && (Year%1000) || (!(Year%400)))) 
	{
		Date--; 
	}
	num = (Date + Year + Year/4 + Year/400 - Year/100 + pWeek_tab[Month] - 2) % 7;
	
	return num; 
}

/***************************************************************************************
** 函数名称: RTC_Set_Time
** 功能描述: RTC时间设置
** 参    数: hour,min,sec,ampm:小时,分钟,秒钟,上午/下午
** 返回值:SUCEE(1),成功
					ERROR(0),失败      
***************************************************************************************/
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeIS;

	RTC_TimeIS.RTC_Hours=hour;
	RTC_TimeIS.RTC_Minutes=min;
	RTC_TimeIS.RTC_Seconds=sec;
	RTC_TimeIS.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeIS);
}

/***************************************************************************************
** 函数名称: RTC_Set_Date
** 功能描述: RTC日期设置
** 参    数: year,month,date,week:年(0~99),月(1~12),日(0~31),工作日数
** 返回值:SUCEE(1),成功
					ERROR(0),失败      
***************************************************************************************/
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	RTC_DateTypeDef RTC_DateIS;
	
	RTC_DateIS.RTC_Date=date;
	RTC_DateIS.RTC_Month=month;
	RTC_DateIS.RTC_WeekDay=week;
	RTC_DateIS.RTC_Year=year;
	
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateIS);
}

/***************************************************************************************
** 函数名称: Get_RTC
** 功能描述: 获取RTC数据
** 参    数: 指向目标区数据的指针,依次内容为：年、月、日、星期、时、分、秒、AM/PM
** 返回值:  无
***************************************************************************************/
u8 Get_RTC(u16 *pData)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	if(pData==0)
		return 1;

	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

	pData[0]=RTC_DateStruct.RTC_Year+2000;
	pData[1]=RTC_DateStruct.RTC_Month;
	pData[2]=RTC_DateStruct.RTC_Date;
	pData[3]=RTC_TimeStruct.RTC_Hours;
	pData[4]=RTC_TimeStruct.RTC_Minutes;
	pData[5]=RTC_TimeStruct.RTC_Seconds;
	pData[6]=RTC_DateStruct.RTC_WeekDay;
	pData[7]=RTC_TimeStruct.RTC_H12;

	return 0;
}


/***************************************************************************************
** 函数名称: STM32F4_RTC_Init
** 功能描述: 实时时钟初始化函数
** 参    数: pin	/ 加扰引脚对应的地址
** 参    数: pi		/ 中断优先级
** 返 回 值: None       
***************************************************************************************/
u8 STM32F4_RTC_Init(void)
{
	RTC_InitTypeDef 	RTC_IS;
	GPIO_InitTypeDef 	GPIO_IS;
	u32 TimesOut=0;
	u8	TryCnt = 10;
	
	/* 使能备用电池存储器 */		
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);//使能后备寄存器访问,使能RTC操作

	/*RTC加扰PWM引脚初始化*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_IS.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_IS.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIO_IS);
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=RTC_CONFIG_FLAG)
	{//第一次配置RTC_BKP
		RTC_DeInit();//RTC default
		RCC_LSEConfig(RCC_LSE_ON);//LSE 开启    
		while ((RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET) && (TryCnt>0))	//检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
			TimesOut=g_Sys_1ms_Counter;
			
			if (g_Para.RunningMode == ACC_RunningMode_SoftwareSimulat)/*纯软件仿真模式*/
				TimesOut=STM32_RTC_TIMEOUT;

			if(TimesOut>=STM32_RTC_TIMEOUT)
			{//pwm加扰,辅助32.768K时钟起振
				GPIOC->BSRRH=GPIO_Pin_13;//置"0"
				TimesOut=0;
				TryCnt--;
				Delay_1us(10);
				GPIOC->BSRRL=GPIO_Pin_13;//置"1"
			}
		}
		if(TryCnt==0)
			return 1;//LSE 开启失败. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 

    RTC_IS.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
    RTC_IS.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
    RTC_IS.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
    RTC_Init(&RTC_IS);
 
		if(RTC_Set_Time(17,31,00,RTC_H12_AM)==ERROR)//设置时间
			return 2;//时间设置失败
		if(RTC_Set_Date(15,3,24,2)==ERROR)//设置日期
			return 3;//日期设置失败

		RTC_WriteBackupRegister(RTC_BKP_DR0,RTC_CONFIG_FLAG);//写入配置完成标志,标记已经初始化过了
	}
	else
	{//已配置过BKP
		RTC_WaitForSynchro();//等待RTC APB寄存器同步
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除RTC闹钟标志
		EXTI_ClearITPendingBit(EXTI_Line17);//清除中断线17标志（内部连接至RTC闹钟）
		EXTI_ClearITPendingBit(EXTI_Line22);//清除中断线22标志（RTC Wakeup中断）
	}
 
	return 0;
}

/***************************************************************************************
** 函数名称: RTC_Set_AlarmA
** 功能描述: 设置闹钟时间(按星期闹铃,24小时制)
** 参    数: 
week:星期几(1~7) RTC_Alarm_Definitions
hour,min,sec:小时,分钟,秒钟
** 返 回 值: None       
***************************************************************************************/
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
	EXTI_InitTypeDef	EXTI_IS;
	RTC_AlarmTypeDef	RTC_AlarmIS;
	RTC_TimeTypeDef		RTC_TimeIS;
	NVIC_InitTypeDef	NVIC_RTC;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//关闭闹钟A 
	
  RTC_TimeIS.RTC_Hours=hour;//小时
	RTC_TimeIS.RTC_Minutes=min;//分钟
	RTC_TimeIS.RTC_Seconds=sec;//秒
	RTC_TimeIS.RTC_H12=RTC_H12_AM;
  
	RTC_AlarmIS.RTC_AlarmDateWeekDay=week;//星期
	RTC_AlarmIS.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//按星期闹
	RTC_AlarmIS.RTC_AlarmMask=RTC_AlarmMask_None;//精确匹配星期，时分秒
	RTC_AlarmIS.RTC_AlarmTime=RTC_TimeIS;
  RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmIS);
 
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);//清除RTC闹钟A的标志
  EXTI_ClearITPendingBit(EXTI_Line17);//清除LINE17上的中断标志位 
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//开启闹钟A 
	
	EXTI_IS.EXTI_Line = EXTI_Line17;//LINE17
  EXTI_IS.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_IS.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
  EXTI_IS.EXTI_LineCmd = ENABLE;//使能LINE17
  EXTI_Init(&EXTI_IS);//配置

	NVIC_RTC.NVIC_IRQChannel = RTC_Alarm_IRQn; 
  NVIC_RTC.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
  NVIC_RTC.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_RTC.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_RTC);//配置
}

/***************************************************************************************
** 函数名称: RTC_Set_WakeUp
** 功能描述: 周期性唤醒定时器设置 
** 参    数: 
wksel:  RTC_Wakeup_Timer_Definitions--->
					#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
					#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
					#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
					#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
					#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
					#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
cnt:		自动重装载值.减到0,产生中断.
** 返 回 值: 无       
***************************************************************************************/
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
	EXTI_InitTypeDef	EXTI_IS;
	NVIC_InitTypeDef	NVIC_RTC;
	
	RTC_WakeUpCmd(DISABLE);//关闭WAKE UP
	RTC_WakeUpClockConfig(wksel);//唤醒时钟选择
	RTC_SetWakeUpCounter(cnt);//设置WAKE UP自动重装载寄存器
	RTC_ClearITPendingBit(RTC_IT_WUT); //清除RTC WAKE UP的标志
  EXTI_ClearITPendingBit(EXTI_Line22);//清除LINE22上的中断标志位 
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//开启WAKE UP 定时器中断
	RTC_WakeUpCmd( ENABLE);//开启WAKE UP 定时器　
	
	EXTI_IS.EXTI_Line = EXTI_Line22;//LINE22
  EXTI_IS.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_IS.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
  EXTI_IS.EXTI_LineCmd = ENABLE;//使能LINE22
  EXTI_Init(&EXTI_IS);//配置
 
 
	NVIC_RTC.NVIC_IRQChannel = RTC_WKUP_IRQn; 
  NVIC_RTC.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
  NVIC_RTC.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_RTC.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_RTC);//配置
}

/***************************************************************************************
** 函数名称: RTC_Alarm_IRQHandler
** 功能描述: RTC闹钟中断服务函数
** 返 回 值: None       
***************************************************************************************/
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A中断?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断标志
		//..干相关事件
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//清除中断线17的中断标志 											 
}

/***************************************************************************************
** 函数名称: RTC_WKUP_IRQHandler
** 功能描述: RTC WAKE UP中断服务函数
** 返 回 值: None       
***************************************************************************************/
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP中断?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	//清除中断标志
		//..干相关事件
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);//清除中断线22的中断标志 								
}

 
/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/
