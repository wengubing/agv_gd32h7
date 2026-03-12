#ifndef __BSP_CAN_FD_H
#define	__BSP_CAN_FD_H

#include "gd32h7xx.h"

#define CANX                       	CAN2
#define CAN_CLK                    RCU_CAN2
#define CANX_ID                    0x1314U
#define CAN_TRANSMIT_NUM             0U
#define CAN_RECEIVE_NUM              1U

#define CAN_TX_GPIO_CLK           RCU_GPIOD
#define CAN_RX_GPIO_CLK           RCU_GPIOD
#define CAN_TX_GPIO_PORT            GPIOD
#define CAN_TX_PIN                 GPIO_PIN_13
#define CAN_RX_GPIO_PORT            GPIOD
#define CAN_RX_PIN                 GPIO_PIN_12
#define CAN_TX_RX_GPIO_AF           GPIO_AF_5

#define CAN_IRQ					    CAN2_Message_IRQn
#define CAN_RX_IRQHANDLER			CAN2_Message_IRQHandler

#define CAN_DEBUG_ON            1
#define CAN_DEBUG_ARRAY_ON      1
#define CAN_DEBUG_FUNC_ON       1

#define  BUFFER_SIZE    8U



#define CAN_INFO(FMT,ARG...)           printf("<<-CAN-INFO->> "FMT"\n",##ARG)
#define CAN_ERROR(FMT,ARG...)          printf("<<-CAN-ERROR->> "FMT"\n",##ARG)
#define CAN_DEBUG(FMT,ARG...)          do{\
																					 if(CAN_DEBUG_ON)\
																					 printf("<<-CAN-DEBUG->> [%d]"FMT"\n",__LINE__, ##ARG);\
																				 }while(0)

#define CAN_DEBUG_ARRAY(ARRAY, NUM)    do{\
                                         int32_t i;\
                                         uint8_t* a = ARRAY;\
                                         if(CAN_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-CAN-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (NUM); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)

#define CAN_DEBUG_FUNC()               do{\
                                         if(CAN_DEBUG_FUNC_ON)\
                                         printf("<<-CAN-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)








static void CAN_GPIO_Config(void);
static void CAN_Mode_Config(void); 
static void CAN_FD_Config(void);   
void CAN_Config(void);
void CAN_SetMsg(can_mailbox_descriptor_struct *transmit_message);
void Init_RxMes(can_mailbox_descriptor_struct *receive_message);




#endif /* __BSP_CAN_FD_H */
