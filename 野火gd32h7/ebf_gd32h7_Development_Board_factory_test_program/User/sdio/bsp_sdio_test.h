#ifndef __BSP_SDIO_TEST_H
#define __BSP_SDIO_TEST_H

#include "gd32h7xx.h"

//#define DATA_PRINT                                          /* uncomment the macro to print out the data */

#define NUMBER_OF_BLOCKS_START    100  /* For Multi Blocks operation (Read/Write) */
#define NUMBER_OF_BLOCKS_END      101  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_32BIT_NUM * (NUMBER_OF_BLOCKS_END-NUMBER_OF_BLOCKS_START))

ErrStatus memory_compare(uint32_t *src, uint32_t *dst, uint16_t length);
void SD_FormatInternal_Test(void);
void SD_Unlock_Lock_Test(void);
void SD_SingleBlock_EraseTest(uint32_t number_of_block);
void SD_SingleBlock_ReadTest(uint32_t number_of_block);
void SD_Data_Preparationt(void);
void SD_SingleBlock_WriteTest(uint32_t number_of_block);
uint32_t SD_SingleBlock_Test(uint32_t number_of_block);
void SD_MultiBlock_Test(uint32_t number_of_startblock, uint32_t number_of_endblock);
uint8_t SD_integrated_testing(void);

#endif /* __BSP_SDIO_TEST_H */
