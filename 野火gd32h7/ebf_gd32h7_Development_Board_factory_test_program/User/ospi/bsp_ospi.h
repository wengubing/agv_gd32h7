#ifndef __BSP_OSPI_H
#define	__BSP_OSPI_H

#include "gd32h7xx.h"


#define  W25Q64CSIGR_FLASH_ID                           0xEF4017
#define  W25Q64CSIGR_MANUFACTURER_DEVICE_ID             0x16

#define  GD25Q64CSIGR_FLASH_ID                          0xC84017
#define  GD25Q64CSIGR_MANUFACTURER_DEVICE_ID            0x16


#define COUNTOF(A)                  (sizeof(A) / sizeof(*(A)))
#define BUFFERSIZE_SPI              (COUNTOF(tx_buffer_spi) - 1)
#define BUFFERSIZE_OSPI             (COUNTOF(tx_buffer_ospi) - 1)
#define FLASH_WRITE_ADDRESS         0x000000
#define FLASH_READ_ADDRESS          FLASH_WRITE_ADDRESS
#define FLASH_WRITE_ADDRESS_1       0x200000
#define FLASH_READ_ADDRESS_1        FLASH_WRITE_ADDRESS_1
#define OSPI_INTERFACE              OSPI0

void OSPI_FLASH_INIT(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct);

#endif /* __BSP_OSPI_H */

