/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: bsp.c
** 创 建 人: hangfa  
** 描    述: 硬件初始化
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2016.04.09
** 标准化重构日期：2024.05.25
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define bsp_GLOBALS

//头文件
#include "includes.h"
#include "..\..\User\bsp.h"

// 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 
#include "..\..\Drivers\inc\drv_timer.h"
#include "..\..\Drivers\inc\drv_ee_para.h"
#include "..\..\Drivers\ModbusRtuPoll\drv_485Usart.h"
#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"
#include "..\..\Drivers\inc\drv_led.h"
#include "..\..\Drivers\inc\drv_stm32f4rtc.h"
#include "..\..\Drivers\inc\drv_IO.h"
#include "..\..\Drivers\inc\drv_analog.h"
#include "..\..\Drivers\inc\drv_232Usart.h"
#include "..\..\Drivers\inc\drv_can.h"
#include "..\..\Drivers\spi_eeprom\spi_eeprom.h"
#include "..\..\Drivers\inc\drv_1Wire.h"
#include "..\..\Drivers\inc\drv_WatchDog.h"
#include "..\..\Drivers\inc\drv_Common.h"
#include "..\..\Drivers\inc\drv_ADC.h"
#include "..\..\User\MB_LinkApp.h"
#include "LinkCanopenMaster.h"
#include "ACC_Project.h"
#include "CommConf.h"
#include "ObjConf.h"
#include "Debug.h"	
#include "ACC_Init.h"	

/***************************************************************************************
** 函数名称: NVIC_Configuration
** 功能描述: stm32f10x配置基本向量中断
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
void NVIC_Config(void)
{
 #ifdef USE_IAP
  //---Set the Vector Table base location at 0x2000
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);
 #else
  //---Set the Vector Table base location at 0x8000000
 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x000);  // 重映射向量表
 #endif
 // 中断优先级分组:选择使用优先级分组第4组
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  // 抢断/响应优先级配置(16 * 0)---分了16个阶级
}

/********************************************************************************************************
* 函 数 名: ALL_GPIO_Ini
* 功能说明: 初始化所有GPIO为默认状态
* 形    参：无
* 返 回 值: 无
********************************************************************************************************/
void ALL_GPIO_Ini(void)
{
  GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOC);
  GPIO_DeInit(GPIOD);
  GPIO_DeInit(GPIOE);
  GPIO_DeInit(GPIOF);
  GPIO_DeInit(GPIOG);
  GPIO_DeInit(GPIOH);
  GPIO_DeInit(GPIOI);

}

/********************************************************************************************************
* 函 数 名: RCC_Update
* 功能说明: 系统时钟PLL更新及查询
* 形    参：无
* 返 回 值: 无
********************************************************************************************************/
void RCC_Update(void)
{
 //************************ PLL Parameters ************************************
 //HSE_VALUE = 25000000
 //PLL_VCO = (HSE_VALUE / my_PLL_M) * my_PLL_N 
 //SYSCLK = PLL_VCO / PLL_P ---160MHZ
 //USB OTG FS, SDIO and RNG Clock =  PLL_VCO / my_PLL_Q
 //PLL_M    25  //(0-63)
 //PLL_N      320  //(192-432)
 //PLL_P      2   //(2, 4, 6, or 8)
 //PLL_Q      8   //(4-15)
 
 SystemCoreClockUpdate();
 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
 
 RCC_GetClocksFreq(&RCC_ClocksStatus);
}

//============================================================================
// 名称： app_system_init
// 功能： 系统初始化
// 参数： 无
// 返回： 无
//==========================================================================
void system_init(void)
{
  Close_TI();

  RCC_Update();
  NVIC_Config();
  ALL_GPIO_Ini();
 
  Open_TI();
}

/**************************************************************************
* 函数名:Bsp_SerialInit
* 功  能:
* 参  数:
* 返回值:bool
**************************************************************************/
bool Bsp_SerialInit()
{
	char i=0;
	CommConf_Init();

	for(i=0;i<USARTMAXCNT;i++)
	{
		Rs485_Usart_InitHard(&RS485_USARTx[i].Ini_par);
	}
	return 1;
}

//获取stm32内部唯一的ID码(96位)
bool Get_STM32_ID(void)
{
	//获取CPU唯一ID
	HardInfo.uID[0] = *(__IO u32*)(0x1FFF7A10);// 低32位
	HardInfo.uID[1] = *(__IO u32*)(0x1FFF7A14);// 中32位
	HardInfo.uID[2] = *(__IO u32*)(0x1FFF7A18);// 高32位
	return 1;
}

//更新系统对象存储大小汇总数据
bool UpdateSysObjSize(void)
{
    HardInfo.SysObjSizeOf.Acc = sizeof(AccData);
    HardInfo.SysObjSizeOf.Auto = sizeof(g_HostData);
    HardInfo.SysObjSizeOf.PathStack = sizeof(PathStack);
    HardInfo.SysObjSizeOf.Hmi = sizeof(ExternalHmi_StDef);
    HardInfo.SysObjSizeOf.SVO = sizeof(SVO);
    HardInfo.SysObjSizeOf.CanOpen_Obj = sizeof(LinkCanOpen);
    return 1;
}

//ADDA功能组对象默认配置设置为_Disable
u8 ADDA_ObjReset(void)
{
	char i=0;
	ADDA_ADFuc_StDef* p2ADFuc = (ADDA_ADFuc_StDef*)&g_ADFucs;
	ADDA_DAFuc_StDef* p2DAFuc = (ADDA_DAFuc_StDef*)&g_DAFucs;
	
	g_ADFucs.Number = sizeof(ADDA_ADFucs_StDef) / sizeof(ADDA_ADFuc_StDef);//获取AD接口组中 功能对象数量
	for (i = 0; i < g_ADFucs.Number; i++)
		p2ADFuc[i].Id = ADCN_Disable;
	
	g_DAFucs.Number = sizeof(ADDA_DAFucs_StDef) / sizeof(ADDA_DAFuc_StDef);//获取DA接口组中 功能对象数量
	for (i = 0; i < g_DAFucs.Number; i++)
		p2DAFuc[i].Id = DACN_Disable;

	return 1;
}

//计算外设总数
bool ObjNumberCal(void)
{
	bool res = 1;

	//计算通信接口总数
	CCFs.Number = sizeof(CCFs_StDef) / sizeof(CCF_StDef);
	
	//计算主从站设备总数
	EQPMsts.Number = sizeof(CCFEQP_Masters_StDef) / sizeof(CCFEQP_Master_StDef);
	EQPSlvs.Number = sizeof(CCFEQP_Slaves_StDef) / sizeof(CCFEQP_Slave_StDef);

	//计算IO输出对象数量
	//g_IOOutputs.Number = sizeof(IO_Outputs_TypeDef) / sizeof(IO_Output_TypeDef);
	/*20251114 修正输出io数量计算错误的问题，因为IO_Output_TypeDef和 Number大小接近，导致Number也被计算在内了*/
	g_IOOutputs.Number = sizeof(IO_Outputs_TypeDef) / sizeof(IO_Output_TypeDef) - 1;

	//计算IO输入对象数量
	g_IOInputs.Number = sizeof(IO_Inputs_TypeDef) / sizeof(IO_Input_TypeDef);

	return res;
}

//检查对象固定长度是否越界
bool CheckAlign(void)
{
	char ErrCnt = 0;

	//串口数量 大于 最大接口数量宏定义
	if (CCFs.Number != CCF_MAXNUM)
		ErrCnt++;

	//hmi通信缓存结构体对齐越界
	if (sizeof(HMI_CommObj_TypeDef) != HMI_Cache_Num * 2)
		ErrCnt++;

	//调度系统通信缓存结构体越界 这里要优化，程序里不应该直接使用goc对象
	if (&(goc.Auto.p2CommObj->PathPoint[0].WayPointRetainThirteen) != &(goc.Auto.p2AGVInf[Para_Index_AGV_WayPointRetainThirteen1]))
		ErrCnt++;

	//EEPObj对象越界
	if (sizeof(ACC_FaultsHmiErr1_UnionDef) != 8)
		ErrCnt++;

	if (sizeof(ACC_ControlledStop_UnionDef) != 8)
		ErrCnt++;

	if (sizeof(ACC_Remind_UnionDef) != 8)
		ErrCnt++;

	if (ErrCnt)
	{
		g_CodeErr.Bit.LengthLimited = 1;
		g_CodeErr.Bit.Unaligned = 1;//长度限制错误
		return 0;
	}
	else
		return 1;
}

//清除或初始化随机对象
bool ObjReset(void)
{
	u16 i = 0;
	ACC_DbugRtx_TypeDef StTemp = { 0 };
	IO_Input_TypeDef* p2In = (IO_Input_TypeDef*)&g_IOInputs;
	IO_Output_TypeDef* p2Out = (IO_Output_TypeDef*)&g_IOOutputs;

	g_DbugRtx = StTemp;

	g_MC_WorkStep = MC_WorkStep_Init;
	memset(&CanMaster, 0, sizeof(CanMaster));
	memset(&g_WheelAngle, 0, sizeof(g_WheelAngle));

	//ADDA功能组对象配置设置为_Disable，避免在默认值0与硬件接口绑定的问题
	ADDA_ObjReset();

	//输入io全部初始化为禁能 避免默认0指向了某个有用的对象
	for (i = 0; i < g_IOInputs.Number; i++)
		p2In[i].ID = ICN_Disable;

	//输出io全部初始化为禁能
	for (i = 0; i < g_IOOutputs.Number; i++)
		p2Out[i].ID = OCN_Disable;

	return 1;
}

//应用驱动初始化初始化
//20240521重要经验：Config_ds18b20不能关，Set_CAN_Int改位置要在Analog_InitHard后面,且波特率参数不能为0
u8 bsp_Init(void)
{
	u8 errcnt=0;

	//运行Init 外部
	Run_Init_User();

	//系统全局对象指针汇总 放在最前面便于调试
	//注意不要在代码中使用goc对象，goc只是用来仿真时便于watch窗口操作
	GocInit();

	//对象长度计算初始化
	ObjNumberCal();	//计算外设总数，20241105前的项目，此调用过完晚，但通信外设没有用到的都有disable，所以无影响

	//清除或初始化随机对象
	ObjReset();

	//固定长度检查
	if(!CheckAlign())
		errcnt++;

	/*系统初始化*/
	system_init();
	/*Led初始化*/
	Led_InitHard();
	/*IO初始化*/
	IO_InitHard();
	/*RTC初始化*/
	STM32F4_RTC_Init();
	/*获取stm32内部唯一的ID码(96位)*/
	Get_STM32_ID();
	/*ADC初始化*/
	ADC_InitHard(ADC1, &ST_ADC.ADC_Cov_data[0], 2);
	/*ADC硬件初始化和SPI硬件初始化之间需要延时来避免“硬件状态依赖——隐式依赖延迟的硬件操作”*/
	{
		uint16_t time = 0xFFFF;
		while(time > 0){time--;}
	}
	/*EEPROM初始化*/
	EEPROM_InitHard();
	//Config_ds18b20(Mode_12bit);

	//设置系统定时器, 定时器更新周期T=toc,unit:us
	Timer_init(SysTimerSet, 50, //toc
				5, 0, 0, DISABLE, 0, 0);
	
	//先禁能CanOpen调度Timer
	canfestival_timer_start(9, DISABLE);				
	
	//--------------获取参数区域的全局指针地址-------------------
	g_pBoardpara = (u16*)&EEPPara;
	if(!TSK_EE_PARA(EE_CMD_INI))
	{//必须先初始化此任务
		TSK_EE_PARA(EE_CMD_PWR_UP_GET_DATA);	
	}

	if(!ParaInit())//系统参数初始化
		errcnt++;

	if(!ObjInit())//对象初始化正常
	{
		errcnt++;
		g_CodeErr.Bit.InitErr=1;
	}

	/*串口硬件初始化*/
	if(g_Para.Initiated)
		if(!Bsp_SerialInit())
			errcnt++;
		
	//Modbus本机参数初始化
	MBLink_MASTER_NUM=CCFs.Record.MstNum;

	//Modbus初始化 缓存区大小的初始化待优化：
	//要么遍历所有开发板从站最大的长度，要么在每个协议栈响应的时候传入当前从站的最大长度
	modbus_init(DSPDataMax,10);	

	Analog_InitHard();

	//CAN_BAUDRATE_500K 
	if(g_Para.RunningMode != ACC_RunningMode_SoftwareSimulat)//非纯软件仿真模式
		Set_CAN_Int(user_CAN_ID,CanMaster.Para.Baud, 0x00, 10);//此地址:0x00  必须修改主频为160M!!!!
	else {}	

    //更新系统对象存储大小汇总数据
	UpdateSysObjSize();

	if(errcnt)
	{
		g_CodeErr.Bit.InitErr=1;
		return INIT_ERROR;
	}	
	else
	{
		return INIT_OK;
	}
}

//***********************************END OF FILE****************************************
