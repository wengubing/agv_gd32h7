/**
20260305，根据野火gd32 can_fd例程修改
  */ 
  
#include "bsp_can_fd.h" 
  
/**
  * @brief  配置CAN用到的I/O口
  * @param  无
  * @retval 无
  */
void CAN_GPIO_Config(void)
{
    /* 配置CAN时钟源 */
    rcu_can_clock_config(IDX_CAN2, RCU_CANSRC_APB2_DIV2);
    
    /* 启用CAN时钟 */
    rcu_periph_clock_enable(CAN_CLK);
    
    /* 启用CAN相关GPIO端口时钟 */
    rcu_periph_clock_enable(CAN_TX_GPIO_CLK);
    rcu_periph_clock_enable(CAN_RX_GPIO_CLK); 
    
    /* 配置CAN_TX GPIO */
    gpio_output_options_set(CAN_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, CAN_TX_PIN);
    gpio_mode_set(CAN_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, CAN_TX_PIN);
    gpio_af_set(CAN_TX_GPIO_PORT, CAN_TX_RX_GPIO_AF, CAN_TX_PIN);
    
    /* 配置CAN_RX GPIO */
    gpio_output_options_set(CAN_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, CAN_RX_PIN);
    gpio_mode_set(CAN_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, CAN_RX_PIN);
    gpio_af_set(CAN_RX_GPIO_PORT, CAN_TX_RX_GPIO_AF, CAN_RX_PIN);
   
}

/**
  * @brief  CAN的模式配置
  * @param  无
  * @retval 无
  */
static void CAN_Mode_Config(void)
{
    can_parameter_struct can_parameter;

    /* 初始化CAN寄存器 */
    can_deinit(CANX);
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);

    /* 初始化 CAN 单元 */
    can_parameter.internal_counter_source = CAN_TIMER_SOURCE_BIT_CLOCK;
    can_parameter.self_reception = ENABLE;                                  //使能自接收
    can_parameter.mb_tx_order = CAN_TX_HIGH_PRIORITY_MB_FIRST;              //邮箱发送顺序：最高优先级 邮箱优先
    can_parameter.mb_tx_abort_enable = ENABLE;                              //使能发送中止
    can_parameter.local_priority_enable = DISABLE;                          //禁能本地优先级
    can_parameter.mb_rx_ide_rtr_type = CAN_IDE_RTR_FILTERED;                //邮箱接收时 IDE 和 RTR 域的过滤类型：过滤IDE和RTR字段
    can_parameter.mb_remote_frame = CAN_STORE_REMOTE_REQUEST_FRAME;         //远程请求帧作为数据帧存储，不传输自动远程响应帧
    can_parameter.rx_private_filter_queue_enable = DISABLE;                 //不使能接收私有过滤使能&接收邮箱队列
    can_parameter.edge_filter_enable = DISABLE;                             //不使能边沿过滤   
    can_parameter.protocol_exception_enable = DISABLE;                      //不使能协议异常检测
    can_parameter.rx_filter_order = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;      //接收过滤顺序
    can_parameter.memory_size = CAN_MEMSIZE_32_UNIT;                        //内存大小
 
    can_parameter.mb_public_filter = 0U;                //邮箱公有过滤器：选择不关心
    
    /* CAN波特率 = CAN时钟/ ( (再同步补偿宽度+传播时间段 + 相位缓冲段1 + 相位缓冲段2) * 波特率分频系数)
     采样比 = ( 再同步补偿宽度+传播时间段 + 相位缓冲段1) / (再同步补偿宽度 +传播时间段+ 相位缓冲段1 + 相位缓冲段2) */
     
    /*波特率 500Kbps, sample point at 80% */
    can_parameter.resync_jump_width = 1U;   //再同步补偿宽度
    can_parameter.prop_time_segment = 2U;   //传播时间段
    can_parameter.time_segment_1 = 5U;      //相位缓冲段1
    can_parameter.time_segment_2 = 2U;      //相位缓冲段2
    can_parameter.prescaler = 30U;          //波特率分频系数

    /*波特率的计算
        CAN波特率 = CAN时钟 / ((再同步补偿宽度 + 传播时间段 + 相位缓冲段1 + 相位缓冲段2) * 波特率分频系数)
        采样比 = (再同步补偿宽度 + 传播时间段 + 相位缓冲段1) / (再同步补偿宽度 + 传播时间段 + 相位缓冲段1 + 相位缓冲段2)

        本示例配置说明：
        CAN时钟 = 150MHz
        波特率分频系数 = 15
        再同步补偿宽度 = 1
        传播时间段 = 2
        相位缓冲段1 = 5
        相位缓冲段2 = 2

        位时间宽度(再同步补偿宽度 + 传播时间段 + 相位缓冲段1 + 相位缓冲段2) = 10

        1Tq = 1 / (CAN时钟 / 波特率分频系数) = 1 / 10 us
        T1bit = (位时间宽度)x Tq = 1us
        波特率 = 1 / T1bit = 150 / (1 + 2 + 5 + 2) / 15 = 1Mbps
        采样比 = (1 + 2 + 5) / (1 + 2 + 5 + 2) = 80 %
    */
    /* 初始化 CAN */
    can_init(CANX, &can_parameter);
    
    /* FD的模式配置 */
    CAN_FD_Config(); 

    /* 配置中断源：CAN 、配置抢占优先级、 配置响应优先级、使能中断通道*/
    nvic_irq_enable(CAN_IRQ, 0U, 0U);

    /* 使能接收邮箱x中断 */
    can_interrupt_enable(CANX, CAN_INT_MB1);
    
    /* 进入正常工作模式模式 */
    can_operation_mode_enter(CANX, CAN_NORMAL_MODE);
}

/**
  * @brief  FD的模式配置
  * @param  无
  * @retval 无
  */
static void CAN_FD_Config(void)   
{
    can_fd_parameter_struct fd_parameter;
    can_struct_para_init(CAN_FD_INIT_STRUCT, &fd_parameter);
    
    /* FD 结构体配置 */
    fd_parameter.bitrate_switch_enable = ENABLE;
    fd_parameter.iso_can_fd_enable = ENABLE;
    fd_parameter.mailbox_data_size = CAN_MAILBOX_DATA_SIZE_16_BYTES;
    fd_parameter.tdc_enable = ENABLE;
    fd_parameter.tdc_offset = 2U;
    /* FD baud rate 1Mbps, sample point at 80% */
    fd_parameter.resync_jump_width = 1U;
    fd_parameter.prop_time_segment = 2U;
    fd_parameter.time_segment_1 = 5U;
    fd_parameter.time_segment_2 = 2U;
    fd_parameter.prescaler = 15U;
    can_fd_config(CAN2, &fd_parameter);
}
/**
  * @brief  完整配置CAN的功能
  * @param  无
  * @retval 无
  */
void CAN_Config(void)
{
    CAN_GPIO_Config();
    CAN_Mode_Config();
    
}

const uint8_t tx_data[BUFFER_SIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8};
//const uint8_t tx_data[BUFFER_SIZE] = {0x0,0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
uint8_t rx_data[BUFFER_SIZE];

/**
  * @brief  初始化 Rx Message数据结构体
  * @param  RxMessage: 指向要初始化的数据结构体
  * @retval None
  */
void Init_RxMes(can_mailbox_descriptor_struct *receive_message)
{
    uint8_t i = 0;
    receive_message->rtr = 0U;                       //接收的是数据
    receive_message->ide = 0U;                       //标准模式
    receive_message->code = CAN_MB_RX_STATUS_EMPTY;  //邮箱代码
    
    /* rx 邮箱 */
    receive_message->id = 0x00;
    receive_message->data = (uint32_t *)(rx_data);
    can_mailbox_config(CANX, CAN_RECEIVE_NUM, receive_message);
    for (i = 0; i < 8; i++)
    {
        receive_message->data[i] = 0xffffffff;
    }
  
}

/**
  * @brief  CAN通信报文内容设置,设置一个数据内容为0-7的数据包
  * @param  TxMessage: 发送报文结构体
  * @retval None
  */
void CAN_SetMsg(can_mailbox_descriptor_struct *transmit_message)
{	  
    
    transmit_message->rtr = 0U;                      //发送的是数据
    transmit_message->ide = 1U;                      //扩展模式
    transmit_message->code = CAN_MB_TX_STATUS_DATA;  //邮箱代码
    transmit_message->brs = 0U;                      //位速率切换
    transmit_message->fdf = 0U;                      //FD 格式指示
    transmit_message->prio = 0U;                     //本地优先级
    transmit_message->data_bytes = BUFFER_SIZE;      //数据长度为8字节
    
    transmit_message->data = (uint32_t *)(tx_data);
    transmit_message->id = CANX_ID;                   //使用的扩展ID
     
}

/*********************************************END OF FILE**********************/

