/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : RTX5及其所有中间件的工程模板实现。
*              实验目的：
*                1. 学习RTX5及其所有中间件的工程模板制作。
*              实验内容：
*                RL-USB + RL-FlashFS文件系统操作：
*                请选择串口操作命令，电脑键盘打印数字即可
*                printf("首次使用，如果操作NAND，务必调用串口命令1，进行NAND Flash的低级格式化和文件系统格式化\r\n");	
*                printf("1 - 显示卡容量和剩余容量\r\n");
*                printf("2 - 显示卡根目录下的文件\r\n");
*                printf("3 - 创建三个text文本并使用不同函数写入内容\r\n");
*                printf("4 - 使用三个不同函数读取文本\r\n");
*                printf("5 - 创建一个text文本并指定一个位置对其进行读写操作\r\n");
*                printf("6 - 删除文件夹和文件\r\n");
*                printf("7 - 测试文件读写速度\r\n");
*                printf("8 - 写数据到CSV文件中\r\n");
*                printf("9 - 从CSV文件中读数据\r\n");
*                printf("m - 所有操作是针对SD卡命令\r\n");
*                printf("u - 所有操作针对U盘命令\r\n");
*                printf("n - 所有操作针对NAND命令\r\n");   
*                printf("a - 打开SD和NAND模拟U盘\r\n");
*                printf("b - 关闭SD和NAND模拟U盘\r\n");
*                printf("f - 启动FTP前，挂载SD卡\r\n");
*                printf("t - 关闭FTP后，卸载SD卡\r\n");
*                RL-TCPnet网络协议操作：
*                （1）强烈推荐将网线接到路由器或者交换机上面测试，因为已经使能了DHCP，可以自动获取IP地址。
*                     创建了一个TCP Server，而且使能了局域网域名NetBIOS，用户只需在电脑端ping armfly
*                     就可以获得板子的IP地址，端口号1001。
*                （2）用户可以在电脑端用网络调试软件创建TCP Client连接此服务器端。
*                （3）按键K1按下，发送8字节的数据给TCP Client。
*                （4）按键K2按下，发送1024字节的数据给TCP Client。
*                （5）按键K3按下，发送5MB字节的数据给TCP Client。
*                （6）各个任务实现的功能如下：
*                     AppTaskUserIF任务   : 按键消息处理。
*                     AppTaskLED任务      : LED闪烁。
*                     AppTaskMsgPro任务   : TCPnet应用任务。
*                     AppTaskEthCheck任务 : 网线插拔状态检测。
*                     AppTaskGUI任务      : GUI任务。
*                     AppTaskFsUsb任务    : 文件系统和USB网络任务。
*                     AppTaskStart任务    : 启动任务，也是最高优先级任务，这里用作BSP驱动包处理。
*                     netCore_Thread任务  : TCPnet内核任务。
*                     netEth0_Thread任务  : TCPnet以太网接口任务。
*                     USBD0_Core_Thread任务: USB内核任务。
*                     USBD_MSC0_Thread任务 : 大容量存储设备任务。
*                     osRtxTimerThread任务: 定时器任务，TCPnet时间基准。
*              注意事项：
*                1. 为了RTX5的调试组件正常使用，RTX5的工程路径切不要有中文，路径不要太长。
*                   而且退出调试状态要清除所有断点。
*                2. 本实验推荐使用串口软件SecureCRT查看打印信息，波特率115200，数据位8，奇偶校验位无，停止位1。
*                3. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2021-02-08   Eric2013     1. CMSIS软包版本 V5.7.0
*                                         2. HAL库版本 V1.9.0
*                                         3. RTX5版本5.5.1
*                                         4. Event Recorder版本1.4.0
*                                         5. RL-TCPnet版本V7.X
*                                         6. RL-USB版本V6.X
*                                         7. RL-TCPnet版本V6.X
*                                         8. emWin版本V6.X
*		V1.1    2021-02-28   Eric2013     1. 添加FTP服务器功能，SD卡做存储介质。
*                                         2. 支持双USB外设同时使用
*                                            (1)MicroUSB接口是USB FS，用于在电脑端虚拟出SD卡和NAND两个U盘
*                                            (2)Type-A接口是USB HS，用于外挂U盘。
*                                         3. 支持了RL-FlashFS外挂SD卡，NAND和U盘三合一
*                                            
*
*	Copyright (C), 2021-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"
#include "MainTask.h"



/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_taskdbg printf
#else
	#define printf_taskdbg(...)
#endif


/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void AppTaskCreate (void);
void AppTaskUserIF(void *argument);
void AppTaskLED(void *argument);
void AppTaskMsgPro(void *argument);
void AppTaskStart(void *argument);


/*
**********************************************************************************************************
											 变量
**********************************************************************************************************
*/
/* 任务的属性设置 */
const osThreadAttr_t ThreadStart_Attr = 
{
	/* 未使用 */
//	.cb_mem = &worker_thread_tcb_1,
//	.cb_size = sizeof(worker_thread_tcb_1),
//	.stack_mem = &worker_thread_stk_1[0],
//	.stack_size = sizeof(worker_thread_stk_1),
//	.priority = osPriorityAboveNormal,
//	.tz_module = 0
	
	.name = "osRtxStartThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal6,
	.stack_size = 2048,
};

const osThreadAttr_t ThreadEthCheck_Attr = 
{
	.name = "osRtxEthCheckThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal5,
	.stack_size = 1024,
};

const osThreadAttr_t ThreadMsgPro_Attr = 
{
	.name = "osRtxMsgProThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal4,
	.stack_size = 2048,
};

const osThreadAttr_t ThreadFsUsb_Attr = 
{
	.name = "osRtxFsUsbThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal3,
	.stack_size = 4096,
};

const osThreadAttr_t ThreadLED_Attr = 
{
	.name = "osRtxLEDThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal2,
	.stack_size = 512,
};

const osThreadAttr_t ThreadUserIF_Attr = 
{
	.name = "osRtxThreadUserIF",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal1,
	.stack_size = 1024,
};

const osThreadAttr_t ThreadGUI_Attr = 
{
	.name = "osRtxThreadGUIX",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal,
	.stack_size = 2048,
};

/* 任务句柄 */
osThreadId_t ThreadIdTaskGUI = NULL;
osThreadId_t ThreadIdTaskFsUsb = NULL;
osThreadId_t ThreadIdTaskUserIF = NULL;
osThreadId_t ThreadIdTaskEthCheck = NULL;
osThreadId_t ThreadIdTaskMsgPro = NULL;
osThreadId_t ThreadIdTaskLED = NULL;
osThreadId_t ThreadIdStart = NULL;


/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main (void) 
{	
	/* HAL库，MPU，Cache，时钟等系统初始化 */
	System_Init();

	/* 内核开启前关闭HAL的时间基准 */
	HAL_SuspendTick();
	
	/* 内核初始化 */
	osKernelInitialize();                                  

	/* 创建启动任务 */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  

	/* 开启多任务 */
	osKernelStart();
	
	while(1);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUI
*	功能说明: GUI应用任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal  (数值越小优先级越低，这个跟uCOS相反) 
*********************************************************************************************************
*/
void AppTaskGUI(void *argument)
{
	while(1)
	{
		MainTask();
	}	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 按键消息处理		
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal1
*********************************************************************************************************
*/
void AppTaskUserIF(void *argument)
{
	uint8_t ucKeyCode;

    while(1)
    {
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				/* K1键按下，直接发送事件标志给任务AppTaskTCPMain，设置bit0 */
				case KEY_DOWN_K1:
					printf("K1键按下，直接发送事件标志给任务ThreadIdTaskMsgPro，bit0被设置\r\n");
					osThreadFlagsSet(ThreadIdTaskMsgPro, KEY1_BIT0);					
					break;	

				/* K2键按下，直接发送事件标志给任务AppTaskTCPMain，设置bit1 */
				case KEY_DOWN_K2:
					printf("K2键按下，直接发送事件标志给任务ThreadIdTaskMsgPro，bit1被设置\r\n");
					osThreadFlagsSet(ThreadIdTaskMsgPro, KEY2_BIT1);
					break;
				
				/* K3键按下，直接发送事件标志给任务AppTaskTCPMain，设置bit2 */
				case KEY_DOWN_K3:
					printf("K3键按下，直接发送事件标志给任务ThreadIdTaskMsgPro，bit2被设置\r\n");
					osThreadFlagsSet(ThreadIdTaskMsgPro, KEY3_BIT2);
					break;

				/* 其他的键值不处理 */
				default:                     
					break;
			}
		}
		
		osDelay(20);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskLED
*	功能说明: LED闪烁。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal2
*********************************************************************************************************
*/
void AppTaskLED(void *argument)
{
	const uint16_t usFrequency = 200; /* 延迟周期 */
	uint32_t tick;

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		bsp_LedToggle(2);
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskFsUsb
*	功能说明: 文件系统和USB协议栈任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal3  
*********************************************************************************************************
*/
void AppTaskFsUsb(void *argument)
{
	while(1)
	{
		DemoFlashFS();
	}	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMsgPro
*	功能说明: TCPnet应用任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal4  
*********************************************************************************************************
*/
void AppTaskMsgPro(void *argument)
{
	while(1)
	{
		TCPnetTest();
	}	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskEthCheck
*	功能说明: 检查网线插拔状态。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal5 
*********************************************************************************************************
*/
void AppTaskEthCheck(void *argument)
{

	/* 初始化变量 */
	ThreadIdTaskMsgPro = NULL;
	g_ucEthLinkStatus = 0;
	
	/* 初始化网络 */
	netInitialize();
	
	while(1)
	{
		/* 网线插拔处理，方便移植，大家也可以根据需要发送任务事件标志做处理 */
		switch (g_ucEthLinkStatus)
		{
			/* 插拔临时状态，无需处理 */
			case 0:
			case 1:	
				break;	

			/* 网线插入，创应用任务 */
			case 2:
				if(ThreadIdTaskMsgPro == NULL)
				{	
					printf_taskdbg("网线插入，创建应用任务\r\n");
					ThreadIdTaskMsgPro = osThreadNew(AppTaskMsgPro, NULL, &ThreadMsgPro_Attr);
				}
				break;
			
			/* 网线拔掉，复位网络，删除应用任务 */
			case 3:
				printf_taskdbg("网线拔掉，复位网络，删除应用任务\r\n");

				/* 释放所有网络资源，含TCPnet内核任务和ETH接口任务 */	
				netUninitialize();
			
				printf_taskdbg("netUninitialize\r\n");

				/* 删除TCPnet应用任务 */
				osThreadTerminate(ThreadIdTaskMsgPro);

				ThreadIdTaskMsgPro = NULL;
				g_ucEthLinkStatus = 0;
			
				/* 重新初始化 */
				netInitialize();
			
				printf_taskdbg("netInitialize\r\n");

			/* 其他的键值不处理 */
			default:                     
				break;
		}
		
		osDelay(10);
	}	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务，这里用作BSP驱动包处理。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal6  
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 1; /* 延迟周期 */
	uint32_t tick;
	
	/* 初始化外设 */
	HAL_ResumeTick();
	bsp_Init();

	/* 创建任务 */
	AppTaskCreate();

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
		bsp_ProPer1ms();
		
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
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
	ThreadIdTaskEthCheck = osThreadNew(AppTaskEthCheck, NULL, &ThreadEthCheck_Attr);  	
	ThreadIdTaskLED = osThreadNew(AppTaskLED, NULL, &ThreadLED_Attr);  
	ThreadIdTaskUserIF = osThreadNew(AppTaskUserIF, NULL, &ThreadUserIF_Attr);
	ThreadIdTaskFsUsb = osThreadNew(AppTaskFsUsb, NULL, &ThreadFsUsb_Attr);  
	ThreadIdTaskGUI = osThreadNew(AppTaskGUI, NULL, &ThreadGUI_Attr);      
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
