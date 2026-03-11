#ifndef BSP_LAN8720_H
#define BSP_LAN8720_H

#include "netif.h"
#include <stdbool.h>

//移植时，修改GD32库文件gd32h7xx_enet.h 修改PHY_TYPE为LAN8700    PHY_ADDRESS为((uint16_t)0U)

/* PHY register address */
#define LAN8720_BMCR                         0   /* 0x0: Basic Mode Control Register */
#define LAN8720_BMSR                         1   /* 0x1: Basic Mode Status Register */
#define LAN8720_PHYID1                       2   /* 0x2: PHY Identifier Register 1 */
#define LAN8720_PHYID2                       3   /* 0x3: PHY Identifier Register 2 */
#define LAN8720_SMR                          18  /* 0x12: Special Modes Register */
#define LAN8720_SECR                         26  /* 0x1A: Symbol Error Counter Register */
#define LAN8720_ISFR                         29  /* 0x1D: Interrupt Source Flag Register */
#define LAN8720_IMR                          30  /* 0x1E: Interrupt Mask Register */
#define LAN8720_PSCSR                        31  /* 0x1F: PHY Special Control/Status Register */

#define LAN8720_ID1                          0x7
#define LAN8720_ID2                          0xc0f

/* function declarations */
/* setup ethernet system(GPIOs, clocks, MAC, DMA, systick) */
void enet_system_setup(void);
void enet_systick_setup(void);
void PrintfPhyReg(void);
void HardwareResetPhy(void);
bool lan8720_check_id( void );
uint16_t lan8720_get_phystatus( void );
uint16_t lan8720_get_connectionstate( void );

#endif /* BSP_LAN8720_H */
