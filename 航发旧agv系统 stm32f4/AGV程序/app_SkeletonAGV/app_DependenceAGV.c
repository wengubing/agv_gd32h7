/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_DependenceAGV.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的依赖代码（底盘程序的调用接口、数据管道、依赖CBB等）
  *******************************************************************************/

#define  APP_DEPENDENCEAGV_GLOBALS


#include "app_DependenceAGV.h"
#include "app_AGVInit.h"

/*自动框架内的软件版本号*/
static uint16_t AGV_SOFTVERSION = 100;

/*全局变量声明，底盘控制的函数中无需声明对象，赋值初始化即可*/
DependenceAGV_InputTypeDef 	g_AGVDependenInputData;

/*初始化成功的标志位，防止野指针乱来导致程序崩溃*/
#if defined(ACC_WXB) 
uint8_t 					g_AGV_DependenceInitCheckSucc = 0x00;
#else
static uint8_t 				g_AGV_DependenceInitCheckSucc = 0x00;
#endif

/*进一步提取指针内容，简化代码查阅的复杂程度*/
HostCtrl_DATA 				*g_AGVHostCtrl			= NULL;	/*AGV框架代码对象指针	*/

						/*要求十字磁条对应 磁导航传感器数据正负*/
						/*            	前方				*/
						/*									*/
						/*          + |------| -			*/
						/*    +   |               |   -		*/
						/*   HMI  |               |			*/
						/*        |               |			*/
						/*    -   |               |   +		*/
						/*          - |------| + 			*/
						/*                  				*/
						/*            	后方				*/


/*SLAM的定位数据，需要外部输入时做好逻辑转换*/
DepAGV_SlamData_InputTypeDef		g_SlamData;

/*虚拟里程计的定位数据，需要外部输入时做好逻辑转换*/
DepAGV_VirtualOdoData_InputTypeDef	g_VirtualOdo;

/*Rfid的标签值数据，需要外部输入时做好逻辑转换 包括数据滤波*/
DepAGV_RFIDData_InputTypeDef	g_RfidData;

/*磁导航的数据，需要外部输入时做好逻辑转换*/
DepAGV_MagData_InputTypeDef		g_F_MagData;
DepAGV_MagData_InputTypeDef		g_B_MagData;
DepAGV_MagData_InputTypeDef		g_L_MagData;
DepAGV_MagData_InputTypeDef		g_R_MagData;

/*二维码的数据，需要外部输入时做好逻辑转换,区分定位二维码和导航二维码*/
DepAGV_PGVData_InputTypeDef		g_Navigation_QRData;
DepAGV_PGVData_InputTypeDef		g_Location_QRData;
/***********************************************************
** 函数名称:	AGV_SoftVersionGet
** 功能描述: 	将当前编译时间连同软件版本号，拼接成固件版本号
** 参    数:	None
** 返 回 值:	例如：250125100的返回数值表示，25年1月25日编译的第100版本固件
***********************************************************/
//
uint32_t AGV_SoftVersionGet(void)
{
	const char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
	const char delimiter[] = " ";
	volatile const char Data[] = __DATE__;
	char *token;
    int count = 0;
	char str[16];
	char str1[5];
	int YearNum,MonthNum,DayNum;
	int a,b,i;
	strncpy(str,(void*)Data,sizeof(Data));
    // 使用strtok函数分割字符串
    token = strtok(str, delimiter);
	a = 0;
	b = sizeof(str)-1;
    while (token != NULL || a <= b || count < 3)
	{
        if (count == 0)
		{
			strcpy(str1,token);
			for(i=0;i<12;i++)
			{
				if(strcmp(str1,months[i]) == 0)
				{
					MonthNum = i+1;
				}
			}
		}
		else if (count == 1)
		{
           DayNum = atoi(token);
        }
		else if (count == 2)
		{
           YearNum = atoi(token);
        }
		a = a+sizeof(token);
        token = strtok(NULL, delimiter);
        count++;
    }
	return AGV_SOFTVERSION+DayNum*1000+MonthNum*100000+(YearNum-2000)*10000000;
}


/***************************************************************************************
** 函数名称:	AGV_ParaIndexInitCheck
** 功能描述:	自动运行需要的关键配置参数初始化数据检查；如果没有进行初始化，给个默认值确保正常运行
** 参    数:无
** 返 回 值:	无
****************************************************************************************/
void AGV_ParaIndexInitCheck(void)
{
	/*填充软件版本号*/
	g_AGVHostCtrl->SoftVerSion = AGV_SOFTVERSION;
	//g_AGVHostCtrl->SoftVerSion = AGV_SoftVersionGet();
	/*路径点匹配时X位置偏差，单位1mm,默认200*/
	if(g_AGVHostCtrl->Para_Index.AGV_WayPointOffset_X == 0)			{g_AGVHostCtrl->Para_Index.AGV_WayPointOffset_X = 200;}
	/*路径点匹配时Y位置偏差，单位1mm,默认200*/
	if(g_AGVHostCtrl->Para_Index.AGV_WayPointOffset_Y == 0)			{g_AGVHostCtrl->Para_Index.AGV_WayPointOffset_Y = 200;}

	/*AGV自转到位的死区上限，单位1°默认0.3f*/
	if(g_AGVHostCtrl->Para_Index.AGV_AngAdjZoneMax == 0)			{g_AGVHostCtrl->Para_Index.AGV_AngAdjZoneMax = 0.3f;}
	/*AGV自转到位的死区下限，单位1°默认0.1f*/
	if(g_AGVHostCtrl->Para_Index.AGV_AngAdjZoneMin == 0)			{g_AGVHostCtrl->Para_Index.AGV_AngAdjZoneMin = 0.1f;}
	/*AGV行走时的允许z姿态偏差，需要比自转到位的死区上限大，单位1°默认5.0f*/
	if(g_AGVHostCtrl->Para_Index.AGV_WalkAngAdjZone == 0)			{g_AGVHostCtrl->Para_Index.AGV_WalkAngAdjZone = 5.0f;}

	/*AGV升降死区高度上限，单位1mm，默认3*/
	if(g_AGVHostCtrl->Para_Index.AGV_DeadZoneHighMax == 0)			{g_AGVHostCtrl->Para_Index.AGV_DeadZoneHighMax = 3;}
	/*AGV升降死区高度下限，单位1mm，默认1*/
	if(g_AGVHostCtrl->Para_Index.AGV_DeadZoneHighMin == 0)			{g_AGVHostCtrl->Para_Index.AGV_DeadZoneHighMin =1;}
	/*升降行程，单位1mm；当调节高度大于满行程的10%都是全速运转*/
	if(g_AGVHostCtrl->Para_Index.AGV_LimitHigh == 0)				{g_AGVHostCtrl->Para_Index.AGV_LimitHigh = 120;}

	/*悬挂升降死区高度上限（大于这个值，进入高度控制），当实际差值小于此高度时，认为升降到位，单位1mm，默认10*/
	if(g_AGVHostCtrl->Para_Index.AGV_DeadZoneHangHighMax == 0)		{g_AGVHostCtrl->Para_Index.AGV_DeadZoneHangHighMax = 10;}
	/*悬挂升降死区高度下限（高度控制结束后，小于这个值），当实际差值小于此高度时，认为升降到位，单位1mm，默认1*/
	if(g_AGVHostCtrl->Para_Index.AGV_DeadZonHangeHighMin == 0)		{g_AGVHostCtrl->Para_Index.AGV_DeadZonHangeHighMin =1;}
	/*升降行程，单位1mm；当调节高度大于满行程的10%都是全速运转*/
	if(g_AGVHostCtrl->Para_Index.AGV_LimitHangHigh == 0)			{g_AGVHostCtrl->Para_Index.AGV_LimitHigh = 120;}

	/*数值越大起步加速过程越快，默认1.0f*/
	if(g_AGVHostCtrl->Para_Index.AGV_MasterHANDLEAcc == 0)			{g_AGVHostCtrl->Para_Index.AGV_MasterHANDLEAcc = 1.0f;}

	/*磁条检测的强度限制，如果低于这个值，认为没有检测到磁条，防干扰，默认200*/
	if(g_AGVHostCtrl->Para_Index.AGV_MagStrengthLimit == 0)			{g_AGVHostCtrl->Para_Index.AGV_MagStrengthLimit = 200;}
	
	/*自动行走的默认速度，默认用6挡，不允许动态修改*/
	if(g_AGVHostCtrl->Para_Index.AGVWalkDefaultSpeedVal == 0)		{g_AGVHostCtrl->Para_Index.AGVWalkDefaultSpeedVal = 6;}
	/*自动原地自转的默认速度，默认用6挡，不允许动态修改*/
	if(g_AGVHostCtrl->Para_Index.AGVPostureDefaultSpeedVal == 0)	{g_AGVHostCtrl->Para_Index.AGVPostureDefaultSpeedVal = 6;}

	/*二维码导航下，所有路径中 超短路径区间，如果小于这个值，点位更新不是在半路程而是1/4路程 默认为0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_QRMode_PathDeltaMin == 0)	{g_AGVHostCtrl->Para_Index.AGV_QRMode_PathDeltaMin = 0;}
	
	/*磁导航下，    所有路径中 超短路径区间，如果小于这个值，点位更新不是在半路程而是1/4路程 默认为0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_MagMode_PathDeltaMin == 0)	{g_AGVHostCtrl->Para_Index.AGV_MagMode_PathDeltaMin = 0;}

	/*当调度系统下发目标点速度为0时，AGV执行的速度，一般设置为调度系统的AGV起步速度*/
	if(g_AGVHostCtrl->Para_Index.AGV_CurrentPer_Defult == 0)		{g_AGVHostCtrl->Para_Index.AGV_CurrentPer_Defult = 30;}

	/*二维码上旋转的最低速度比例，默认0.03*/
	if(g_AGVHostCtrl->Para_Index.AGV_Posture_QR_KMin == 0)			{g_AGVHostCtrl->Para_Index.AGV_Posture_QR_KMin = 0.03f;}
	/*二维码上旋转的整体速度比例，默认0.1*/
	if(g_AGVHostCtrl->Para_Index.AGV_Posture_QR_K == 0)				{g_AGVHostCtrl->Para_Index.AGV_Posture_QR_K = 0.1f;}
	
	/*Slam里程计上旋转的最低速度比例，默认0.03*/
	if(g_AGVHostCtrl->Para_Index.AGV_Posture_ODO_KMin == 0)			{g_AGVHostCtrl->Para_Index.AGV_Posture_ODO_KMin = 0.03f;}
	/*Slam里程计上旋转的整体速度比例，默认1.0*/
	if(g_AGVHostCtrl->Para_Index.AGV_Posture_ODO_K == 0)			{g_AGVHostCtrl->Para_Index.AGV_Posture_ODO_K = 1.00f;}

	/*磁条上旋转的速度比例，默认1*/
	if(g_AGVHostCtrl->Para_Index.AGV_Posture_OdOMagnetic_k == 0)	{g_AGVHostCtrl->Para_Index.AGV_Posture_OdOMagnetic_k = 1.0f;}
	/*磁条上旋转的最低速度比例，默认0.1*/
	if(g_AGVHostCtrl->Para_Index.AGV_Posture_OdOMagnetic_KMin == 0)	{g_AGVHostCtrl->Para_Index.AGV_Posture_OdOMagnetic_KMin = 0.1f;}
	/*磁条上旋转提前减速角度，默认0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_Posture_OdOMagnetic_Angle == 0){g_AGVHostCtrl->Para_Index.AGV_Posture_OdOMagnetic_Angle = 0.0f;}
	/*磁条上旋转可检测到的变化量程，单位mm 默认70mm*/
	if(g_AGVHostCtrl->Para_Index.AGV_Posture_OdOMagnetic_MaxOff == 0){g_AGVHostCtrl->Para_Index.AGV_Posture_OdOMagnetic_MaxOff = 70;}
	/*AGV运行时，旋转的方向，默认是1；如果AGV自转方向不对，需要把其改成-1*/
	if(g_AGVHostCtrl->Para_Index.AGV_Postur_Orientation_diff == 0)	{g_AGVHostCtrl->Para_Index.AGV_Postur_Orientation_diff = -1;}

	/*里程计减速时需要提前减速的距离，单位0.1mm，默认0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_Correction_ODO_Dis == 0)		{g_AGVHostCtrl->Para_Index.AGV_Correction_ODO_Dis = 0;}
	/*里程计减速的最低速度比例，默认0.03*/
	if(g_AGVHostCtrl->Para_Index.AGV_Correction_ODO_KMin == 0)		{g_AGVHostCtrl->Para_Index.AGV_Correction_ODO_KMin = 0.03f;}

	/*识别RFID停车时需要提前减速的距离，单位0.1mm，默认0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_Correction_RFID_Dis == 0)	{g_AGVHostCtrl->Para_Index.AGV_Correction_RFID_Dis = 0.0f;}
	/*识别RFID停车时的整体减速比例，默认0.03*/
	if(g_AGVHostCtrl->Para_Index.AGV_Correction_RFID_k == 0)		{g_AGVHostCtrl->Para_Index.AGV_Correction_RFID_k = 0.03f;}
	
	/*识别二维码停车时需要提前减速的距离，单位0.1mm，默认0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_Correction_QR_Dis == 0)		{g_AGVHostCtrl->Para_Index.AGV_Correction_QR_Dis = 0.0f;}
	/*识别二维码停车时的整体减速比例，默认0.05*/
	if(g_AGVHostCtrl->Para_Index.AGV_Correction_QR_k == 0)			{g_AGVHostCtrl->Para_Index.AGV_Correction_QR_k = 0.05f;}

	/*在十字磁条上，识别二维码停车时需要提前减速的距离，单位0.1mm，默认0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_Correction_MagQR_Dis == 0)	{g_AGVHostCtrl->Para_Index.AGV_Correction_MagQR_Dis = 0.0f;}
	/*在十字磁条上，识别二维码停车时的整体减速比例，默认0.05*/
	if(g_AGVHostCtrl->Para_Index.AGV_Correction_MagQR_k == 0)		{g_AGVHostCtrl->Para_Index.AGV_Correction_MagQR_k = 0.05f;}

	/*识别十字磁条停车时需要提前减速的距离，单位0.1mm，默认0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_Correction_Mag_Dis == 0)		{g_AGVHostCtrl->Para_Index.AGV_Correction_Mag_Dis = 0.0f;}
	/*识别十字磁条停车时的整体减速比例，默认0.05*/
	if(g_AGVHostCtrl->Para_Index.AGV_Correction_Mag_k == 0)			{g_AGVHostCtrl->Para_Index.AGV_Correction_Mag_k = 0.05f;}

	/*在RFID停车的情况下，RFID停车的信号阈值，如果RFID检测到的信号值超过它，将直接停车，默认4*/
	if(g_AGVHostCtrl->Para_Index.AGV_ParkRFIDSignal == 0)			{g_AGVHostCtrl->Para_Index.AGV_ParkRFIDSignal = 4;}
	/*在RFID停车的情况下，RFDI检测到之后，最长停车距离，单位1mm，默认10*/
	if(g_AGVHostCtrl->Para_Index.AGV_ParkRFIDDisLimit == 0)			{g_AGVHostCtrl->Para_Index.AGV_ParkRFIDDisLimit = 10;}
	/*在RFID停车的情况下，RFDI检测到之后，最长停车时间，单位1ms，默认1000*/
	if(g_AGVHostCtrl->Para_Index.AGV_ParkRFIDTimeOutLimit == 0)		{g_AGVHostCtrl->Para_Index.AGV_ParkRFIDTimeOutLimit = 1000;}
	
	/*高度控制的提前降速值，如果低于这个值，将以最低速度挡位进行升降，默认0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_Altitude_Dis == 0)		{g_AGVHostCtrl->Para_Index.AGV_Altitude_Dis = 0.0f;}
	/*高度控制的减速距离，目标高度距离这个值，将开始从6挡位减速至1挡，默认50*/
	if(g_AGVHostCtrl->Para_Index.AGV_Altitude_SlownDis == 0)		{g_AGVHostCtrl->Para_Index.AGV_Altitude_SlownDis = 50;}
	
	/*高度控制的提前降速值，如果低于这个值，将以最低速度挡位进行升降，默认0*/
	//if(g_AGVHostCtrl->Para_Index.AGV_HangAltitude_Dis == 0)		{g_AGVHostCtrl->Para_Index.AGV_HangAltitude_Dis = 0.0f;}
	/*高度控制的减速距离，目标高度距离这个值，将开始从6挡位减速至1挡，默认20*/
	if(g_AGVHostCtrl->Para_Index.AGV_HangAltitude_SlownDis == 0)	{g_AGVHostCtrl->Para_Index.AGV_HangAltitude_SlownDis = 20;}
	
	/*AGV停车检测超距——需要停车的时候超过目标距离的k倍数，仍未停车就报警 默认1.2*/
	if(g_AGVHostCtrl->Para_Index.AGV_WalkDisOverrun_K == 0)			{g_AGVHostCtrl->Para_Index.AGV_WalkDisOverrun_K = 1.2f;}

	if(g_AGVHostCtrl->Para_Index.AGV_GoodsMove_En != 0X00)
	{
		/*AGV停车检测超距——需要停车的时候超过目标距离的k倍数，仍未停车就报警 默认1.2*/
		if(g_AGVHostCtrl->Para_Index.AGV_GoodsMove_xLimit == 0)		{g_AGVHostCtrl->Para_Index.AGV_GoodsMove_xLimit = 30;}
		/*AGV自转到位的死区上限，单位1°默认0.3f*/
		if(g_AGVHostCtrl->Para_Index.AGV_GoodsMove_yLimit == 0)		{g_AGVHostCtrl->Para_Index.AGV_GoodsMove_yLimit = 40;}
		/*AGV自转到位的死区下限，单位1°默认0.1f*/
		if(g_AGVHostCtrl->Para_Index.AGV_GoodsMove_zLimit == 0)		{g_AGVHostCtrl->Para_Index.AGV_GoodsMove_zLimit = 1.0f;}
	}

	if(g_AGVHostCtrl->AGVPendantFun.PutGoodsQRMag == 0X01)
	{
		/*当存在放工装纠正xz功能时，虚拟磁导航在末端路径上，需要调整映射的参数，默认1*/
		if(g_AGVHostCtrl->Para_Index.AGV_VirtualMagneticMapCoef == 0.0f)			{g_AGVHostCtrl->Para_Index.AGV_VirtualMagneticMapCoef = 1.0f;}

		/*取工装时，偏移过大——PGV读出的x偏移加上z轴偏移映射成前后磁条后的限制值 单位1mm 默认30mm*/
		if(g_AGVHostCtrl->Para_Index.AGV_MagParkZero_Limit == 0)	{g_AGVHostCtrl->Para_Index.AGV_MagParkZero_Limit = 40;}
	}
}
/***************************************************************************************
** 函数名称:	AGV_CBBDependence
** 功能描述:	自动运行需要的CBB依赖数据管道检查；例如：虚拟里程计、里程计、虚拟磁条、RFID
** 参    数:	无
** 返 回 值:	0-检查通过		其他-检查不通过，自动功能无法运行
****************************************************************************************/
uint8_t AGV_DependenceInitCheck(void)
{
	uint8_t res = 0;

#if defined(ACC_WXB) 
	/*文工底盘程序，在全局初始化；其他模板程序在此初始化*/
#else
	/*自动功能的相关初始化*/
	{
		app_SkeletonAGV_FunInit();
	}
#endif
	do
	{
		/*1.自动运行框架代码的控制结构体都没有，直接错误返回*/
		if(g_AGVDependenInputData.AGVHostCtrl == NULL)	{res = 1;break;}
		g_AGVHostCtrl = g_AGVDependenInputData.AGVHostCtrl;
		{
			g_AGVHostCtrl->AGVParkNowMode = g_AGVHostCtrl->AGVWalkParkMode;
		}


		/*6.*/
		/*初始化虚拟里程计数据输入的指针*/
		g_AGVDependenInputData.VirtualOdoData = &g_VirtualOdo;
		/*初始化SLAM数据输入的指针*/
		g_AGVDependenInputData.SlamData = &g_SlamData;
		/*初始化RFID数据输入的指针*/
		g_AGVDependenInputData.RfidData = &g_RfidData;
		/*初始化磁导航数据输入的指针*/
		g_AGVDependenInputData.F_MagData = &g_F_MagData;
		g_AGVDependenInputData.B_MagData = &g_B_MagData;
		g_AGVDependenInputData.L_MagData = &g_L_MagData;
		g_AGVDependenInputData.R_MagData = &g_R_MagData;
		/*初始化PGV数据输入的指针*/
		g_AGVDependenInputData.Navigation_QRData = &g_Navigation_QRData;
		g_AGVDependenInputData.Location_QRData = &g_Location_QRData;
		
		/*执行到这里，初始化的检查通过，AGV框架程序才能正常的运行*/
		g_AGV_DependenceInitCheckSucc = 0xA5;

	}while(0);


#if defined(ACC_WXB) 
	/*文工底盘程序，在全局初始化；其他模板程序在此初始化*/
#else
	/*设置参数用户设置的回调接口，指定了函数调用位置，确保流程中的调用是正确的*/
	{
		app_SkeletonAGV_ParaIndexInit();
	}
#endif
	/*一部分关键的设置参数，检查核对；若没有赋值；给个默认值*/
	{
		AGV_ParaIndexInitCheck();
	}
	/*测试代码*/
	{
		g_AGVHostCtrl->PosValid  = true;
		/*默认使用第速度6挡位*/
		g_HostData.AGVWalkSpeedVal = g_HostData.Para_Index.AGVWalkDefaultSpeedVal;
	}
	return res;
}

/***************************************************************************************
** 函数名称:	AGV_AutoModelAlterInCallBack
** 功能描述:	从自动模式切换至遥控模式外部可以操控的控制，例如系统重启、标志位刷新等
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
__weak void AGV_AutoModelAlterInCallBack(void)
{
	return;
}
/***************************************************************************************
** 函数名称:	AGV_AutoModelAlterOutCallBack
** 功能描述:	从遥控模式切换至自动模式外部可以操控的控制，例如系统重启、标志位刷新等
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
__weak void AGV_AutoModelAlterOutCallBack(void)
{
	return;
}

/***************************************************************************************
** 函数名称:	AGV_ControlModelAlter
** 功能描述:	切入自动、切出自动 需要进行维护的相关数据内容，需要再后台程序中持续调用
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_ControlModelAlter(void)
{
	static Control_MODEL	LastControlModel = Style_Control_MODEL_NULL;
	/*从调度切成遥控需要清除的内容*/
	do
	{
		if(LastControlModel == g_HostData.ControlMODEL){break;}
		/*切入自动模式*/
		if(	  LastControlModel != Style_Auto
			&&g_HostData.ControlMODEL == Style_Auto)
		{
			/*如果切出自动模式的时候是清空了工装算磁条偏移量的中间判断变量，切回自动后强制重新计算一次*/
			if(g_HostData.GoodsQRData.flag == 0XFE){g_HostData.MagParkQRData.flag = 0XAE;}
			/*如果切出自动模式的时候是清空了工装偏移的中间判断变量，切回自动后强制重新计算一次*/
			if(g_HostData.GoodsQRData.flag == 0XFE){g_HostData.GoodsQRData.flag = 0XAE;}
			/*外部操作回调*/
			AGV_AutoModelAlterInCallBack();
		}
		/*切出自动模式*/
		else if(	LastControlModel == Style_Auto
				 &&g_HostData.ControlMODEL != Style_Auto)
		{
			/*清除当前路径解析状态，再切回调度时，调度系统可以重写下发任务*/
			PathTypePara.g_DownloadStatus = DownLoad_NULL;

			/*清除当前的路径标签值，移动后再切回自动不会导致走错路的故障误报*/
			g_HostData.CurrentPath_Tag = 0;

			g_HostData.LastPointFlag = 0;

			g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;

			if(g_HostData.AGVPendantFun.ModelAlterStopTask == 1)
			{
				g_HostData.AGVAutoMode = AutoMode_Stop;
			}

			/*如果存在任何自动运行的故障，都需要回到点位上，再切回调度；
			  重新通过状态机判断应该执行什么动作*/
			if(g_AGVHostCtrl->AGV_Alarm_Evt1 != 0X00)
			{
				g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
			}
			/*如果有走错、走过的报警  清空当前所有路径列表*/
			{
				if(	  M_TST_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_NavigationFail)		!= 0X00
					||M_TST_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_PathPointNotMatch)	!= 0X00)
				{
					//g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_Err;
				}
			}
			/*如果有巡线偏差过大的报警  清空工装算磁条偏移量的中间判断变量*/
			{
				if(M_TST_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_LateralDeviationOver) != 0X00)
				{
					memset(&g_HostData.MagParkQRData,0,sizeof(g_HostData.MagParkQRData));
					memset(&g_HostData.AGVMagParkZeroOffset,0,sizeof(g_HostData.AGVMagParkZeroOffset));
				}
			}
			/*如果有工装偏移的报警  清空工装偏移的中间判断变量*/
			{
				if(M_TST_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_GoodsMoveErr) != 0X00)
				{
					memset(&g_HostData.GoodsQRData,0,sizeof(g_HostData.GoodsQRData));
					g_HostData.GoodsQRData.flag = 0XFE;
				}
			}

			/*自动运行特有的故障，在退出自动时 清除*/
			M_K_CLR_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_NavigationFail);			/*走错、走过的报警*/
			M_K_CLR_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_GoodsMoveErr);			/*工装偏移的报警*/
			M_K_CLR_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_LateralDeviationOver);	/*巡线偏差过大的报警*/
			M_K_CLR_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_MagPostureErr);			/*磁条旋转过程中丢失目标磁条的报警*/
			M_K_CLR_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_PathPointNotMatch);		/*Tag标签不匹配*/
			M_K_CLR_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_DownoadPathError);		/*路径下载失败*/
			M_K_CLR_BIT(g_AGVHostCtrl->AGV_Alarm_Evt1, AGV_ALM_NotFindTag);				/*未在点位上线*/
			/*外部操作回调*/
			AGV_AutoModelAlterOutCallBack();
		}
	}while(0);
	/*更新模式*/
	LastControlModel = g_HostData.ControlMODEL;

}
/***************************************************************************************
** 函数名称:	AGV_MotionControl_Dependence
** 功能描述:	自动运行需要在运动控制的函数中循环调用的依赖；
** 参    数: 	无
** 返 回 值:	无
****************************************************************************************/
void AGV_MotionControl_Dependence(void)
{
	/*初始化检查没通过，直接返回*/
	if(g_AGV_DependenceInitCheckSucc == 0x00){return;}

	/*当切入自动和切出自动时需要进行更新的内容*/
	AGV_ControlModelAlter();

}
/***************************************************************************************
** 函数名称:	AGV_QRNavigate_InPutSensorData
** 功能描述:	二维码导航下需要输入到自动框架的传感器数据
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
void AGV_QRNavigate_InPutSensorData(void)
{
	/*虚拟里程计数据接入自动运行数据管道*/
	{
		/*实时xzy坐标输入*/
		g_AGVHostCtrl->NavigationData.NavLidar_x 	= g_VirtualOdo.RTx + g_AGVHostCtrl->CurrentPath_X;
		g_AGVHostCtrl->NavigationData.NavLidar_y 	= g_VirtualOdo.RTy + g_AGVHostCtrl->CurrentPath_Y;
		g_AGVHostCtrl->NavigationData.NavLidar_z 	= g_VirtualOdo.RTz;
		{
			float Temp_Towards = g_AGVHostCtrl->NavigationData.NavLidar_z;
			if(  (Temp_Towards < 45.0f&&Temp_Towards >= 0.0f)
				||(Temp_Towards <= 360.0f&&Temp_Towards >= 315.0f))
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 0;
			}
			else if(Temp_Towards < 135.0f&&Temp_Towards >= 45.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 9000;
			}
			else if(Temp_Towards < 225.0f&&Temp_Towards >= 135.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 18000;
			}
			else if(Temp_Towards < 315.0f&&Temp_Towards >= 225.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 27000;
			}
		}
		/*行走距离输入*/
		if(g_VirtualOdo.Distance < 0)
		{
			g_AGVHostCtrl->PointToStart_Dis = -g_VirtualOdo.Distance*1000;/*AGV需要的距离是mm单位；虚拟里程计输出的是m，需要扩大1000倍*/
		}
		else
		{
			g_AGVHostCtrl->PointToStart_Dis = g_VirtualOdo.Distance*1000;/*AGV需要的距离是mm单位；虚拟里程计输出的是m，需要扩大1000倍*/
		}
	}
	/*地标二维码传感器数据接入自动运行数据管道*/
	{
		g_AGVHostCtrl->ParkQRData.QR_x 		= g_Navigation_QRData.QR_x;
		g_AGVHostCtrl->ParkQRData.QR_y 		= g_Navigation_QRData.QR_y;
		g_AGVHostCtrl->ParkQRData.QR_z 		= g_Navigation_QRData.QR_z;
		//g_AGVHostCtrl->ParkQRData.Offset_z 	= g_Navigation_QRData.Offset_z;
		g_AGVHostCtrl->ParkQRData.QR_Tag 	= g_Navigation_QRData.QR_Tag;
		g_AGVHostCtrl->ParkQRData.PintSta 	= g_Navigation_QRData.PintSta;
	}
	/*工装二维码传感器数据接入自动运行数据管道*/
	{
		g_AGVHostCtrl->FixtureQRData.QR_x 		= g_Location_QRData.QR_x;
		g_AGVHostCtrl->FixtureQRData.QR_y 		= g_Location_QRData.QR_y;
		g_AGVHostCtrl->FixtureQRData.QR_z 		= g_Location_QRData.QR_z;
		g_AGVHostCtrl->FixtureQRData.Offset_z 	= g_Location_QRData.Offset_z;
		g_AGVHostCtrl->FixtureQRData.QR_Tag 	= g_Location_QRData.QR_Tag;
		g_AGVHostCtrl->FixtureQRData.PintSta 	= g_Location_QRData.PintSta;
	}
	/*以地标二维码的状态作为 是否在点位上的标志*/
	{
		if(   g_AGVHostCtrl->CurrentPointTag != g_AGVHostCtrl->ParkQRData.QR_Tag
			&&g_AGVHostCtrl->ParkQRData.PintSta != 0X00)
		{
			g_AGVHostCtrl->PointUpdateFlag = 1;
			g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->ParkQRData.QR_Tag;
			g_AGVHostCtrl->NavigationData.NavLidar_Tag = g_AGVHostCtrl->ParkQRData.QR_Tag;
		}
		else
		{
			g_AGVHostCtrl->PointUpdateFlag = 0;
		}
	}
}
/***************************************************************************************
** 函数名称:	AGV_MagNavigate_InPutSensorData
** 功能描述:	磁导航下需要输入到自动框架的传感器数据
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
uint16_t 	XZW_MagSta_Filter_Time = 200;
uint8_t 	XZW_MagSta_Filter_Lose = 1;
void AGV_MagNavigate_InPutSensorData(void)
{
	/*磁导航数据接入自动运行数据管道*/
	{
		int16_t MagStrengthLimit = -g_AGVHostCtrl->Para_Index.AGV_MagStrengthLimit;
		/*在正向巡线模式下，左右磁条数据需要处理，前后不用*/
		if(g_HostData.ActualAGVWorkMode == AGVMode_GoLine)
		{
			/*前磁导航数据处理，简单处理*/
			{
				
				if(g_F_MagData.Strength <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x01;
					g_AGVHostCtrl->MagneticData.Distance_F 	= g_F_MagData.Distance;
					g_AGVHostCtrl->MagneticData.Strength_F 	= g_F_MagData.Strength;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x00;
					g_AGVHostCtrl->MagneticData.Distance_F 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_F 	= 0;
				}
			}
			/*后磁导航数据处理，简单处理*/
			{
				if(g_B_MagData.Strength <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x01;
					g_AGVHostCtrl->MagneticData.Distance_B 	= g_B_MagData.Distance;
					g_AGVHostCtrl->MagneticData.Strength_B 	= g_B_MagData.Strength;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x00;
					g_AGVHostCtrl->MagneticData.Distance_B 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_B 	= 0;
				}
			}
#if defined(ACC_WXB) 
			/*左磁导航数据处理，简单处理*/
			{
				if(g_L_MagData.Strength <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Sta_L = 0x01;
					g_AGVHostCtrl->MagneticData.Distance_L 	= g_L_MagData.Distance;
					g_AGVHostCtrl->MagneticData.Strength_L 	= g_L_MagData.Strength;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_L = 0x00;
					g_AGVHostCtrl->MagneticData.Distance_L 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_L 	= 0;
				}
			}
			/*右磁导航数据处理，简单处理*/
			{
				if(g_R_MagData.Strength <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Sta_R = 0x01;
					g_AGVHostCtrl->MagneticData.Distance_R 	= g_R_MagData.Distance;
					g_AGVHostCtrl->MagneticData.Strength_R 	= g_R_MagData.Strength;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_R = 0x00;
					g_AGVHostCtrl->MagneticData.Distance_R 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_R 	= 0;
				}
			}
#else
			/*左磁导航数据处理，根据运行状态和磁导航检测状态处理*/
			{
				uint8_t L_or_R_Sta_L = 0;
				int16_t Strength_L = 0;
				int16_t Distance_L = 0;
				L_or_R_Sta_L	= g_L_MagData.MN_DetectState;
				if(L_or_R_Sta_L == 0X07)
				{
					g_AGVHostCtrl->MagneticData.Sta_L = 0x00;
					Distance_L 	= g_L_MagData.MN_DistanceMiddle;
					Strength_L 	= g_L_MagData.MN_InductionStrengthMiddle;
				}
				else if(L_or_R_Sta_L == 0X03)
				{
					g_AGVHostCtrl->MagneticData.Sta_L = 0x00;
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Forward)
					{
						Distance_L 	= g_L_MagData.MN_DistanceMiddle;
						Strength_L 	= g_L_MagData.MN_InductionStrengthMiddle;
					}
					else
					{
						Distance_L 	= g_L_MagData.MN_DistanceRight;
						Strength_L 	= g_L_MagData.MN_InductionStrengthRight;
					}
				}
				else if(L_or_R_Sta_L == 0X06)
				{
					g_AGVHostCtrl->MagneticData.Sta_L = 0x00;
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Forward)
					{
						Distance_L 	= g_L_MagData.MN_DistanceLeft;
						Strength_L 	= g_L_MagData.MN_InductionStrengthLeft;
					}
					else
					{
						Distance_L 	= g_L_MagData.MN_DistanceMiddle;
						Strength_L 	= g_L_MagData.MN_InductionStrengthMiddle;
					}
				}
				else if(L_or_R_Sta_L == 0X02)
				{
					Distance_L 	= g_L_MagData.MN_DistanceMiddle;
					Strength_L 	= g_L_MagData.MN_InductionStrengthMiddle;
					if(Strength_L <= MagStrengthLimit)
					{
						g_AGVHostCtrl->MagneticData.Sta_L = 0x01;
						g_AGVHostCtrl->MagneticData.Distance_L 	= Distance_L;
						g_AGVHostCtrl->MagneticData.Strength_L 	= Strength_L;
					}
					else
					{
						g_AGVHostCtrl->MagneticData.Sta_L = 0x00;
						g_AGVHostCtrl->MagneticData.Distance_L 	= 0;
						g_AGVHostCtrl->MagneticData.Strength_L 	= 0;
					}
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_L = 0x00;
					Distance_L 	= 0;
					Strength_L 	= 0;
				}
			}
			/*右磁导航数据处理，根据运行状态和磁导航检测状态处理*/
			{
				uint8_t L_or_R_Sta_R = 0;
				int16_t Strength_R = 0;
				int16_t Distance_R = 0;
				L_or_R_Sta_R	= g_R_MagData.MN_DetectState;
				if(L_or_R_Sta_R == 0X07)
				{
					g_AGVHostCtrl->MagneticData.Sta_R = 0x00;
					Distance_R 	= g_R_MagData.MN_DistanceMiddle;
					Strength_R 	= g_R_MagData.MN_InductionStrengthMiddle;
				}
				else if(L_or_R_Sta_R == 0X03)
				{
					g_AGVHostCtrl->MagneticData.Sta_R = 0x00;
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Left)
					{
						Distance_R 	= g_R_MagData.MN_DistanceMiddle;
						Strength_R 	= g_R_MagData.MN_InductionStrengthMiddle;
					}
					else
					{
						Distance_R 	= g_R_MagData.MN_DistanceRight;
						Strength_R 	= g_R_MagData.MN_InductionStrengthRight;
					}
				}
				else if(L_or_R_Sta_R == 0X06)
				{
					g_AGVHostCtrl->MagneticData.Sta_R = 0x00;
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Left)
					{
						Distance_R 	= g_R_MagData.MN_DistanceLeft;
						Strength_R 	= g_R_MagData.MN_InductionStrengthLeft;
					}
					else
					{
						Distance_R 	= g_R_MagData.MN_DistanceMiddle;
						Strength_R 	= g_R_MagData.MN_InductionStrengthMiddle;
					}
				}
				else if(L_or_R_Sta_R == 0X02)
				{
					Distance_R 	= g_R_MagData.MN_DistanceMiddle;
					Strength_R 	= g_R_MagData.MN_InductionStrengthMiddle;
					if(Strength_R <= MagStrengthLimit)
					{
						g_AGVHostCtrl->MagneticData.Sta_R = 0x01;
						g_AGVHostCtrl->MagneticData.Distance_R 	= Distance_R;
						g_AGVHostCtrl->MagneticData.Strength_R 	= Strength_R;
					}
					else
					{
						g_AGVHostCtrl->MagneticData.Sta_R = 0x00;
						g_AGVHostCtrl->MagneticData.Distance_R 	= 0;
						g_AGVHostCtrl->MagneticData.Strength_R 	= 0;
					}
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_R = 0x00;
					Distance_R 	= 0;
					Strength_R 	= 0;
				}

				
			}
#endif

		}
		else if(g_HostData.ActualAGVWorkMode == AGVMode_GoTransverse)
		{
#if defined(ACC_WXB)
			/*前磁导航数据处理，简单处理*/
			{
				if(g_F_MagData.Strength <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x01;
					g_AGVHostCtrl->MagneticData.Distance_F 	= g_F_MagData.Distance;
					g_AGVHostCtrl->MagneticData.Strength_F 	= g_F_MagData.Strength;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x00;
					g_AGVHostCtrl->MagneticData.Distance_F 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_F 	= 0;
				}
			}
			/*后磁导航数据处理，简单处理*/
			{
				if(g_B_MagData.Strength <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x01;
					g_AGVHostCtrl->MagneticData.Distance_B 	= g_B_MagData.Distance;
					g_AGVHostCtrl->MagneticData.Strength_B 	= g_B_MagData.Strength;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x00;
					g_AGVHostCtrl->MagneticData.Distance_B 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_B 	= 0;
				}
			}
#else
			/*前磁导航数据处理，根据运行状态和磁导航检测状态处理*/
			{
				uint8_t L_or_R_Sta_F = 0;
				int16_t Strength_F = 0;
				int16_t Distance_F = 0;
				/**/
				{
					static uint32_t	Mag_F_Sta_Time = 0;
					static uint8_t	Mag_F_Sta_LoseNum = 0;
					static uint8_t	L_or_R_Sta_F_Temp = 0;
					do
					{
						if(g_AGVHostCtrl->Sys1msCounter < Mag_F_Sta_Time+XZW_MagSta_Filter_Time)
						{
							L_or_R_Sta_F = L_or_R_Sta_F_Temp;
							break;
						}
						Mag_F_Sta_Time = g_AGVHostCtrl->Sys1msCounter;
						if(L_or_R_Sta_F_Temp != g_F_MagData.MN_DetectState)
						{
							if(Mag_F_Sta_LoseNum < XZW_MagSta_Filter_Lose && g_F_MagData.MN_DetectState == 0X02)
							{
								Mag_F_Sta_LoseNum++;
							}
							else
							{
								L_or_R_Sta_F_Temp	= g_F_MagData.MN_DetectState;
							}
							break;
						}
						Mag_F_Sta_LoseNum = 0;
					}while(0);
				}
				
				if(L_or_R_Sta_F == 0X07)
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x01;
					/*向左移动，出现07 此时应该用左侧磁条数据*/
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Left)
					{
						Distance_F 	= g_F_MagData.MN_DistanceLeft;
						Strength_F 	= g_F_MagData.MN_InductionStrengthLeft;
					}
					else
					{
						Distance_F 	= g_F_MagData.MN_DistanceRight;
						Strength_F 	= g_F_MagData.MN_InductionStrengthLeft;
					}
				}
				else if(L_or_R_Sta_F == 0X03)
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x01;
					/*向左移动，后出现03 此时应该用右侧磁条数据*/
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Left)
					{
						Distance_F 	= g_F_MagData.MN_DistanceMiddle;
						Strength_F 	= g_F_MagData.MN_InductionStrengthMiddle;
					}
					else
					{
						Distance_F 	= g_F_MagData.MN_DistanceLeft;
						Strength_F 	= g_F_MagData.MN_InductionStrengthLeft;
					}
				}
				else if(L_or_R_Sta_F == 0X06)
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x01;
					/*向左移动，先出现06 此时应该用右侧磁条数据*/
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Left)
					{
						Distance_F 	= g_F_MagData.MN_DistanceRight;
						Strength_F 	= g_F_MagData.MN_InductionStrengthRight;
					}
					else
					{
						Distance_F 	= g_F_MagData.MN_DistanceMiddle;
						Strength_F 	= g_F_MagData.MN_InductionStrengthMiddle;
					}
				}
				else if(L_or_R_Sta_F == 0X02)
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x01;
					Distance_F 	= g_F_MagData.MN_DistanceMiddle;
					Strength_F 	= g_F_MagData.MN_InductionStrengthMiddle;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_F = 0x00;
					Distance_F 	= 0;
					Strength_F 	= 0;
				}

				if(Strength_F <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Distance_F 	= Distance_F;
					g_AGVHostCtrl->MagneticData.Strength_F 	= Strength_F;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Distance_F 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_F 	= 0;
				}
			}
			/*后磁导航数据处理，根据运行状态和磁导航检测状态处理*/
			{
				uint8_t L_or_R_Sta_B = 0;
				int16_t Strength_B = 0;
				int16_t Distance_B = 0;
				/**/
				{
					static uint32_t	Mag_B_Sta_Time = 0;
					static uint8_t	Mag_B_Sta_LoseNum = 0;
					static uint8_t 	L_or_R_Sta_B_Temp = 0;
					do
					{
						if(g_AGVHostCtrl->Sys1msCounter < Mag_B_Sta_Time+XZW_MagSta_Filter_Time)
						{
							L_or_R_Sta_B = L_or_R_Sta_B_Temp;
							break;
						}
						Mag_B_Sta_Time = g_AGVHostCtrl->Sys1msCounter;
						if(L_or_R_Sta_B_Temp != g_B_MagData.MN_DetectState)
						{
							if(Mag_B_Sta_LoseNum < XZW_MagSta_Filter_Lose && g_B_MagData.MN_DetectState == 0X02)
							{
								Mag_B_Sta_LoseNum++;
							}
							else
							{
								L_or_R_Sta_B_Temp = g_B_MagData.MN_DetectState;
							}
							break;
						}
						Mag_B_Sta_LoseNum = 0;
					}while(0);
				}
				if(L_or_R_Sta_B == 0X07)
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x01;
					/*向左移动，先出现03 此时应该用右侧磁条数据*/
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Left)
					{
						Distance_B 	= g_B_MagData.MN_DistanceLeft;
						Strength_B 	= g_B_MagData.MN_InductionStrengthLeft;
					}
					else
					{
						Distance_B 	= g_B_MagData.MN_DistanceRight;
						Strength_B 	= g_B_MagData.MN_InductionStrengthRight;
					}
				}
				else if(L_or_R_Sta_B == 0X03)
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x01;
					/*向左移动，先出现03 此时应该用右侧磁条数据*/
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Left)
					{
						Distance_B 	= g_B_MagData.MN_DistanceLeft;
						Strength_B 	= g_B_MagData.MN_InductionStrengthLeft;
					}
					else
					{
						Distance_B 	= g_B_MagData.MN_DistanceMiddle;
						Strength_B 	= g_B_MagData.MN_InductionStrengthMiddle;
					}
				}
				else if(L_or_R_Sta_B == 0X06)
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x01;
					/*向左移动，后出现06 此时应该用中间侧磁条数据*/
					if(g_AGVHostCtrl->AGVRunDir == AGVRunDir_Left)
					{
						Distance_B 	= g_B_MagData.MN_DistanceMiddle;
						Strength_B 	= g_B_MagData.MN_InductionStrengthMiddle;
					}
					else
					{
						Distance_B 	= g_B_MagData.MN_DistanceRight;
						Strength_B 	= g_B_MagData.MN_InductionStrengthRight;
					}
				}
				else if(L_or_R_Sta_B == 0X02)
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x01;
					Distance_B 	= g_B_MagData.MN_DistanceMiddle;
					Strength_B 	= g_B_MagData.MN_InductionStrengthMiddle;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_B = 0x00;
					Distance_B 	= 0;
					Strength_B 	= 0;
				}
				
				if(Strength_B <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Distance_B 	= Distance_B;
					g_AGVHostCtrl->MagneticData.Strength_B 	= Strength_B;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Distance_B 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_B 	= 0;
				}
			}
#endif
			/*左磁导航数据处理，简单处理*/
			{
				if(g_L_MagData.Strength <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Sta_L = 0x01;
					g_AGVHostCtrl->MagneticData.Distance_L 	= g_L_MagData.Distance;
					g_AGVHostCtrl->MagneticData.Strength_L 	= g_L_MagData.Strength;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_L = 0x00;
					g_AGVHostCtrl->MagneticData.Distance_L 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_L 	= 0;
				}
			}
			/*右磁导航数据处理，简单处理*/
			{
				if(g_R_MagData.Strength <= MagStrengthLimit)
				{
					g_AGVHostCtrl->MagneticData.Sta_R = 0x01;
					g_AGVHostCtrl->MagneticData.Distance_R 	= g_R_MagData.Distance;
					g_AGVHostCtrl->MagneticData.Strength_R 	= g_R_MagData.Strength;
				}
				else
				{
					g_AGVHostCtrl->MagneticData.Sta_R = 0x00;
					g_AGVHostCtrl->MagneticData.Distance_R 	= 0;
					g_AGVHostCtrl->MagneticData.Strength_R 	= 0;
				}
			}
		}
		/*没有在巡线模式下，用老版逻辑数据即可*/
		else
		{
			if(g_F_MagData.Strength <= MagStrengthLimit)
			{
				g_AGVHostCtrl->MagneticData.Sta_F = 0x01;
				g_AGVHostCtrl->MagneticData.Distance_F 	= g_F_MagData.Distance;
				g_AGVHostCtrl->MagneticData.Strength_F 	= g_F_MagData.Strength;
			}
			else
			{
				g_AGVHostCtrl->MagneticData.Sta_F = 0x00;
				g_AGVHostCtrl->MagneticData.Distance_F 	= 0;
				g_AGVHostCtrl->MagneticData.Strength_F 	= 0;
			}

			
			if(g_B_MagData.Strength <= MagStrengthLimit)
			{
				g_AGVHostCtrl->MagneticData.Sta_B = 0x01;
				g_AGVHostCtrl->MagneticData.Distance_B 	= g_B_MagData.Distance;
				g_AGVHostCtrl->MagneticData.Strength_B 	= g_B_MagData.Strength;
			}
			else
			{
				g_AGVHostCtrl->MagneticData.Sta_B = 0x00;
				g_AGVHostCtrl->MagneticData.Distance_B 	= 0;
				g_AGVHostCtrl->MagneticData.Strength_B 	= 0;
			}

			
			if(g_L_MagData.Strength <= MagStrengthLimit)
			{
				g_AGVHostCtrl->MagneticData.Sta_L = 0x01;
				g_AGVHostCtrl->MagneticData.Distance_L 	= g_L_MagData.Distance;
				g_AGVHostCtrl->MagneticData.Strength_L 	= g_L_MagData.Strength;
			}
			else
			{
				g_AGVHostCtrl->MagneticData.Sta_L = 0x00;
				g_AGVHostCtrl->MagneticData.Distance_L 	= 0;
				g_AGVHostCtrl->MagneticData.Strength_L 	= 0;
			}

			if(g_R_MagData.Strength <= MagStrengthLimit)
			{
				g_AGVHostCtrl->MagneticData.Sta_R = 0x01;
				g_AGVHostCtrl->MagneticData.Distance_R 	= g_R_MagData.Distance;
				g_AGVHostCtrl->MagneticData.Strength_R 	= g_R_MagData.Strength;
			}
			else
			{
				g_AGVHostCtrl->MagneticData.Sta_R = 0x00;
				g_AGVHostCtrl->MagneticData.Distance_R 	= 0;
				g_AGVHostCtrl->MagneticData.Strength_R 	= 0;
			}
		}
		
		
		
		
	}
	/*虚拟里程计数据接入自动运行数据管道*/
	{
		/*实时xzy坐标输入*/
		if(   g_AGVHostCtrl->VirtualOdoFixFlag != 0x00
			&&g_AGVHostCtrl->VirtualPointFlag  != 0x00)
		{
			g_AGVHostCtrl->NavigationData.NavLidar_x 	= g_AGVHostCtrl->CurrentPath_X;
			g_AGVHostCtrl->NavigationData.NavLidar_y 	= g_AGVHostCtrl->CurrentPath_Y;
		}
		else
		{
			g_AGVHostCtrl->NavigationData.NavLidar_x 	= g_VirtualOdo.RTx + g_AGVHostCtrl->CurrentPath_X;
			g_AGVHostCtrl->NavigationData.NavLidar_y 	= g_VirtualOdo.RTy + g_AGVHostCtrl->CurrentPath_Y;
		}

		/*当自转到位后，需要等待后台将里程计校订完成后，才继续使用里程计结果；否则不更新即可*/
		if (g_HostData.AGVPosture == AGVPosture_ODO_Mag && g_HostData.PosturRunSta != 0x04)
		{
			g_AGVHostCtrl->NavigationData.NavLidar_z = g_VirtualOdo.RTz / 100.0f;
		}
		else if (g_HostData.AGVPosture == AGVPosture_QR)
		{
			g_AGVHostCtrl->NavigationData.NavLidar_z = g_VirtualOdo.RTz / 100.0f;
		}
		
		/*上传调度系统的正交朝向*/
		{
			float Temp_Towards = g_AGVHostCtrl->NavigationData.NavLidar_z;
			if(  (Temp_Towards < 45.0f&&Temp_Towards >= 0.0f)
				||(Temp_Towards <= 360.0f&&Temp_Towards >= 315.0f))
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 0;
			}
			else if(Temp_Towards < 135.0f&&Temp_Towards >= 45.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 9000;
			}
			else if(Temp_Towards < 225.0f&&Temp_Towards >= 135.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 18000;
			}
			else if(Temp_Towards < 315.0f&&Temp_Towards >= 225.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 27000;
			}
		}
		/*行走距离输入*/
		if(g_VirtualOdo.Distance < 0)
		{
			g_AGVHostCtrl->PointToStart_Dis = -g_VirtualOdo.Distance*1000;/*AGV需要的距离是mm单位；虚拟里程计输出的是m，需要扩大1000倍*/
		}
		else
		{
			g_AGVHostCtrl->PointToStart_Dis = g_VirtualOdo.Distance*1000;/*AGV需要的距离是mm单位；虚拟里程计输出的是m，需要扩大1000倍*/
		}
	}
	/*工装二维码传感器数据接入自动运行数据管道*/
	{
		g_AGVHostCtrl->FixtureQRData.QR_x 		= g_Location_QRData.QR_x;
		g_AGVHostCtrl->FixtureQRData.QR_y 		= g_Location_QRData.QR_y;
		g_AGVHostCtrl->FixtureQRData.QR_z 		= g_Location_QRData.QR_z;
		g_AGVHostCtrl->FixtureQRData.Offset_z 	= g_Location_QRData.Offset_z;
		g_AGVHostCtrl->FixtureQRData.QR_Tag 	= g_Location_QRData.QR_Tag;
		g_AGVHostCtrl->FixtureQRData.PintSta 	= g_Location_QRData.PintSta;
	}
	/*当前RFID标签结果输入到自动运行模块*/
	{
		g_AGVHostCtrl->RFIDData.TagValue		= g_RfidData.Tag;
		g_AGVHostCtrl->RFIDData.LastTagValue	= g_RfidData.LastTag;
		g_AGVHostCtrl->RFIDData.ReadStatus		= g_RfidData.Status;
		g_AGVHostCtrl->RFIDData.RSSIStatus		= g_RfidData.RSSIStatus;
	}
	/*地标二维码传感器数据接入自动运行数据管道*/
	{
		g_AGVHostCtrl->ParkQRData.QR_x 		= g_Navigation_QRData.QR_x;
		g_AGVHostCtrl->ParkQRData.QR_y 		= g_Navigation_QRData.QR_y;
		g_AGVHostCtrl->ParkQRData.QR_z 		= g_Navigation_QRData.QR_z;
		//g_AGVHostCtrl->ParkQRData.Offset_z 	= g_Navigation_QRData.Offset_z;
		g_AGVHostCtrl->ParkQRData.QR_Tag 	= g_Navigation_QRData.QR_Tag;
		g_AGVHostCtrl->ParkQRData.PintSta 	= g_Navigation_QRData.PintSta;
	}
	/*是否在点位上的判断*/
	{
		/*如果是以工装二维码作为停车标志，将二维码识别状态作为是否在点位上的标志*/
		if(g_AGVHostCtrl->AGVParkNowMode == AGVParking_FixtureQR)
		{
			if(g_AGVHostCtrl->FixtureQRData.PintSta != 0X00)
			{
				g_AGVHostCtrl->PointUpdateFlag = 1;
				if(g_AGVHostCtrl->CurrentPath_Tag != 0)
				{
					g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->CurrentPath_Tag;
				}
			}
			else
			{
				g_AGVHostCtrl->PointUpdateFlag = 0;
			}
		}
		else if(g_AGVHostCtrl->AGVParkNowMode == AGVParking_FixQR_ODO)
		{
			if(g_AGVHostCtrl->FixtureQRData.PintSta != 0X00)
			{
				if(g_AGVHostCtrl->CurrentPath_Tag != 0)
				{
					g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->CurrentPath_Tag;
				}
			}
			else
			{
				g_AGVHostCtrl->PointUpdateFlag = 0;
			}
		}
		/*如果是RFID作为停车标志，状态作为是否在点位上的标志*/
		else if(  g_AGVHostCtrl->AGVParkNowMode == AGVParking_RFID
				||g_AGVHostCtrl->AGVParkNowMode == AGVParking_RFIDSignal)
		{
			if(   g_AGVHostCtrl->CurrentPointTag != g_AGVHostCtrl->RFIDData.TagValue
				&&g_AGVHostCtrl->RFIDData.ReadStatus != 0X00)
			{
				g_AGVHostCtrl->PointUpdateFlag = 1;
				g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->RFIDData.TagValue;
				g_AGVHostCtrl->NavigationData.NavLidar_Tag = g_AGVHostCtrl->RFIDData.TagValue;
			}
			else
			{
				g_AGVHostCtrl->PointUpdateFlag = 0;
			}
		}
		/*如果是十字磁条作为停车标志，状态作为是否在点位上的标志*/
		else if(g_AGVHostCtrl->AGVParkNowMode == AGVParking_CrossMag)
		{
			uint8_t CrossMagSta = 0x00;
			uint8_t ModeCheckSta = 0x00;
			if(g_HostData.ActualAGVWorkMode == AGVMode_GoLine)
			{
				if(g_HostData.MagneticData.Sta_L != 0x00)
				{
					CrossMagSta = CrossMagSta|0x01;
				}
				if(g_HostData.MagneticData.Sta_R != 0x00)
				{
					CrossMagSta = CrossMagSta|0x02;
				}
				/*通过十字磁条停车的模式 确定到位判断的标准*/
				if(g_HostData.AGVCrossMagParkMode == 0X01 || g_HostData.AGVCrossMagParkMode == 0X03)
				{
					ModeCheckSta = CrossMagSta!=0X00?0X01:0X00;
				}
				else if(g_HostData.AGVCrossMagParkMode == 0X00 || g_HostData.AGVCrossMagParkMode == 0X02)
				{
					ModeCheckSta = CrossMagSta==0X03?0X01:0X00;
				}
				if(ModeCheckSta == 0X01)
				{
					g_AGVHostCtrl->PointUpdateFlag = 1;
					if(g_AGVHostCtrl->CurrentPath_Tag != 0)
					{
						g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->CurrentPath_Tag;
						g_HostData.RFIDData.TagValue = g_AGVHostCtrl->CurrentPointTag;
					}
				}
			}
			/*横向巡线，检测前后磁条;回写tag值，自动运行行走控制才能正常进行*/
			else if(g_HostData.ActualAGVWorkMode == AGVMode_GoTransverse)
			{
				if(g_HostData.MagneticData.Sta_F != 0x00)
				{
					CrossMagSta = CrossMagSta|0x04;
				}
				if(g_HostData.MagneticData.Sta_B != 0x00)
				{
					CrossMagSta = CrossMagSta|0x08;
				}
				/*通过十字磁条停车的模式 确定到位判断的标准*/
				if(g_HostData.AGVCrossMagParkMode == 0X02 || g_HostData.AGVCrossMagParkMode == 0X03)
				{
					ModeCheckSta = CrossMagSta!=0X00?0X01:0X00;
				}
				else if(g_HostData.AGVCrossMagParkMode == 0X00 || g_HostData.AGVCrossMagParkMode == 0X01)
				{
					ModeCheckSta = CrossMagSta==0X0C?0X01:0X00;
				}
				if(ModeCheckSta == 0X01)
				{
					g_AGVHostCtrl->PointUpdateFlag = 1;
					if(g_AGVHostCtrl->CurrentPath_Tag != 0)
					{
						g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->CurrentPath_Tag;
						g_HostData.RFIDData.TagValue = g_AGVHostCtrl->CurrentPointTag;
					}
				}
			}
			/*静止模式下，检测4个磁条;回写tag值，自动运行行走控制才能正常进行*/
			else if(  g_HostData.ActualAGVWorkMode == AGVMode_GoStill
					&&(g_HostData.MagneticData.Sta_F != 0x00&&g_HostData.MagneticData.Sta_B != 0x00 && g_HostData.MagneticData.Sta_L != 0x00&&g_HostData.MagneticData.Sta_R != 0x00))
			{
				g_AGVHostCtrl->PointUpdateFlag = 1;
				if(g_AGVHostCtrl->CurrentPath_Tag != 0)
				{
					g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->CurrentPath_Tag;
				}
			}
			/*里程计停车的情况下，不需要RFID的更新*/
			else if(g_AGVHostCtrl->WalkParkFlag == AGVWalkParkFlag_OdoPark)
			{
				
			}
			/*以上都不满足，但是又在RFID标签上，将RFID的Tag作为点位标签；
				但是不更新PointUpdateFlag，防止路径链表意外更新*/
			else if(g_AGVHostCtrl->RFIDData.ReadStatus != 0X00)
			{
				g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->RFIDData.TagValue;
				g_AGVHostCtrl->NavigationData.NavLidar_Tag = g_AGVHostCtrl->RFIDData.TagValue;
			}
			else
			{
				g_AGVHostCtrl->PointUpdateFlag = 0;
			}
		}
		else if(g_AGVHostCtrl->AGVParkNowMode == AGVParking_ParkQR_ODO)
		{
			if (g_AGVHostCtrl->CurrentPointTag != g_AGVHostCtrl->ParkQRData.QR_Tag
				&& g_AGVHostCtrl->ParkQRData.PintSta != 0X00)
			{
				g_AGVHostCtrl->PointUpdateFlag = 1;
				g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->ParkQRData.QR_Tag;
				g_AGVHostCtrl->NavigationData.NavLidar_Tag = g_AGVHostCtrl->ParkQRData.QR_Tag;
			}
			else
			{
				g_AGVHostCtrl->PointUpdateFlag = 0;
			}
		}
	}
}
/***************************************************************************************
** 函数名称:	AGV_SlamNavigate_InPutSensorData
** 功能描述:	激光导航下需要输入到自动框架的传感器数据
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
void AGV_SlamNavigate_InPutSensorData(void)
{

	/*地标二维码传感器数据接入自动运行数据管道*/
	{
		g_AGVHostCtrl->ParkQRData.QR_x		= g_Location_QRData.QR_x;
		g_AGVHostCtrl->ParkQRData.QR_y		= g_Location_QRData.QR_y;
		g_AGVHostCtrl->ParkQRData.QR_z 		= g_Location_QRData.QR_z;
		//g_AGVHostCtrl->ParkQRData.Offset_z	= g_Location_QRData.Offset_z;
		g_AGVHostCtrl->ParkQRData.QR_Tag 	= g_Location_QRData.QR_Tag;
		g_AGVHostCtrl->ParkQRData.PintSta	= g_Location_QRData.PintSta;
	}				
	/*里程计数据接入自动运行数据管道-未完成，
	  暂时直接将SLAM原始数据接入,暂时只能应付直线行走，无法应对曲线*/
	{
		/*实时xzy坐标输入*/
		g_AGVHostCtrl->NavigationData.NavLidar_x = g_SlamData.X/10.0f;
		g_AGVHostCtrl->NavigationData.NavLidar_y = g_SlamData.Y/10.0f;
		if(g_SlamData.Z < 0)
		{
			g_AGVHostCtrl->NavigationData.NavLidar_z =g_SlamData.Z/100.0f+360.0f;
		}
		else
		{
			g_AGVHostCtrl->NavigationData.NavLidar_z =g_SlamData.Z/100.0f;
		}
		{
			float Temp_Towards = g_AGVHostCtrl->NavigationData.NavLidar_z;
			if(  (Temp_Towards < 45.0f&&Temp_Towards >= 0.0f)
				||(Temp_Towards <= 360.0f&&Temp_Towards >= 315.0f))
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 0;
			}
			else if(Temp_Towards < 135.0f&&Temp_Towards >= 45.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 9000;
			}
			else if(Temp_Towards < 225.0f&&Temp_Towards >= 135.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 18000;
			}
			else if(Temp_Towards < 315.0f&&Temp_Towards >= 225.0f)
			{
				g_AGVHostCtrl->NavigationData.NavLidar_Toward = 27000;
			}
		}
		/*SLAM算行走距离*/
		{
			int32_t D_Value_x = 0,D_Value_y = 0;
			D_Value_x = (g_AGVHostCtrl->CurrentPath_X - g_AGVHostCtrl->NavigationData.NavLidar_x);
			D_Value_y = (g_AGVHostCtrl->CurrentPath_Y - g_AGVHostCtrl->NavigationData.NavLidar_y);
			/*AGV需要的距离是mm单位*/
			if(g_HostData.AGV_Course == AGVCourse_Y_Negative||g_HostData.AGV_Course == AGVCourse_Y_Positive)
			{
				g_AGVHostCtrl->PointToStart_Dis = (g_AGVHostCtrl->CurrentPath_Y > g_AGVHostCtrl->NavigationData.NavLidar_y?D_Value_y:-D_Value_y);
			}
			else if(g_HostData.AGV_Course == AGVCourse_X_Negative||g_HostData.AGV_Course == AGVCourse_X_Positive)
			{
				g_AGVHostCtrl->PointToStart_Dis = (g_AGVHostCtrl->CurrentPath_X > g_AGVHostCtrl->NavigationData.NavLidar_x?D_Value_x:-D_Value_x);
			}
			else
			{
				g_AGVHostCtrl->PointToStart_Dis = sqrt((pow(D_Value_x,2) + pow(D_Value_y,2)));
			}
		}
	}
	/*是否在点位上的判断*/
	{
		float D_Value_x = 0,D_Value_y = 0;
		D_Value_x = abs(g_HostData.CurrentDelta_X - g_HostData.CurrentPath_X);
		D_Value_y = abs(g_HostData.CurrentDelta_Y - g_HostData.CurrentPath_Y);
		if(D_Value_x <= g_HostData.Para_Index.AGV_WayPointOffset_X && D_Value_y <= g_HostData.Para_Index.AGV_WayPointOffset_Y)
		{
			g_AGVHostCtrl->PointUpdateFlag = 1;
			g_AGVHostCtrl->CurrentPointTag = g_AGVHostCtrl->CurrentPath_Tag;
		}
		else
		{
			g_AGVHostCtrl->PointUpdateFlag = 0;
		}
	}

}
/***************************************************************************************
** 函数名称:	AGV_Skeleton_InPutSensorData_Dependence
** 功能描述:	将自动运行依赖的传感器CBB数据管道组织好，输入到自动运行框架中
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
void AGV_Skeleton_InPutSensorData_Dependence(void)
{
	/*依据导航方式的不同,将传感器数据输入到自动运行模块*/
	switch(g_AGVHostCtrl->AGVNavigateMode)
	{
		case AGVNavigateMode_QR:		/*二维码导航 需要虚拟里程计、工装二维码、地标二维码*/
		{
			AGV_QRNavigate_InPutSensorData();

		}break;
		case AGVNavigateMode_Magnetic:	/*磁条导航 需要工装二维码、RFID、虚拟里程计*/
		{
			AGV_MagNavigate_InPutSensorData();
		}
		break;
		default:
		case AGVNavigateMode_NULL:		/*初始化*/
		case AGVNavigateMode_GPS:		/*GPS导航，暂时和SLAM不做区分*/
		case AGVNavigateMode_SLAM:		/*激光导航 需要地标二维码、真实里程计*/
		{
			AGV_SlamNavigate_InPutSensorData();
		}
		break;
	}

}
/***************************************************************************************
** 函数名称:	AGV_Skeleton_InPutChassisData_Dependence
** 功能描述:	将自动运行依赖底盘数据管道组织好，输入到自动运行框架中
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
__weak void AGV_Skeleton_InPutChassisData_Dependence(void)
{

}
/***************************************************************************************
** 函数名称:	AGV_Skeleton_InPutChassisData_Dependence
** 功能描述:	将自动运行对底盘输出的数据管道组织好，接管遥控程序
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
__weak void AGV_Skeleton_OutPutChassisData_Dependence(void)
{

}
/***************************************************************************************
** 函数名称:	AGV_Skeleton_InData_Dependence
** 功能描述:	在运动控制的函数中，不论有没有在自动模式下都需要持续调用的函数，如果不调用则调度系统
			无法得知当前AGV的状态和所处位置
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
void AGV_Skeleton_InData_Dependence(void)
{
	/*初始化检查没通过，直接返回*/
	if(g_AGV_DependenceInitCheckSucc == 0x00){return;}
	/*将底盘接口的输入数据管道组织好*/
	{
		AGV_Skeleton_InPutChassisData_Dependence();
	}
	/*依据导航方式的不同,将传感器的输入数据管道组织好*/
	{
		AGV_Skeleton_InPutSensorData_Dependence();
	}

}

/***************************************************************************************
** 函数名称:	AGV_Skeleton_Dependence
** 功能描述:	在运动控制的函数中，实现自动运行的依赖代码；函数内部会直接调用自动运行框架代码
			并且接管遥控器对地盘进行控制
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_Skeleton_Dependence(void)
{
	/*初始化检查没通过，直接返回*/
	if(g_AGV_DependenceInitCheckSucc == 0x00){return;}

	Tsk_MasterAutoControl();         //当前进入自动模式

	/*将底盘接口的输出数据管道组织好，接管底盘遥控*/
	{
		AGV_Skeleton_OutPutChassisData_Dependence();
	}

}


/************************************END OF FILE************************************/
