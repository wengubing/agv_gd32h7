#include "LinkCanopenMaster.h"
#include "canfestival.h"
#include "timer.h"
#include "gd32h7xx_misc.h"
#include "gd32h7xx_rcu.h"
#include "gd32h7xx_timer.h"

/*
 * LinkCanopenMaster.h 中原先保留的是 STM32 工程里的定时器宏定义。
 * 这里在 GD32 驱动文件内先取消旧宏，再重新映射到 GD32H7 的标准库对象，
 * 这样可以尽量少改上层 CanFestival 代码。
 */
#ifdef CanFestivalTimer_Base
#undef CanFestivalTimer_Base
#endif
#ifdef CanFestivalTimer_CLK
#undef CanFestivalTimer_CLK
#endif
#ifdef CanFestivalTimer_IT
#undef CanFestivalTimer_IT
#endif
#ifdef CanFestivalTimer_FLAG
#undef CanFestivalTimer_FLAG
#endif
#ifdef CanFestivalTimer_IRQn
#undef CanFestivalTimer_IRQn
#endif
#ifdef CanFestivalTimer_IRQHandler
#undef CanFestivalTimer_IRQHandler
#endif

/*
 * GD32H7 平台下 CanFestival 定时器资源映射：
 * Base : 使用 TIMER4 作为 CanFestival 调度定时器
 * CLK  : TIMER4 对应的外设时钟使能位
 * IT   : 使用通道0比较中断，等效于 STM32 里的 CC1 中断
 * FLAG : 通道0比较中断标志位
 * IRQn : TIMER4 对应的 NVIC 中断号
 */
#define		CanFestivalTimer_Base			TIMER4
#define		CanFestivalTimer_CLK			RCU_TIMER4
#define		CanFestivalTimer_IT				TIMER_INT_CH0
#define		CanFestivalTimer_FLAG			TIMER_INT_FLAG_CH0
#define		CanFestivalTimer_IRQn			TIMER4_IRQn
#define		CanFestivalTimer_IRQHandler	    TIMER4_IRQHandler

/************************** Modul variables **********************************/
// Store the last timer value to calculate the elapsed time
static TIMEVAL last_time_set = TIMEVAL_MAX;//上一次的时间计数


/*2015-12-22：新增,by dengyang */
#define		CanFestivalTimer_CCRX_Val		1000						
#define		CanFestivalTimer_ARR_Val		TIMEVAL_MAX
#define		CanFestivalTimer_PSC_Val		300							// 300M/1000000 --1us  (TIMER4时钟=APB1×2=150MHz×2=300MHz，SYSCLK=600MHz，AHB/2，APB1/2)




/*用来定时的,时间到了就需要调用一下函数TimeDispatch()
value:Set time value 0x0000-0xffff
*/
void setTimer(TIMEVAL value)
{
	/*
	 * 读取当前 CH0 的比较寄存器值（上一轮触发基准点），
	 * 再叠加 value 得到“下一次”比较触发时刻。
	 *
	 * 这样做可以保持周期调度的连续性，避免直接用 CNT 作为基准时
	 * 因为函数执行抖动引入的累计误差。
	 */
	UNS32 capture = timer_channel_capture_value_register_read(CanFestivalTimer_Base, TIMER_CH_0);

	/*
	 * 更新 CH0 比较值：CNT 计数到该值时触发比较中断。
	 * 定时器硬件会在计数回卷时自然处理溢出。
	 */
	timer_channel_output_pulse_value_config(CanFestivalTimer_Base, TIMER_CH_0, capture + value);
}

/*查询距离下一个定时触发还有多少时间
TIMEVAL:Return current timer value
*/
TIMEVAL getElapsedTime(void)
{
	/* 读取当前计数器值（自由运行计数） */
	TIMEVAL timer = (TIMEVAL)timer_counter_read(CanFestivalTimer_Base);
	
	/*
	 * 与上次调度记录点做差，得到 elapsed 时间。
	 * 这里保持与 STM32 参考实现一致的计算方式。
	 */
	timer = (timer >= last_time_set) ? (timer - last_time_set) : (last_time_set - timer);
	
	return timer;
}

/*canfestival定时器配置*/
void CanFestivalTimer_Config()
{
    /*
     * timer_initpara    : 定时器基本计数单元配置结构体
     * timer_ocintpara   : 输出比较通道配置结构体
     *
     * 当前思路与 STM32 参考工程保持一致：
     * 1. 先初始化一个自由运行的基础定时器
     * 2. 再把 CH0 配成 Output Compare Timing 模式
     * 3. 当计数器 CNT 到达比较值 CH0CV 时产生比较中断
     * 4. 中断中再调用 CanFestival 的调度函数 TimeDispatch()
     */
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocintpara;

    /* 使能 TIMER4 外设时钟，否则后续寄存器配置不会生效 */
    rcu_periph_clock_enable(CanFestivalTimer_CLK);

    /*
     * 复位定时器外设到默认状态，避免受到上一次初始化或其他模块配置影响。
     * 这一步相当于 STM32 工程里的 TIM_DeInit()。
     */
    timer_deinit(CanFestivalTimer_Base);

    /*
     * 先装载库默认值，再覆盖为 CanFestival 需要的参数，便于保证结构体其余成员处于安全状态。
     */
    timer_struct_para_init(&timer_initpara);

    /*
     * 预分频值：80 - 1
     * 若定时器输入时钟为 80MHz，则分频后计数时钟为 1MHz，
     * 即计数器每加 1 表示 1us。
     */
    timer_initpara.prescaler = (uint16_t)(CanFestivalTimer_PSC_Val - 1U);

    /* 边沿对齐计数模式，普通向上计数定时器使用该模式即可 */
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;

    /* 向上计数：CNT 从 0 递增到自动重装值 period */
    timer_initpara.counterdirection = TIMER_COUNTER_UP;

    /* 自动重装值 ARR，决定计数器的回卷上限 */
    timer_initpara.period = CanFestivalTimer_ARR_Val;

    /* 时钟分频因子选择 1，不再对定时器数字滤波时钟做额外分频 */
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;

    /* 高级定时器才会用到重复计数，这里保持 0 即可 */
    timer_initpara.repetitioncounter = 0U;

    /* 将上述基础参数写入 TIMER4 */
    timer_init(CanFestivalTimer_Base, &timer_initpara);

    /*
     * 配置输出比较通道 0：
     * CanFestival 这里只需要“比较到点后产生中断”，
     * 不需要真正输出 PWM 波形，因此使用 Timing 模式。
     */
    timer_channel_output_struct_para_init(&timer_ocintpara);

    /* 使能主通道输出状态，允许 CH0 比较功能工作 */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;

    /* 不使用互补输出通道 */
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;

    /* 主通道输出极性高电平，仅保持默认标准配置 */
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;

    /* 互补通道未使用，这里保持默认高极性 */
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;

    /* 空闲状态输出低电平 */
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;

    /* 互补通道空闲状态输出低电平 */
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    /* 将输出比较参数配置到 TIMER4 的通道0 */
    timer_channel_output_config(CanFestivalTimer_Base, TIMER_CH_0, &timer_ocintpara);

    /*
     * 通道0工作在 Timing 模式：
     * 当 CNT == CH0CV 时，仅产生比较事件/中断，不驱动实际波形翻转。
     */
    timer_channel_output_mode_config(CanFestivalTimer_Base, TIMER_CH_0, TIMER_OC_MODE_TIMING);

    /*
     * 设置首次比较值。
     * 这里先给一个默认初值 1000，对应 1000us。
     * 后续 CanFestival 会通过 setTimer() 动态修改下一次比较触发时间。
     */
    timer_channel_output_pulse_value_config(CanFestivalTimer_Base, TIMER_CH_0, CanFestivalTimer_CCRX_Val);

    /*
     * 关闭比较寄存器影子功能，保证软件写入的新比较值立即生效，
     * 这与 STM32 参考代码中关闭 OC preload 的目的相同。
     */
    timer_channel_output_shadow_config(CanFestivalTimer_Base, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /*
     * 配置 NVIC：
     * 抢占优先级设为 1，子优先级设为 0。
     * 这样定时器比较事件到来后，CPU 才会进入 TIMER4 的中断服务函数。
     */
    nvic_irq_enable(CanFestivalTimer_IRQn, 1U, 0U);

    /* 先清除可能残留的比较中断标志，避免一使能中断就误进入 ISR */
    timer_interrupt_flag_clear(CanFestivalTimer_Base, CanFestivalTimer_FLAG);

    /* 使能通道0比较中断 */
    timer_interrupt_enable(CanFestivalTimer_Base, CanFestivalTimer_IT);

    /* 将计数器清零，从 0 开始重新计时 */
    timer_counter_value_config(CanFestivalTimer_Base, 0U);

    /* 启动定时器，开始运行计数 */
    timer_enable(CanFestivalTimer_Base);

    /*停止定时器*/
	timer_disable(CanFestivalTimer_Base);
}

void CanFestivalTimer_Enable(ControlStatus NewState)
{
	/*
	 * 兼容 STM32 版本的启停接口：
	 * ENABLE  -> 启动 TIMER4 计数
	 * DISABLE -> 停止 TIMER4 计数
	 */
	if(NewState)
	{
		timer_enable(CanFestivalTimer_Base);
	}
	else
	{
		timer_disable(CanFestivalTimer_Base);
	}
}


void CanFestivalTimer_Ctrl(ControlStatus NewState)
{
	/*
	 * 兼容 STM32 版本的中断开关接口：
	 * ENABLE  -> 使能 CH0 比较中断
	 * DISABLE -> 关闭 CH0 比较中断
	 */
	if(NewState)
	{
		timer_interrupt_enable(CanFestivalTimer_Base, CanFestivalTimer_IT);
	}
	else
	{
		timer_interrupt_disable(CanFestivalTimer_Base, CanFestivalTimer_IT);
	}
}


void CanFestivalTimer_Dispatch(void)
{
	/*
	 * 记录本次中断到来时的计数器值，
	 * 给 getElapsedTime() 提供“上一次调度时间基准”。
	 */
	last_time_set = (TIMEVAL)timer_counter_read(CanFestivalTimer_Base);

	/*
	 * CanFestival 核心时间调度入口：
	 * 到点后由它驱动报警、心跳、PDO/SDO 超时等内部定时事件。
	 */
	TimeDispatch();
}

//===========================================
// 名称：CanFestivalTimer_IRQHandler
// 功能：CanFestival定时器调度中断服务程序
// 参数：无
// 返回：无
// 说明：无
//============================================================================
void CanFestivalTimer_IRQHandler(void)
{
	/*
	 * 仅处理 CH0 比较中断标志：
	 * 防止误处理其他来源的 TIMER4 中断。
	 */
	if (SET == timer_interrupt_flag_get(CanFestivalTimer_Base, CanFestivalTimer_FLAG))
	{
		/* 先清标志再调度，避免中断重入 */
		timer_interrupt_flag_clear(CanFestivalTimer_Base, CanFestivalTimer_FLAG);

//		isr_sem_send (&sem_CanOpenDispatch);//传递调度任务信号事件,由RTOS去调度执行
		CanFestivalTimer_Dispatch();
	}
}


