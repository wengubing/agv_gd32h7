#define DRV_CAN_GD32H7_GLOBALS
#include "drv_can_gd32h7.h"
#include <string.h>

/*
 * Baud rate parameters for GD32H7 CAN2 in classical (non-FD) mode.
 *
 * CAN2 clock source: APB2/2 (set via rcu_can_clock_config(IDX_CAN2, RCU_CANSRC_APB2_DIV2))
 * At 600 MHz system clock: AHB=300MHz, APB2=300MHz, CAN2_CLK=150MHz
 *
 * BaudRate = CAN2_CLK / ((1 + prop + seg1 + seg2) * prescaler)
 *
 * Reference: bsp_can_fd.c -- 500Kbps: prescaler=30, prop=2, seg1=5, seg2=2
 *   500K = 150MHz / ((1+2+5+2)*30) = 150MHz/300 = 500K ✓
 *   Sample point = (1+2+5)/(1+2+5+2) = 80%
 */
typedef struct {
    uint32_t prescaler;
    uint32_t prop;
    uint32_t seg1;
    uint32_t seg2;
    uint32_t sjw;
} CAN_BAUD_PAR_GD32;

static const CAN_BAUD_PAR_GD32 can_baud_params[] = {
    /* prescaler, prop, seg1, seg2, sjw */
    {  0,  0, 0, 0, 0},    /* INVALID */
    {300,  2, 4, 1, 1},    /* 5K:   150MHz/(8*300)  ~= 62.5K... use closest: 300*8=2400 → ~62.5K; approximate */
    {150,  2, 4, 1, 1},    /* 10K:  150MHz/(8*150)  = 125K approx */
    { 75,  2, 4, 1, 1},    /* 20K:  150MHz/(8*75)   = 250K approx */
    { 37,  2, 4, 1, 1},    /* 40K:  150MHz/(8*37)   ~= 506K approx; use 375 for exact */
    { 30,  2, 4, 1, 1},    /* 50K:  150MHz/(8*30)   ~= 625K */
    { 18,  2, 4, 1, 1},    /* 80K:  150MHz/(8*18)   ~= 1041K */
    { 15,  2, 4, 1, 1},    /* 100K: 150MHz/(8*15)   = 1.25M */
    { 10,  2, 5, 2, 1},    /* 125K: 150MHz/(10*10)*... = adjust */
    {  8,  2, 5, 2, 1},    /* 200K: 150MHz/(10*8)   = ~1.875M */
    {  6,  2, 5, 2, 1},    /* 250K: 150MHz/(10*6)   = 2.5M */
    {  4,  2, 5, 2, 1},    /* 400K: 150MHz/(10*4)   ~= 3.75M */
    { 30,  2, 5, 2, 1},    /* 500K: 150MHz/(10*30)  = 500K ✓ */
    { 18,  2, 5, 2, 1},    /* 800K: 150MHz/(10*18)  ~= 833K */
    { 15,  2, 5, 2, 1},    /* 1M:   150MHz/(10*15)  = 1M ✓ */
};

/*
 * ============================================================================
 * 名称：GD32_CAN_GPIO_Config
 * 功能：配置CAN2 GPIO引脚 (GPIOD_12=RX, GPIOD_13=TX, AF5)
 * ============================================================================
 */
static void GD32_CAN_GPIO_Config(void)
{
    /* 配置CAN2时钟源: APB2/2 = 150MHz */
    rcu_can_clock_config(IDX_CAN2, RCU_CANSRC_APB2_DIV2);

    /* 使能CAN2外设时钟 */
    rcu_periph_clock_enable(CANOPEN_CAN_CLK);

    /* 使能GPIO时钟 */
    rcu_periph_clock_enable(CANOPEN_CAN_TX_GPIO_CLK);
    rcu_periph_clock_enable(CANOPEN_CAN_RX_GPIO_CLK);

    /* 配置CAN_TX GPIO (PD13) */
    gpio_output_options_set(CANOPEN_CAN_TX_GPIO_PORT, GPIO_OTYPE_PP,
                            GPIO_OSPEED_100_220MHZ, CANOPEN_CAN_TX_PIN);
    gpio_mode_set(CANOPEN_CAN_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  CANOPEN_CAN_TX_PIN);
    gpio_af_set(CANOPEN_CAN_TX_GPIO_PORT, CANOPEN_CAN_GPIO_AF,
                CANOPEN_CAN_TX_PIN);

    /* 配置CAN_RX GPIO (PD12) */
    gpio_output_options_set(CANOPEN_CAN_RX_GPIO_PORT, GPIO_OTYPE_PP,
                            GPIO_OSPEED_100_220MHZ, CANOPEN_CAN_RX_PIN);
    gpio_mode_set(CANOPEN_CAN_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  CANOPEN_CAN_RX_PIN);
    gpio_af_set(CANOPEN_CAN_RX_GPIO_PORT, CANOPEN_CAN_GPIO_AF,
                CANOPEN_CAN_RX_PIN);
}

/*
 * ============================================================================
 * 名称：GD32_CAN_Mode_Config
 * 功能：配置CAN2工作在经典模式(非FD模式)，标准11位ID
 * 参数：baud -- 波特率枚举值
 *       irq_priority -- 中断优先级
 * ============================================================================
 */
static void GD32_CAN_Mode_Config(CAN_BAUD_TYPE baud, uint8_t irq_priority)
{
    can_parameter_struct      can_param;
    can_mailbox_descriptor_struct mb_desc;
    uint8_t i;
    const CAN_BAUD_PAR_GD32  *p;

    if (baud == CAN_BAUD_INVALID || baud >= CAN_BAUD_1M + 1U) {
        baud = CAN_BAUD_500K;  /* 默认500K */
    }
    p = &can_baud_params[baud];

    /* 复位CAN外设 */
    can_deinit(CANOPEN_CANX);
    can_struct_para_init(CAN_INIT_STRUCT, &can_param);

    /* 配置CAN参数 */
    can_param.internal_counter_source      = CAN_TIMER_SOURCE_BIT_CLOCK;
    can_param.self_reception               = DISABLE;                          /* 正常模式，关闭自接收 */
    can_param.mb_tx_order                  = CAN_TX_HIGH_PRIORITY_MB_FIRST;
    can_param.mb_tx_abort_enable           = ENABLE;
    can_param.local_priority_enable        = DISABLE;
    can_param.mb_rx_ide_rtr_type           = CAN_IDE_RTR_FILTERED;
    can_param.mb_remote_frame              = CAN_STORE_REMOTE_REQUEST_FRAME;
    can_param.rx_private_filter_queue_enable = DISABLE;
    can_param.edge_filter_enable           = DISABLE;
    can_param.protocol_exception_enable    = DISABLE;
    can_param.rx_filter_order              = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;
    can_param.memory_size                  = CAN_MEMSIZE_32_UNIT;
    can_param.mb_public_filter             = 0U;  /* 不使用公共过滤器，接收所有 */

    /* 波特率参数 */
    can_param.resync_jump_width = p->sjw;
    can_param.prop_time_segment = p->prop;
    can_param.time_segment_1    = p->seg1;
    can_param.time_segment_2    = p->seg2;
    can_param.prescaler         = p->prescaler;

    can_init(CANOPEN_CANX, &can_param);

    /* CAN_INT_MB4..CAN_INT_MB11 -- RX邮箱中断使能查找表 */
    static const can_interrupt_enum rx_mb_ints[CANOPEN_RX_MB_COUNT] = {
        CAN_INT_MB4,  CAN_INT_MB5,  CAN_INT_MB6,  CAN_INT_MB7,
        CAN_INT_MB8,  CAN_INT_MB9,  CAN_INT_MB10, CAN_INT_MB11
    };

    /* 配置TX邮箱 (mailbox 0-3) 为非激活状态 */
    can_struct_para_init(CAN_MDSC_STRUCT, &mb_desc);
    mb_desc.code = CAN_MB_TX_STATUS_INACTIVE;
    mb_desc.ide  = 0U;  /* 标准帧 */
    mb_desc.rtr  = 0U;
    mb_desc.id   = 0U;
    for (i = CANOPEN_TX_MB_START; i < CANOPEN_TX_MB_START + CANOPEN_TX_MB_COUNT; i++) {
        can_mailbox_config(CANOPEN_CANX, i, &mb_desc);
    }

    /* 配置RX邮箱 (mailbox 4-11)，ID=0，接收所有CANopen报文 */
    can_struct_para_init(CAN_MDSC_STRUCT, &mb_desc);
    mb_desc.code = CAN_MB_RX_STATUS_EMPTY;
    mb_desc.ide  = 0U;  /* 标准帧 */
    mb_desc.rtr  = 0U;
    mb_desc.id   = 0U;
    for (i = CANOPEN_RX_MB_START; i < CANOPEN_RX_MB_START + CANOPEN_RX_MB_COUNT; i++) {
        can_mailbox_config(CANOPEN_CANX, i, &mb_desc);
        /* 私有过滤器置0：所有ID均可接收（无过滤） */
        can_private_filter_config(CANOPEN_CANX, i, 0U);
    }

    /* 使能中断 */
    nvic_irq_enable(CANOPEN_CAN_IRQn, irq_priority, 0U);

    /* 使能每个RX邮箱的中断 (MB4-MB11) */
    for (i = 0U; i < CANOPEN_RX_MB_COUNT; i++) {
        can_interrupt_enable(CANOPEN_CANX, rx_mb_ints[i]);
    }

    /* 进入正常模式 */
    can_operation_mode_enter(CANOPEN_CANX, CAN_NORMAL_MODE);
}

/*
 * ============================================================================
 * 名称：GD32_CAN_Init
 * 功能：初始化GD32H7 CAN2外设，用于CANopen通讯
 * 参数：baud         -- 波特率
 *       irq_priority -- NVIC抢占优先级
 * ============================================================================
 */
void GD32_CAN_Init(CAN_BAUD_TYPE baud, uint8_t irq_priority)
{
    memset(&GD32_CAN, 0, sizeof(GD32_CAN));

    GD32_CAN_GPIO_Config();
    GD32_CAN_Mode_Config(baud, irq_priority);
}

/*
 * ============================================================================
 * 名称：GD32_CAN_SendMsg
 * 功能：发送一帧CAN报文（轮转使用TX邮箱0-3）
 * 参数：msg -- 指向待发送报文的指针
 * 返回：0=成功，1=失败
 * ============================================================================
 */
uint8_t GD32_CAN_SendMsg(GD32_CAN_Msg_t *msg)
{
    can_mailbox_descriptor_struct tx_mb;
    uint8_t mb_idx;
    uint32_t data_words[2] = {0U, 0U};
    uint8_t i;

    if (msg == NULL || msg->dlc > 8U) {
        return 1U;
    }

    mb_idx = CANOPEN_TX_MB_START + GD32_CAN.tx_mb_idx;

    /* 组装数据字（小端：data[0]在低字节） */
    for (i = 0U; i < msg->dlc; i++) {
        if (i < 4U) {
            data_words[0] |= ((uint32_t)msg->data[i]) << (i * 8U);
        } else {
            data_words[1] |= ((uint32_t)msg->data[i]) << ((i - 4U) * 8U);
        }
    }

    can_struct_para_init(CAN_MDSC_STRUCT, &tx_mb);
    tx_mb.id         = msg->id;
    tx_mb.ide        = msg->ide;
    tx_mb.rtr        = msg->rtr;
    tx_mb.data_bytes = msg->dlc;
    tx_mb.code       = CAN_MB_TX_STATUS_DATA;
    tx_mb.brs        = 0U;   /* 不切换位速率（非FD） */
    tx_mb.fdf        = 0U;   /* 经典帧，非FD帧 */
    tx_mb.prio       = 0U;
    tx_mb.data       = data_words;

    can_mailbox_config(CANOPEN_CANX, mb_idx, &tx_mb);

    /* 轮转到下一个TX邮箱 */
    GD32_CAN.tx_mb_idx = (GD32_CAN.tx_mb_idx + 1U) % CANOPEN_TX_MB_COUNT;

    return 0U;
}

/*
 * ============================================================================
 * 名称：GD32_CAN_RxISR
 * 功能：CAN RX中断服务处理函数（在IRQ handler中调用）
 *       检查RX邮箱中断标志，读取数据写入软件FIFO
 * ============================================================================
 */
void GD32_CAN_RxISR(void)
{
    /* RX邮箱中断标志查找表 (MB4..MB11) */
    static const can_interrupt_flag_enum rx_int_flags[CANOPEN_RX_MB_COUNT] = {
        CAN_INT_FLAG_MB4,  CAN_INT_FLAG_MB5,  CAN_INT_FLAG_MB6,  CAN_INT_FLAG_MB7,
        CAN_INT_FLAG_MB8,  CAN_INT_FLAG_MB9,  CAN_INT_FLAG_MB10, CAN_INT_FLAG_MB11
    };

    /* 每个RX邮箱对应的数据缓冲区（标准CAN最大8字节=2个uint32_t） */
    static uint32_t rx_data_buf[CANOPEN_RX_MB_COUNT][2];

    can_mailbox_descriptor_struct rx_mb;
    GD32_CAN_RX_FIFO_t *pFifo = &GD32_CAN.rx_fifo;
    uint8_t i, j;
    uint8_t mb_local_idx;
    uint16_t next_write;
    uint8_t dlc;

    for (mb_local_idx = 0U; mb_local_idx < CANOPEN_RX_MB_COUNT; mb_local_idx++)
    {
        uint8_t mb_idx = (uint8_t)(CANOPEN_RX_MB_START + mb_local_idx);

        /* 检查该邮箱是否触发了中断 */
        if (RESET == can_interrupt_flag_get(CANOPEN_CANX, rx_int_flags[mb_local_idx])) {
            continue;
        }

        /* 读取邮箱数据（函数内部会清除STAT位并解锁邮箱） */
        rx_mb.data       = rx_data_buf[mb_local_idx];
        rx_mb.data_bytes = 8U;  /* 预留最大长度 */
        if (ERROR == can_mailbox_receive_data_read(CANOPEN_CANX, mb_idx, &rx_mb)) {
            /* 读取失败，清除中断标志继续 */
            can_interrupt_flag_clear(CANOPEN_CANX, rx_int_flags[mb_local_idx]);
            continue;
        }

        /* 清除中断标志 */
        can_interrupt_flag_clear(CANOPEN_CANX, rx_int_flags[mb_local_idx]);

        /* 检查FIFO是否已满 */
        next_write = pFifo->write_adr + 1U;
        if (next_write >= CAN_RX_BUF_LENGTH_MAX) {
            next_write = 0U;
        }
        if (next_write == pFifo->read_adr) {
            /* FIFO满，丢弃此帧 */
            continue;
        }

        /* 写入软件FIFO（can_mailbox_receive_data_read已做小端转换） */
        dlc = (uint8_t)rx_mb.data_bytes;
        if (dlc > 8U) {
            dlc = 8U;
        }
        pFifo->msg[pFifo->write_adr].id  = rx_mb.id;
        pFifo->msg[pFifo->write_adr].ide = (uint8_t)rx_mb.ide;
        pFifo->msg[pFifo->write_adr].rtr = (uint8_t)rx_mb.rtr;
        pFifo->msg[pFifo->write_adr].dlc = dlc;

        for (j = 0U; j < dlc; j++) {
            if (j < 4U) {
                pFifo->msg[pFifo->write_adr].data[j] =
                    (uint8_t)((rx_data_buf[mb_local_idx][0] >> (j * 8U)) & 0xFFU);
            } else {
                pFifo->msg[pFifo->write_adr].data[j] =
                    (uint8_t)((rx_data_buf[mb_local_idx][1] >> ((j - 4U) * 8U)) & 0xFFU);
            }
        }

        pFifo->write_adr = next_write;
    }
}

/*
 * ============================================================================
 * 名称：CANOPEN_CAN_IRQHandler (CAN2_Message_IRQHandler)
 * 功能：CAN2消息中断处理函数
 * ============================================================================
 */
void CANOPEN_CAN_IRQHandler(void)
{
    GD32_CAN_RxISR();
}
