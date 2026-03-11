/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Fun_Template.c
** 创 建 人: Master
** 描    述: 模板项目
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期:  2025年2月10日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define Fun_Template_GLOBALS

//自定义引用			
#include "Fun_Template.h"
#include "AgvChassisController.h"
#include "ACC_Project.h"
#include "HMI.h"
#include "ACC_Init.h"

//自动和遥控模式下 都会被响应的按键
void ACC_GetCmdFormWirelessRemote_Auto_Template(void)
{
	//照明命令
	if (g_WHData.J14 == 1)
	{
		g_ChannelSwitch.CtrlCmd.Button.Lighting = 1;
	}
	else
	{
		g_ChannelSwitch.CtrlCmd.Button.Lighting = 0;
	}
	//喇叭命令
	if (g_WHData.J13 == 1)
	{
		g_ChannelSwitch.CtrlCmd.Button.Horn = 1;
	}
	else
	{
		g_ChannelSwitch.CtrlCmd.Button.Horn = 0;
	}
	//遥控器控制调度/遥控切换
	if(0)
	{
		g_ChannelSwitch.Bits.DspModeSel = 1;
	}
	else
	{
		g_ChannelSwitch.Bits.DspModeSel = 0;
	}
	//获取急停命令
	g_ChannelSwitch.CtrlCmd.Button.EStop = g_WHData.EmgStop;
}

//在单车、自转模式、按下自动对中时,自动根据一侧前后的测距值控制z轴速度，直到测距值之差到达指定范围
bool ACC_AutoAdjustVz_Template(void)
{
	f32 Dist_RF = g_ShelfAutadj.in_DistanceRight[1];//右侧前方测距值 外侧的传感器可能在工装外面所以用里侧的
	f32 Dist_RB = g_ShelfAutadj.in_DistanceRight[3];//右侧后方测距值
	f32 K_Diff2Vz = 0.5;
	f32 Diff_Dist_Min = 5;//测距值之差最小值
	f32 CmdMin = 5;//纠正命令最小值
	f32 CmdMax = 20;//纠正命令最大值
	f32 CmdTemp = 0;

	if ((g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_A || g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_B)//单车模式
		&& g_ChannelSwitch.CtrlCmd.Mode == ACCMode_Rotation//自转模式
		&& g_ChannelSwitch.CtrlCmd.Button.ShelfAutadj//自动对中按钮按下
		&& fabs(Dist_RF - Dist_RB) > Diff_Dist_Min//测距值之差大于设定值
		//&& 自动对中条件满足（两个测距值都在合理范围内）
	)
	{
		CmdTemp = (Dist_RF - Dist_RB) * K_Diff2Vz;
		//限幅
		g_ChannelSwitch.CtrlCmd.Vxyz.z = AbsFuc(CmdTemp, CmdMax);
		//保证最小命令量
		g_ChannelSwitch.CtrlCmd.Vxyz.z = AbsFucLimitMin(g_ChannelSwitch.CtrlCmd.Vxyz.z, CmdMin);
	}
	else
	{
		//什么都不做
	}

	return 1;
}

//在单车、横向模式、按下自动对中时,自动根据两侧的平均测距值控制x轴速度，直到测距值之差到达指定范围
bool ACC_AutoAdjustVx_Template(void)
{
	f32 Dist_L = (g_ShelfAutadj.in_DistanceLeft[1] + g_ShelfAutadj.in_DistanceLeft[2]) / 2;//左侧平均测距值 外侧的传感器可能在工装外面所以用里侧的
	f32 Dist_R = (g_ShelfAutadj.in_DistanceRight[1] + g_ShelfAutadj.in_DistanceRight[2]) / 2;//右侧平均测距值
	f32 K_Diff2Vx = 0.5;
	f32 Diff_Dist_Min = 5;//测距值之差最小值
	f32 CmdMin = 5;//纠正命令最小值
	f32 CmdMax = 20;//纠正命令最大值
	f32 CmdTemp = 0;

	if ((g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_A || g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_B)//单车模式
		&& g_ChannelSwitch.CtrlCmd.Mode == ACCMode_InfFlexible//横向模式
		&& g_ChannelSwitch.CtrlCmd.Button.ShelfAutadj//自动对中按钮按下
		&& fabs(Dist_L - Dist_R) > Diff_Dist_Min//测距值之差大于设定值
		//&& 自动对中条件满足（两个测距值都在合理范围内）
	)
	{
		//按比例计算纠正命令
		CmdTemp = (Dist_R - Dist_L) * K_Diff2Vx;
		//限幅
		g_ChannelSwitch.CtrlCmd.Vxyz.x = AbsFuc(CmdTemp, CmdMax);
		//保证最小命令量
		g_ChannelSwitch.CtrlCmd.Vxyz.x = AbsFucLimitMin(g_ChannelSwitch.CtrlCmd.Vxyz.x, CmdMin);
	}
	else
	{
		//什么都不做
	}
	return 1;
}

//从无线遥控器获取控制命令 Template
bool ACC_GetCmdFormWirelessRemote_Template(void)
{
	unsigned char temp = 0;
	unsigned char temp_Sel = 0;

	//授权钥匙信号
	//如果有，就从g_WHData解析，如果没有应默认为授权状态
	g_ChannelSwitch.Bits.RemoteKeyAuth = 1;
	
	//模式命令
	if (g_WHData.WorkMode < 15)
		temp = g_WHData.WorkMode + 1;

	if (temp == 1)
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_PosFlexible;		
	else if (temp == 2)
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_PosTrace;
	else if (temp == 3)
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_InfFlexible;
	else if (temp == 4)
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_InfTrace;
	else if (temp == 5)
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Rotation;
	else if (temp == 6)
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Still;
	else if (temp == 7)
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Push;
	else
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Still;

	//摇杆命令
	g_ChannelSwitch.CtrlCmd.Vxyz.x = g_WHData.XAxisOutput;
	g_ChannelSwitch.CtrlCmd.Vxyz.y = g_WHData.YAxisOutput;
	g_ChannelSwitch.CtrlCmd.Vxyz.z = g_WHData.ZAxisOutput;

	//避障命令
	if (g_WHData.J43 == 0) //Reg16.0  
	{
		g_ChannelSwitch.CtrlCmd.Button.ObsRadarFront = 1;
		g_ChannelSwitch.CtrlCmd.Button.ObsRadarBack = 1;
		g_ChannelSwitch.CtrlCmd.Button.ObsRadarLeft = 1;
		g_ChannelSwitch.CtrlCmd.Button.ObsRadarRight = 1;

		g_ChannelSwitch.WhRadarEn.Front = 1;
		g_ChannelSwitch.WhRadarEn.Back = 1;
		g_ChannelSwitch.WhRadarEn.Left = 1;
		g_ChannelSwitch.WhRadarEn.Right = 1;
	}
	else
	{
		g_ChannelSwitch.CtrlCmd.Button.ObsRadarFront = 0;
		g_ChannelSwitch.CtrlCmd.Button.ObsRadarBack = 0;
		g_ChannelSwitch.CtrlCmd.Button.ObsRadarLeft = 0;
		g_ChannelSwitch.CtrlCmd.Button.ObsRadarRight = 0;

		g_ChannelSwitch.WhRadarEn.Front = 0;
		g_ChannelSwitch.WhRadarEn.Back = 0;
		g_ChannelSwitch.WhRadarEn.Left = 0;
		g_ChannelSwitch.WhRadarEn.Right = 0;
	}

	//升降命令
	if (g_WHData.J15 == 1)
	{
		g_ChannelSwitch.CtrlCmd.Button.LiftUp = 1;
	}
	else
	{
		g_ChannelSwitch.CtrlCmd.Button.LiftUp = 0;
	}
	if (g_WHData.J23 == 1)
	{
		g_ChannelSwitch.CtrlCmd.Button.LiftDown = 1;
	}
	else
	{
		g_ChannelSwitch.CtrlCmd.Button.LiftDown = 0;
	}

	//自动对中按钮
	if (0)
	{
		g_ChannelSwitch.CtrlCmd.Button.ShelfAutadj = 1;
	}
	else
	{
		g_ChannelSwitch.CtrlCmd.Button.ShelfAutadj = 0;
	}
	//地标对正按钮
	if (0)
	{
		g_ChannelSwitch.CtrlCmd.Button.LanMarkAutoadj = 1;
	}
	else
	{
		g_ChannelSwitch.CtrlCmd.Button.LanMarkAutoadj = 0;
	}
	//工装对正按钮
	if (0)
	{
		g_ChannelSwitch.CtrlCmd.Button.ToolingAutoadj = 1;
	}
	else
	{
		g_ChannelSwitch.CtrlCmd.Button.ToolingAutoadj = 0;
	}

	//急停、照明、喇叭等按钮响应
	ACC_GetCmdFormWirelessRemote_Auto_Template();

	//行走挡位
	if (g_WHData.J42 < 6)
		g_ChannelSwitch.CtrlCmd.WalkGear = g_WHData.J42 + 1;

	//升降挡位
	if (g_WHData.LiftSpeedVal < 6)
		g_ChannelSwitch.CtrlCmd.LiftGear = g_WHData.LiftSpeedVal + 1;

	/*根据升降高度，在到达软件限位前限制升降档位*/
	{
		s16 Hg = g_LiftData.Height.Avg;//实时高度
		u16 Hg_H = g_Para.Lift.LimitUp;//高限位
		u16 Hg_L = g_Para.Lift.LimitDown;//低限位
		u16 Hg_D = 10;//触发档位限制的高度差

		//上升快要到了限制档位
		if (Hg_H - Hg < Hg_D && g_ChannelSwitch.CtrlCmd.Button.LiftUp)
			g_ChannelSwitch.CtrlCmd.LiftGear = AbsFuc(g_ChannelSwitch.CtrlCmd.LiftGear, 1);

		//下降快要到了限制档位
		if (Hg - Hg_L < Hg_D && g_ChannelSwitch.CtrlCmd.Button.LiftDown)
			g_ChannelSwitch.CtrlCmd.LiftGear = AbsFuc(g_ChannelSwitch.CtrlCmd.LiftGear, 1);
	}
	
	//联动车辆选择
	{
		//联动应用场景
		if(	  g_Para.ApplicationType == ACC_Application_AssemlyRemote
			|| g_Para.ApplicationType == ACC_Application_AssemlyAGV)
		{
			temp_Sel = g_WHData.J44;

			if (temp_Sel == 0)		{g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_A;}
			else if (temp_Sel == 1)	{g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_B;}
			else if (temp_Sel == 2)	{g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_Asb;}
			else					{g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_A;}
		}
		//单车应用场景，持续选择A车
		else
		{
			g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_A;
		}
	}

	/*自动自转对正工装
	ACC_AutoAdjustVz_Template();*/

	/*自动横向对正工装
	ACC_AutoAdjustVx_Template();*/

	return 1;
}

//项目扩展异常检出
bool EmgJudge_ProjectExtend_Template(void)
{
	ACC_Faults_LoseControl_PrjExp_UnionDef_Template* p2LoseControl = (ACC_Faults_LoseControl_PrjExp_UnionDef_Template*)&g_Faults.ProjectExtend.LoseControl;
	ACC_Faults_ControlledStop_PrjExp_UnionDef_Template* p2ControlledStop = (ACC_Faults_ControlledStop_PrjExp_UnionDef_Template*)&g_Faults.ProjectExtend.ControlledStop;
	ACC_Faults_Warn_PrjExp_UnionDef_Template* p2Warn = (ACC_Faults_Warn_PrjExp_UnionDef_Template*)&g_Faults.ProjectExtend.Warn;

	/*项目自定义 非受控停车异常检出*/
	if (0)
		p2LoseControl->Bit.xxx1 = 1; //xxx1故障检出
	else
		p2LoseControl->Bit.xxx1 = 0;

	/*项目自定义 受控停车异常检出*/
	if (0)
		p2ControlledStop->Bit.xxx1 = 1;
	else
		p2ControlledStop->Bit.xxx1 = 0;

	/*项目自定义 提醒*/
	if (0)
		p2Warn->Bit.xxx1 = 1; //xxx1故障检出
	else
		p2Warn->Bit.xxx1 = 0;

	if (p2LoseControl->All)
		return 1;
	else
		return 0;
}

//项目自定义界面
bool HMI_Screen_PrjExp_Template(void)
{
	//未初始化 先初始化(装在系统值)
	if (ExtHmi.PageInitialized != 1)
	{
		((Screen_Typedef_Template*)ExtHmi.p2Data)->xxx1 = g_Para.xxx1;
		ExtHmi.PageInitialized = 1;
	}
	else//已初始化 直接从界面读取
	{
		if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)//响应读取
		{
			((Screen_Typedef_Template*)ExtHmi.p2Data)->xxx1 = g_Para.xxx1;
		}
		else if (ExtHmi.p2HMI_CommObj->FucBit.Write)//响应写入
		{
			g_Para.xxx1 = ((Screen_Typedef_Template*)ExtHmi.p2Data)->xxx1;

			ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
		}
	}
	return 1;
}

//项目自定义 多倍通参数配置切换
bool PrjExp_DBSET_Template(DBCOMSET_Typedef_Template* p)
{
	//输入 hmi界面回调中直接操作了
	//内存复制方式将返回数据从接收缓冲区复制到反馈字符串缓冲区
	memcpy(&p->FeedBackStr, EQP_DBSET.p2RData, sizeof(EQP_DBSET_WObj_StDef));

	//停止命令为1时，立即强制转到停止状态 
	if (p->In_SetExit)
	{
		p->STM = DBCOMSET_STM_Stop;
		p->In_SetExit = 0;
	}

	//状态机
	switch (p->STM)
	{
		case DBCOMSET_STM_Init://初始化
		if (enable_status)
		{
			//装载发送数据
			p->SendCmdObj.Cmd[0] = 'D';
			p->SendCmdObj.Cmd[1] = 'B';
			p->SendCmdObj.Cmd[2] = 'S';
			p->SendCmdObj.Cmd[3] = 'E';
			p->SendCmdObj.Cmd[4] = 'T';
			
			//ID在conf.c ObjInit_Doublecom_546_Asb中根据EqpID初始化
			
			p->SendCmdObj.Type[0] = 'M';
			p->SendCmdObj.Type[1] = 'O';
			p->SendCmdObj.Type[2] = 'D';
			p->SendCmdObj.Type[3] = 'E';

			//根据输入参数装载目标模式
			switch (p->In_Data)
			{
				case 11:
					p->SendCmdObj.Val[0] = '1';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 21:
					p->SendCmdObj.Val[0] = '2';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 31:
					p->SendCmdObj.Val[0] = '3';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 41:
					p->SendCmdObj.Val[0] = '4';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 51:
					p->SendCmdObj.Val[0] = '5';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 61:
					p->SendCmdObj.Val[0] = '6';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 71:
					p->SendCmdObj.Val[0] = '7';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 81:
					p->SendCmdObj.Val[0] = '8';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 91:
					p->SendCmdObj.Val[0] = '9';
					p->SendCmdObj.Val[1] = '1';
					break;
				case 12:
					p->SendCmdObj.Val[0] = '1';
					p->SendCmdObj.Val[1] = '2';
					break;
				case 22:
					p->SendCmdObj.Val[0] = '2';
					p->SendCmdObj.Val[1] = '2';
					break;
				case 32:
					p->SendCmdObj.Val[0] = '3';
					p->SendCmdObj.Val[1] = '2';
					break;
				case 42:
					p->SendCmdObj.Val[0] = '4';
					p->SendCmdObj.Val[1] = '2';
					break;
				case 52:
					p->SendCmdObj.Val[0] = '5';
					p->SendCmdObj.Val[1] = '2';
					break;
				case 62:
					p->SendCmdObj.Val[0] = '6';
					p->SendCmdObj.Val[1] = '2';
					break;
				case 72:
					p->SendCmdObj.Val[0] = '7';
					p->SendCmdObj.Val[1] = '2';
					break;
				case 82:
					p->SendCmdObj.Val[0] = '8';
					p->SendCmdObj.Val[1] = '2';
					break;
				case 92:
					p->SendCmdObj.Val[0] = '9';
					p->SendCmdObj.Val[1] = '2';
					break;
				
				default:
				break;
			}

			//内存复制方式将发送数据装载到发送缓冲区
			memcpy(EQP_DBSET.p2WData, &p->SendCmdObj, sizeof(EQP_DBSET_WObj_StDef));

			p->STM = DBCOMSET_STM_SwitchProtocol;
		}
		break;

		case DBCOMSET_STM_SwitchProtocol://切换串口协议
		{
			//关闭联动通信主站 和 从站
			Para_EQPInit(Para_EQP_Assembly_Master, CCFCN_Disable, 0X0A);
			Para_EQPInit(Para_EQP_Assembly_Slave, CCFCN_Disable, 0X0A);

			//开启多倍通配置切换
			Para_EQPInit(Para_EQP_DBSET, g_Para.Asb.CN_Master, 0X01);

			/*Modbus协议栈状态机 设置成复位状态*/
			mb_manger_app.mb_step = 0X04;

			//进入等待配置状态
			p->MsRecord = g_Sys_1ms_Counter;
			p->STM = DBCOMSET_STM_Wait;
		}
		break;

		case DBCOMSET_STM_Wait://等待
		{
			if (g_Sys_1ms_Counter - p->MsRecord >= 1000)//等待1s
			{
				p->STM = DBCOMSET_STM_SendCmd;
			}
		}
		break;

		case DBCOMSET_STM_SendCmd://发送配置指令1次？
		{
			//如果有开始配置指令
			/*因为协议栈切换后自动发送 所以这里就不需要了
			if (p->In_SetStart)*/
			{
				p->In_SetStart = 0;
				p->MsRecord = g_Sys_1ms_Counter;

				//清除接收缓存
				memset(EQP_DBSET.p2RData, 0, sizeof(EQP_DBSET_WObj_StDef));

				p->STM = DBCOMSET_STM_WaitFeedBack;
			}
		}
		break;

		case DBCOMSET_STM_WaitFeedBack://等待成功返回，持续检查长度和字符串匹配，超时跳返回超时
		{
			//检查返回字符串
			if (
				//p->FeedBackStr_Len == sizeof(EQP_DBSET_WObj_StDef) &&
				  (p->FeedBackStr.Cmd[0] == 'D' 
				&& p->FeedBackStr.Cmd[1] == 'B' 
				&& p->FeedBackStr.Cmd[2] == 'Y' 
				&& p->FeedBackStr.Cmd[3] == 'S' 
				&& p->FeedBackStr.Cmd[4] == 'E')
			||
				  (p->FeedBackStr.Cmd[0] == 's' 
				&& p->FeedBackStr.Cmd[1] == 'c' 
				&& p->FeedBackStr.Cmd[2] == 'r' 
				&& p->FeedBackStr.Cmd[3] == 'i' 
				&& p->FeedBackStr.Cmd[4] == 'p')
			)
			{
				p->MsRecord = g_Sys_1ms_Counter;
				p->STM = DBCOMSET_STM_Success;
			}
			else if (g_Sys_1ms_Counter - p->MsRecord >= p->FeedBackTimeOut
				&& p->FeedBackTimeOut < 65535)//超时
			{
				p->MsRecord = g_Sys_1ms_Counter;
				p->STM = DBCOMSET_STM_Timeout;
			}
		}
		break;

		case DBCOMSET_STM_Timeout://返回超时
		{
			//反馈超时信息
			//延时500ms，以便屏幕看到多倍通配置失败
			if (g_Sys_1ms_Counter - p->MsRecord >= 500)
				p->STM = DBCOMSET_STM_Stop;
		}
		break;

		case DBCOMSET_STM_Success://返回成功，跳停止
		{
			//反馈成功信息
			//延时500ms，以便屏幕看到多倍通配置成功
			if (g_Sys_1ms_Counter - p->MsRecord >= 500)
				p->STM = DBCOMSET_STM_Stop;
		}
		break;

		case DBCOMSET_STM_Stop://停止,串口协议切换回modbus，返回初始化
		{		
			//关闭多倍通配置切换
			Para_EQPInit(Para_EQP_DBSET, CCFCN_Disable, 0X01);

			//开启联动通信主站 或 从站
			//这里需要根据联动角色恢复，或者直接手动重启系统
			if(g_Asb.Para.Role == AsbRole_Master)//联动选择主车,初始化主车联动通信
			{
				Para_EQPInit(Para_EQP_Assembly_Master, g_Para.Asb.CN_Master, 0X0A);
			}
			else if(g_Asb.Para.Role == AsbRole_Slave)//联动选择从车，初始化从车联动通信
			{
				Para_EQPInit(Para_EQP_Assembly_Slave, g_Para.Asb.CN_Slave, 0X0A);
			}
			else
			{}
				
			/*Modbus协议栈状态机 设置成复位状态*/
			mb_manger_app.mb_step = 0X04;
			//置位按钮为失能状态
			p->En = 0;
			//读写使能状态置0
			enable_status = 0;
			//返回初始化
			p->STM = DBCOMSET_STM_Init;
		}
		break;

		default:
		break;
	}

	return 1;
}

//项目自定义 AccStart函数
bool PrjExp_AccStart_Template(void)
{
	/*此处添加代码的注意事项：
	1.此项目的自定义代码的目的 是在不修改ACC核心代码的前提下，添加项目特有的功能，
	比如有的项目有一个特殊的io输入，设备才能行走等；
	2.需要主要这个函数位置和时机；
	3.尽量注意代码规范，以便如果有必要后面吸收到ACC核心代码中时，能更容易的吸收；
	*/

	return 1;
}

//项目自定义 AccEnd函数
bool PrjExp_AccEnd_Template(void)
{
	/*此处添加代码的注意事项：同上*/

	return 1;
}

//项目自定义 TskAccStart函数
bool PrjExp_TskAccStart_Template(void)
{
	/*此处添加代码的注意事项：同上*/
	
	//响应多倍通参数配置切换命令
	PrjExp_DBSET_Template(&g_DBComSet_Template);
	
	return 1;
}

//项目自定义 联动坐标变换处理函数
//放在项目配置文件实现，以便不同项目灵活处理
bool PrjExp_MR_Init_Asb_Axis_Template(void)
{
	AsbCenter_TypeDef* p2Center_In = &g_Asb.Output.Center;
	AsbCenter_TypeDef* p2Center_Out = &g_Asb.Output.Center_Cal;

	AccData.AsbCtrl.MoveDir_Reverse = 0;

	if (g_Para.Walk_Asb.Bit.FB_Axis_Forward || g_Para.Walk_Asb.Bit.FB_Axis_Moon
		|| g_Para.Walk_Asb.Bit.LR_Axis_Fixed || g_Para.Walk_Asb.Bit.LR_Axis_Moon
		)//存在特殊联动运动中心配置
	{
		//p2Center_In->X = g_Asb.Input.Place.Pos;
		//p2Center_In->Y = g_Asb.Input.Place.Pos;
		p2Center_In->X = g_Asb.P2Obj.p2Self->Dis;
		p2Center_In->Y = g_Asb.P2Obj.p2Self->Dis;
		//*p2Center_Out = *p2Center_In;
	}
	else
	{
		*p2Center_Out = *p2Center_In;
		return 1;
	}

	if (g_Asb.Para.SiteNo == AsbSiteNo_FB)//前后模式
	{
		//if (g_Asb.Para.Seat == AsbSiteSeat_1 && g_Para.Walk_Asb.Bit.FB_Axis_Moon)
		if (g_Para.Walk_Asb.Bit.FB_Axis_Moon)//选择中心变换 与 阵型位置解绑
		{
			/*坐标原点向后平移到 后车的联动运动中心上*/
			p2Center_Out->X = 0;
			//依次为：两车端面距离、自己的后半长、对方的2号模组离前端距离（假设两车对称）
			p2Center_Out->Y = (p2Center_In->Y + g_Asb.Para.HalfCar_Edge.Back + g_Asb.Para.HalfCar_Edge.Front + g_Para.Walk.Mxy[1].y);
		}
		//else if (g_Asb.Para.Seat == AsbSiteSeat_2 && g_Para.Walk_Asb.Bit.FB_Axis_Forward)//后面的车
		else if (g_Para.Walk_Asb.Bit.FB_Axis_Forward)
		{
			/*坐标原点向前平移到最前面的2号模组上*/
			p2Center_Out->X = 0;
			p2Center_Out->Y = -(g_Para.Walk.Mxy[1].y);
		}
	}
	else if (g_Asb.Para.SiteNo == AsbSiteNo_FB1)//前后模式 前车反向
	{
		if (g_Para.Walk_Asb.Bit.FB_Axis_Moon)
		{
			/*坐标原点 反方向 前平移到 另车的联动运动中心上*/
			p2Center_Out->X = abs(g_Para.Walk.Mxy[0].x);
			//依次为：两车端面距离、自己的前半长、对方的2号模组离前端距离（假设两车对称）
			p2Center_Out->Y = -(p2Center_In->Y + g_Asb.Para.HalfCar_Edge.Front + g_Asb.Para.HalfCar_Edge_Slave.Front + g_Para.Walk.Mxy[g_Asb.Para.Asb_Point_Num - 1].y);

			/*坐标原点平移的位置 如果存在特殊的，在下面进行配置
			p2Center_Out->X = abs(g_Para.Walk.Mxy[0].x);//原点x坐标移到对面
			p2Center_Out->Y = -(p2Center_In->Y + 1000);//原点y坐标前移：测距+剩下的1米，这里1米包括自身车和对方车的结构尺寸
			//注意上面的“对方车的结构尺寸”，和不同的车联动的时候，这个值是不一样的，
			//1是需要hmi界面上添加选择从车是哪个车、2是此处需要根据hmi选择的车将这个车的结构尺寸值预制到代码的这个地方
			*/	
		}
		else if (g_Para.Walk_Asb.Bit.FB_Axis_Forward)
		{
			/*坐标原点向前平移到最前面的2号模组上*/
			p2Center_Out->X = 0;
			p2Center_Out->Y = -(g_Para.Walk.Mxy[g_Asb.Para.Asb_Point_Num - 1].y);

			/*坐标原点平移的位置 如果存在特殊的，在下面进行配置
			p2Center_Out->X = 0;//x不变
			p2Center_Out->Y = -1000;//y向上平移1米
			*/
			
			//运动反向
			AccData.AsbCtrl.MoveDir_Reverse = 1;
		}
	}
	else if (g_Asb.Para.SiteNo == AsbSiteNo_LR)//左右模式
	{
		if (g_Para.Walk_Asb.Bit.LR_Axis_Fixed)
		{
			/*坐标维持不动*/
			p2Center_Out->X = 0;
			p2Center_Out->Y = 0;
		}
		else if (g_Para.Walk_Asb.Bit.LR_Axis_Moon)
		{
			/*坐标原点向左平移到 从车的联动运动中心上*/
			//依次为：两车端面距离、自己的左半长+2号模组横向里几何中心的距离、对方的2号模组离右端距离（假设两车对称）
			p2Center_Out->X = (p2Center_In->X
				+ g_Asb.Para.HalfCar_Edge.Right + g_Asb.Para.HalfCar_Edge.Left + abs(g_Para.Walk.Mxy[0].x));
			p2Center_Out->Y = 0;
		}
	}

	return 1;
}

//***********************************END OF FILE***********************************

