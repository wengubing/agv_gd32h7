#include "gd32h7xx_enet.h"
#include "lan8720a/bsp_lan8720a.h"
#include "main.h"


const uint8_t gd32_str[] = {"\r\n ############ Welcome GigaDevice ############\r\n"};
static __IO uint32_t enet_init_status = 0;
static void enet_gpio_config(void);
static void enet_mac_dma_config(void);
#ifdef USE_ENET_INTERRUPT
static void nvic_configuration(void);
#endif /* USE_ENET_INTERRUPT */

/*!
    \brief      设置以太网系统（GPIO、时钟、MAC、DMA、systick）
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_system_setup(void)
{
    uint32_t ahb_frequency = 0;
    uint16_t temp_phy_enet_system_setup = 0U;
#ifdef USE_ENET_INTERRUPT
    nvic_configuration();
#endif /* USE_ENET_INTERRUPT */

    /* 为以太网引脚配置GPIO端口 */
    enet_gpio_config();

    /* 配置以太网MAC/DMA */
    enet_mac_dma_config();
    
    if(0 == enet_init_status) 
    {
        while(1);
    }

#ifdef USE_ENET_INTERRUPT
#ifdef USE_ENET0
    enet_interrupt_enable(ENET0, ENET_DMA_INT_NIE);
    enet_interrupt_enable(ENET0, ENET_DMA_INT_RIE);

#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    enet_desc_select_enhanced_mode(ENET0);
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

#endif /* USE_ENET0 */
#ifdef USE_ENET1
    enet_interrupt_enable(ENET1, ENET_DMA_INT_NIE);
    enet_interrupt_enable(ENET1, ENET_DMA_INT_RIE);

#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    enet_desc_select_enhanced_mode(ENET1);
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

#endif /* USE_ENET1 */
#endif /* USE_ENET_INTERRUPT */

    /* 将系统时钟源配置为HCLK */
    systick_clksource_set(SYSTICK_CLKSOURCE_CKSYS);

    /* an interrupt every 10ms */
    ahb_frequency = rcu_clock_freq_get(CK_AHB);
    SysTick_Config(ahb_frequency / 100);
}

/*!
    \brief      配置以太网接口
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void enet_mac_dma_config(void)
{
    ErrStatus reval_state = ERROR;
#ifdef USE_ENET0
    /* enable ethernet clock  */
    rcu_periph_clock_enable(RCU_ENET0);
    rcu_periph_clock_enable(RCU_ENET0TX);
    rcu_periph_clock_enable(RCU_ENET0RX);

    /* reset ethernet on AHB bus */
    enet_deinit(ENET0);

    reval_state = enet_software_reset(ENET0);
    if(ERROR == reval_state) {
        while(1) {}
    }

    /* configure the parameters which are usually less cared for enet initialization */
//  enet_initpara_config(ENET0, HALFDUPLEX_OPTION, ENET_CARRIERSENSE_ENABLE|ENET_RECEIVEOWN_ENABLE|ENET_RETRYTRANSMISSION_DISABLE|ENET_BACKOFFLIMIT_10|ENET_DEFERRALCHECK_DISABLE);
//  enet_initpara_config(ENET0, DMA_OPTION, ENET_FLUSH_RXFRAME_ENABLE|ENET_SECONDFRAME_OPT_ENABLE|ENET_NORMAL_DESCRIPTOR);

#ifdef CHECKSUM_BY_HARDWARE
    enet_init_status = enet_init(ENET0, ENET_AUTO_NEGOTIATION, ENET_AUTOCHECKSUM_DROP_FAILFRAMES, ENET_BROADCAST_FRAMES_PASS);

#else
    enet_init_status = enet_init(ENET0, ENET_AUTO_NEGOTIATION, ENET_NO_AUTOCHECKSUM, ENET_BROADCAST_FRAMES_PASS);
#endif /* CHECKSUM_BY_HARDWARE */
#endif /* USE_ENET0 */

#ifdef USE_ENET1
    /* enable ethernet clock  */
    rcu_periph_clock_enable(RCU_ENET1);
    rcu_periph_clock_enable(RCU_ENET1TX);
    rcu_periph_clock_enable(RCU_ENET1RX);

    /* reset ethernet on AHB bus */
    enet_deinit(ENET1);

    reval_state = enet_software_reset(ENET1);
    if(ERROR == reval_state) {
        while(1) {}
    }

    /* configure the parameters which are usually less cared for enet initialization */
//  enet_initpara_config(ENET1, HALFDUPLEX_OPTION, ENET_CARRIERSENSE_ENABLE|ENET_RECEIVEOWN_ENABLE|ENET_RETRYTRANSMISSION_DISABLE|ENET_BACKOFFLIMIT_10|ENET_DEFERRALCHECK_DISABLE);
//  enet_initpara_config(ENET1, DMA_OPTION, ENET_FLUSH_RXFRAME_ENABLE|ENET_SECONDFRAME_OPT_ENABLE|ENET_NORMAL_DESCRIPTOR);

#ifdef CHECKSUM_BY_HARDWARE
    enet_init_status = enet_init(ENET1, ENET_AUTO_NEGOTIATION, ENET_AUTOCHECKSUM_DROP_FAILFRAMES, ENET_BROADCAST_FRAMES_PASS);
#else
    enet_init_status = enet_init(ENET1, ENET_AUTO_NEGOTIATION, ENET_NO_AUTOCHECKSUM, ENET_BROADCAST_FRAMES_PASS);
#endif /* CHECKSUM_BY_HARDWARE */
#endif /* USE_ENET1 */
}

#ifdef USE_ENET_INTERRUPT
/*!
    \brief      配置嵌套矢量中断控制器
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void nvic_configuration(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

#ifdef USE_ENET0
    nvic_irq_enable(ENET0_IRQn, 0, 0);
#endif /* USE_ENET0 */
#ifdef USE_ENET1
    nvic_irq_enable(ENET1_IRQn, 0, 0);
#endif /* USE_ENET1 */
}
#endif /* USE_ENET_INTERRUPT */

/*!
    \brief      配置不同的GPIO端口
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void enet_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);

    /* enable SYSCFG clock */
    rcu_periph_clock_enable(RCU_SYSCFG);

#ifdef MII_MODE

#ifdef PHY_CLOCK_MCO
    /* output HXTAL clock (25MHz) on CKOUT0 pin(PA8) to clock the PHY */
    rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV1);
#endif /* PHY_CLOCK_MCO */

#ifdef USE_ENET0
    syscfg_enet_phy_interface_config(ENET0, SYSCFG_ENET_PHY_MII);
#endif /* USE_ENET0 */
#ifdef USE_ENET1
    syscfg_enet_phy_interface_config(ENET1, SYSCFG_ENET_PHY_MII);
#endif /* USE_ENET1 */

#elif defined RMII_MODE
    /* choose DIV12 to get 50MHz from 600MHz on CKOUT0 pin (PA8) to clock the PHY */
    rcu_ckout0_config(RCU_CKOUT0SRC_PLL0P, RCU_CKOUT0_DIV12);

#ifdef USE_ENET0
    syscfg_enet_phy_interface_config(ENET0, SYSCFG_ENET_PHY_RMII);
#endif /* USE_ENET0 */
#ifdef USE_ENET1
    syscfg_enet_phy_interface_config(ENET1, SYSCFG_ENET_PHY_RMII);
#endif /* USE_ENET1 */

#endif /* MII_MODE */

#ifdef USE_ENET0
#ifdef MII_MODE

    /* PA1: ETH0_MII_RX_CLK */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_1);

    /* PA2: ETH0_MDIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_2);

    /* PA7: ETH0_MII_RX_DV */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_7);

    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_7);

    /* PB8: ETH0_MII_TXD3 */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_8);

    /* PB10: ETH0_MII_RX_ER */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_10);

    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_8);
    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_10);

    /* PC1: ETH0_MDC */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_1);

    /* PC2: ETH0_MII_TXD2 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_2);

    /* PC3: ETH0_MII_TX_CLK */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_3);

    /* PC4: ETH0_MII_RXD0 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_4);

    /* PC5: ETH0_MII_RXD1 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_5);

    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_3);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_4);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_5);

    /* PH2: ETH0_MII_CRS */
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_2);

    /* PH3: ETH0_MII_COL */
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_3);

    /* PH6: ETH0_MII_RXD2 */
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6);

    /* PH7: ETH0_MII_RXD3 */
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_7);

    gpio_af_set(GPIOH, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOH, GPIO_AF_11, GPIO_PIN_3);
    gpio_af_set(GPIOH, GPIO_AF_11, GPIO_PIN_6);
    gpio_af_set(GPIOH, GPIO_AF_11, GPIO_PIN_7);

    /* PG11: ETH0_MII_TX_EN */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_11);

    /* PG13: ETH0_MII_TXD0 */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_13);

    /* PG14: ETH0_MII_TXD1 */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_14);

    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_13);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_14);

    /* PD8: ETH0_INT */
    gpio_mode_set(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);

#elif defined RMII_MODE

    /* PA1: ETH0_RMII_REF_CLK */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_1);

    /* PA2: ETH0_MDIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_2);

    /* PA7: ETH0_RMII_CRS_DV */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_7);

    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_7);

    /* PB11: ETH0_RMII_TX_EN */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_11);

    /* PG13: ETH0_RMII_TXD0 */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_13);

    /* PG14: ETH0_RMII_TXD1 */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_14);

    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_13);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_14);

    /* PC1: ETH0_MDC */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_1);

    /* PC4: ETH0_RMII_RXD0 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_4);

    /* PC5: ETH0_RMII_RXD1 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_5);

    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_4);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_5);
		
		
		/* PC5: ETH0_RMII_RXD1 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_5);
  	
#endif /* MII_MODE */
#endif /* USE_ENET0 */

}

/*!
    \brief      配置10ms定时器
    \param[in]  无
    \param[out] 无
    \retval     无
*/
void enet_systick_setup(void)
{
    
    /* 系统时钟源配置为HCLK */
    uint32_t ahb_frequency = 0;
    systick_clksource_set(SYSTICK_CLKSOURCE_CKSYS);
    /* 每10毫秒中断一次 */
    ahb_frequency = rcu_clock_freq_get(CK_AHB);
    SysTick_Config(ahb_frequency / 100);
}

/*!
    \brief      打印基本寄存器
    \param[in]  无
    \param[out] 无
    \retval     无
*/
void PrintfPhyReg(void)
{
	uint16_t reg_value  = 0;
	  
    enet_phy_write_read(ENET0, ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BCR, &reg_value);
	
	printf("PHY_REG_BCR:0x%x \n",reg_value);
		
	enet_phy_write_read(ENET0, ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &reg_value);
	
	printf("PHY_REG_BSR:0x%x \n",reg_value);	
	
}


/*!
    \brief      硬件复位以太网
    \param[in]  无
    \param[out] 无
    \retval     无
*/
void HardwareResetPhy(void)
{
    rcu_periph_clock_enable(RCU_GPIOF);

    /*PF6: ETH0_RESET*/
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6);

    /* RESET PHY */
    gpio_bit_reset(GPIOF,GPIO_PIN_6); 
    delay_10ms(4);
    gpio_bit_set(GPIOF,GPIO_PIN_6);
    delay_10ms(50);
}

/*!
    \brief      检查lan8720 ID
    \param[in]  无
    \param[out] 无
    \retval     true:ID检测通过，false:ID检测失败
*/
bool lan8720_check_id( void )
{
    uint16_t id1, id2;

    enet_phy_write_read(ENET0, ENET_PHY_READ, PHY_ADDRESS, LAN8720_PHYID1, &id1);
    enet_phy_write_read(ENET0, ENET_PHY_READ, PHY_ADDRESS, LAN8720_PHYID2, &id2);

    if (id1 == LAN8720_ID1 && id2>>4 == LAN8720_ID2) 
    {
        printf("lan8720_check_id pass\r\n");
        return true;
    } 
    else 
    {
        printf("lan8720_check_id fail\r\n");
        return false;
    }
}

/*!
    \brief      获取LAN8720的PHY特殊控制/状态寄存器值
    \param[in]  无
    \param[out] 无
    \retval     LAN8720的PSCSR寄存器值
*/
uint16_t lan8720_get_phystatus( void )
{
    uint16_t phyreg = 0;   
  
    if(enet_phy_write_read(ENET0, ENET_PHY_READ, PHY_ADDRESS, LAN8720_PSCSR, &phyreg) == SUCCESS)
    {
        printf("PHY Special Control/Status Register:0x%x\r\n",phyreg);
        return phyreg;
    }
    return 0;
}

/*!
    \brief      获取LAN8720的连接状态
    \param[in]  无
    \param[out] 无
    \retval     1连接，0断开
*/
uint16_t lan8720_get_connectionstate( void )
{
    uint16_t connectionstate = 0;   
  
    if(enet_phy_write_read(ENET0, ENET_PHY_READ, PHY_ADDRESS, LAN8720_BMSR, &connectionstate) == SUCCESS)
    {
        
        connectionstate &= PHY_LINKED_STATUS;
        connectionstate = connectionstate>>2;
        printf("connectionstate:%d\n",connectionstate);
        return connectionstate;
    }
    return 0;
}
