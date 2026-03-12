/**
  ******************************************************************************
  * @file     main.c
  * @author   embedfire / hangfa
  * @version  V2.0
  * @date     2025
  * @brief    GD32H7 RTX5 主程序（集成CANopen主站）
  ******************************************************************************
  */

#include "gd32h7xx.h"
#include <stdio.h>
#include "cmsis_os2.h"

/* 底层驱动 */
#include "bsp_gpio_led.h"
#include "bsp_gpio_key.h"
#include "bsp_usart.h"                                      /* 串口打印调试 */

/* CANopen主站 */
#include "Drivers/CanOpen/drv_servo_gd32h7.h"              /* 伺服数据结构及公共类型 */
#include "Drivers/CanOpen/example/ObjectDictionary.h"       /* 对象字典 */
#include "Drivers/CanOpen/example/LinkCanopenMaster.h"      /* CANopen主站接口 */

/* ����ջ�����Զ���
 * ʹ�� uint64_t �������鶨��ջ��ȷ�� 8 �ֽڶ��루Cortex-M7 Ҫ��ջ���룩
 * �����С = 512/8 = 64 ��Ԫ�أ��ܴ�С = 64 * 8 = 512 �ֽ�
 */
static uint64_t LED_stack[512/8];                     // LED ����ջ�ռ�
static const osThreadAttr_t LED_attr =                // LED ��������
{
    .name       = "LED",                               // �������ƣ����ڵ��ԣ�
    .stack_mem  = LED_stack,                           // ָ������ջ��ָ��
    .stack_size = sizeof(LED_stack),                   // ջ��С���ֽڣ�
    .priority   = osPriorityNormal                      // �������ȼ�����ͨ��
};

static uint64_t Key_stack[512/8];                     // LED ����ջ�ռ�
static const osThreadAttr_t Key_attr =                // LED ��������
{
    .name       = "Key",                               // �������ƣ����ڵ��ԣ�
    .stack_mem  = Key_stack,                           // ָ������ջ��ָ��
    .stack_size = sizeof(Key_stack),                   // ջ��С���ֽڣ�
    .priority   = osPriorityHigh                      // �������ȼ����ߣ�
};

static uint64_t StartThread_stack[1024/8];              // �������� ����ջ�ռ�
static const osThreadAttr_t ThreadStart_Attr =          // �������� ��������
{
    .name       = "osRtxStartThread",                   // �������ƣ����ڵ��ԣ�
    .stack_mem  = StartThread_stack,                    // ָ������ջ��ָ��
    .stack_size = sizeof(StartThread_stack),            // ջ��С���ֽڣ�
    .priority   = osPriorityIdle                      	// �������ȼ�����ͣ�
};

/* CANopen 任务栈及属性 */
static uint64_t CanOpen_stack[2048/8];              /* CANopen任务堆栈（2KB，协议栈需要较大堆栈） */
static const osThreadAttr_t CanOpen_attr = {
    .name       = "CANopen",
    .stack_mem  = CanOpen_stack,
    .stack_size = sizeof(CanOpen_stack),
    .priority   = osPriorityAboveNormal             /* 高于普通任务，确保CAN报文及时处理 */
};

/* ---为消息和信号量测试自定义任务栈声明 --- */
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
    .priority = osPriorityAboveNormal 					// �Ը����ȼ���ģ����Ҫͬ��������
};

/* ---����ȫ�ֶ���ID --- */
osMessageQueueId_t msgQueue;           // ��Ϣ����ID
osSemaphoreId_t binarySemaphore;       // ��ֵ�ź���ID

/*函数声明 --- */
void Sender_task(void *arg);    /* 消息队列发送任务 */
void Receiver_task(void *arg);  /* 消息队列接收任务 */
void Semaphore_task(void *arg); /* 信号量同步任务 */
void LED_task(void *arg);       /* LED 闪烁任务 */
void Key_task(void *arg);       /* 按键扫描任务 */
void AppTaskStart(void *arg);   /* 启动任务 */
void CanOpen_task(void *arg);   /* CANopen主站任务 */


/* 线程ID */
osThreadId_t ThreadIdSender = NULL;
osThreadId_t ThreadIdReceiver = NULL;
osThreadId_t ThreadIdSemaphore = NULL;
osThreadId_t ThreadIdTaskLED = NULL;
osThreadId_t ThreadIdStart = NULL;
osThreadId_t ThreadIdTaskKey = NULL;

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 硬件初始化，启动RTX5 内核
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
    /* 使能cpu加速缓存*/
    SCB_EnableICache();     /* 使能 I-Cache */
    SCB_EnableDCache();     /* 使能 D-Cache */

    /* 系统初始化 */
    SystemCoreClockUpdate();                            /* 更新全局变量 SystemCoreClock */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);   /* 设置中断优先级组为 4 位抢占优先级（RTX5 要求）*/

    /* 使能 FPU（浮点运算单元） */
    /* Cortex-M7 内核需要手动使能 FPU，否则浮点指令会触发 HardFault */
    SCB->CPACR |= (3UL << 20) | (3UL << 22);            /* 使能 CP10、CP11 全权限 */

    /* 初始化按键 */
    KEY_GPIO_Config();

    /* 初始化串口打印 */
    USART_Config();
    printf("main: 串口打印初始化ok\r\n");
    printf("main: 系统时钟 %lu Hz\r\n", SystemCoreClock);

    /* RTX5 内核初始化 */
    osKernelInitialize();                               /* 初始化内核对象、就绪列表等 */

    /* 创建启动任务 */
    ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  

    /* ����������� */
    osKernelStart();                                    // ��ʼ��������ȣ��˺������᷵��

    /* �����ϲ���ִ�е������Ϊ��ֹ���⣬������ѭ�� */
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
    /* 创建消息队列发送任务 */
    ThreadIdSender = osThreadNew(Sender_task, NULL, &Sender_attr);
    /* 创建消息队列接收任务 */
    ThreadIdReceiver = osThreadNew(Receiver_task, NULL, &Receiver_attr);
    /* 创建信号量同步任务 */
    ThreadIdSemaphore = osThreadNew(Semaphore_task, NULL, &SemaphoreTask_attr);
    /* 创建LED闪烁任务 */
    ThreadIdTaskLED = osThreadNew(LED_task, NULL, &LED_attr);
    /* 创建按键扫描任务 */
    ThreadIdTaskKey = osThreadNew(Key_task, NULL, &Key_attr);
    /* 创建CANopen主站任务 */
    osThreadNew(CanOpen_task, NULL, &CanOpen_attr);
}

/*
*********************************************************************************************************
*	�� �� ��: LED_task
*	����˵��: ����LED1��LED2���� LED ������˸����
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal  
*********************************************************************************************************
*/
void LED_task(void *arg)
{
	const uint16_t usFrequency = 1000; /* �ӳ����� */
	uint32_t tick;

	/* ��ȡ��ǰʱ�� */
	tick = osKernelGetTickCount(); 
    while(1)
    {
		//LED״̬��ת
        LED1_TOGGLE;
		
		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: Sender_task
*	����˵��: ��Ϣ���з�������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal  
*********************************************************************************************************
*/
void Sender_task(void *arg) 
{
	const uint16_t usFrequency = 2000; /* �ӳ����ڣ�ÿ2�뷢��һ�� */
	uint32_t tick;
    uint32_t send_data = 0;
    osStatus_t status;

    while(1) 
	{
        send_data++;
        /* ���Խ����������ݷ�����У���ʱʱ��Ϊ0����ʾ������������������� */
        status = osMessageQueuePut(msgQueue, &send_data, 0, 0);
        if (status == osOK) 
		{
			LED2_TOGGLE;
            // Ҳ����������ͨ�����ڴ�ӡָʾ���ͳɹ�
            // printf("Sent: %d\n", send_data);
        } else 
		{
            // ����������������
        }
		
		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: Receiver_task
*	����˵��: ��Ϣ���н�������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal  
*********************************************************************************************************
*/
void Receiver_task(void *arg) 
{
    uint32_t recv_data;
    osStatus_t status;

    while(1) {
        /* �ȴ�������Ϣ����ʱʱ��ΪosWaitForever����ʾһֱ�ȵ�����ϢΪֹ */
        status = osMessageQueueGet(msgQueue, &recv_data, NULL, osWaitForever);
        if (status == osOK) 
		{
            // ���յ����ݣ����������ﴦ�������緭תLED1��ָʾ
            // printf("Received: %d\n", recv_data);
            LED_2(0); // ������˸һ��LED1��Ϊ����ָʾ
            osDelay(50);
            LED_2(1);
        }
    }
}

/*
*********************************************************************************************************
*	�� �� ��: Semaphore_task
*	����˵��: �ź���ͬ������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityAboveNormal  
*********************************************************************************************************
*/
void Semaphore_task(void *arg) 
{
    osStatus_t status;
    while(1) 
	{
        /* �ȴ��ź�������ʱʱ��ΪosWaitForever */
        status = osSemaphoreAcquire(binarySemaphore, osWaitForever);
        if (status == osOK) 
		{
            /* �ɹ���ȡ���ź�����ִ����Ҫͬ���Ĳ��� */
            LED_3(0); 		// ����LED2ָʾ
            osDelay(500); 	// ģ�⴦��ʱ��
            LED_3(1);
            /* ע�⣺��ֵ�ź�������ȡ�󣬼���ֵ��Ϊ0�����������������ISR�ٴ��ͷţ������´λ�ȡ�������� */
        }
    }
}

/*
*********************************************************************************************************
*	�� �� ��: Key_task
*	����˵��: ����������(ʾ����ģ���ⲿ�¼��ͷ��ź���)
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityHigh  
*********************************************************************************************************
*/
volatile uint32_t SemaphoreCnt;
void Key_task(void *arg) 
{	
	const uint16_t usFrequency = 50; /* �ӳ����� */
	uint32_t tick;

	/* ��ȡ��ǰʱ�� */
	tick = osKernelGetTickCount(); 
	
    while(1) 
	{
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			/* ��ISR���ͷ��ź�����RTX5֧�ִ��ж��е���osSemaphoreRelease */
			osSemaphoreRelease(binarySemaphore);
			SemaphoreCnt++;			
		}
		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}



/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityIdle
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
    const uint16_t usFrequency = 1; /* 延迟频率 */
    uint32_t tick;

    /* LED驱动GPIO初始化 */
    LED_GPIO_Config();

    /* 初始化按键 */
    KEY_GPIO_Config();

    /* ---创建消息队列 --- */
    msgQueue = osMessageQueueNew(5, sizeof(uint32_t), NULL);
    if (msgQueue == NULL) {
        while(1);
    }

    /* ---创建二值信号量 --- */
    binarySemaphore = osSemaphoreNew(1, 0, NULL);
    if (binarySemaphore == NULL) {
        while(1);
    }

    /* 创建任务 */
    AppTaskCreate();

    /* 获取当前时间 */
    tick = osKernelGetTickCount();

    while(1)
    {
        /* 更新系统毫秒计数器，供CanOpen延时判断使用 */
        g_Sys_1ms_Counter = (u32)osKernelGetTickCount();

        /* 延迟 */
        tick += usFrequency;
        osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: CanOpen_task
*	功能说明: CANopen主站任务
*           - 初始化CANopen主站（CAN硬件 + CanFestival定时器）
*           - 周期处理：CAN接收分发、CAN发送、主站状态机推进
*	优 先 级: osPriorityAboveNormal
*	调用周期: 1ms
*
* 使用说明：
*   调用本任务前请先配置 CanMaster.Para：
*     CanMaster.Para.Baud     = CAN_BAUD_500K;       // 波特率
*     CanMaster.Para.SVOType  = Can_SVOType_Copley;  // 伺服类型
*     CanMaster.Para.CheckQuantity = 4;              // 期望上线从站数量
*     CanMaster.in_CanOpenStart = true;              // 启动CANopen
*********************************************************************************************************
*/
void CanOpen_task(void *arg)
{
    uint32_t tick;

    /* 配置CANopen默认参数（可在启动前由上层覆盖） */
    CanMaster.Para.Baud         = CAN_BAUD_500K;
    CanMaster.Para.SVOType      = Can_SVOType_Copley;
    CanMaster.Para.CheckQuantity = 1;
    CanMaster.in_CanOpenStart   = true;

    /* 启动CanFestival定时器（TIMER2, NVIC优先级1） */
    canfestival_timer_start(1U, ENABLE);

    printf("CanOpen_task: CANopen主站任务启动\r\n");

    tick = osKernelGetTickCount();

    while(1)
    {
        /* 更新毫秒计数器 */
        g_Sys_1ms_Counter = (u32)osKernelGetTickCount();

        /* 处理CAN接收（从软件FIFO读取，提交给协议栈） */
        Tsk_CanOpen_RxMsgPro(&CanObjectDict_Data);

        /* 驱动CAN发送（从软件FIFO读取，写入硬件） */
        Tsk_CanOpen_TxMsgPro(&CanObjectDict_Data);

        /* 推进CANopen主站状态机 */
        tsk_canopen_master(&CanMaster, &CanObjectDict_Data);

        /* 延迟1ms */
        tick += 1U;
        osDelayUntil(tick);
    }
}

/*********************************************END OF FILE**********************/
