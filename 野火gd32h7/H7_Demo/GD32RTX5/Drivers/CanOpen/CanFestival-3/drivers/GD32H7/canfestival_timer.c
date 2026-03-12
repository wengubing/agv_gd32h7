/*
 * CanFestival timer driver for GD32H7
 *
 * Adapted from STM32 version.
 *
 * Uses TIMER2 (APB1 general-purpose timer) at 1μs resolution.
 *
 * Clock chain at 600MHz system clock:
 *   AHB  = 300MHz, APB1 = AHB/2 = 150MHz
 *   TIMER2_CLK = APB1 * 2 = 300MHz  (APB1 prescaler ≠ 1, timer input clock doubled)
 *   Prescaler for 1μs resolution: 300 - 1 = 299
 *   ARR = 0xFFFF (16-bit free-run counter)
 */

#include "gd32h7xx.h"
#include "../../include/GD32H7/canfestival.h"
#include "timer.h"
#include "../../example/LinkCanopenMaster.h"

/*****************************************************************************
 * Module variables
 *****************************************************************************/
/* Store the last timer value to calculate the elapsed time */
static TIMEVAL last_time_set = TIMEVAL_MAX;

/* TIMER2 配置参数:
 *   TIMER2_CLK = 300MHz (APB1=150MHz, timer input ×2)
 *   Prescaler  = 300 - 1 = 299  → 1μs per tick
 *   ARR        = 0xFFFF
 *   CH0 compare → CanFestival timeout interrupt
 */
#define CanFestivalTimer_PSC_Val    (300U - 1U)
#define CanFestivalTimer_ARR_Val    TIMEVAL_MAX
#define CanFestivalTimer_CCR_Val    1000U   /* initial compare value (1ms) */

/*
 * ============================================================================
 * 名称：setTimer
 * 功能：设置下次定时器中断触发时间
 * 参数：value -- 距当前计数值的偏移量 (单位: μs)
 * ============================================================================
 */
void setTimer(TIMEVAL value)
{
    UNS32 capture;

    capture = (UNS32)TIMER_CH0CV(CanFestivalTimer_Base);
    timer_channel_output_pulse_value_config(CanFestivalTimer_Base,
                                            TIMER_CH_0,
                                            (uint32_t)(capture + value));
}

/*
 * ============================================================================
 * 名称：getElapsedTime
 * 功能：获取自上次调度以来的经过时间
 * 返回：经过的时间 (单位: μs)
 * ============================================================================
 */
TIMEVAL getElapsedTime(void)
{
    UNS16 timer = (UNS16)timer_counter_read(CanFestivalTimer_Base);

    return (TIMEVAL)(timer >= last_time_set
                     ? timer - last_time_set
                     : last_time_set - timer);
}

/*
 * ============================================================================
 * 名称：CanFestivalTimer_Config
 * 功能：配置CanFestival定时器 (TIMER2)
 * 参数：pi        -- NVIC抢占优先级 (<0 不配置NVIC)
 *       NewState  -- ENABLE/DISABLE
 * ============================================================================
 */
void CanFestivalTimer_Config(uint8_t pi, FunctionalState NewState)
{
    timer_parameter_struct        timer_param;
    timer_oc_parameter_struct     oc_param;

    /* 使能TIMER2时钟 */
    rcu_periph_clock_enable(CanFestivalTimer_CLK);

    /* 复位定时器 */
    timer_deinit(CanFestivalTimer_Base);

    /* 基本定时器参数 */
    timer_struct_para_init(&timer_param);
    timer_param.prescaler         = CanFestivalTimer_PSC_Val;
    timer_param.alignedmode       = TIMER_COUNTER_EDGE;
    timer_param.counterdirection  = TIMER_COUNTER_UP;
    timer_param.period            = CanFestivalTimer_ARR_Val;
    timer_param.clockdivision     = TIMER_CKDIV_DIV1;
    timer_param.repetitioncounter = 0U;
    timer_init(CanFestivalTimer_Base, &timer_param);

    /* 输出比较通道0 — 仅用作软件比较，不输出引脚 */
    timer_channel_output_struct_para_init(&oc_param);
    oc_param.outputstate  = TIMER_CCX_ENABLE;
    oc_param.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    oc_param.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    oc_param.outputnstate = TIMER_CCXN_DISABLE;
    oc_param.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    oc_param.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(CanFestivalTimer_Base, TIMER_CH_0, &oc_param);

    timer_channel_output_mode_config(CanFestivalTimer_Base, TIMER_CH_0,
                                     TIMER_OC_MODE_TIMING);
    timer_channel_output_pulse_value_config(CanFestivalTimer_Base, TIMER_CH_0,
                                            CanFestivalTimer_CCR_Val);
    timer_channel_output_shadow_config(CanFestivalTimer_Base, TIMER_CH_0,
                                       TIMER_OC_SHADOW_DISABLE);

    /* 配置NVIC */
    if (pi != 0xFFU) {
        nvic_irq_enable(CanFestivalTimer_IRQn, pi, 0U);
    }

    /* 清除中断标志，使能通道0比较中断 */
    timer_interrupt_flag_clear(CanFestivalTimer_Base, CanFestivalTimer_FLAG);
    timer_interrupt_enable(CanFestivalTimer_Base, CanFestivalTimer_IT);

    /* 复位计数器 */
    timer_counter_value_config(CanFestivalTimer_Base, 0U);

    /* 使能或禁止定时器 */
    if (NewState == ENABLE) {
        timer_enable(CanFestivalTimer_Base);
    } else {
        timer_disable(CanFestivalTimer_Base);
    }
}

/*
 * ============================================================================
 * 名称：CanFestivalTimer_Enable
 * 功能：启动/停止定时器
 * ============================================================================
 */
void CanFestivalTimer_Enable(FunctionalState NewState)
{
    if (NewState == ENABLE) {
        timer_enable(CanFestivalTimer_Base);
    } else {
        timer_disable(CanFestivalTimer_Base);
    }
}

/*
 * ============================================================================
 * 名称：CanFestivalTimer_Ctrl
 * 功能：使能/禁止定时器通道0比较中断
 * ============================================================================
 */
void CanFestivalTimer_Ctrl(FunctionalState NewState)
{
    if (NewState == ENABLE) {
        timer_interrupt_enable(CanFestivalTimer_Base, CanFestivalTimer_IT);
    } else {
        timer_interrupt_disable(CanFestivalTimer_Base, CanFestivalTimer_IT);
    }
}

/*
 * ============================================================================
 * 名称：CanFestivalTimer_Dispatch
 * 功能：更新last_time_set并调用CanFestival时间调度
 * ============================================================================
 */
void CanFestivalTimer_Dispatch(void)
{
    last_time_set = (UNS16)timer_counter_read(CanFestivalTimer_Base);
    TimeDispatch();  /* 在每次定时器到期时调用 */
}

/*
 * ============================================================================
 * 名称：CanFestivalTimer_IRQHandler  (TIMER2_IRQHandler)
 * 功能：TIMER2中断处理函数
 * ============================================================================
 */
void CanFestivalTimer_IRQHandler(void)
{
    if (timer_interrupt_flag_get(CanFestivalTimer_Base, CanFestivalTimer_FLAG) != RESET) {
        timer_interrupt_flag_clear(CanFestivalTimer_Base, CanFestivalTimer_FLAG);
        CanFestivalTimer_Dispatch();
    }
}
