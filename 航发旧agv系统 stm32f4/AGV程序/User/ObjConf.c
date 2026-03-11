/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ObjConf.c
** 创 建 人: 文小兵  
** 描    述: 对象配置
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define ObjConf_GLOBALS

//自定义引用			
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "ObjConf.h"
#include "drv_IO.h"
#include "ADDA.h"
#include "LinearCal.h"
#include "app_SkeletonAGV.h"
#include "Debug.h"
#include "CommConf.h"
#include "HMI_ConfInit.h"
#include "Conf_Template.h"
#include "Conf_Test.h"
#include "IOConf.h"
#include "SwitchboardData.h"
#include "DateTime.h"

//#include "Patch.h"

//外部对象声明

/*内部方法声明*/


/*AD端口根据业务功能的逻辑检查，*/
void ADDA_ADInit_IDfunCheck(void)
{
	if(g_Para.BatteryType == ACC_LeadBatteries_TN48V || g_Para.BatteryType == ACC_LeadBatteries_TN72V)
	{
		/*天能48V/72V电池采集标定参数设置*/
		/*模拟量输入量程*/
		g_ADFucs.SysCtrlVoltage.RatioOrgA = 0;
		g_ADFucs.SysCtrlVoltage.RatioOrgB = 3900;
		/*物理量量程*/
		g_ADFucs.SysCtrlVoltage.RatioActA = 0;
		g_ADFucs.SysCtrlVoltage.RatioActB = 4800;	//0.1V
	}

	/*自适应压力传感器数据来源*/
	if(   g_ADFucs.FL_Pressure.Id	== ADCN_Disable
		&&g_ADFucs.FR_Pressure.Id	== ADCN_Disable
		&&g_ADFucs.B_Pressure.Id	== ADCN_Disable)
	{
		g_Para.AppInSource.FucBit.MCBLoadPressure = 0;	/*从周正模块获取压力数据*/
	}
	else
	{
		g_Para.AppInSource.FucBit.MCBLoadPressure = 1;	/*从开发板直接的ADC块获取压力数据*/
	}
	
	/*自适应 升降控制是否是科润阳光*/
	if(g_DAFucs.KoRunCtrl.Id == DACN_Disable)
	{
		g_Para.AppOutSource.FucBit.DACLifeMoter			= 0;	/*通过Can伺服控制举升电机转速*/
	}
	else
	{
		g_Para.AppOutSource.FucBit.DACLifeMoter			= 1;	/*通过科润阳光控制举升电机转速*/
	}

	/*自适应 */
}

//从eeprom对象中读取参数
bool ParaInit_EEPObj2Sys(void)
{
	char i=0;
	//g_Para.EqpID=EEPPara.User.EqpID;//设备ID在前面已经读取
	g_Para.ChassisType=EEPPara.User.ChassisType;            //车型
    g_Para.FunctionSwitch = EEPPara.User.FunctionSwitch;    //功能使能

	//角度0位置标定参数
	for (i = 0; i < g_Para.WheelAngle.Num; i++)
		g_Para.WheelAngle.Offset[i] = EEPPara.User.WheelAngleZero[i]/100.0f;

    //升降参数
	g_Para.Lift = EEPPara.User.Prj.Prj1.Lift;
    //虚拟里程计参数
    g_Para.Obo = EEPPara.User.Prj.Prj1.Obo;
    //报警检出参数
    g_Para.EmgJudge = EEPPara.User.Prj.Prj1.EmgJudge;
    //安装标定参数
    g_Para.Inst = EEPPara.User.Prj.Prj1.Inst;
    //控制系数参数
    g_Para.CtrlCoef = EEPPara.User.Prj.Prj1.CtrlCoef;
	//自动运行参数
	g_Para.Auto = EEPPara.User.Prj.Prj1.Auto;
	// 其他参数
    g_Para.Other = EEPPara.User.Prj.Prj1.Other;

	//检查读取can波特率参数
	if(EEPPara.User.CanBaud != 0)
		g_Para.CanMst.Baud=EEPPara.User.CanBaud;
	else
		g_Para.CanMst.Baud=CAN_BAUDRATE_500K;


	return 1;
}

//代码版本号设定与检查
bool ParaInit_CodeVersion(void)
{
	bool res = 0;
	//设定代码版本号 格式：xxxx(年)x(月)x(日)x(秒)8511(自定义)
    //利用__DATE__、__TIME__两个宏定义获取编译日期和时间
    //将日期和时间字符串转换为指定的数字格式
    //该参数只保存到eeprom，运行时读取和系统中的对比，系统参数不去读eep对象
    //暂时手动输入，后续可自动获取
    //g_Para.CodeVersion = 2024071401;
	g_Para.CodeVersion = 50904;
    EEPPara.WriteDate = AppInfo_GetCompileYear() * 100000000 + AppInfo_GetCompileMonth() * 1000000 + 
						AppInfo_GetCompileDay() * 10000 + AppInfo_GetCompileHour() * 100 + AppInfo_GetCompileMinute();
    //判断版本号
    if(g_Para.CodeVersion != EEPPara.CodeVersion 
		&& g_Para.RunningMode != ACC_RunningMode_SoftwareSimulat
		&& AtDbug.Bit.Skip_EqpIDSet == 0
	)
    {
		//强行报警提示，关闭伺服使能
		g_Para.Debug.ForceSVODisable = 1;
		g_Para.Debug.ForceSVODisable_Lift = 1;
		HardInfo.SysBitExpand.Bit.EEP_CodeVersionErr = 1;
		//g_CodeErr.All = 0xFFFFFFFF;
		//预充也不启动
		g_PowerOn.PowerOnStep = ACC_PowerOnStep_StartupFail;
		res = 0;
    }
	else
	{
		res = 1;
	}

	return res;
}

//根据uID设置EqpID
bool ParaInit_EqpID_ByUID(void)
{
	if (HardInfo.uID[0] == 0x2001F498)
		g_Para.EqpID = EqpID_Template;
	else if (HardInfo.uID[0] == 0x002D004B)
		g_Para.EqpID = EqpID_Template;
	else
		g_Para.EqpID = EqpID_Template;

	return 1;
}

//EqpID设置
bool ParaInit_EqpID_Set(void)
{
	if (g_Para.RunningMode == ACC_RunningMode_SoftwareSimulat)
	{
		g_Para.EqpID = EqpID_MDKSimulator;
	}
	else if (g_Para.RunningMode == ACC_RunningMode_BareSimulat)
	{
		g_Para.EqpID = EqpID_BareSimulat;
	}
	else
	{
		g_Para.EqpID = EEPPara.User.EqpID;//从EEPROM读取设备ID
		//g_Para.EqpID = EqpID_Template;//使用模板
		//ParaInit_EqpID_ByUID();//根据硬件ID初始化设备ID,EEPROM不可用时
	}

	return 1;
}

//从eeeprom对象中读取参数
bool ParaInit_EEP(void)
{
	bool res = 0;
	if(g_Para.RunningMode!=ACC_RunningMode_SoftwareSimulat)//以上设备不读取EEP
	{
		if(!ParaInit_EEPObj2Sys())
			res = 0;
		else
			res = 1;
	}
	else
	{
		res = 1;
	}

	return res;
}

//io配置检查
bool ParaInit_CheckOK_IO(void)
{
	char ErrCnt = 0;
	u16 i = 0;
	IO_Output_TypeDef* p2IO = (IO_Output_TypeDef*)&g_IOOutputs;

	//清除IO口映射标志位
	memset((u16*)(&g_Para.Check.IO_Mapped), 0, sizeof(g_Para.Check.IO_Mapped));

	for (i = 0; i < g_IOOutputs.Number; i++)
	{
		//未使能的IO口不检查
		if (p2IO[i].ID == OCN_Disable)
			continue;

		//检查范围
		//if (p2IO[i].ID < IOBit_YOUT0 || (p2IO[i].ID > IOBit_YOU_ALL_OFF && p2IO[i].ID != OCN_Disable))
		if (p2IO[i].ID < IOBit_YOUT0 || p2IO[i].ID > IOBit_YOUT71)
			ErrCnt++;

		//检查是否有重复的IO口配置
		if (GET_ASSIGN_COIL((u16*)( & g_Para.Check.IO_Mapped), p2IO[i].ID))
			ErrCnt++;
		else
			SET_ASSIGN_COIL((u16*)(&g_Para.Check.IO_Mapped), p2IO[i].ID);

		//如果有错误则退出检查
		if (ErrCnt)
			break;
	}

	if (ErrCnt)
	{
		g_Para.Check.Bit.IO_MapErr = 1;
		return 0;
	}
	else
	{
		g_Para.Check.Bit.IO_MapErr = 0;
		return 1;
	}
}

//最后参数检查
bool ParaInit_CheckOK(void)
{
	char ErrCnt=0;
	char i = 0;

	//检查can波特率参数
	if(g_Para.CanMst.Baud == 0)//can波特率参数检查 如不正确系统将反复重启
		g_Para.CanMst.Baud=CAN_BAUDRATE_500K;
	else {}

	/*检查canopen中驱动器类型和参数中配置是否一致 没能实现
	if (SVO_Type == 0 && g_Para.CanMst.SVOType != Can_SVOType_Delta)
		ErrCnt++;
	else if (SVO_Type == 1 && g_Para.CanMst.SVOType != Can_SVOType_Kinco)
		ErrCnt++;
	else {}*/

	//检查是否有差速舵轮的坐标位置太靠近原点，但又不在原点的情况
	//因为这种情况会导致某个轮子半径为负的情况，软件尚未适配处理
	//正好位于原点时，已特殊处理
	if (g_Para.ChassisType == ACC_ChassisType_Titan || g_Para.ChassisType == ACC_ChassisType_Rhino)
	{
		for (i = 0; i < g_Para.Walk.MotorNum/2; i++)
		{
			//判断模组位置到原点的距离，是否小于Lc参数
			if (sqrt(pow(fabs(g_Para.Walk.Mxy[i].x),2) + pow(fabs(g_Para.Walk.Mxy[i].y),2)) < g_Para.Walk.Lc)
				ErrCnt++;
			else {}
		}
	}
	else {}

	//io配置检查
	if(ParaInit_CheckOK_IO() == 0)
		ErrCnt++;
	else {}

	//返回检查结果
	if (ErrCnt)
	{
		g_Para.Check.ErrFlag = 1;
		return 0;
	}
	else
	{
		g_Para.Check.ErrFlag = 0;
		return 1;
	}
}

//系统参数对象初始化
bool ParaInit(void)
{
	char errcnt=0;

    //代码版本号设定与检查
    if(ParaInit_CodeVersion() == 0)
		errcnt++;
	else {}

	//EqpID设置
	if (AtDbug.Bit.Skip_EqpIDSet == 0)
		ParaInit_EqpID_Set();
	else {}

	//项目配置模板先调用一次（即使在具体项目配置中漏了也有默认值）
	Para_Template();

	//初始化项目参数函数指针
	ParaInit_p2FucInit(g_Para.EqpID);

	//根据设备ID初始化编译预制参数
	if(g_Para.p2Fucs.p2PrjParaInit)
		g_Para.p2Fucs.p2PrjParaInit();
    else
		errcnt++;

	//从eeeprom对象中读取参数
	if (AtDbug.Bit.Skip_EEP2Sys == 0)
	{
		if (ParaInit_EEP() == 0)
			errcnt++;
		else {}
	}
	else {}

	//根据运行模式强制覆盖部分调试参数
    ParaInit_CodeReconf();

	//参数最后检查
	if (ParaInit_CheckOK() == 0)
		errcnt++;
	else {}

	/*检查是否初始化成功并返回*/
	if(errcnt)
	{
		g_CodeErr.Bit.InitErr=1;
		return 0;
	}	
	else
	{
		g_Para.Initiated = 1;
		return 1;
	}
}

//系统参数对象 写入 EEPROM对象
bool ParaInit_Sys2EEPObj(void)
{
	char i=0;
	EEPPara.User.EqpID = g_Para.EqpID;                      //设备ID
	EEPPara.User.ChassisType = g_Para.ChassisType;          //车型
    EEPPara.User.FunctionSwitch = g_Para.FunctionSwitch;    //功能使能
	EEPPara.User.CanBaud = g_Para.CanMst.Baud;              //can波特率

	//角度0位置标定参数
	for(i=0;i< g_Para.WheelAngle.Num;i++)
		EEPPara.User.WheelAngleZero[i] = g_Para.WheelAngle.Offset[i]*100;
	
	EEPPara.User.Prj.Prj1.Lift = g_Para.Lift;               //升降参数
    //虚拟里程计参数
    EEPPara.User.Prj.Prj1.Obo = g_Para.Obo;
    //报警检出参数
    EEPPara.User.Prj.Prj1.EmgJudge = g_Para.EmgJudge;
    //安装标定参数
    EEPPara.User.Prj.Prj1.Inst = g_Para.Inst;
    //控制系数参数
    EEPPara.User.Prj.Prj1.CtrlCoef = g_Para.CtrlCoef;
	//自动运行参数
	EEPPara.User.Prj.Prj1.Auto = g_Para.Auto;

    //版本号参数
    EEPPara.CodeVersion = g_Para.CodeVersion;

	//项目自定义参数
	g_Para.p2Fucs.p2ParaInit_Sys2EEPObj();
	
	//其他参数
     EEPPara.User.Prj.Prj1.Other = g_Para.Other;
	return 1;
}

//canopen对象初始化
bool ObjInit_CanMst(void)
{
	CanMaster.Para = g_Para.CanMst;
	return 1;
}
//SVO对象初始化
bool ObjInit_SVO_DATA(void)
{
	SvoType_EnumDef temp;
	if(g_Para.CanMst.SVOType == Can_SVOType_Kinco)
	{
		//默认4代步科
		temp = SvoType_Kinco;
	}
	else if(g_Para.CanMst.SVOType == Can_SVOType_Delta)
	{
		temp = SvoType_Delta;
	}
	else if(g_Para.CanMst.SVOType == Can_SVOType_Windcon)
	{
		temp = SvoType_Fdk;
	}
	else if(g_Para.CanMst.SVOType == Can_SVOType_Wheatstone)
	{
		temp = SvoType_Hst;
	}	
	else if(g_Para.CanMst.SVOType == Can_SVOType_Keya)
	{
		temp = SvoType_Keya;
	}	
	else if(g_Para.CanMst.SVOType == Can_SVOType_Copley)
	{
		temp = SvoType_Copley;
	}		
	else
	{
		/*其他驱动器，暂时不支持*/
	}
	{
		/*暂时粗暴的处理，将所有驱动器的类型改成步科或台达*/
		uint8_t i=0;
		for(i=0 ; i<SVO_MAXNUM ; i++)
		{
			if(SVO[i].SvoType == SvoType_Null)
			{
				SVO[i].SvoType = temp;
			}
		}
	}

		return 1;
}
//轮系角度对象参数初始化
bool ObjInit_WheelAngle(void)
{
	u8 i=0;
	u8 Num=WheelAngleNum_MAX;

	for(i=0;i<Num;i++)
    {
		g_WheelAngle[i].Type = g_Para.WheelAngle.WheelAngleType;
		g_WheelAngle[i].Prop = g_Para.WheelAngle.Prop;
		g_WheelAngle[i].SetA = g_Para.WheelAngle.SetA;
		g_WheelAngle[i].SetB = g_Para.WheelAngle.SetB;

		g_WheelAngle[i].OrgSetA = g_Para.WheelAngle.OrgSetA[i];
		g_WheelAngle[i].OrgSetB = g_Para.WheelAngle.OrgSetB[i];
		g_WheelAngle[i].Offset = g_Para.WheelAngle.Offset[i];

	}
		
	return 1;
}

//边界参数初始化异常检查
bool ObjInit_BoundCheckOut(void)
{
	char errcnt=0;	

	//先检查宏，再检查变量
	//角度传感器数量 不能超过 hmi显示内容
	if(WheelAngleNum_MAX > HMI_AngleSensor_MaxNum)
		errcnt++;

	//角度传感器数量 不能超过 eeprom 0°标定参数数量
	if (WheelAngleNum_MAX > Para_WheelAngle_MAXNUM)
		errcnt++;

	//角度传感器数量参数 不能超过 角度传感器对象最大数量
	if(g_Para.WheelAngle.Num > WheelAngleNum_MAX)
		errcnt++;

	//角度传感器数量参数 不能超过 角度传感器转接板通信对象的数量
	if((g_Para.WheelAngle.Num*2 + 2) > CommConvReadMax)
		errcnt++;

	//角度传感器数量 要等于 舵轮轮系数量
	if(    g_Para.WheelAngle.Num != (g_Para.Walk.MotorNum/2) 
		&& (g_Para.ChassisType == ACC_ChassisType_Titan || g_Para.ChassisType == ACC_ChassisType_Rhino))
	//if(WheelAngleNum_MAX != MR_ModuleNumMxa)
		errcnt++;

	//轮子数量参数 不能超过 canopen主站对象最大数量
	if(g_Para.Walk.MotorNum > MAX_NodeId_NUM)
		errcnt++;

	//轮子数量参数 不能超过 抽象伺服对象最大数量
	if(g_Para.Walk.MotorNum > MAX_NodeId_NUM)
		errcnt++;

	//轮子数量参数 不能超过 舵轮解算CBB对象
	if(g_Para.Walk.MotorNum > (MR_ModuleNumMxa*2))
		errcnt++;

	//触摸屏页面对象大小 不能超过页面通信最大缓存
	if(sizeof(CanopenStatus_Typedef) > sizeof(ExtHmi.p2HMI_CommObj->Screen_Head))//伺服信息
		errcnt++;

    //避障CBB定义的雷达数量 与 底盘核心模块定义要一致
    //if(ACC_OBSRADARMAX != Radar_MaxNum)
	//更新为 CBB定义的雷达数量 不能比 底盘核心模块定义大
    if(ACC_OBSRADARMAX < Radar_MaxNum)
        errcnt++;
    else {}

    //平台升降CBB定义的平台升降电机数量 与 底盘核心模块定义要一致
	if(ACC_LIFTMOTOR_MAX != LIFT_HG_MAX)
        errcnt++;
    else {}

    //解算CBB定义的电机数量 与 底盘核心模块定义要一致
	if(ACC_WALKMOTOR_MAX != MR_ModuleNumMxa * 2)
        errcnt++;
    else {}


	if(errcnt)
	{
		g_CodeErr.Bit.BoundCheckOut = 1;
		return 1;
	}
	else
		return 0;
}


//PowerOn对象参数初始化
bool ObjInit_PowerOn(void)
{
//	ACC_DATA_TypeDef* p2Acc = &AccData;
	ACC_PowerOn_TypeDef* p2PowerOn = &g_PowerOn;

	if(g_Para.BatteryType == ACC_LithiumBattery_48V_Low)
	{
		p2PowerOn->BatteryType = ACC_CanopenStep_Self_LithiumBattery_Low;
	}
	else if(g_Para.BatteryType == ACC_LithiumBattery_300V_High)
	{
		p2PowerOn->BatteryType = ACC_CanopenStep_Self_LithiumBattery_High;
	}
	else if(g_Para.BatteryType == ACC_LeadBatteries_TN48V
		  ||g_Para.BatteryType == ACC_LeadBatteries_HBK48V
		  ||g_Para.BatteryType == ACC_LeadBatteries_TN72V)
	{
		p2PowerOn->BatteryType = ACC_BatteryType_LeadBatteries;
	}
	else if(g_Para.BatteryType == ACC_Cable_Power)
	{
		p2PowerOn->BatteryType = ACC_CanopenStep_Cable_Power_Supply;
	}
	else
	{
		p2PowerOn->BatteryType = ACC_CanopenStep_Others;
	}
	

	//...
	return 1;
}

//联动对象初始化
bool Objinit_Assembly(AsbCtrl_TypeDef *p)
{
	return 1;
}

//通信接口配置
bool ObjInit_EQPComm(void)
{
	bool res = false;

	/*按照配置进行初始化*/
	EQPComm_Init();
	
	//串口初始化要在eeprom和系统参数对象初始化完成后调用
	/*如果函数指针是空，就别调用了，防止程序跑飞*/
	if(g_Para.p2Fucs.p2EQPInit != NULL)
	{
		/*特殊的Modbus初始化*/
		res = g_Para.p2Fucs.p2EQPInit();
	}
	
	
	
	
	return res;
}


//PGV 输入数据管道组合 自适应配置
void ObjInit_PGVInputType_Init(void)
{
	/**/
	{
		/*对工装 单个 直连的自制*/
		if(g_Para.EQPCommSource.EN.PGVUpward == 1)
		{
			g_Para.PGVInputType.HF_Tool_Sole	= 1;
			g_Para.PGVInputType.Tool_Sole 		= 1;
		}
		/*对工装 单个 直连的大*/
		else if(0)
		{
			g_Para.PGVInputType.DH_Tool_Sole	= 1;
			g_Para.PGVInputType.Tool_Sole 		= 1;
		}
		/*对工装 单个 直连的倍加福*/
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Upward == 1)
		{
			g_Para.PGVInputType.BJF_Tool_Sole	= 1;
			g_Para.PGVInputType.Tool_Sole 		= 1;
		}
		/*对工装 单个 导航转接板*/
		else if(g_Para.SerialExpObjSource.PGVUpward == 1)
		{
			g_Para.PGVInputType.SE_Tool_Sole	= 1;
			g_Para.PGVInputType.Tool_Sole 		= 1;
		}
		/*对工装 两个 直连的自制*/
		else if(g_Para.EQPCommSource.EN.PGVUpward_F == 1
			&&  g_Para.EQPCommSource.EN.PGVUpward_B == 1
		)
		{
			g_Para.PGVInputType.HF_Tool_Merge	= 1;
			g_Para.PGVInputType.Tool_Merge 		= 1;
		}
		/*对工装 两个 直连的大*/
		else if(0)
		{
			g_Para.PGVInputType.DH_Tool_Merge	= 1;
			g_Para.PGVInputType.Tool_Merge 		= 1;
		}
		/*对工装 两个 直连的倍加福*/
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Upward_F == 1
			&&  g_Para.EQPCommSource.EN.BJF_PGV_Upward_B == 1
		)
		{
			g_Para.PGVInputType.BJF_Tool_Merge	= 1;
			g_Para.PGVInputType.Tool_Merge 		= 1;
		}
		/*对工装 两个 导航转接板*/
		else if(g_Para.SerialExpObjSource.PGVUpward_F == 1
			&&  g_Para.SerialExpObjSource.PGVUpward_B == 1
		)
		{
			g_Para.PGVInputType.SE_Tool_Merge	= 1;
			g_Para.PGVInputType.Tool_Merge 		= 1;
		}
	}
	/**/
	{
		/*对地 单个 直连的自制*/
		if(g_Para.EQPCommSource.EN.PGVDownward == 1)
		{
			g_Para.PGVInputType.HF_Groud_Sole	= 1;
			g_Para.PGVInputType.Groud_Sole 		= 1;
		}
		/*对地 单个 直连的大*/
		else if(0)
		{
			g_Para.PGVInputType.DH_Groud_Sole	= 1;
			g_Para.PGVInputType.Groud_Sole 		= 1;
		}
		/*对地 单个 直连的倍加福*/
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Downward == 1)
		{
			g_Para.PGVInputType.BJF_Groud_Sole	= 1;
			g_Para.PGVInputType.Groud_Sole 		= 1;
		}
		/*对地 单个 导航转接板*/
		else if(g_Para.SerialExpObjSource.PGVDownward == 1)
		{
			g_Para.PGVInputType.SE_Groud_Sole	= 1;
			g_Para.PGVInputType.Groud_Sole 		= 1;
		}
		/*对地 两个 直连的自制*/
		else if(g_Para.EQPCommSource.EN.PGVDownward_F == 1
			&&  g_Para.EQPCommSource.EN.PGVDownward_B == 1
		)
		{
			g_Para.PGVInputType.HF_Groud_Merge	= 1;
			g_Para.PGVInputType.Groud_Merge		= 1;
		}
		/*对地 两个 直连的大*/
		else if(0)
		{
			g_Para.PGVInputType.DH_Groud_Merge	= 1;
			g_Para.PGVInputType.Groud_Merge		= 1;
		}
		/*对地 两个 直连的倍加福*/
		else if(g_Para.EQPCommSource.EN.BJF_PGV_Downward_F == 1
			&&  g_Para.EQPCommSource.EN.BJF_PGV_Downward_B == 1
		)
		{
			g_Para.PGVInputType.BJF_Groud_Merge	= 1;
			g_Para.PGVInputType.Groud_Merge		= 1;
		}
		/*对地 两个 导航转接板*/
		else if(g_Para.SerialExpObjSource.PGVDownward_F == 1
			&&  g_Para.SerialExpObjSource.PGVDownward_B == 1
		)
		{
			g_Para.PGVInputType.SE_Groud_Merge	= 1;
			g_Para.PGVInputType.Groud_Merge		= 1;
		}
	}
}

//其它系统对象初始化
bool ObjInit_Others(void)
{
    //在解析遥控器命令之前 初始化急停没有按下 避免误报
	g_ChannelSwitch.CtrlCmd.Button.EStop = 1;
	return 1;
}

//对象初始化
bool ObjInit(void)
{
	char errCnt=0;

	//canopen对象初始化
	ObjInit_CanMst();
	
	//SVO对象初始化
	ObjInit_SVO_DATA();

    //HMI对象初始化
    ObjInit_HMI();

    //输入io对象初始化
    ObjInit_IOInputs();

    //输出io对象初始化
    ObjInit_IOOutputs();

    //共用体指针初始化
    g_IOCN2DHRBit.pIO2DHRBit = (IO_IO2DHRBit_StDef *)IO_DHR;
    g_IOCN2DHRBit.pCN2DHRBit = (IO_CN2DHRBit_StDef *)IO_DHR;
	SwitchboardData_Init();

	//PGV数据输入源头自适应
	ObjInit_PGVInputType_Init();

    //CBB对象参数初始化
    if(!CBB_ObjInit())
		errCnt++;
	else {}

    //ACC对象参数初始化
    ACC_ObjInit(&AccData, &g_Para);

    //PowerOn对象参数初始化
    ObjInit_PowerOn();

	//WheelAngle对象参数初始化
	ObjInit_WheelAngle();

	//ADDA对象初始化
	if(!ADDA_Init())
		errCnt++;
	else {}

	//联动对象初始化
	Objinit_Assembly(&g_Asb);
	
	//通信接口配置初始化 放到了项目参数初始化中
	if (!ObjInit_EQPComm())
		errCnt++;
	else {}

    //边界检查初始化
	if(ObjInit_BoundCheckOut())
		errCnt++;
	else {}
		
	//其它系统对象初始化
	ObjInit_Others();

	if(errCnt)
		return 0;
	else
		return 1;
}

//***********************************END OF FILE***********************************

