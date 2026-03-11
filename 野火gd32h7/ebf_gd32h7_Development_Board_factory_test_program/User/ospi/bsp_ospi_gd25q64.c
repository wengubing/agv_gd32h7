/**
  ******************************************************************************
  * @file     bsp_ospi_gd25q64.c
  * @author   embedfire
  * @version  V1.0
  * @date     2023
  * @brief    flash gd25q64xx应用函数接口
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
#include "gd32h7xx.h"
#include "ospi/bsp_ospi_gd25q64.h"
#include "ospi/bsp_ospi.h" 


/*!
    \brief      读flash id
    \param[in]  ospi_periph		: OSPIx(x=0,1)
    \param[in]  ospi_struct		: OSPI参数初始化结构的成员和成员值如下所示:
                prescaler		: between 0 and 255
                fifo_threshold	: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift	: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size		: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime		: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type		: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
								  OSPI_MACRONIX_RAM_MODE,
                wrap_size		: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
								  OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode:             Flash接口模式只能选择一个参数，如下所示:
				SPI_MODE:         SPI  模式
				OSPI_MODE:        OSPI 模式
    \retval     none
*/
uint32_t OSPI_FLASH_READ_ID(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode)
{
    uint8_t temp_id[4];
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize read ID command */
    if(SPI_MODE == mode) {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;//指定指令模式
        cmd_struct.addr_mode = OSPI_ADDRESS_NONE;//地址模式
        cmd_struct.data_mode = OSPI_DATA_1_LINE;//数据模式
        cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;//数据阶段之前插入的空指令周期数
    } else {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_4_LINES;
        cmd_struct.addr_mode = OSPI_ADDRESS_4_LINES;
        cmd_struct.data_mode = OSPI_DATA_4_LINES;
        cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_4;
    }
    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;//指定配置是应用于常规寄存器还是写入操作的寄存器
    cmd_struct.instruction = GD25Q64CSIGR_READ_ID_CMD;//要发送的指令
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;//指定指令大小
    cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;//地址大小
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;//地址阶段是否使能DTR模式
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;//交替字节模式
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;//交替字节大小
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;//交替字节阶段是否使能DTR模式
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;//数据阶段是否使能DTR模式
    cmd_struct.nbdata = 3;                      /*传输的数据量*/

    /* send the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* receive data */
    ospi_receive(ospi_periph, temp_id);
    return (temp_id[0] << 16) | (temp_id[1] << 8) | temp_id[2] ;
}

/*!
    \brief      使能重置flash
    \param[in]  ospi_periph     : OSPIx(x=0,1)
    \param[in]  ospi_struct		: OSPI参数初始化结构的成员和成员值如下所示:
                prescaler		: between 0 and 255
                fifo_threshold	: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift	: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size		: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime		: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type		: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
								  OSPI_MACRONIX_RAM_MODE,
                wrap_size		: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
								  OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode:             Flash接口模式只能选择一个参数，如下所示:
				SPI_MODE:         SPI  模式
				OSPI_MODE:        OSPI 模式
    \retval     none
*/
void OSPI_FLASH_RESET_ENABLE(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* 初始化使能 flash 重新设置命令 */
    if(SPI_MODE == mode) 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
    } 
    else 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_4_LINES;
    }
    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction = GD25Q64CSIGR_RESET_ENABLE_CMD;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.addr_mode = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_mode = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata = 0;

    /* 发送命令 */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);
}

/*!
    \brief      重置flash
    \param[in]  ospi_periph     : OSPIx(x=0,1)
    \param[in]  ospi_struct		: OSPI参数初始化结构的成员和成员值如下所示:
                prescaler		: between 0 and 255
                fifo_threshold	: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift	: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size		: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime		: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type		: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
								  OSPI_MACRONIX_RAM_MODE,
                wrap_size		: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
								  OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode:             Flash接口模式只能选择一个参数，如下所示:
				SPI_MODE:         SPI  模式
				OSPI_MODE:        OSPI 模式
    \retval     none
*/
void OSPI_FLASH_RESET_MEMORY(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* 初始化flash复位命令 */
    if(SPI_MODE == mode) {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
    } else {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_4_LINES;
    }
    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction = GD25Q64CSIGR_RESET_MEMORY_CMD;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.addr_mode = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_mode = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata = 0;

    /* 发送命令 */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);
}

/*!
    \brief      向FLASH发送 写使能 命令
    \param[in]  ospi_periph     : OSPIx(x=0,1)
    \param[in]  ospi_struct		: OSPI参数初始化结构的成员和成员值如下所示:
                prescaler		: between 0 and 255
                fifo_threshold	: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift	: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size		: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime		: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type		: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
								  OSPI_MACRONIX_RAM_MODE,
                wrap_size		: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
								  OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode:             Flash接口模式只能选择一个参数，如下所示:
				SPI_MODE:         SPI  模式
				OSPI_MODE:        OSPI 模式
    \retval     none
*/
void OSPI_FLASH_WRITE_ENBALE(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode)
{
    ospi_autopolling_struct autopl_cfg_struct = {0};
    ospi_regular_cmd_struct cmd_struct = {0};

    /* 初始化写使能命令 */
    cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction = GD25Q64CSIGR_WRITE_ENABLE_CMD;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.addr_mode = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_mode = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata = 0;

    /* 发送命令 */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* 配置自动轮询模式为等待写使能 */

    cmd_struct.data_mode = OSPI_DATA_1_LINE;
    cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    
    cmd_struct.instruction = GD25Q64CSIGR_READ_STATUS_REG_CMD;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.nbdata = 1;

    /* 发送命令 */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    autopl_cfg_struct.match = 2U;
    autopl_cfg_struct.mask  = 2U;
    autopl_cfg_struct.match_mode = OSPI_MATCH_MODE_AND;
    autopl_cfg_struct.interval = GD25Q64CSIGR_AUTOPOLLING_INTERVAL_TIME;
    autopl_cfg_struct.automatic_stop = OSPI_AUTOMATIC_STOP_MATCH;
    ospi_autopolling_mode(ospi_periph, ospi_struct, &autopl_cfg_struct);
}

/*!
    \brief      等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
    \param[in]  ospi_periph     : OSPIx(x=0,1)
    \param[in]  ospi_struct		: OSPI参数初始化结构的成员和成员值如下所示:
                prescaler		: between 0 and 255
                fifo_threshold	: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift	: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size		: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime		: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type		: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
								  OSPI_MACRONIX_RAM_MODE,
                wrap_size		: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
								  OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode:             Flash接口模式只能选择一个参数，如下所示:
				SPI_MODE:         SPI  模式
				OSPI_MODE:        OSPI 模式
    \retval     none
*/
void OSPI_FLASH_AUTOPOLLING_MEM_READY(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode)
{
    ospi_autopolling_struct autopl_cfg_struct = {0};
    ospi_regular_cmd_struct cmd_struct = {0};

    /* 初始化读状态注册命令 */
    if(SPI_MODE == mode) 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
        cmd_struct.addr_mode = OSPI_ADDRESS_NONE;
        cmd_struct.data_mode = OSPI_DATA_1_LINE;
        cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    } 
    else 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_4_LINES;
        cmd_struct.addr_mode = OSPI_ADDRESS_4_LINES;
        cmd_struct.data_mode = OSPI_DATA_4_LINES;
        cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_8;
    }

    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.instruction = GD25Q64CSIGR_READ_STATUS_REG_CMD;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    cmd_struct.address = 0U;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.nbdata = 1;

    /* 发送命令 */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* 配置OSPI自动轮询方式 */
    autopl_cfg_struct.match = 0U;
    autopl_cfg_struct.mask  = GD25Q64CSIGR_SR_WIP;
    autopl_cfg_struct.match_mode = OSPI_MATCH_MODE_AND;
    autopl_cfg_struct.interval = GD25Q64CSIGR_AUTOPOLLING_INTERVAL_TIME;
    autopl_cfg_struct.automatic_stop = OSPI_AUTOMATIC_STOP_MATCH;
    ospi_autopolling_mode(ospi_periph, ospi_struct, &autopl_cfg_struct);
}

/*!
    \brief      擦除指定的flash块函数
    \param[in]  ospi_periph     : OSPIx(x=0,1)
    \param[in]  ospi_struct		: OSPI参数初始化结构的成员和成员值如下所示:
                prescaler		: between 0 and 255
                fifo_threshold	: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift	: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size		: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                                  OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime		: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type		: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
								  OSPI_MACRONIX_RAM_MODE,
                wrap_size		: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
								  OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode            : Flash接口模式只能选择一个参数，如下所示:
				SPI_MODE        : SPI  模式
				OSPI_MODE       : OSPI 模式
    \param[in]  addr_size       : 地址的大小
    \param[in]  address         : 擦除地址
    \param[in]  block_size      : 要擦除的块大小
    \retval     none
*/
void OSPI_FLASH_BLOCK_ERASE(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode,  uint32_t addr,
                            erase_size block_size)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* 初始化块擦除命令 */
    if(SPI_MODE == mode) 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
        cmd_struct.addr_mode = OSPI_ADDRESS_1_LINE;
    } 
    else 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_4_LINES;
        cmd_struct.addr_mode = OSPI_ADDRESS_4_LINES;
    }
    if(GD25Q64CSIGR_ERASE_64K == block_size) 
    {
        cmd_struct.instruction = GD25Q64CSIGR_BLOCK_ERASE_64K_CMD;
    } 
    else 
    {

        cmd_struct.instruction = GD25Q64CSIGR_SECTOR_ERASE_4K_CMD;

    }
    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    cmd_struct.address = addr;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_mode = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata = 0;

    /* 发送命令 */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);
}

/*!
    \brief      向flash写入数据
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                  prescaler: between 0 and 255
                  fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                  sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                  device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                  cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                  memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                  wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                  delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        OSPI_MODE: OSPI mode
    \param[in]  addr_size: the size of address
    \param[in]  pdata: pointer to data to be written
    \param[in]  addr: write start address
    \param[in]  data_size: size of data to write
    \retval     none
*/
void OSPI_FLASH_PAGE_PROGRAM(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode, addr_size addr_size, uint8_t *pdata,
                             uint32_t addr, uint32_t data_size)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* 初始化程序命令 */
    if(SPI_MODE == mode) 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
        cmd_struct.addr_mode = OSPI_ADDRESS_1_LINE;
        cmd_struct.data_mode = OSPI_DATA_1_LINE;
    } 
    else 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_4_LINES;
        cmd_struct.addr_mode = OSPI_ADDRESS_4_LINES;
        cmd_struct.data_mode = OSPI_DATA_4_LINES;
    }
    if(GD25Q64CSIGR_3BYTES_SIZE == addr_size) 
    {
        cmd_struct.instruction = GD25Q64CSIGR_PAGE_PROG_CMD;
        cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    } 

    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.address = addr;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.nbdata = data_size;
    cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;

    /* 发送命令 */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);
    /* 数据传输 */
    ospi_transmit(ospi_periph, pdata);
}

/*!
    \brief      从flash中读取数据
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                  prescaler: between 0 and 255
                  fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                  sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                  device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                  cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                  memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                  wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                  delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        OSPI_MODE: OSPI mode
    \param[in]  addr_size: the size of address
    \param[in]  pdata: pointer to data to be read
    \param[in]  addr: read start address
    \param[in]  data_size: size of data to read
    \retval     none
*/
void OSPI_FLASH_READ(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode, addr_size addr_size, uint8_t *pdata,
                     uint32_t addr, uint32_t data_size)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* 初始化读命令 */
    if(SPI_MODE == mode) 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
        cmd_struct.addr_mode = OSPI_ADDRESS_1_LINE;
        cmd_struct.data_mode = OSPI_DATA_1_LINE;
        cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_8;
    } 
    else 
    {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_4_LINES;
        cmd_struct.addr_mode = OSPI_ADDRESS_4_LINES;
        cmd_struct.data_mode = OSPI_DATA_4_LINES;
        cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_16;
    }
  
    cmd_struct.instruction = GD25Q64CSIGR_FAST_READ_CMD;
    cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.address = addr;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.nbdata = data_size;

    /* 发送命令 */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* 接收数据 */
    ospi_receive(ospi_periph, pdata);
}

/*!
    \brief      write flash volatile configuration register
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                  prescaler: between 0 and 255
                  fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                  sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                  device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                  cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                  memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                  wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                  delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        OSPI_MODE: OSPI mode
    \param[in]  addr_size: the size of address
    \param[in]  value: the value of transmit
    \retval     none
*/
void OSPI_FLASH_WRITE_VOLATILECFG_REGISTER(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode, addr_size addr_size,
        uint32_t addr, uint8_t value)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize write enable for volatile status register command */
    if(SPI_MODE == mode) {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
    } else {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_8_LINES;
    }
    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction = 0x81U;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.addr_mode = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_mode = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata = 0;

    /* send the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* initialize write volatile configuration register command */
    if(SPI_MODE == mode) {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
        cmd_struct.addr_mode = OSPI_ADDRESS_1_LINE;
        cmd_struct.data_mode = OSPI_DATA_1_LINE;
    } else {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_8_LINES;
        cmd_struct.addr_mode = OSPI_INSTRUCTION_8_LINES;
        cmd_struct.data_mode = OSPI_INSTRUCTION_8_LINES;
    }
    if(GD25Q64CSIGR_3BYTES_SIZE == addr_size) {
        cmd_struct.addr_size = OSPI_ADDRESS_24_BITS;
    } else {
        cmd_struct.addr_size = OSPI_ADDRESS_32_BITS;
    }
    cmd_struct.operation_type = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction = 0x81U;
    cmd_struct.ins_size = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.address = addr;
    cmd_struct.addr_dtr_mode = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.alter_bytes_mode = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode = OSPI_ABDTR_MODE_DISABLE;
    cmd_struct.data_dtr_mode = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata = 1;

    /* send the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);
    ospi_transmit(ospi_periph, &value);
}
