
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

#include "..\..\Drivers\inc\drv_WatchDog.h"
#include "..\..\Drivers\inc\drv_timer.h"

/* Private typedef -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

//============================================================================
// 名称：ExWatchdogFeed
// 功能：外部看门狗喂狗( 翻转指定IO给外部看门狗计数脉冲)
// 参数：无
// 返回：无
// 说明：放在定主程序里面周期性调用(推荐间隔2ms执行一次此函数)
//============================================================================
void ExWatchdogFeed(void)
{
		GPIO_PORT_EX_WATCHDOG->ODR^=GPIO_PIN_EX_WATCHDOG;
}

//============================================================================
// 名称：watchdog_init
// 功能：外部开门狗配置
// 参数：
// 返回：
// 说明：此函数应该放在MCU成功初始化后进行配置调用
//============================================================================
void ExtWatchdog_init(void)
{
 GPIO_InitTypeDef GPIO_IS;

 RCC_AHB1PeriphClockCmd(GPIO_RCC_EX_WATCHDOG | GPIO_RCC_DType_CLR, ENABLE);

 GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_IS.GPIO_OType = GPIO_OType_PP;
 GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;

 GPIO_IS.GPIO_Pin = GPIO_PIN_EX_WATCHDOG;
 GPIO_Init(GPIO_PORT_EX_WATCHDOG, &GPIO_IS);
 
 GPIO_IS.GPIO_Pin = GPIO_PIN_DType_CLR;
 GPIO_Init(GPIO_PORT_DType_CLR, &GPIO_IS);
 
 GPIO_PORT_EX_WATCHDOG->BSRRH=GPIO_PIN_EX_WATCHDOG;
 
 /*清D_Type输出端口状态*/
 //备注:在此清D触发器输出状态,有几点需要注意:
 //1.如果每次上电复位后清状态会,会导致一个问题:MCU复位指示状态不会保持,没有警示目的;
 //2.若果每次上电复位,需要判断是异常复位还是正常上电复位:
 //①异常复位,就不能清此标志,需要故障解除后,才允许清此D触发器输出状态;
 //②正常上电复位,就可以立即清标识，不需要状态解除;
 //3.判断是否那种复位结果可以使用BKP掉电备份区域去检测...
 GPIO_PORT_DType_CLR->BSRRH=GPIO_PIN_DType_CLR;
 Delay_1us(200);
 GPIO_PORT_DType_CLR->BSRRL=GPIO_PIN_DType_CLR;
}

//============================================================================
// 名称：watchdog_init
// 功能：初始化看门狗
// 参数：nms：看门狗复位计数时间，单位ms，范围10ms~4s
// 返回：
// 说明：看门狗一旦启动是不能停止的
//============================================================================
void watchdog_init(u16 nms)
{
    u16 reload_value;

    if ( nms < 10 )//限制喂狗间隔范围
    {
        nms = 10;
    }
    if ( nms > 4000 )
    {
        nms = 4000;
    }
		
//		/* 检测系统是否从独立看门狗复位中恢复 */
//		if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
//		{		
//			/* 清除复位标志 */
//			RCC_ClearFlag();
//		}
//		else
//		{
//			/* 标志没有设置 */
//		}
//		
//		/* 使能LSI */
//		RCC_LSICmd(ENABLE);
//		
//		/* 等待直到LSI就绪 */
//		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
//		{}
		
    reload_value = nms;//计算计数器重装值
    IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
    IWDG_SetPrescaler( IWDG_Prescaler_32 );//32kHz/32=1000Hz
		/*特别注意，由于这里_ulIWDGTime的最小单位是ms, 所以这里重装计数的
			计数时 需要除以1000
		 Counter reload_value = (_ulIWDGTime / 1000) /(1 / IWDG counter clock period)
													= (_ulIWDGTime / 1000) / (32/LSI)
													= (_ulIWDGTime / 1000) / (32/LsiFreq)
													= LsiFreq * _ulIWDGTime / 32000
		 实际测试LsiFreq = 34000，所以这里取1的时候 大概就是1ms 
		*/
    IWDG_SetReload( reload_value );//设置计数器重装值
    IWDG_ReloadCounter();
    DBGMCU_APB1PeriphConfig( DBGMCU_IWDG_STOP, ENABLE );// 调试时看门狗定时器停止计数
    IWDG_Enable();

#ifdef EXT_WatchDog_FUNC

  ExtWatchdog_init();

#endif

}

//============================================================================
// 名称：watchdog_feed
// 功能：看门狗喂狗
// 参数：无
// 返回：无
// 说明：超过时间不喂狗将引起看门狗复位
//============================================================================
void watchdog_feed(void)
{
	IWDG_ReloadCounter();
	
#ifdef EXT_WatchDog_FUNC

  ExWatchdogFeed();

#endif

}

