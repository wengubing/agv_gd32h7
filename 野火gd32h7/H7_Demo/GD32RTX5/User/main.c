/**
  ******************************************************************************
  * @file     main.c
  * @author   embedfire
  * @version  V1.0
  * @date     2023
  * @brief    测试led
  ******************************************************************************
  */ 

#include "gd32h7xx.h"
#include <stdio.h>
#include "cmsis_os2.h"

//底层驱动
#include "bsp_gpio_led.h"    
#include "bsp_gpio_key.h" 
#include "bsp_usart.h"                                      //调试打印串口
#include "can_test.h"                                       //can收发测试

/* 任务栈和属性定义
 * 使用 uint64_t 类型数组定义栈，确保 8 字节对齐（Cortex-M7 要求栈对齐）
 * 数组大小 = 512/8 = 64 个元素，总大小 = 64 * 8 = 512 字节
 */
static uint64_t LED_stack[512/8];                     // LED 任务栈空间
static const osThreadAttr_t LED_attr =                // LED 任务属性
{
    .name       = "LED",                               // 任务名称（用于调试）
    .stack_mem  = LED_stack,                           // 指向任务栈的指针
    .stack_size = sizeof(LED_stack),                   // 栈大小（字节）
    .priority   = osPriorityNormal                      // 任务优先级（普通）
};

static uint64_t Key_stack[512/8];                     // LED 任务栈空间
static const osThreadAttr_t Key_attr =                // LED 任务属性
{
    .name       = "Key",                               // 任务名称（用于调试）
    .stack_mem  = Key_stack,                           // 指向任务栈的指针
    .stack_size = sizeof(Key_stack),                   // 栈大小（字节）
    .priority   = osPriorityHigh                      // 任务优先级（高）
};

static uint64_t StartThread_stack[1024/8];              // 启动任务 任务栈空间
static const osThreadAttr_t ThreadStart_Attr =          // 启动任务 任务属性
{
    .name       = "osRtxStartThread",                   // 任务名称（用于调试）
    .stack_mem  = StartThread_stack,                    // 指向任务栈的指针
    .stack_size = sizeof(StartThread_stack),            // 栈大小（字节）
    .priority   = osPriorityIdle                      	// 任务优先级（最低）
};

/* ---为队列和信号量测试定义的任务栈和属性 --- */
static uint64_t Sender_stack[512/8];
static const osThreadAttr_t Sender_attr = 
{
    .name = "Sender",
    .stack_mem = Sender_stack,
    .stack_size = sizeof(Sender_stack),
    .priority = osPriorityNormal
};
static uint64_t Receiver_stack[512/8];
static const osThreadAttr_t Receiver_attr = 
{
    .name = "Receiver",
    .stack_mem = Receiver_stack,
    .stack_size = sizeof(Receiver_stack),
    .priority = osPriorityNormal
};
static uint64_t SemaphoreTask_stack[512/8];
static const osThreadAttr_t SemaphoreTask_attr = 
{
    .name = "SemTask",
    .stack_mem = SemaphoreTask_stack,
    .stack_size = sizeof(SemaphoreTask_stack),
    .priority = osPriorityAboveNormal 					// 稍高优先级，模拟需要同步的任务
};

/* ---声明全局对象ID --- */
osMessageQueueId_t msgQueue;           // 消息队列ID
osSemaphoreId_t binarySemaphore;       // 二值信号量ID

/*任务函数声明 --- */
void Sender_task(void *arg);	//消息队列发送任务
void Receiver_task(void *arg);	//消息队列接收任务
void Semaphore_task(void *arg);	//信号量同步任务
void LED_task(void *arg);      	// LED 控制任务
void Key_task(void *arg);		//按键检测任务
void AppTaskStart(void *arg);	//启动任务


/* 任务句柄 */
osThreadId_t ThreadIdSender = NULL;
osThreadId_t ThreadIdReceiver = NULL;
osThreadId_t ThreadIdSemaphore = NULL;
osThreadId_t ThreadIdTaskLED = NULL;
osThreadId_t ThreadIdStart = NULL;
osThreadId_t ThreadIdTaskKey = NULL;

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 主函数，系统初始化、RTX5 内核启动
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
	/* 使能cpu高速缓存*/ 
    SCB_EnableICache();     // 使能 I-Cache
    SCB_EnableDCache();     // 使能 D-Cache
    
    /* 系统初始化 */
    SystemCoreClockUpdate();                            // 更新全局变量 SystemCoreClock
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);   // 设置中断优先级分组为 4 位抢占优先级（RTX5 要求）

    /* 启用 FPU（浮点运算单元） */
    /* Cortex-M7 内核需要手动开启 FPU，否则浮点指令会触发 HardFault */
    SCB->CPACR |= (3UL << 20) | (3UL << 22);            // 启用 CP10、CP11 全权限
	
	/* 初始化按键 */
    KEY_GPIO_Config();
	
	/* 初始化调试串口 */
	USART_Config();
	printf("main：调试打印测试ok\r\n");
	
	/*can收发测试 初始化*/
	CanTest_Init();
	printf("main：CanTest_Init运行ok\r\n");
	printf("main：开始CanTest_SendOnce()\r\n");
	CanTest_SendOnce();
	
	/*临时循环can发送测试
	while(1)
	{
		CanTest_KeySend();
	}*/
	

    /* RTX5 内核初始化 */
    osKernelInitialize();                               // 初始化内核对象、就绪列表等

	/* 创建启动任务 */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  

    /* 启动任务调度 */
    osKernelStart();                                    // 开始多任务调度，此函数不会返回

    /* 理论上不会执行到这里，但为防止意外，保留死循环 */
    while(1);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	//创建消息队列发送任务
	ThreadIdSender = osThreadNew(Sender_task, NULL, &Sender_attr);
	//创建消息队列接收任务
	ThreadIdReceiver = osThreadNew(Receiver_task, NULL, &Receiver_attr);
	//创建信号量同步任务
	ThreadIdSemaphore = osThreadNew(Semaphore_task, NULL, &SemaphoreTask_attr);
	//创建LED闪烁任务
	ThreadIdTaskLED = osThreadNew(LED_task, NULL, &LED_attr);
	//创建按键检测任务
	ThreadIdTaskKey = osThreadNew(Key_task, NULL, &Key_attr);
}

/*
*********************************************************************************************************
*	函 数 名: LED_task
*	功能说明: 控制LED1与LED2两个 LED 交替闪烁任务。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal  
*********************************************************************************************************
*/
void LED_task(void *arg)
{
	const uint16_t usFrequency = 1000; /* 延迟周期 */
	uint32_t tick;

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
    while(1)
    {
		//LED状态翻转
        LED1_TOGGLE;
		
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: Sender_task
*	功能说明: 消息队列发送任务。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal  
*********************************************************************************************************
*/
void Sender_task(void *arg) 
{
	const uint16_t usFrequency = 2000; /* 延迟周期，每2秒发送一次 */
	uint32_t tick;
    uint32_t send_data = 0;
    osStatus_t status;

    while(1) 
	{
        send_data++;
        /* 尝试将递增的数据放入队列，超时时间为0，表示如果队列满则立即返回 */
        status = osMessageQueuePut(msgQueue, &send_data, 0, 0);
        if (status == osOK) 
		{
			LED2_TOGGLE;
            // 也可以在这里通过串口打印指示发送成功
            // printf("Sent: %d\n", send_data);
        } else 
		{
            // 队列满，处理错误
        }
		
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: Receiver_task
*	功能说明: 消息队列接收任务。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal  
*********************************************************************************************************
*/
void Receiver_task(void *arg) 
{
    uint32_t recv_data;
    osStatus_t status;

    while(1) {
        /* 等待接收消息，超时时间为osWaitForever，表示一直等到有消息为止 */
        status = osMessageQueueGet(msgQueue, &recv_data, NULL, osWaitForever);
        if (status == osOK) 
		{
            // 接收到数据，可以在这里处理，比如翻转LED1来指示
            // printf("Received: %d\n", recv_data);
            LED_2(0); // 快速闪烁一下LED1作为接收指示
            osDelay(50);
            LED_2(1);
        }
    }
}

/*
*********************************************************************************************************
*	函 数 名: Semaphore_task
*	功能说明: 信号量同步任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityAboveNormal  
*********************************************************************************************************
*/
void Semaphore_task(void *arg) 
{
    osStatus_t status;
    while(1) 
	{
        /* 等待信号量，超时时间为osWaitForever */
        status = osSemaphoreAcquire(binarySemaphore, osWaitForever);
        if (status == osOK) 
		{
            /* 成功获取到信号量，执行需要同步的操作 */
            LED_3(0); 		// 点亮LED2指示
            osDelay(500); 	// 模拟处理时间
            LED_3(1);
            /* 注意：二值信号量被获取后，计数值变为0。除非有其他任务或ISR再次释放，否则下次获取会阻塞。 */
        }
    }
}

/*
*********************************************************************************************************
*	函 数 名: Key_task
*	功能说明: 按键检测程序(示例：模拟外部事件释放信号量)
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityHigh  
*********************************************************************************************************
*/
volatile uint32_t SemaphoreCnt;
void Key_task(void *arg) 
{	
	const uint16_t usFrequency = 50; /* 延迟周期 */
	uint32_t tick;

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
    while(1) 
	{
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			/* 在ISR中释放信号量。RTX5支持从中断中调用osSemaphoreRelease */
			osSemaphoreRelease(binarySemaphore);
			SemaphoreCnt++;			
		}
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}



/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityIdle  
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 1; /* 延迟周期 */
	uint32_t tick;
	
	/*LED相关GPIO初始化*/
	LED_GPIO_Config();
	
	/* 初始化按键 */
	KEY_GPIO_Config();
	
	
    /* ---创建消息队列 --- */
    // 创建一个可以存储5条消息，每条消息大小为4字节（正好放一个整数）的队列
    msgQueue = osMessageQueueNew(5, sizeof(uint32_t), NULL);
    if (msgQueue == NULL) 
	{
        // 处理创建失败的情况
        while(1);
    }

    /* ---创建二值信号量 --- */
    // 创建一个最大计数值为1，初始计数值为0的二值信号量
    binarySemaphore = osSemaphoreNew(1, 0, NULL);
    if (binarySemaphore == NULL) 
	{
        while(1);
    }	

	/* 创建任务 */
	AppTaskCreate();

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		/* 需要周期性处理的程序*/

		
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}
/*********************************************END OF FILE**********************/
