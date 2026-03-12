#ifndef __DRV_CAN_GD32H7_H__
#define __DRV_CAN_GD32H7_H__

#include "gd32h7xx.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef DRV_CAN_GD32H7_GLOBALS
    #define DRV_CAN_EXT
#else
    #define DRV_CAN_EXT extern
#endif

/* CAN configuration for CANopen */
#define CANOPEN_CANX                CAN2
#define CANOPEN_CAN_CLK             RCU_CAN2
#define CANOPEN_CAN_TX_GPIO_CLK     RCU_GPIOD
#define CANOPEN_CAN_RX_GPIO_CLK     RCU_GPIOD
#define CANOPEN_CAN_TX_GPIO_PORT    GPIOD
#define CANOPEN_CAN_TX_PIN          GPIO_PIN_13
#define CANOPEN_CAN_RX_GPIO_PORT    GPIOD
#define CANOPEN_CAN_RX_PIN          GPIO_PIN_12
#define CANOPEN_CAN_GPIO_AF         GPIO_AF_5
#define CANOPEN_CAN_IRQn            CAN2_Message_IRQn
#define CANOPEN_CAN_IRQHandler      CAN2_Message_IRQHandler

/* Mailbox allocation:
 * Mailboxes 0-3: TX
 * Mailboxes 4-11: RX (accept all CANopen messages)
 */
#define CANOPEN_TX_MB_START         0U
#define CANOPEN_TX_MB_COUNT         4U
#define CANOPEN_RX_MB_START         4U
#define CANOPEN_RX_MB_COUNT         8U

/* Software FIFO sizes */
#define CAN_RX_BUF_LENGTH_MAX       64U
#define CAN_TX_BUF_LENGTH_MAX       64U

/* Baud rate types */
typedef enum {
    CAN_BAUD_INVALID = 0,
    CAN_BAUD_5K,
    CAN_BAUD_10K,
    CAN_BAUD_20K,
    CAN_BAUD_40K,
    CAN_BAUD_50K,
    CAN_BAUD_80K,
    CAN_BAUD_100K,
    CAN_BAUD_125K,
    CAN_BAUD_200K,
    CAN_BAUD_250K,
    CAN_BAUD_400K,
    CAN_BAUD_500K,
    CAN_BAUD_800K,
    CAN_BAUD_1M,
} CAN_BAUD_TYPE;

/* CAN message structure */
typedef struct {
    uint32_t id;        /* CAN ID (11-bit standard) */
    uint8_t  ide;       /* 0=standard, 1=extended */
    uint8_t  rtr;       /* 0=data frame, 1=remote frame */
    uint8_t  dlc;       /* data length 0-8 */
    uint8_t  data[8];   /* data bytes */
} GD32_CAN_Msg_t;

/* RX FIFO */
typedef struct {
    GD32_CAN_Msg_t   msg[CAN_RX_BUF_LENGTH_MAX];
    volatile uint16_t read_adr;
    volatile uint16_t write_adr;
} GD32_CAN_RX_FIFO_t;

/* TX FIFO */
typedef struct {
    GD32_CAN_Msg_t   msg[CAN_TX_BUF_LENGTH_MAX];
    volatile uint16_t read_adr;
    volatile uint16_t write_adr;
} GD32_CAN_TX_FIFO_t;

/* CAN interface instance */
typedef struct {
    GD32_CAN_RX_FIFO_t rx_fifo;
    GD32_CAN_TX_FIFO_t tx_fifo;
    uint8_t tx_mb_idx;   /* current TX mailbox index, rotating 0-(CANOPEN_TX_MB_COUNT-1) */
} GD32H7_CANi_t;

/* Global CAN instance */
DRV_CAN_EXT GD32H7_CANi_t GD32_CAN;

/* Function prototypes */
void    GD32_CAN_Init(CAN_BAUD_TYPE baud, uint8_t irq_priority);
uint8_t GD32_CAN_SendMsg(GD32_CAN_Msg_t *msg);
void    GD32_CAN_RxISR(void);

#endif /* __DRV_CAN_GD32H7_H__ */
