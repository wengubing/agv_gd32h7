/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief   LCD应用函数接口，支持ARGB888/RGB888/RGB565/ARGB4444/ARGB1555 (不含中文显示)
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
  
#include "lcd/bsp_lcd.h"
#include "image_RGB565.h"
#include "fonts/fonts.h"

//uint8_t blended_address_buffer[1600][480] __attribute__((section(".ARM.__at_0x0800C800")));
LCD_TypeDef cur_lcd = INCH_5;

/*用于存储当前选择的字体格式*/
static sFONT *LCD_Currentfonts;
/* 用于存储当前字体颜色和字体背景颜色的变量*/
static uint32_t CurrentTextColor   = 0x000000;
static uint32_t CurrentBackColor   = 0xFFFFFF;
/* 用于存储层对应的显存空间 和 当前选择的层*/
static uint32_t CurrentFrameBuffer = LCD_FRAME_BUFFER;

 /**
  * @brief  初始化控制LCD的IO
  * @param  无
  * @retval 无
  */
void LCD_GPIO_Config(void)
{ 

    /* 使能LCD使用到的引脚时钟 */
    //红色数据线
    LTDC_R3_GPIO_CLK_ENABLE(LTDC_R3_GPIO_CLK);
    LTDC_R4_GPIO_CLK_ENABLE(LTDC_R4_GPIO_CLK);
    LTDC_R5_GPIO_CLK_ENABLE(LTDC_R5_GPIO_CLK);
    LTDC_R6_GPIO_CLK_ENABLE(LTDC_R6_GPIO_CLK);
    LTDC_R7_GPIO_CLK_ENABLE(LTDC_R7_GPIO_CLK);
    //绿色数据线
    LTDC_G2_GPIO_CLK_ENABLE(LTDC_G2_GPIO_CLK);
    LTDC_G3_GPIO_CLK_ENABLE(LTDC_G3_GPIO_CLK);
    LTDC_G4_GPIO_CLK_ENABLE(LTDC_G4_GPIO_CLK);
    LTDC_G5_GPIO_CLK_ENABLE(LTDC_G5_GPIO_CLK);
    LTDC_G6_GPIO_CLK_ENABLE(LTDC_G6_GPIO_CLK);
    LTDC_G7_GPIO_CLK_ENABLE(LTDC_G7_GPIO_CLK);
    //蓝色数据线
    LTDC_B3_GPIO_CLK_ENABLE(LTDC_B3_GPIO_CLK);
    LTDC_B4_GPIO_CLK_ENABLE(LTDC_B4_GPIO_CLK);
    LTDC_B5_GPIO_CLK_ENABLE(LTDC_B5_GPIO_CLK);
    LTDC_B6_GPIO_CLK_ENABLE(LTDC_B6_GPIO_CLK);
    LTDC_B7_GPIO_CLK_ENABLE(LTDC_B7_GPIO_CLK);
    
    LTDC_CLK_GPIO_CLK_ENABLE(LTDC_CLK_GPIO_CLK);
    LTDC_HSYNC_GPIO_CLK_ENABLE(LTDC_HSYNC_GPIO_CLK);
    LTDC_VSYNC_GPIO_CLK_ENABLE(LTDC_VSYNC_GPIO_CLK);
    LTDC_DE_GPIO_CLK_ENABLE(LTDC_DE_GPIO_CLK);
    LTDC_BL_GPIO_CLK_ENABLE(LTDC_BL_GPIO_CLK);

/* GPIO配置 */
 /* 红色数据线 */  

    gpio_af_set(LTDC_R3_GPIO_PORT, LTDC_R3_GPIO_AF, LTDC_R3_GPIO_PIN);
    gpio_mode_set(LTDC_R3_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R3_GPIO_PIN);
    gpio_output_options_set(LTDC_R3_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R3_GPIO_PIN);

    gpio_af_set(LTDC_R4_GPIO_PORT, LTDC_R4_GPIO_AF, LTDC_R4_GPIO_PIN);
    gpio_mode_set(LTDC_R4_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R4_GPIO_PIN);
    gpio_output_options_set(LTDC_R4_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R4_GPIO_PIN);

    gpio_af_set(LTDC_R5_GPIO_PORT, LTDC_R5_GPIO_AF, LTDC_R5_GPIO_PIN);
    gpio_mode_set(LTDC_R5_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R5_GPIO_PIN);
    gpio_output_options_set(LTDC_R5_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R5_GPIO_PIN);
    
    gpio_af_set(LTDC_R6_GPIO_PORT, LTDC_R6_GPIO_AF, LTDC_R6_GPIO_PIN);
    gpio_mode_set(LTDC_R6_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R6_GPIO_PIN);
    gpio_output_options_set(LTDC_R6_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R6_GPIO_PIN);
    
    gpio_af_set(LTDC_R7_GPIO_PORT, LTDC_R7_GPIO_AF, LTDC_R7_GPIO_PIN);
    gpio_mode_set(LTDC_R7_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R7_GPIO_PIN);
    gpio_output_options_set(LTDC_R7_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R7_GPIO_PIN);
    
    //绿色数据线
    gpio_af_set(LTDC_G2_GPIO_PORT, LTDC_G2_GPIO_AF, LTDC_G2_GPIO_PIN);
    gpio_mode_set(LTDC_G2_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G2_GPIO_PIN);
    gpio_output_options_set(LTDC_G2_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G2_GPIO_PIN);
    
    gpio_af_set(LTDC_G3_GPIO_PORT, LTDC_G3_GPIO_AF, LTDC_G3_GPIO_PIN);
    gpio_mode_set(LTDC_G3_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G3_GPIO_PIN);
    gpio_output_options_set(LTDC_G3_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G3_GPIO_PIN);

    gpio_af_set(LTDC_G4_GPIO_PORT, LTDC_G4_GPIO_AF, LTDC_G4_GPIO_PIN);
    gpio_mode_set(LTDC_G4_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G4_GPIO_PIN);
    gpio_output_options_set(LTDC_G4_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G4_GPIO_PIN);

    gpio_af_set(LTDC_G5_GPIO_PORT, LTDC_G5_GPIO_AF, LTDC_G5_GPIO_PIN);
    gpio_mode_set(LTDC_G5_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G5_GPIO_PIN);
    gpio_output_options_set(LTDC_G5_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G5_GPIO_PIN);
    
    gpio_af_set(LTDC_G6_GPIO_PORT, LTDC_G6_GPIO_AF, LTDC_G6_GPIO_PIN);
    gpio_mode_set(LTDC_G6_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G6_GPIO_PIN);
    gpio_output_options_set(LTDC_G6_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G6_GPIO_PIN);
    
    gpio_af_set(LTDC_G7_GPIO_PORT, LTDC_G7_GPIO_AF, LTDC_G7_GPIO_PIN);
    gpio_mode_set(LTDC_G7_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G7_GPIO_PIN);
    gpio_output_options_set(LTDC_G7_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G7_GPIO_PIN);
  
  //蓝色数据线
    gpio_af_set(LTDC_B3_GPIO_PORT, LTDC_B3_GPIO_AF, LTDC_B3_GPIO_PIN);
    gpio_mode_set(LTDC_B3_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B3_GPIO_PIN);
    gpio_output_options_set(LTDC_B3_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B3_GPIO_PIN);

    gpio_af_set(LTDC_B4_GPIO_PORT, LTDC_B4_GPIO_AF, LTDC_B4_GPIO_PIN);
    gpio_mode_set(LTDC_B4_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B4_GPIO_PIN);
    gpio_output_options_set(LTDC_B4_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B4_GPIO_PIN);

    gpio_af_set(LTDC_B5_GPIO_PORT, LTDC_B5_GPIO_AF, LTDC_B5_GPIO_PIN);
    gpio_mode_set(LTDC_B5_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B5_GPIO_PIN);
    gpio_output_options_set(LTDC_B5_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B5_GPIO_PIN);
    
    gpio_af_set(LTDC_B6_GPIO_PORT, LTDC_B6_GPIO_AF, LTDC_B6_GPIO_PIN);
    gpio_mode_set(LTDC_B6_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B6_GPIO_PIN);
    gpio_output_options_set(LTDC_B6_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B6_GPIO_PIN);
    
    gpio_af_set(LTDC_B7_GPIO_PORT, LTDC_B7_GPIO_AF, LTDC_B7_GPIO_PIN);
    gpio_mode_set(LTDC_B7_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B7_GPIO_PIN);
    gpio_output_options_set(LTDC_B7_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B7_GPIO_PIN);
  
    //控制信号线
  
    gpio_af_set(LTDC_CLK_GPIO_PORT, LTDC_CLK_GPIO_AF, LTDC_CLK_GPIO_PIN);
    gpio_mode_set(LTDC_CLK_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_CLK_GPIO_PIN);
    gpio_output_options_set(LTDC_CLK_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_CLK_GPIO_PIN);
  
  
    gpio_af_set(LTDC_HSYNC_GPIO_PORT, LTDC_HSYNC_GPIO_AF, LTDC_HSYNC_GPIO_PIN);
    gpio_mode_set(LTDC_HSYNC_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_HSYNC_GPIO_PIN);
    gpio_output_options_set(LTDC_HSYNC_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_HSYNC_GPIO_PIN);
  
    gpio_af_set(LTDC_VSYNC_GPIO_PORT, LTDC_VSYNC_GPIO_AF, LTDC_VSYNC_GPIO_PIN);
    gpio_mode_set(LTDC_VSYNC_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_VSYNC_GPIO_PIN);
    gpio_output_options_set(LTDC_VSYNC_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_VSYNC_GPIO_PIN);
  
    gpio_af_set(LTDC_DE_GPIO_PORT, LTDC_DE_GPIO_AF, LTDC_DE_GPIO_PIN);
    gpio_mode_set(LTDC_DE_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_DE_GPIO_PIN);
    gpio_output_options_set(LTDC_DE_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_DE_GPIO_PIN);
  
  //背光BL                            
    gpio_mode_set(LTDC_BL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LTDC_BL_GPIO_PIN);
    gpio_output_options_set(LTDC_BL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_BL_GPIO_PIN);
    gpio_bit_set(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN);
    
}

/*!
    \brief      LCD 初始化配置
    \param[in]  无
    \param[out] 无
    \retval     无
*/
void LCD_Init(void)
{
    tli_parameter_struct tli_init_struct;

    rcu_periph_clock_enable(RCU_TLI);
    LCD_GPIO_Config();

    /* 配置 PLLSAI 分频器，它的输出作为像素同步时钟CLK*/
   // CK_PLL2VCOSRC = CK_PLL2SRC / PLL2PSC
   // VCO 源时钟频率范围必须在 1MHz 到 16MHz 之间
   // CK_PLL2VCO = CK_PLL2VCOSRC × （PLL2N + PLL2FRAN / 2^13）
   // CK_PLL2P = CK_ PLL2VCO / PLL2P
   // CK_PLL2R = CK_ PLL2VCO / PLL2R
   /* LTDC时钟太高会导花屏，若对刷屏速度要求不高，降低时钟频率可减少花屏现象*/
   // CK_PLL2VCOSRC = 25/25=1M
   // CK_PLL2VCO = 1*(240+0) = 20M
   // CK_PLL2R = 240 / 3 = 10M
    /* configure PLL2 to generate TLI clock 25MHz/25*30/3 = 10MHz*/
    rcu_pll_input_output_clock_range_config(IDX_PLL2, RCU_PLL2RNG_1M_2M, RCU_PLL2VCO_150M_420M);
    if(ERROR == rcu_pll2_config(25, 240, 10, 10, 10)) 
    {
        while(1);
    }
    rcu_pll_clock_output_enable(RCU_PLL2R);
    rcu_tli_clock_div_config(RCU_PLL2R_DIV8);  //由屏幕手册其时钟频率为33.3MHz

    rcu_osci_on(RCU_PLL2_CK);

    if(ERROR == rcu_osci_stab_wait(RCU_PLL2_CK)) 
    {
        while(1);
    }

    /* 配置TLI参数结构 */
    /*信号极性配置*/
    /* 行同步信号极性 */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;                                                   //水平同步极性，设置低电平或是高电平有效
    /* 垂直同步信号极性 */  
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;                                                   //垂直同步极性
    /* 数据使能信号极性 */
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;                                                     //数据使能极性
    /* 像素同步时钟极性 */ 
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;                                               //像素时钟极性
    /* LCD显示定时配置 */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;                                             //水平同步宽度
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;                                               //垂直同步宽度
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;                   //水平同步后沿宽度
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;                       //垂直同步后沿高度
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH- 1;     //有效宽度
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT -1;        //有效高度
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;    //总宽度
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;         //总高度
    
    tli_init_struct.backcolor_red =     0xFF;			//屏幕背景层红色部分 	设为黑色
    tli_init_struct.backcolor_green =   0xFF;			//屏幕背景层绿色部分
    tli_init_struct.backcolor_blue =    0xFF;			//屏幕背景色蓝色部分
    
    tli_init(&tli_init_struct);

}



/*!
    \brief      颜色设置
    \param[in]  layerx: LAYERx(x=0,1)
    \param[in]  redkey: color key red
    \param[in]  greenkey: color key green 
    \param[in]  bluekey: color key blue
    \param[out] none
    \retval     none
*/
void Colour_Set(uint32_t layerx,uint8_t redkey,uint8_t greenkey,uint8_t bluekey)
{
    tli_parameter_struct tli_init_struct;

    /* 配置TLI参数结构 */
    /*信号极性配置*/
    /* 行同步信号极性 */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;                                                   //水平同步极性，设置低电平或是高电平有效
    /* 垂直同步信号极性 */  
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;                                                   //垂直同步极性
    /* 数据使能信号极性 */
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;                                                     //数据使能极性
    /* 像素同步时钟极性 */ 
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;                                               //像素时钟极性
    /* LCD显示定时配置 */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;                                             //水平同步宽度
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;                                               //垂直同步宽度
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;                   //水平同步后沿宽度
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;                       //垂直同步后沿高度
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH- 1;     //有效宽度
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT -1;        //有效高度
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;    //总宽度
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;         //总高度
    
    tli_init_struct.backcolor_red =     redkey;			    //屏幕背景层红色部分 	设为黑色
    tli_init_struct.backcolor_green =   greenkey;			//屏幕背景层绿色部分
    tli_init_struct.backcolor_blue =    bluekey;			//屏幕背景色蓝色部分
    
    tli_init(&tli_init_struct);

}









/**
  * @brief 初始化LTD的 层 参数
  *           - 设置显存空间
  *           - 设置分辨率
  * @param  None
  * @retval None
  */
void LCD_LayerInit(void)
{
    
    tli_layer_parameter_struct         tli_layer_init_struct;
    
    /* TLI layer0配置 */
    /* TLI窗口大小配置 */
    tli_layer_init_struct.layer_window_leftpos = 00+0+lcd_param[cur_lcd].hsw + lcd_param[cur_lcd].hbp;
    tli_layer_init_struct.layer_window_rightpos = (ACTIVE_WIDTH + lcd_param[cur_lcd].hsw + lcd_param[cur_lcd].hbp - 1);
    tli_layer_init_struct.layer_window_toppos = 00+0+lcd_param[cur_lcd].vsw + lcd_param[cur_lcd].vbp;
    tli_layer_init_struct.layer_window_bottompos = (ACTIVE_HEIGHT + 0 + lcd_param[cur_lcd].vsw + lcd_param[cur_lcd].vbp - 1);
    /* TLI窗口像素格式配置 */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;     //TLI_LxPPF  PPF[2:0]  010：RGB565
    /* TLI窗口指定alpha配置 */
    tli_layer_init_struct.layer_sa = 0x00;                   //层透明度 255为完全不透明
    /* TLI层默认alpha R、G、B值配置 */
    tli_layer_init_struct.layer_default_blue = 0xff;        //该层显示范围外的颜色
    tli_layer_init_struct.layer_default_green = 0x00;       
    tli_layer_init_struct.layer_default_red = 0x00;
    tli_layer_init_struct.layer_default_alpha = 0;
    /* TLI窗口混合配置 */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_SA;       //层混合模式 归一化的像素 Alpha 乘以归一化的恒定 Alpha
    tli_layer_init_struct.layer_acf2 = LAYER_ACF1_SA;       //111：归一化的像素 Alpha 乘以归一化的恒定 Alpha       
    /* TLI层帧缓冲基址配置 */
    /* 该成员应写入(一行像素数据占用的字节数+3)
    Line Lenth = 行有效像素个数 x 每个像素的字节数 + 3 
    行有效像素个数 = LCD_PIXEL_WIDTH 
    每个像素的字节数 = 2（RGB565/RGB1555）/ 3 (RGB888)/ 4（ARGB8888） */
    tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER;     //缓存地址
    tli_layer_init_struct.layer_frame_line_length = ((ACTIVE_WIDTH * 2) + 3);       //行长度 这个值为一行的字节数+3
    tli_layer_init_struct.layer_frame_buf_stride_offset = (ACTIVE_WIDTH * 2);       //步幅偏移 定义了从某行起始处到下一行起始处之间的字节数
    tli_layer_init_struct.layer_frame_total_line_number = ACTIVE_HEIGHT;            //总行数 定义了一帧行数
    tli_layer_init(LAYER0, &tli_layer_init_struct);
    tli_dither_config(TLI_DITHER_ENABLE);
    
    /*配置第 2 层，若没有重写某个成员的值，则该成员使用跟第1层一样的配置 */
    /* 配置本层的显存首地址，这里配置它紧挨在第1层的后面*/     
    tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER + BUFFER_OFFSET;
    
    /* 配置混合因子，使用像素Alpha参与混合 */  
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;       //层混合模式 归一化的像素 Alpha 乘以归一化的恒定 Alpha
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;       //111：归一化的像素 Alpha 乘以归一化的恒定 Alpha 
    /* 初始化第2层 */
    tli_layer_init(LAYER1, &tli_layer_init_struct);
    
     /*使能前景及背景层 */
    tli_layer_enable(LAYER0);
    tli_layer_enable(LAYER1);
    
    tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
    
//      /* 设定字体(英文) */    
//    LCD_SetFont(&LCD_DEFAULT_FONT); 
    
}

/**
  * @brief  设置字体格式(英文)
  * @param  fonts: 选择要设置的字体格式
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/* 不同液晶屏的参数 */
const LCD_PARAM_TypeDef lcd_param[LCD_TYPE_NUM]={

  /* 5寸屏参数 */
  {
    /*根据液晶数据手册的参数配置*/
    .hbp = 46,  //HSYNC后的无效像素
    .vbp = 23,  //VSYNC后的无效行数

    .hsw = 1,   //HSYNC宽度
    .vsw = 3,   //VSYNC宽度

    .hfp = 40,  //HSYNC前的无效像素
    .vfp = 13,  //VSYNC前的无效行数
    
    .comment_clock_2byte = 33, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 21, //Argb8888等四字节像素时推荐使用的液晶时钟频率

    
    .lcd_pixel_width = ((uint16_t)800),//液晶分辨率，宽
    .lcd_pixel_height = ((uint16_t)480),//液晶分辨率，高

  },
  
   /* 7寸屏参数（与5寸一样） */
  {
    /*根据液晶数据手册的参数配置*/
    .hbp = 46,  //HSYNC后的无效像素
    .vbp = 23,  //VSYNC后的无效行数

    .hsw = 1,  	//HSYNC宽度
    .vsw = 1,   //VSYNC宽度

    .hfp = 22,  	//HSYNC前的无效像素
    .vfp = 22,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 33, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 21, //Argb8888等四字节像素时推荐使用的液晶时钟频率

    
    .lcd_pixel_width = ((uint16_t)800),//液晶分辨率，宽
    .lcd_pixel_height = ((uint16_t)480),//液晶分辨率，高
  
  },

  /* 4.3寸屏参数 */
  {
      /*根据液晶数据手册的参数配置*/
    .hbp = 8,  //HSYNC后的无效像素
    .vbp = 2,  //VSYNC后的无效行数

    .hsw = 41,  	//HSYNC宽度
    .vsw = 10,   //VSYNC宽度

    .hfp = 4,  	//HSYNC前的无效像素
    .vfp = 4,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 15, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 15, //Argb8888等四字节像素时推荐使用的液晶时钟频率
    
    .lcd_pixel_width = 480,//液晶分辨率，宽
    .lcd_pixel_height = 272,//液晶分辨率，高
  }
};

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
