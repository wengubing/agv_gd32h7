#ifndef __DRV_WATCHDOG_H__
#define __DRV_WATCHDOG_H__

#define	EXT_WatchDog_FUNC 1			

//外部看门狗WDI输入端口
#define GPIO_RCC_EX_WATCHDOG     (RCC_AHB1Periph_GPIOC)
#define GPIO_PORT_EX_WATCHDOG     GPIOC
#define GPIO_PIN_EX_WATCHDOG    GPIO_Pin_2

//D触发器CLR端口
#define GPIO_RCC_DType_CLR      (RCC_AHB1Periph_GPIOC)
#define GPIO_PORT_DType_CLR      GPIOC
#define GPIO_PIN_DType_CLR     GPIO_Pin_3

extern void  watchdog_init(u16 nms);
extern void  watchdog_feed(void);

#endif
