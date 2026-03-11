/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: app_Assembly.c
** 创 建 人: 文小兵
** 描    述: 联动控制模块
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.1.0
** 创建日期: 2024年11月5日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define  Assembly_GLOBALS

//包含对象头文件
#include "app_Assembly.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "basecalfunc.h"

//#include "basecalfunc.h"
//#include "CBB_Init.h"

//外部函数声明
//避免通用编辑器无法识别虚函数报错
extern bool AsbSwitchEnterInit(void);

/*单动联动切换时调用此函数切换底盘轮系坐标
//弱函数定义，用户可以在自己的代码中实现此函数
bool __attribute__((weak)) AsbSwitchEnterInit(void)
{
	return 1;
}*/

//校验异常检出
bool Asb_CheckErrJdg(AsbCtrl_TypeDef* p)
{
    AsbErr_TypeDef* pErr = &p->P2Obj.p2Self->AsbErr;

    if (p->Collect.Check == 0xFFFF && p->Para.Bit.ForceOverCheck == 0)//汇总数据校验码为0xFFFF时，表示异常
        pErr->CheckErr = 1;
    else
        pErr->CheckErr = 0;

    return pErr->CheckErr;
}

//联动故障判断
bool Asb_ErrJdg(AsbCtrl_TypeDef* p)
{
    AsbErr_TypeDef* pErr = &p->P2Obj.p2Self->AsbErr;
	AsbSwitch_TypeDef* p2AsbSwitch = &((p->P2Obj.p2Self)->AsbSwitch);

    //激光+测距时
    f32 Distance_FL = p->Input.Sensor.Distance_FL;
    f32 Distance_BR = p->Input.Sensor.Distance_BR;
    //f32 Distance_Avg = (Distance_FL + Distance_BR) / 2;	    //平均距离
    f32 LaserSpot = p->Input.Sensor.LaserSpot;				//光斑偏移

	f32 temp = 0;

	if(p2AsbSwitch->Cmd == AsbAsbMode_Asb)//联动参数设置且互传收到后 不能进联动后，因为要在进联动时检查某些状态
	{
        //-----------------------------所有车都要检出-----------------------------
        //联动开关异常检出
		pErr->CarPanelSelErr = (p->Input.AsbSel_Panel == 0);
        //有线不能联动
        pErr->CantWired = (p->Input.Wired);
        //-----------------------------A车检出-----------------------------
        if(p->Para.Role == AsbRole_Master)
        {
            //高差异常检出 汇总信息中最高高度-最低高度>最大高度差
            temp = p->Collect.HgMax - p->Collect.HgMin;
            if(fabs(temp) > p->Para.HgDiffMax && p->Para.HgDiffMax<65535)
                pErr->HgDiffOverlimit = 1;
            else
                pErr->HgDiffOverlimit = 0;

            /*进入联动时
            //左右测距差值异常检出
			temp = p->Input.Sensor.Distance_FL - p->Input.Sensor.Distance_BR;
            if(fabs(temp) > p->Para.SensorAlarm.DiffLR_EnterMax && p->Para.SensorAlarm.DiffLR_EnterMax<65535)
                pErr->D_LandR_Overlimit_Enter = 1;
            else
                pErr->D_LandR_Overlimit_Enter = 0;

            //光斑传感器偏移过大
            if(fabs(LaserSpot) > p->Para.SensorAlarm.LaserSpot_EnterMax && p->Para.SensorAlarm.LaserSpot_EnterMax<65535)
                pErr->LandscapeOverlimit_Enter = 1;
            else
                pErr->LandscapeOverlimit_Enter = 0;

            //联动距离过大
            if(Distance_Avg > p->Para.SensorAlarm.DisEnterMax && p->Para.SensorAlarm.DisEnterMax<65535)
                pErr->LaserRangingToofar_Enter = 1;
            else
                pErr->LaserRangingToofar_Enter = 0;

            //联动距离过小
            if(Distance_Avg < p->Para.SensorAlarm.DisEnterMin && p->Para.SensorAlarm.DisEnterMin!=0)
                pErr->LaserRangingToonear_Enter = 1;
            else
                pErr->LaserRangingToonear_Enter = 0;*/

            //联动过程中
            //左右测距差值异常检出
            temp = Distance_FL - Distance_BR;
            if(fabs(temp) > p->Para.SensorAlarm.DiffLR_Max && p->Para.SensorAlarm.DiffLR_Max<65535)
                pErr->D_LandR_OverlimitMax = 1;
            else
                pErr->D_LandR_OverlimitMax = 0;

            //光斑传感器偏移过大
            if(fabs(LaserSpot) > p->Para.SensorAlarm.LaserSpotMax && p->Para.SensorAlarm.LaserSpotMax<65535)
                pErr->LandscapeLaserOverlimit = 1;
            else
                pErr->LandscapeLaserOverlimit = 0;

            //联动距离 变化过大
            temp = p2AsbSwitch->Dis - p2AsbSwitch->Dis_Enter;
            if(fabs(temp) > p->Para.SensorAlarm.DisChangeMax && p->Para.SensorAlarm.DisChangeMax<65535)
                pErr->LaserRangingChange = 1;
            else
                pErr->LaserRangingChange = 0;

            //联动动作同步异常检出
            if(p->Collect.Ms_MoveTogetherTimeout > p->Para.SynTimeout && p->Para.SynTimeout<65535)
                pErr->MoveSynErr = 1;
            else
            {
                //pErr->MoveSynErr = 0;
            }

            /*参数校验错误检查*/
            if(p2AsbSwitch->STM == AsbAsbMode_Asb)
				Asb_CheckErrJdg(p);
			else {}
        }
        //-----------------------------B车检出-----------------------------
        else if(p->Para.Role == AsbRole_Slave)
        {

        }
        else {}
	}
	else//非联动状态下
	{
        //直接通过 内存清零 整体对位域进行赋值清零
        memset(pErr,0,sizeof(AsbErr_TypeDef));
	}
	return 1;
}


//联动条件满足判断 只检查传感器，联动开关等在联动异常判断中检出！主车的！
bool Asb_EnterJdg(AsbCtrl_TypeDef* p)
{
	char res=0;
	f32 temp = 0;

    //激光+测距时
    f32 Distance_FL = p->Input.Sensor.Distance_FL;
    f32 Distance_BR = p->Input.Sensor.Distance_BR;
    f32 Distance_Avg = (Distance_FL + Distance_BR) / 2;	    //平均距离
    f32 LaserSpot = p->Input.Sensor.LaserSpot;				//光斑偏移

    AsbErr_TypeDef* pErr = &p->P2Obj.p2Self->AsbErr;

	if(p->Para.Role == AsbRole_Master)//A车要判断 传感器是否满足联动条件
	{
        //左右测距差值小于设定值
        temp = fabs(Distance_FL - Distance_BR);
        if(temp > p->Para.SensorAlarm.DiffLR_EnterMax && p->Para.SensorAlarm.DiffLR_EnterMax<65535)
            pErr->D_LandR_Overlimit_Enter = 1;
        else
            pErr->D_LandR_Overlimit_Enter = 0;

        //光斑偏移过大
        if(fabs(LaserSpot) > p->Para.SensorAlarm.LaserSpot_EnterMax && p->Para.SensorAlarm.LaserSpot_EnterMax<65535)
            pErr->LandscapeOverlimit_Enter = 1;
        else
            pErr->LandscapeOverlimit_Enter = 0;

        //测距太近
        if(Distance_Avg < p->Para.SensorAlarm.DisEnterMin && p->Para.SensorAlarm.DisEnterMin < 65535)
            pErr->LaserRangingToonear_Enter = 1;
        else
            pErr->LaserRangingToonear_Enter = 0;

        //测距太远
        if(Distance_Avg > p->Para.SensorAlarm.DisEnterMax && p->Para.SensorAlarm.DisEnterMax<65535)
            pErr->LaserRangingToofar_Enter = 1;
        else
            pErr->LaserRangingToofar_Enter  =0;

        //以上报警标志都为0时，进入联动状态
        if(pErr->D_LandR_Overlimit_Enter == 0
            && pErr->LandscapeOverlimit_Enter == 0
            && pErr->LaserRangingToonear_Enter == 0
            && pErr->LaserRangingToofar_Enter == 0
            //&& pErr->CheckErr == 0
        )
            res = 1;
        else
            res = 0;
	}
	else//从车不判断
	{
		res = 1;
	}
	return res;
}

/*clr enter unit mode err
char unit_enter_err_clr()
{
	char res=0;
	if(p->iam==AsbModel_A)
	{
		p->P2Obj.p2Self->uniterr.D_LandR_overlimit_in=0;
		p->P2Obj.p2Self->uniterr.landscape_overlimit_in=0;
	}
	else
		res=0;
}*/

//静止操作
bool Asb_KeepStill(AsbCtrl_TypeDef* p)
{
	p->P2Obj.p2Self->CmdOut.Mode = AsbMode_Still;
	p->P2Obj.p2Self->CmdOut.Vxyz.x = 0;
	p->P2Obj.p2Self->CmdOut.Vxyz.y = 0;
	p->P2Obj.p2Self->CmdOut.Vxyz.z = 0;
	p->P2Obj.p2Self->CmdOut.Bit.LiftUp = 0;
	p->P2Obj.p2Self->CmdOut.Bit.LiftDown = 0;

	return 1;
}

//模式跟随，但清除动作命令
bool Asb_CmdClear(AsbCtrl_TypeDef* p)
{
	p->P2Obj.p2Self->CmdOut.Mode = p->P2Obj.p2Self->CmdIn.Mode;
	p->P2Obj.p2Self->CmdOut.Vxyz.x = 0;
	p->P2Obj.p2Self->CmdOut.Vxyz.y = 0;
	p->P2Obj.p2Self->CmdOut.Vxyz.z = 0;
	p->P2Obj.p2Self->CmdOut.Bit.LiftUp = 0;
	p->P2Obj.p2Self->CmdOut.Bit.LiftDown = 0;

	return 1;
}


//单联动状态机
void Asb_STM(AsbCtrl_TypeDef* p)
{
	AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;
	AsbSwitch_TypeDef* p2AsbSwitch = &(p2Car->AsbSwitch);

    p2AsbSwitch->Dis = p2Car->Dis;	                        //实时测距值

    /*联动状态命令获取*/
	if (1)//p->Input.AsbSel_Panel)//此处不判断联动开关是否选联动，以便以遥控器命令为准检出联动开关异常
	{
		if(p->Input.AsbSel_Cmd == AsbHcmMode_Asb)           //遥控器选联动
			p2AsbSwitch->Cmd = AsbAsbMode_Asb;
		else if(
			(p->Input.AsbSel_Cmd == AsbHcmMode_StrNo1 && p->Para.Iam == AsbCarStrNo_1)||
			(p->Input.AsbSel_Cmd == AsbHcmMode_StrNo2 && p->Para.Iam == AsbCarStrNo_2)
		)//选自己单动
			p2AsbSwitch->Cmd = AsbAsbMode_Single;
		else//静止
			p2AsbSwitch->Cmd = AsbAsbMode_OtherCar;

        //遥控器单联动切换时，复位联动相关报警
        if (p2Car->AsbSel_Last != p->Input.AsbSel_Cmd)
        {
			//此处多余，在报警检出中，已有非联动状态下的报警清除
            //p->P2Obj.p2Self->AsbErr.AsbEnterErr = 0;
            p2Car->AsbSel_Last = p->Input.AsbSel_Cmd;
        }
        else
        {

        }
	}
	else//联动开关未选联动时
		p2AsbSwitch->Cmd = AsbAsbMode_Single;				//单动

	switch (p2AsbSwitch->STM)
	{
		case AsbAsbMode_Nnull:
		{
			if(p2AsbSwitch->Cmd == AsbAsbMode_Asb)//联动
			{
				if(Asb_EnterJdg(p) == 1 || p->Para.Bit.ForceAsb_OverJdg)
				{
					p2AsbSwitch->STM = AsbAsbMode_AsbReadyOne;

                    /*切换底盘轮系坐标，先切参数，以便和Cmd一起发到主车，进行参数校验*/
                    AsbSwitchEnterInit();

                    /*先锁定联动距离 以便通过联动通信统一联动距离 但是此后切换模式 车可能动 先忽略*/
                    p2AsbSwitch->Dis_Enter = p2AsbSwitch->Dis;
                    p->MsRecord.AsbSTM_Delay = p->Input.Ms;
				}
				else
                {
                    //条件不满足 报警提示 此处多余 在Asb_EnterJdg中已有单独的报警置位
					//p->P2Obj.p2Self->AsbErr.AsbEnterErr = 1;
                }
			}
			else//非联动 直接进入
			{
				p2AsbSwitch->STM=p2AsbSwitch->Cmd;
                AsbSwitchEnterInit();                       //切换底盘轮系坐标
			}
		}
		break;

		case AsbAsbMode_OtherCar://联动遥控器未选择自己
		{
			if(p2AsbSwitch->STM != p2AsbSwitch->Cmd)
				p2AsbSwitch->STM = AsbAsbMode_Nnull;
            else {}
		}
		break;

        case AsbAsbMode_Single:
		{
			if(p2AsbSwitch->STM != p2AsbSwitch->Cmd)
				p2AsbSwitch->STM = AsbAsbMode_Nnull;
            else {}
		}
		break;

		case AsbAsbMode_AsbReadyOne:
		{
			if(p2AsbSwitch->Cmd != AsbAsbMode_Asb)
				p2AsbSwitch->STM = AsbAsbMode_Nnull;
            else {}

			if(p->Collect.AsbSTM == AsbAsbMode_AsbReadyOne || p->Para.Bit.ForceAsb_OverOtherCar)
			{
                //延时一下
                if (p->Input.Ms - p->MsRecord.AsbSTM_Delay > p->Para.AsbSTM_Delay)
                {
                    if (1)//Asb_CheckErrJdg(p) == 0)//进联动后检查
                        //if (Asb_CheckErrJdg(p) == 0)
                    {
                        /*通过Collect.AsbSTM == AsbAsbMode_AsbReadyOne 确认所有车都收到了主车广播的联动距离后 再次初始化底盘*/
                        AsbSwitchEnterInit();

                        p2AsbSwitch->STM = AsbAsbMode_Asb;
                    }
                    else {}//报警
                }
                else
                    __asm__("nop");
            }
			else {}
		}
		break;

		case AsbAsbMode_Asb:
		{
            if (p2AsbSwitch->STM != p2AsbSwitch->Cmd)
            {
                p2AsbSwitch->STM = AsbAsbMode_Nnull;
				p2Car->ModeSwitch.STM = AsbModeChg_Init; //复位模式切换同步状态机
            }
			else {}
		}
		break;

		default:
		break;
	}
}

//联动 模式切换过程 状态机
AsbModeChg_TypeDef Asb_ModeChgSyn(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;
    AsbSwitch_TypeDef* p2AsbSwitch = &p2Car->AsbSwitch;

    //在AbsSTM中已经处理
    /*仅联动状态机处于联动状态时 才进行模式切换同步*/
    if (p2AsbSwitch->STM != AsbAsbMode_Asb)
    {
        p2Car->ModeSwitch.STM = AsbModeChg_Init;
        return p2Car->ModeSwitch.STM;
    }
    else {}

    switch (p2Car->ModeSwitch.STM)
    {
    case AsbModeChg_Init:
    {
        //自己模式非空时进入单车就绪状态
        if (p2Car->ModeReal != AsbMode_Null)
            p2Car->ModeSwitch.STM = AsbModeChg_OneReady;
        else {}
    }
    break;

    case AsbModeChg_OneReady:
    {
        //自己模式为空 退回初始化状态
        if (p2Car->ModeReal == AsbMode_Null)
        {
            p2Car->ModeSwitch.STM = AsbModeChg_Exit;
        }
        else if (p2Car->ModeReal == p->Collect.ModeReal || p->Para.Bit.ForceModeEnter)
        {
            //两车模式都非空 且模式相同时 进入运动就绪状态
            p2Car->ModeSwitch.STM = AsbModeChg_TwoReady;
        }
        else {}
    }
    break;

    case AsbModeChg_TwoReady:
    {
        //只要有一个车模式空 退回初始化状态
        //if (p->Collect.Mode == AsbMode_Null || p2Car->CmdIn.Mode == AsbMode_Null)
        if (p->Collect.ModeReal == AsbMode_Null && p->Para.Bit.ForceModeEnter == 0)
            p2Car->ModeSwitch.STM = AsbModeChg_Exit;
        else {}
    }
    break;

    case AsbModeChg_Exit:
    {
        //退出状态 复位
        p2Car->ModeSwitch.STM = AsbModeChg_Init;
    }
    break;

    default:
        break;
    }

    return (p2Car->ModeSwitch.STM);
}

//控制命令处理
bool Asb_CmdParse(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;

	//先装载原始命令
	p2Car->CmdOut = p->Input.OrgCmd;

	//非联动、也非选择自己单动 保持静止
	if (p2Car->AsbSwitch.STM == AsbAsbMode_Nnull || p2Car->AsbSwitch.STM == AsbAsbMode_OtherCar)
	{
		Asb_KeepStill(p);
	}
	else if (p2Car->AsbSwitch.STM == AsbAsbMode_Single)//单动 输出原始命令
	{
		p2Car->CmdOut = p2Car->CmdIn;
	}
    else if (p2Car->AsbSwitch.STM == AsbAsbMode_AsbReadyOne)//联动 只自己模式切换好 只清除摇杆、升降动作命令
    {
   	 	Asb_CmdClear(p);
    }
	else if (p2Car->AsbSwitch.STM == AsbAsbMode_Asb)//联动
    {
    	//模式状态机同步一致后时能动作命令
    	if(p2Car->ModeSwitch.STM == AsbModeChg_TwoReady)
    	{
            if(p->Para.Bit.ForceUseOwnCmd == 0)
            {
			    p2Car->CmdOut.Mode = p->Collect.Mode;
			    //将汇总后的最小档位输出
			    p2Car->CmdOut.WalkGear = p->Collect.WalkGearMin;
			    p2Car->CmdOut.LiftGear = p->Collect.LiftGearMin;
            }
            else
            {
                p2Car->CmdOut.Mode = p2Car->CmdIn.Mode;
                p2Car->CmdOut.WalkGear = p2Car->CmdIn.WalkGear;
                p2Car->CmdOut.LiftGear = p2Car->CmdIn.LiftGear;
            }
		}
		else
		{
			Asb_CmdClear(p);
		}

    }
    else
    {
        //单车模式下 直接使用原始命令
        p2Car->CmdOut = p->Input.OrgCmd;
	}

    //如果有联动报警 模式强制静止
    if(*((u32*)&(p2Car->AsbErr)))
    {
		p2Car->CmdOut.Mode = AsbMode_Null;
    }
    else
    {
		//do nothing
	}

	return 1;
}

//联动复合运动限制
bool Asb_CmdLimit(AsbCtrl_TypeDef* p)
{
	if (p->Input.AsbSel_Cmd == AsbAsbMode_Asb)
	{
		//if (ACC_IsPosWalkMode(p->Input.FinaleCmd.Mode))
		if(0)
		{
			//if (p->Para.OffVx)

		}
		//else if (ACC_IsInfWalkMode(p->Input.FinaleCmd.Mode))
		if(0)
		{
			/*if (p->Para.OffVx)
				p->CtrlCmdOut.Vxyz.y = 0;
			else{}*/
		}
		else {}

		/*if (p->Para.OffVz)
			p->CtrlCmdOut.Vxyz.z = 0;
		else {};*/
	}
	return 1;
}

//故障输入恢复延迟
bool Asb_Input2Cars_ErrUpdate(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;

    if (p->Input.OrgErr.Faults >= p2Car->Err.Faults)//输入故障级别更严重时 直接更新
    {
        p2Car->Err.Faults = p->Input.OrgErr.Faults;
        p->MsRecord.ErrRecover = p->Input.Ms;
    }
    else
    {
        if (p2Car->Ms - p->MsRecord.ErrRecover > p->Para.ErrRecover)
        {
            p2Car->Err.Faults = p->Input.OrgErr.Faults;
            p->MsRecord.ErrRecover = p2Car->Ms;
        }
        else {}
    }

    p2Car->Err.Bit = p->Input.OrgErr.Bit;

    return 1;
}

//每个车都将输入内容整理到对象对象中
bool Asb_Input2Cars(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;
	p2Car->Ms = p->Input.Ms;
	//联动距离，主车根据联动方向从自己的传感器数据中获取
	//从车从联动通信的联动数据中获取
    if (p->Para.Role == AsbRole_Master)
    {
        p2Car->Dis = p->Input.Place.Pos;
		/*计算联动距离的一半 用联动进入时锁定的值 确保和从车一致*/
        p2Car->Dis_Half = p2Car->AsbSwitch.Dis_Enter / 2;

        /*用实时值
		p2Car->Dis_Half = p2Car->Dis / 2;*/

        //传递到主车的广播数据中
        p->Input.CustomM.SiteNo = p->Para.SiteNo;
    }
    else
    {
        p2Car->Dis = p->Collect.Dis;                        //从车从汇总数据中获取联动距离
        p2Car->Dis_Half = p2Car->Dis / 2;                   //从车从汇总数据中获取联动距离的一半 已经是锁定的值
        
        /*从车从主车的广播数据中获取联动阵型*/
        p->Para.SiteNo = p->Comm.MasterData.CustomM.SiteNo;

    }//从车在 Asb_CommData2Cars(p) 中获取联动距离

    //计算运动中心偏移坐标
    //根据联动方向获取
    switch (p->Para.SiteNo)
    {
        case AsbSiteNo_FB:                              //前后联动
        {
            if (p->Para.Seat == AsbSiteSeat_1)
            {
                p2Car->Center.X = 0;
				p2Car->Center.Y = (p2Car->Dis_Half + p->Para.HalfCar.Back);
            }
            else if (p->Para.Seat == AsbSiteSeat_2)
            {
				p2Car->Center.X = 0;
                p2Car->Center.Y = -(p2Car->Dis_Half + p->Para.HalfCar.Back);
            }
            else {}
        }
        break;

        case AsbSiteNo_FB1:                              //前后联动 车头对车头
        {
            if (p->Para.Seat == AsbSiteSeat_1)
            {
                p2Car->Center.X = 0;
                p2Car->Center.Y = (p2Car->Dis_Half + p->Para.HalfCar.Back);
            }
            else if (p->Para.Seat == AsbSiteSeat_2)
            {
                p2Car->Center.X = 0;
                p2Car->Center.Y = (p2Car->Dis_Half + p->Para.HalfCar.Back);
            }
            else {}
        }
        break;
        default:
        break;
    }

    //联动中标志处理
    if (p->Para.Seat == AsbSiteSeat_NULL)
        p2Car->InAsb = 0;
    else
		p2Car->InAsb = 1;

	p2Car->WheelRMax = p->Input.WheelRMax;
	p2Car->WheelVMax = p->Input.WheelVMax;

    //联动命令
    p2Car->CmdIn = p->Input.OrgCmd;
	p2Car->ModeReal = p->Input.ModeReal;

    //状态
	p2Car->ModeReal = p->Input.ModeReal;
	p2Car->Hg = p->Input.Hg;
	p2Car->Obs = p->Input.Obs;

	//p2Car->Err = p->Input.OrgErr;
    Asb_Input2Cars_ErrUpdate(p);

	//状态位
	p2Car->Bit = p->Input.Bit;

    //巡线传感器偏差
    p2Car->Track = p->Input.Track;

	//参数校验
    //计算CRC校验
    p->Input.CheckResult = CRCCAL(0xFFFF, (u8*)&(p->Input.CheckData), sizeof(AsbLocalParaCheck_TypeDef));
	p2Car->Check = p->Input.CheckResult;

    //计算
    p->Input.Place.PosDiff = p2Car->AsbSwitch.Dis - p2Car->AsbSwitch.Dis_Enter;

    return 1;
}

//从与各从车的通信对象中解析数据
bool Asb_CommData2Cars(AsbCtrl_TypeDef* p)
{
	char i=0;

	for(i=0;i<ASB_CAR_MAX_NUM;i++)
	{
        //主车根据配置设置各车是否参与联动
        if (p->Para.Seats[i] == AsbSiteSeat_NULL)
        {
            p->Cars[i].InAsb = 0;
        }
        else
        {
            p->Cars[i].InAsb = 1;
        }

        //如果不参与联动或角色为主机，跳过当前循环
        if (p->Cars[i].InAsb == 0 || i == p->Para.Iam - 1)
            continue;
        else {}

        p->Cars[i].Ms = p->Comm.SlaveData[i].Ms;
        p->Cars[i].Dis = p->Comm.SlaveData[i].Dis;
        p->Cars[i].WheelRMax = p->Comm.SlaveData[i].WheelRMax;
        p->Cars[i].WheelVMax = p->Comm.SlaveData[i].WheelVMax;

        p->Cars[i].CmdIn.Mode = p->Comm.SlaveData[i].Mode;
        p->Cars[i].CmdIn.WalkGear = p->Comm.SlaveData[i].Bit.WalkGear;
        p->Cars[i].CmdIn.LiftGear = p->Comm.SlaveData[i].Bit.LiftGear;
        p->Cars[i].CmdIn.Bit.LiftUp = p->Comm.SlaveData[i].Bit.LiftUp;//升起
        p->Cars[i].CmdIn.Bit.LiftDown = p->Comm.SlaveData[i].Bit.LiftDown;//下降
        p->Cars[i].ModeReal = p->Comm.SlaveData[i].ModeReal;

        p->Cars[i].Hg = p->Comm.SlaveData[i].Hg;
        p->Cars[i].Obs = p->Comm.SlaveData[i].Obs;
        p->Cars[i].Err.Faults = p->Comm.SlaveData[i].Err;
        p->Cars[i].Err.Bit.UpLimited = p->Comm.SlaveData[i].Bit.ErrUpLimited;      //限制上升
        p->Cars[i].Err.Bit.DownLimited = p->Comm.SlaveData[i].Bit.ErrDownLimited;  //限制下降
        p->Cars[i].Err.Bit.MoveLimited = p->Comm.SlaveData[i].Bit.ErrMoveLimited;  //限制移动
        p->Cars[i].Bit.Moving = p->Comm.SlaveData[i].Bit.Moving;                   //是否在运动中
        p->Cars[i].Bit.UpLimited = p->Comm.SlaveData[i].Bit.UpLimited;             //上限位触发
        p->Cars[i].Bit.DownLimited = p->Comm.SlaveData[i].Bit.DownLimited;         //下限位触发

        p->Cars[i].Check = p->Comm.SlaveData[i].Check;

		p->Cars[i].AsbSwitch.STM = p->Comm.SlaveData[i].AsbSTM;
		p->Cars[i].ModeSwitch.STM = p->Comm.SlaveData[i].ModeSTM;

        //巡线传感器偏差
        p->Cars[i].Track = p->Comm.SlaveData[i].Track;
    }

    return 1;
}

/*主车汇总所有联动车避障 两车方向一顺时*/
bool Asb_Collect_Obs(AsbCtrl_TypeDef* p)
{
	char i=0;
	bool BarF=0, BarB=0, BarL=0, BarR=0;
	AsbLaser_TypeDef LaserF=AsbObs_Nomal, LaserB=AsbObs_Nomal, LaserL=AsbObs_Nomal, LaserR=AsbObs_Nomal;

	//Bar避障处理
	for(i=0;i<ASB_CAR_MAX_NUM;i++)
	{
        //如果不在联动状态 则不参与避障判断 跳过当前循环
        if (p->Cars[i].InAsb == 0)
            continue;
        else {}

        //触边
        if(p->Cars[i].Obs.FBar)
            BarF = 1;
        else {}

        if(p->Cars[i].Obs.BBar)
            BarB = 1;
        else {}

        if(p->Cars[i].Obs.LBar)
            BarL = 1;
        else {}

        if(p->Cars[i].Obs.RBar)
            BarR = 1;
        else {}

        //激光
        if(p->Cars[i].Obs.FLaser > LaserF)
            LaserF = p->Cars[i].Obs.FLaser;
        else {}

        if(p->Cars[i].Obs.BLaser > LaserB)
            LaserB = p->Cars[i].Obs.BLaser;
        else {}

        if(p->Cars[i].Obs.LLaser > LaserL)
            LaserL = p->Cars[i].Obs.LLaser;
        else {}

        if(p->Cars[i].Obs.RLaser > LaserR)
            LaserR = p->Cars[i].Obs.RLaser;
        else {}
	}

	//合并结果
	p->Collect.Obs.BBar = BarF;
	p->Collect.Obs.FBar = BarB;
	p->Collect.Obs.LBar = BarL;
	p->Collect.Obs.RBar = BarR;

	p->Collect.Obs.FLaser = LaserF;
	p->Collect.Obs.BLaser = LaserB;
	p->Collect.Obs.LLaser = LaserL;
	p->Collect.Obs.RLaser = LaserR;

	return 1;
}

/*主车汇总所有联动车 巡线传感器偏差*/
bool Asb_Collect_Track(AsbCtrl_TypeDef* p)
{
	u8 i = 0;
    s16 TrackTempL = 0, TrackTempR = 0;

    for(i=0;i<ASB_CAR_MAX_NUM;i++)
	{
        //如果不在联动状态 则不参与判断 跳过当前循环
        if (p->Cars[i].InAsb == 0)
            continue;
        else {}

        if(p->Para.SiteNo == AsbSiteNo_FB)//前后一顺联动
        {
            if(p->Para.Seats[i] == AsbSiteSeat_1)//前车
            {
                p->Collect.Track.Front = p->Cars[i].Track.Front;
            }
            else if(p->Para.Seats[i] == AsbSiteSeat_2)//后车
            {
                p->Collect.Track.Back = p->Cars[i].Track.Back;
            }
            else {}

            //横向的偏差 = 求和取平均
            TrackTempL = TrackTempL + p->Cars[i].Track.Left;
            TrackTempR = TrackTempR + p->Cars[i].Track.Right;
        }
        else {}//其它联动方式待完善
    }

    //横向的偏差 = 求和取平均
    p->Collect.Track.Left = TrackTempL / 2;
    p->Collect.Track.Right = TrackTempR / 2;                //临时除以2

    return 1;
}

/*主车直接广播自己的偏差*/
bool Asb_Collect_Track_Org(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;

    p->Collect.Track.Front = p2Car->Track.Front;
    p->Collect.Track.Back = p2Car->Track.Back;
    p->Collect.Track.Left = p2Car->Track.Left;
    p->Collect.Track.Right = p2Car->Track.Right;

    return 1;
}

//主车汇总其它车的状态
bool Asb_Collect(AsbCtrl_TypeDef* p)
{
    char i=0;
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;
    u32 WheelRMax = p->Cars[0].WheelRMax;
    u32 WheelVMax = p->Cars[0].WheelVMax;
    u16 HgMin = p->Cars[0].Hg;
    u16 HgMax = p->Cars[0].Hg;
    Asb_FaultsType_TypeDef ErrMax = p->Cars[0].Err.Faults;
    bool HaveUpLimited = p->Cars[0].Err.Bit.UpLimited;      //至少有一个限制上升触发
    bool HaveDownLimited = p->Cars[0].Err.Bit.DownLimited;  //至少有一个限制下降触发
    bool HaveMoveLimited = p->Cars[0].Err.Bit.MoveLimited;      //至少有一个限制移动触发
    AsbMode_TypeDef ModeReal = p->Cars[0].ModeReal;
    //bit
    bool Up = p->Cars[0].CmdIn.Bit.LiftUp;
    bool Down = p->Cars[0].CmdIn.Bit.LiftDown;
    bool HaveMoving = p->Cars[0].Bit.Moving;                //至少有一个车在运动中
    bool MoveTogether = 1;                                  //所有车都在运动中
    bool UpLimited = p->Cars[0].Bit.UpLimited;              //至少有一个车上限位触发
    bool DownLimited = p->Cars[0].Bit.DownLimited;          //至少有一个车下限位触发

    //控制
    char WalkGearMin = p->Cars[0].CmdIn.WalkGear;
    char LiftGearMin = p->Cars[0].CmdIn.LiftGear;

    AsbAsbMode_TypeDef AsbSTM = p->Cars[0].AsbSwitch.STM;   //单联动控制状态机

	AsbModeChg_TypeDef ModeSTM = p->Cars[0].ModeSwitch.STM; //模式切换状态机
	AsbMode_TypeDef Mode = p->Cars[0].CmdIn.Mode;			//模式 都是同一个模式 或 为无效

	if(AsbSTM == AsbAsbMode_Asb)
		AsbSTM = AsbAsbMode_AsbReadyOne;//已进入联动的 也是自己就绪
	else {}

    //循环处理
    for(i=1;i<ASB_CAR_MAX_NUM;i++)//当1号车不在联动中时存在bug！
	{
        //如果不在联动状态 则不参与避障判断 跳过当前循环
        if (p->Cars[i].InAsb == 0)
            continue;
        else {}

        //WheelRMax
        if(p->Cars[i].WheelRMax > WheelRMax)
            WheelRMax = p->Cars[i].WheelRMax;
        else {}

        //WheelVMax
        if(p->Cars[i].WheelVMax > WheelVMax)
            WheelVMax = p->Cars[i].WheelVMax;
        else {}

        //HgMin
        if(p->Cars[i].Hg < HgMin)
            HgMin = p->Cars[i].Hg;
        else {}

        //HgMax
        if(p->Cars[i].Hg > HgMax)
            HgMax = p->Cars[i].Hg;
        else {}

        //ErrMax
        if(p->Cars[i].Err.Faults > ErrMax)
            ErrMax = p->Cars[i].Err.Faults;
        else {}

        //HaveUpLimited
        if(p->Cars[i].Err.Bit.UpLimited)
            HaveUpLimited = 1;
        else {}

        //HaveDownLimited
        if(p->Cars[i].Err.Bit.DownLimited)
            HaveDownLimited = 1;
        else {}

        //ModeReal
        if(ModeReal != p->Cars[i].ModeReal)
            ModeReal = AsbMode_Null;
        else {}

        //Bit
        //Up
        if(p->Cars[i].CmdIn.Bit.LiftUp)
            Up = 1;
        else {}

        //Down
        if(p->Cars[i].CmdIn.Bit.LiftDown)
            Down = 1;
        else {}

        //Moving
        if(p->Cars[i].Bit.Moving)
            HaveMoving = 1;
        else {}

        //MoveTogether
        if(p->Cars[i].Bit.Moving != p->Cars[0].Bit.Moving)//全和1号车比看是否相同，注意1号车必须参与联动
            MoveTogether = 0;
        else {}

        //UpLimited
        if(p->Cars[i].Bit.UpLimited)
            UpLimited = 1;
        else {}

        //DownLimited
        if(p->Cars[i].Bit.DownLimited)
            DownLimited = 1;
        else {}

        //控制
        //WalkGearMin
        if(p->Cars[i].CmdIn.WalkGear < WalkGearMin)
            WalkGearMin = p->Cars[i].CmdIn.WalkGear;
        else {}

        //LiftGearMin
        if(p->Cars[i].CmdIn.LiftGear < LiftGearMin)
            LiftGearMin = p->Cars[i].CmdIn.LiftGear;
        else {}

        //AsbSTM
        if(p->Cars[i].AsbSwitch.STM != AsbAsbMode_AsbReadyOne && p->Cars[i].AsbSwitch.STM != AsbAsbMode_Asb)
            AsbSTM = AsbAsbMode_Nnull;  //如果有一个车的状态机不是自己就绪或联动状态 则结果无效
        else {}

        //ModeSTM
        if(p->Cars[i].ModeSwitch.STM != ModeSTM)
            ModeSTM = AsbModeChg_Init;  //如果有一个车的状态机不一致 则结果无效
        else {}

        //Mode
        if (Mode != p->Cars[i].CmdIn.Mode)
            Mode = AsbMode_Null;  //如果有一个车的模式不一致 则结果无效
        else {}

        //校验
        if (p->Cars[i].Check != p->Cars[0].Check)
        {
            p->Collect.Check = 0xffff; //校验不一致
        }
        else
        {
            p->Collect.Check = p->Cars[0].Check;
        }
    }

    //汇总数据
    p->Collect.Ms = p->Input.Ms;                            //主车的Ms
    p->Collect.Dis = p2Car->AsbSwitch.Dis_Enter;            //主车的联动距离
    p->Collect.WheelRMax = WheelRMax;
    p->Collect.WheelVMax = WheelVMax;
    p->Collect.HgMin = HgMin;
    p->Collect.HgMax = HgMax;
    Asb_Collect_Obs(p);

     //汇总各车的巡线传感器偏差
    if(p->Para.FuncSwitch.TrackOrg == 0)
        Asb_Collect_Track(p);
    else
        Asb_Collect_Track_Org(p);

    p->Collect.ErrMax.Faults = ErrMax;
    p->Collect.ErrMax.Bit.UpLimited = HaveUpLimited;        //至少有一个限制上升触发
    p->Collect.ErrMax.Bit.DownLimited = HaveDownLimited;    //至少有一个限制下降触发
    p->Collect.ErrMax.Bit.MoveLimited = HaveMoveLimited;    //至少有一个限制移动触发
    p->Collect.ModeReal = ModeReal;                         //主车的模式
    //bit
    p->Collect.Bit.LiftUp = Up;
    p->Collect.Bit.LiftDown = Down;

    if(0)
		p->Collect.Bit.Moving = HaveMoving;
	else
		p->Collect.Bit.Moving = MoveTogether;                   //全都在动才为1

    p->Collect.Bit.UpLimited = UpLimited;
    p->Collect.Bit.DownLimited = DownLimited;

    //控制
    p->Collect.WalkGearMin = WalkGearMin;
    p->Collect.LiftGearMin = LiftGearMin;
    p->Collect.AsbSTM = AsbSTM;                            //单联动控制状态机
    p->Collect.ModeSTM = ModeSTM;                          //模式切换状态机
    p->Collect.Mode = Mode;                                //模式 都是同一个模式 或 为无效

    //联动距离 就是主车的联动距离
    //p->Collect.Dis = p->P2Obj.p2Self.Dis;                   //

    //计算联动同步动作超时时间
    if (p->P2Obj.p2Self->ModeReal != AsbMode_Null && MoveTogether == 0)
    {

    }
    else
    {
        p->Collect.Ms_MoveTogether = p->Input.Ms;
    }
    p->Collect.Ms_MoveTogetherTimeout = p->Input.Ms - p->Collect.Ms_MoveTogether;

    return 1;
}

//从车汇总主车的状态
bool Asb_Collect_Slave(AsbCtrl_TypeDef* p)
{
    p->Collect.Ms = p->Comm.MasterData.Ms;
    p->Collect.Dis = p->Comm.MasterData.Dis;                //主车的联动距离
    p->Collect.WheelRMax = p->Comm.MasterData.WheelRMax;
    p->Collect.WheelVMax = p->Comm.MasterData.WheelVMax;
    p->Collect.HgMin = p->Comm.MasterData.Hg;
    p->Collect.HgMax = p->Comm.MasterData.Hg;
    p->Collect.Obs = p->Comm.MasterData.Obs;
    p->Collect.ErrMax.Faults = p->Comm.MasterData.Err;
    p->Collect.ErrMax.Bit.UpLimited = p->Comm.MasterData.Bit.ErrUpLimited;//限制上升
    p->Collect.ErrMax.Bit.DownLimited = p->Comm.MasterData.Bit.ErrDownLimited;//限制下降
    p->Collect.ErrMax.Bit.MoveLimited = p->Comm.MasterData.Bit.ErrMoveLimited;//限制移动
    p->Collect.ModeReal = p->Comm.MasterData.ModeReal;      //主车的模式
    p->Collect.WalkGearMin = p->Comm.MasterData.Bit.WalkGear;//主车的行走档位
    p->Collect.LiftGearMin = p->Comm.MasterData.Bit.LiftGear;//主车的升降档位
    p->Collect.Bit.LiftUp = p->Comm.MasterData.Bit.LiftUp;   //升起
    p->Collect.Bit.LiftDown = p->Comm.MasterData.Bit.LiftDown; //下降
    p->Collect.Bit.Moving = p->Comm.MasterData.Bit.Moving;   //是否在运动中
    p->Collect.Bit.UpLimited = p->Comm.MasterData.Bit.UpLimited;//上限位触发
    p->Collect.Bit.DownLimited = p->Comm.MasterData.Bit.DownLimited;//下限位触发

    p->Collect.AsbSTM = p->Comm.MasterData.AsbSTM;          //单联动控制状态机
    p->Collect.ModeSTM = p->Comm.MasterData.ModeSTM;        //模式切换状态机
    p->Collect.Mode = p->Comm.MasterData.Mode;              //模式 都是同一个模式 或 为无效

    //巡线传感器偏差
    p->Collect.Track = p->Comm.MasterData.Track;

    //还有其他的，只是查看，没有控制作用，都没有控制作用，在Comm对象里查看也是一样的

    return 1;
}

//通信内容解析处理
bool Asb_CommParse(AsbCtrl_TypeDef* p)
{
    //如果是主车 汇总其它车的状态
    if (p->Para.Role == AsbRole_Master)
    {
        //从与各从车的通信对象中解析数据
        Asb_CommData2Cars(p);

        //汇总车数据
        Asb_Collect(p);
        //Asb_Collect_Obs(p);
    }
    else//从车 从与主车的通信对象中解析汇总数据到collect对象中
    {
        Asb_Collect_Slave(p);
    }

    return 1;
}

//各车都要等降的慢的车调平
bool Asb_PositionAdj_Hg_EveryOneWait(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;
    f32 Hg_Self = p2Car->Hg;
    f32 Hg_Other = p->Cars[1].Hg;
    f32 Hg_Diff = Hg_Self - Hg_Other;

    //临时处理，后面需要优化
    if(p->Para.Role == AsbRole_Master)
        Hg_Other = p->Cars[1].Hg;
    else
        Hg_Other = p->Cars[0].Hg;

    Hg_Diff = Hg_Self - Hg_Other;

    //当下降为电池电磁阀同步时，电机调速无效，通过回差控制让降得快的车等降得慢的车
    if (p2Car->CmdOut.Mode == AsbMode_Lift && p2Car->CmdOut.Bit.LiftDown)
    {
        if (Hg_Diff < (-p->Para.HgDiff_Tg))
            p->Output.Adj.Down_Wait = 1;
        else if (Hg_Diff > (-p->Para.HgDiff_Re))
            p->Output.Adj.Down_Wait = 0;
        else
            __nop();

        if (p->Output.Adj.Down_Wait)
            p2Car->CmdOut.Bit.LiftDown = 0;
        else
            __nop();
    }

    return 1;
}

//联动纠正计算
bool Asb_PositionAdj(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;
    AsbSiteNo_TypeDef SiteNo = p->Para.SiteNo;
    AsbSiteSeat_TypeDef Seat = p->Para.Seat;

    /*adj在后面使用有判断是否联动 这里计算也判断一下*/
    if (p->P2Obj.p2Self->AsbSwitch.STM != AsbAsbMode_Asb)
    {
        //清除所有纠正数据
        p2Car->Adj.Vy = 0;
        p2Car->Adj.Vx = 0;
        p2Car->Adj.Vz = 0;
        p2Car->Adj.Ax = 0;
        p2Car->Adj.Ay = 0;
        p2Car->Adj.Az = 0;
        p2Car->Adj.UpDown = 0;
        p2Car->Adj.Down_Wait = 0;

        return 1;
    }
    
    //AsbSwitch_TypeDef* p2AsbSwitch = &((p->P2Obj.p2Self)->AsbSwitch);
    //仅主车进行纠正 后面拓展到其它车都进行纠正
    if(p->Para.Role == AsbRole_Master && p->Para.Bit.ForceUseOwnCmd == 0)
	{
        //正向模式
        if (p2Car->CmdOut.Mode == AsbMode_PosStraight
           || p2Car->CmdOut.Mode == AsbMode_PosFlexible
           || p2Car->CmdOut.Mode == AsbMode_PosTrace
        )//行走
		{
            //
            if(SiteNo == AsbSiteNo_FB)//前后联动
			{
                if (Seat == AsbSiteSeat_1)//在前
                {
                    if (p2Car->CmdIn.Vxyz.y > 0)//向前
                    {
                        p2Car->Adj.Vy = -(p->Input.Place.PosDiff) * p->Para.K_DisAdj;
                        p2Car->Adj.Vx = p->Input.Place.Inf * p->Para.K_InfAdj;
						p2Car->Adj.Vz = p->Input.Place.Ang * p->Para.K_AngAdj;
                    }
                    else if (p2Car->CmdIn.Vxyz.y < 0)//向后
                    {
                        p2Car->Adj.Vy = (p->Input.Place.PosDiff) * p->Para.K_DisAdj;
                        p2Car->Adj.Vx = p->Input.Place.Inf * p->Para.K_InfAdj;
						p2Car->Adj.Vz = p->Input.Place.Ang * p->Para.K_AngAdj;
                    }
                    else//静止停车
                    {
                        p2Car->Adj.Vy = 0;
                        p2Car->Adj.Vx = 0;
                        p2Car->Adj.Vz = 0;
                    }
                }
                else if (Seat == AsbSiteSeat_2)//在后
                {
                    /*if (p2Car->CmdIn.Vxyz.y > 0)//向前
                    {
                        p2Car->Adj.Vy = (p->Input.Place.PosDiff) * p->Para.K_DisAdj;
                        p2Car->Adj.Vx = -p->Input.Place.Inf * p->Para.K_InfAdj;
						p2Car->Adj.Vz = p->Input.Place.Ang * p->Para.K_AngAdj;
                    }
                    else if (p2Car->CmdIn.Vxyz.y < 0)//向后
                    {
                        p2Car->Adj.Vy = (p->Input.Place.PosDiff) * p->Para.K_DisAdj;
                        p2Car->Adj.Vx = -p->Input.Place.Inf * p->Para.K_InfAdj;
						p2Car->Adj.Vz = p->Input.Place.Ang * p->Para.K_AngAdj;
                    }
                    else if (p2Car->CmdIn.Vxyz.y == 0 && (p2Car->CmdIn.Vxyz.x != 0 || p2Car->CmdIn.Vxyz.z != 0))
                    {
                        p2Car->Adj.Vy = (p->Input.Place.PosDiff) * p->Para.K_DisAdj;
                        p2Car->Adj.Vx = -p->Input.Place.Inf * p->Para.K_InfAdj;
                        p2Car->Adj.Vz = p->Input.Place.Ang * p->Para.K_AngAdj;
                    }*/
                    if (p2Car->CmdIn.Vxyz.y != 0 || p2Car->CmdIn.Vxyz.x != 0 || p2Car->CmdIn.Vxyz.z != 0)
                    {
                        p2Car->Adj.Vy = (p->Input.Place.PosDiff) * p->Para.K_DisAdj;
                        p2Car->Adj.Vx = -p->Input.Place.Inf * p->Para.K_InfAdj;
                        p2Car->Adj.Vz = p->Input.Place.Ang * p->Para.K_AngAdj;
                    }
                    else
                    {
                        p2Car->Adj.Vy = 0;
                        p2Car->Adj.Vx = 0;
                        p2Car->Adj.Vz = 0;
                    }

                    /*简化版
                    //横向对齐纠正
                    p2Car->Adj.Vx = (p->Input.Sensor.LaserSpot) * p->Para.K_InfAdj;
                    //姿态角度纠正
                    p2Car->Adj.Vz = -(p->Input.Sensor.Distance_FL - p->Input.Sensor.Distance_BR) * p->Para.K_AngAdj;*/
                }
                else {}
            }
            else if(SiteNo == AsbSiteNo_FB1)//前后联动 车头对车头
            {
                //后面再开发
            }
            else {}//其它位置后面再开发
        }
        else if(p2Car->CmdOut.Mode == AsbMode_Lift)
        {
            //多车上升时,大家都等最低的车，下降时都等最高的车
            /*暂简化为仅主车根据从车进行等待或追赶*/
            if(p2Car->CmdOut.Bit.LiftUp)
                p2Car->Adj.UpDown = -(p2Car->Hg - p->Cars[1].Hg) * p->Para.K_HgAdj;
            else if (p2Car->CmdOut.Bit.LiftDown)
            {
                p2Car->Adj.UpDown = (p2Car->Hg - p->Cars[1].Hg) * p->Para.K_HgAdj;
            }
            else
                p2Car->Adj.UpDown = 0;

        }
        else {}
    }
    else {}

    //各车都要等降的慢的车调平
	if (p->Para.HgDiff_Tg != 65535)
		Asb_PositionAdj_Hg_EveryOneWait(p);

    return 1;
}

//联动模块输出
bool Asb_Output(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;
	AsbOutput_TypeDef* p2Output = &p->Output;

	p2Output->AsbMode = p2Car->AsbSwitch.STM;
    p2Output->AsbErr = p2Car->AsbErr;
    p2Output->CarErr = p->Collect.ErrMax;
    p2Output->Obs = p->Collect.Obs;
    p2Output->Bit = p->Collect.Bit;

    //控制命令
    p2Output->Cmd = p2Car->CmdOut;

    //纠正量
    p2Output->Adj = p2Car->Adj;

	/*纠正量直接应用到控制命令 存在档位不同纠正效果不同问题
	p2Output->Cmd.Vxyz.x = p2Car->CmdOut.Vxyz.x + p2Car->Adj.Vx;
	p2Output->Cmd.Vxyz.y = p2Car->CmdOut.Vxyz.y + p2Car->Adj.Vy;
	p2Output->Cmd.Vxyz.z = p2Car->CmdOut.Vxyz.z + p2Car->Adj.Vz;*/

    //巡线传感器偏差
    p2Output->Track = p->Collect.Track;

    //轮系坐标偏移
    p2Output->Center = p2Car->Center;

    //解算缩放参数
    if (p->Para.Bit.ForceUseOwnCmd == 0)
    {
        p2Output->WheelRMax = p->Collect.WheelRMax;
        p2Output->WheelVMax = p->Collect.WheelVMax;
    }
    else
    {
        p2Output->WheelRMax = p2Car->WheelRMax;
        p2Output->WheelVMax = p2Car->WheelVMax;
    }

    return 1;
}

//联动通信更新
bool Asb_CommUpdate(AsbCtrl_TypeDef* p)
{
    AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;
    //AsbOutput_TypeDef* p2Output = &p->Output;
    AsbCommSlave_TypeDef* p2CommS = &p->Comm.SlaveData[0];//从车都把自己的信息放在联动通信数据的开头
	AsbCommMaster_TypeDef* p2CommM = &p->Comm.MasterData;

	//从车更新通信对象car -> comm
    if(p->Para.Role == AsbRole_Master)
        p2CommS->Ms = p2Car->Ms;
    else
        p2CommS->Ms = p->Comm.MasterData.Ms;                //从车回写收到的ms

	p2CommS->Dis = p2Car->Dis;
	p2CommS->WheelRMax = p2Car->WheelRMax;
    p2CommS->WheelVMax = p2Car->WheelVMax;
    p2CommS->ModeReal = p2Car->ModeReal;
    p2CommS->Hg = p2Car->Hg;
    p2CommS->Obs = p2Car->Obs;
    p2CommS->Err = p2Car->Err.Faults;
    p2CommS->Bit.ErrUpLimited = p2Car->Err.Bit.UpLimited;   //限制上升
    p2CommS->Bit.ErrDownLimited = p2Car->Err.Bit.DownLimited; //限制下降
    p2CommS->Bit.LiftUp = p2Car->CmdIn.Bit.LiftUp;
    p2CommS->Bit.LiftDown = p2Car->CmdIn.Bit.LiftDown;
    p2CommS->Bit.Moving = p2Car->Bit.Moving;
    p2CommS->Bit.UpLimited = p2Car->Bit.UpLimited;          //上限位触发
    p2CommS->Bit.DownLimited = p2Car->Bit.DownLimited;      //下限位触发
    p2CommS->Bit.WalkGear = p2Car->CmdIn.WalkGear;
    p2CommS->Bit.LiftGear = p2Car->CmdIn.LiftGear;
    //巡线传感器偏差
    p2CommS->Track = p2Car->Track;

    p2CommS->Check = p2Car->Check;

    p2CommS->AsbSTM = p2Car->AsbSwitch.STM;
    p2CommS->ModeSTM = p2Car->ModeSwitch.STM;
    p2CommS->Mode = p2Car->CmdIn.Mode;

    //自定义信息 直接从输入复制过来
    p2CommS->CustomS = p->Input.CustomS;

    //主车更新通信对象
    //p2CommM->Ms = p->Collect.Ms;
    //主车广播自己的ms
    if(p->Para.Role == AsbRole_Master)
    {
        p2CommM->Ms = p2Car->Ms;
        p2CommM->Dis = p->Collect.Dis;
        //p2CommM->Dis = p2Car->Dis;
        p2CommM->WheelRMax = p->Collect.WheelRMax;
        p2CommM->WheelVMax = p->Collect.WheelVMax;
        //状态
        p2CommM->ModeReal = p->Collect.ModeReal;
        if(p->Collect.Bit.LiftUp)//上升时，广播谁最低
            p2CommM->Hg = p->Collect.HgMin;
        else if(1)//下降时，广播谁最高
            p2CommM->Hg = p->Collect.HgMax;
        else
            p2CommM->Hg = 0; //静止时不广播高度
        p2CommM->Obs = p->Collect.Obs;
        p2CommM->Err = p->Collect.ErrMax.Faults;
        p2CommM->Bit.ErrUpLimited = p->Collect.ErrMax.Bit.UpLimited;//限制上升
        p2CommM->Bit.ErrDownLimited = p->Collect.ErrMax.Bit.DownLimited;//限制下降
        p2CommM->Bit.LiftUp = p->Collect.Bit.LiftUp;            //上升
        p2CommM->Bit.LiftDown = p->Collect.Bit.LiftDown;        //下降
        p2CommM->Bit.Moving = p->Collect.Bit.Moving;            //运动中
        p2CommM->Bit.UpLimited = p->Collect.Bit.UpLimited;      //上限位触发
        p2CommM->Bit.DownLimited = p->Collect.Bit.DownLimited;  //下限位触发
        p2CommM->Bit.WalkGear = p->Collect.WalkGearMin;        //最小行走档位
        p2CommM->Bit.LiftGear = p->Collect.LiftGearMin;        //最小升降档位

        //控制
        p2CommM->AsbSTM = p->Collect.AsbSTM;
        p2CommM->ModeSTM = p->Collect.ModeSTM;
        p2CommM->Mode = p->Collect.Mode;

        //巡线传感器偏差
        p2CommM->Track = p->Collect.Track;

        p2CommM->Check = p->Collect.Check;

        //自定义信息 直接从输入复制过来
        p2CommM->CustomM = p->Input.CustomM;

        //向不同从车单独传递的通信内容 需要为每个从车建立通信对象
    }
    else {}

    return 1;
}

//参数变化设定
bool Asb_ParaParse(AsbCtrl_TypeDef* p)
{
    return 1;
}

//联动模块复位
bool Asb_Reset(AsbCtrl_TypeDef* p)
{
    //复位各车的状态对象
//    char i = 0;
//    for(i=0;i<ASB_CAR_MAX_NUM;i++)
//    {
//        memset(&p->Cars[i], 0, sizeof(AsbCar_TypeDef));
//    }

//    //复位Collect对象
//	memset(&p->Collect, 0, sizeof(AsbCollectObj_TypeDef));

    //根据联动编号设置P2Obj对象
    if (p->Para.Iam > 0 && p->Para.Iam <= ASB_CAR_MAX_NUM)
    {
        p->P2Obj.p2Self = &p->Cars[p->Para.Iam - 1]; //
    }
    else
    {
        p->P2Obj.p2Self = NULL; //无效指针
	}

    //复位Output对象
//	memset(&p->Output, 0, sizeof(AsbOutput_TypeDef));

//    //复位通信对象
//    memset(&p->Comm, 0, sizeof(AsbComm_TypeDef));

	p->P2Obj.p2Self->AsbSwitch.STM = AsbAsbMode_Nnull;
	p->P2Obj.p2Self->ModeSwitch.STM = AsbModeChg_Init;

    return 1;
}

//联动处理总方法
bool Asb_Main(AsbCtrl_TypeDef* p)
{
    //参数变化设定
    Asb_ParaParse(p);

	//实时输入预处理 整理到单车对象中
	if (p->Para.Bit.Input2CarsOff == 0)
        Asb_Input2Cars(p);
    else {}

	//通信内容解析处理
    if (p->Para.Bit.CommParseOff == 0)
        Asb_CommParse(p);
    else {}

	//单联动切换
	Asb_STM(p);

    //模式切换同步
	Asb_ModeChgSyn(p);

    //控制命令处理
	Asb_CmdParse(p);

    //联动位置纠正计算
    Asb_PositionAdj(p);

    //运动限制
	Asb_CmdLimit(p);

    //联动异常检出
	if(p->Para.Bit.ForceOverErr == 0)
        Asb_ErrJdg(p);
    else
    {
        //memset(&p->Output.AsbErr, 0, sizeof(AsbErr_TypeDef));
		memset(&p->P2Obj.p2Self->AsbErr, 0, sizeof(AsbErr_TypeDef));
    }

    //输出
    if (p->Para.Bit.OutputOff == 0)
        Asb_Output(p);
    else {}

	//更新通信对象
	if (p->Para.Bit.CommUpdateOff == 0)
        Asb_CommUpdate(p);
    else {}

    //其它处理
    //计算从站返回时间差
    p->Comm.DiffMs = p->Input.Ms - p->Comm.SlaveData[1].Ms;

	return 1;

}

//联动对象初始化 例子
bool Asb_InitExample(AsbCtrl_TypeDef* p)
{
	p->Para.Iam = AsbCarStrNo_1;                            //编号1
	p->Para.Role = AsbRole_Master;                          //主车

    p->P2Obj.p2Self = &p->Cars[p->Para.Iam - 1];            //

    p->Para.HalfCar.Front = 1000;                           //前侧半长
	p->Para.HalfCar.Back = 1000;                            //后侧半长
	p->Para.HalfCar.Left = 1000;                            //左侧半宽
	p->Para.HalfCar.Right = 1000;                           //右侧半宽

	p->Para.K_HgAdj = 1;                                    //
	p->Para.K_DisAdj = 10;                                  //联动距离纠正系数

	p->Para.HgDiffMax = 30;                                 //最大高度差
    //传感器报警参数 正向（暂时不分正向横向 先用一套参数）
    p->Para.SensorAlarm.DisEnterMax = 6000;
    p->Para.SensorAlarm.DisEnterMin = 300;
    p->Para.SensorAlarm.DiffLR_EnterMax = 20;
    p->Para.SensorAlarm.LaserSpot_EnterMax = 10;
    p->Para.SensorAlarm.Ang_EnterMax = 65535;
    p->Para.SensorAlarm.DiffLR_Max = 40;
    p->Para.SensorAlarm.DisChangeMax = 50;
    p->Para.SensorAlarm.DisChangeMax_Inf = 50;
    p->Para.SensorAlarm.AngChangeMax = 3;
    p->Para.SensorAlarm.LaserSpotMax = 50;

    p->Para.SynTimeout = 3000;

    //下面仅主车需配置 AsbSiteSeat_NULL：不参与联动
    p->Para.Seats[0] = AsbSiteSeat_1;                       //参与联动
    p->Para.Seats[1] = AsbSiteSeat_2;                       //参与联动

	return 1;
}

//***********************************END OF FILE***********************************
