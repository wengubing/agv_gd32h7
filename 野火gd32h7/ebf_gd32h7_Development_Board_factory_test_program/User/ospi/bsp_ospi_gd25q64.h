#ifndef __BSP_OSPI_GD25Q64_H
#define __BSP_OSPI_GD25Q64_H

#include "gd32h7xx_ospi.h"



#define  W25Q64CSIGR_FLASH_ID                           0xEF4017
#define  W25Q64CSIGR_MANUFACTURER_DEVICE_ID             0x16

#define  GD25Q64CSIGR_FLASH_ID                          0xC84017
#define  GD25Q64CSIGR_MANUFACTURER_DEVICE_ID            0x16


#define COUNTOF(A)               (sizeof(A) / sizeof(*(A)))
#define BUFFERSIZE_SPI               (COUNTOF(tx_buffer_spi) - 1)
#define BUFFERSIZE_OSPI               (COUNTOF(tx_buffer_ospi) - 1)
#define FLASH_WRITE_ADDRESS      0x000000
#define FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS
#define FLASH_WRITE_ADDRESS_1      0x200000
#define FLASH_READ_ADDRESS_1       FLASH_WRITE_ADDRESS_1
#define OSPI_INTERFACE           OSPI0


/*****************************************************************************************************************/

/* GD25Q64CSIGR大小配置 */
#define GD25Q64CSIGR_BLOCK_64K                    (uint32_t)(16*4*1024)                 /* 16 sectors of 64KBytes */
#define GD25Q64CSIGR_SECTOR_4K                    (uint32_t)(4 * 1024)                  /* 1 sectors of 4KBytes */

#define GD25Q64CSIGR_FLASH_SIZE                   (uint32_t)(64*1024*1024/8)           /* 64 Mbits => 8MBytes */
#define GD25Q64CSIGR_PAGE_SIZE                    (uint32_t)256                         /* pages of 256 Bytes */

/* GD25Q64CSIGR定时配置 */
#define GD25Q64CSIGR_BULK_ERASE_MAX_TIME          460000U
#define GD25Q64CSIGR_SECTOR_ERASE_MAX_TIME        1000U
#define GD25Q64CSIGR_SUBSECTOR_4K_ERASE_MAX_TIME  400U
#define GD25Q64CSIGR_WRITE_REG_MAX_TIME           40U

#define GD25Q64CSIGR_RESET_MAX_TIME               100U                                  /* when SWreset during erase operation */
#define GD25Q64CSIGR_AUTOPOLLING_INTERVAL_TIME    0x10U

#define GD25Q64CSIGR_OK                           (0)
#define GD25Q64CSIGR_ERROR                        (-1)


/* GD25Q64CSIGR命令 */
#define GD25Q64CSIGR_READ_CMD                                   0x03U       /* normal read 3 bytes address */
#define GD25Q64CSIGR_FAST_READ_CMD                              0x0BU       /* fast read 3 bytes address */

#define GD25Q64CSIGR_PAGE_PROG_CMD                              0x02U       /* page program 3 bytes address */

#define GD25Q64CSIGR_SECTOR_ERASE_4K_CMD                        0x20U       /* sector erase 4KB 3 bytes address */
#define GD25Q64CSIGR_BLOCK_ERASE_32K_CMD                        0x52U       /* block erase 32KB 3 bytes address */
#define GD25Q64CSIGR_BLOCK_ERASE_64K_CMD                        0xD8U       /* block erase 64KB 3 bytes address */
#define GD25Q64CSIGR_CHIP_ERASE_CMD                             0x60U       /* chip erase */
#define GD25Q64CSIGR_CHIP_ERASE_CMD_1                           0xC7U       /* chip erase */

#define GD25Q64CSIGR_WRITE_ENABLE_CMD                           0x06U       /* write enable */
#define GD25Q64CSIGR_WRITE_DISABLE_CMD                          0x04U       /* write disable */
#define GD25Q64CSIGR_PROG_ERASE_SUSPEND_CMD                     0x75U       /* program/erase suspend */
#define GD25Q64CSIGR_PROG_ERASE_RESUME_CMD                      0x7AU       /* program/erase resume */
#define GD25Q64CSIGR_ENTER_DEEP_POWER_DOWN_CMD                  0xB9U       /* enter deep power down */
#define GD25Q64CSIGR_RELEASE_FROM_DEEP_POWER_DOWN_CMD           0xABU       /* release form deep power down */

#define GD25Q64CSIGR_RESET_ENABLE_CMD                           0x66U       /*!< reset enable */
#define GD25Q64CSIGR_RESET_MEMORY_CMD                           0x99U       /*!< reset memory */

#define GD25Q64CSIGR_READ_ID_CMD                                0x9FU       /* read identification */
#define GD25Q64CSIGR_READ_SERIAL_FLASH_DISCO_PARAM_CMD          0x5AU       /* read serial flash discoverable parameter */
#define GD25Q64CSIGR_READ_UNIQUE_ID_CMD                         0x4BU       /* read unique identification */

/* 寄存器操作 */
#define GD25Q64CSIGR_READ_STATUS_REG_CMD                        0x05U       /* read status register */
#define GD25Q64CSIGR_READ_STATUS_REG1_CMD                      GD25Q64CSIGR_READ_STATUS_REG_CMD       /* read status register */
#define GD25Q64CSIGR_READ_STATUS_REG2_CMD                      0x35U       /* read status register */
#define GD25Q64CSIGR_READ_STATUS_REG3_CMD                      0x15U       /* read status register */

#define GD25Q64CSIGR_WRITE_STATUS_REG_CMD                       0x01U       /* write status register */
#define GD25Q64CSIGR_WRITE_STATUS_REG1_CMD                     GD25Q64CSIGR_WRITE_STATUS_REG_CMD       /* write status register */
#define GD25Q64CSIGR_WRITE_STATUS_REG2_CMD                     0x31U       /* write status register */
#define GD25Q64CSIGR_WRITE_STATUS_REG3_CMD                     0x11U       /* write status register */

#define GD25Q64CSIGR_READ_SECURITY_REG_CMD                      0x48U       /* read security register */
#define GD25Q64CSIGR_PROG_SECURITY_REG_CMD                      0x42U       /* program security register */
#define GD25Q64CSIGR_ERASE_SECURITY_REG_CMD                     0x44U       /* erase Security register */

/* GD25Q64CSIGR registers */
/* status register */
#define GD25Q64CSIGR_SR_WIP                                     0x01U       /* write in progress */
#define GD25Q64CSIGR_SR_WEL                                     0x02U       /* write enable latch */
#define GD25Q64CSIGR_SR_PB                                      0x3CU       /* block protected against program and erase operations */

typedef enum {
    SPI_MODE = 0,                             /* 1-1-1 commands, power on H/W default setting  */
    OSPI_MODE                                 /* 4-4-4 commands */
} interface_mode;

typedef enum {
    GD25Q64CSIGR_3BYTES_SIZE = 0,              /* 3 Bytes address mode */
    GD25Q64CSIGR_4BYTES_SIZE                   /* 4 Bytes address mode */
} addr_size;

typedef enum {
    GD25Q64CSIGR_ERASE_4K = 0,                 /* 4K size sector erase */
    GD25Q64CSIGR_ERASE_64K,                    /* 64K size block erase */
    GD25Q64CSIGR_ERASE_CHIP                    /* whole bulk erase */
} erase_size;

void OSPI_FLASH_INIT(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct);
uint32_t OSPI_FLASH_READ_ID(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode);
void OSPI_FLASH_RESET_ENABLE(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode);
void OSPI_FLASH_RESET_MEMORY(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode);
void OSPI_FLASH_WRITE_ENBALE(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode);
void OSPI_FLASH_AUTOPOLLING_MEM_READY(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode);
void OSPI_FLASH_BLOCK_ERASE(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode, uint32_t addr,
                            erase_size block_size);
void OSPI_FLASH_PAGE_PROGRAM(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode, addr_size addr_size, uint8_t *pdata,
                             uint32_t addr, uint32_t data_size);
void OSPI_FLASH_READ(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode, addr_size addr_size, uint8_t *pdata,
                     uint32_t addr, uint32_t data_size);

void OSPI_FLASH_WRITE_VOLATILECFG_REGISTER(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode, addr_size addr_size,
        uint32_t addr, uint8_t value);


#endif /* __BSP_OSPI_GD25Q64_H */
