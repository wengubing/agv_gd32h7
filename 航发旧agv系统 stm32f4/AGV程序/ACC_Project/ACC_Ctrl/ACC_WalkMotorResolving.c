/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_WalkMotorResolving.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/

#define  ACC_WALK_MOTOR_RESOLVING_GLOBALS

#include "ACC_WalkMotorResolving.h"
#include "ACC_Init.h"
#include "ACC_Project.h"

/**************************************************************************
* 函数名:ACC_ModeSwitch_Steering
* 功  能:舵轮模式切换
* 参  数:
* 返回值:ACC_SteeringStep_TypeDef
* 备注：此函数在cbb中有默认弱函数实现，如果需要自定义，可在此文件中实现
*************************************************************************
ACC_SteeringStep_TypeDef ACC_ModeSwitch_Steering(ACC_DATA_TypeDef* p2Acc)
{
	return p2Acc->ModeSwitch.SteeringStep;
}*/

/***********************************************************
** 函数名称:	WheelAngleZeroSet
** 功能描述:	舵轮0点标定
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//舵轮0点标定
bool WheelAngleZeroSet(void)
{
	char i = 0;
	for(i=0;i<g_Para.WheelAngle.Num;i++)
	{
		g_Para.WheelAngle.Offset[i] = -g_WheelAngle[i].OrgRes;
	}
	return 1;
}

/**************************************************************************
* 函数名:Walk_MotorResolving
* 功  能:组织行走结算的CBB
* 参  数:NC
* 返回值:NC
**************************************************************************/
bool Walk_MotorResolving(void)
{
	char i=0;
	ACC_DATA_TypeDef *p2Acc=&AccData;
	MotorResolving_TypeDef *p2MR=&g_MotorResolving;
	Mecanum_TypeDef *p2MR_Mec=&g_MecanumResolving;

	
	if(p2Acc->ChassisType== ACC_ChassisType_Turtle)//麦轮
	{
		//建立命令管道
		p2MR_Mec->Oxyz[0] = p2Acc->CalCmd.VOxyz.x;
		p2MR_Mec->Oxyz[1] = p2Acc->CalCmd.VOxyz.y;
		p2MR_Mec->Oxyz[2] = p2Acc->CalCmd.VOxyz.z;

		//解算缩放参数输入到CBB
		if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
		{
			p2MR_Mec->RotUnif.LxyMax.ExtIn = g_Asb.Output.WheelRMax;
			p2MR_Mec->SpeedZoom.SpeedMax.ExtIn = g_Asb.Output.WheelVMax;
		}
		else
		{
			p2MR_Mec->RotUnif.LxyMax.ExtIn = Mec_InvalidExtIn;
			p2MR_Mec->SpeedZoom.SpeedMax.ExtIn = Mec_InvalidExtIn;
		}

		//调用CBB解算接口
		Mec_Main(p2MR_Mec);

		//解算对象输出到acc对象
		for (i = 0; i < p2MR_Mec->Para.WalkMotorNum; i++)
		{
			p2Acc->WalkMotor[i].V = p2MR_Mec->Out.Speed[i];
		}
	}
	else if(p2Acc->ChassisType== ACC_ChassisType_Single)//单轮RGV
	{
		p2Acc->WalkMotor[0].V =p2Acc->SpeedPlan[p2Acc->CalCmd.WalkGear-1]/642.0f*p2Acc->CalCmd.VOxyz.y;
	}
	else if(p2Acc->ChassisType== ACC_ChassisType_DiffVCar)//差速小车
	{
		ACCMode_TypeDef TgtMode = p2Acc->ModeSwitchSMT; 
		float Temp_WalkMotor_L = 0;
		float Temp_WalkMotor_R = 0;
		//行走模式
		if(TgtMode == ACCMode_PosStraight || TgtMode == ACCMode_PosFlexible 
		|| TgtMode == ACCMode_PosTrace)
		{
            if(p2Acc->CalCmd.VOxyz.y > 5.0f)
			{
				Temp_WalkMotor_L =  -p2Acc->CalCmd.VOxyz.y + p2Acc->CalCmd.VOxyz.z;
				Temp_WalkMotor_R =  -p2Acc->CalCmd.VOxyz.y - p2Acc->CalCmd.VOxyz.z;
				//防止前进时，转弯速度超过前进速度，导致车乱走
				if(Temp_WalkMotor_L > 0){Temp_WalkMotor_L = 0;}
				if(Temp_WalkMotor_R > 0){Temp_WalkMotor_R = 0;}
			}
			else if(p2Acc->CalCmd.VOxyz.y < -5.0f)
			{
				Temp_WalkMotor_L =  -p2Acc->CalCmd.VOxyz.y + p2Acc->CalCmd.VOxyz.z;
				Temp_WalkMotor_R =  -p2Acc->CalCmd.VOxyz.y - p2Acc->CalCmd.VOxyz.z;
				//防止后退时，转弯速度超过前进速度，导致车乱走
				if(Temp_WalkMotor_L < 0){Temp_WalkMotor_L = 0;}
				if(Temp_WalkMotor_R < 0){Temp_WalkMotor_R = 0;}
			}
		}
		//横移
		else if(	TgtMode == ACCMode_InfStraight || TgtMode == ACCMode_InfFlexible 
				 || TgtMode == ACCMode_InfTrace)
		{
			/*不做处理*/
		}
		//自转
		else if(TgtMode == ACCMode_Rotation)
		{
			if(p2Acc->CalCmd.VOxyz.z > 5.0f || p2Acc->CalCmd.VOxyz.z < -5.0f)
			{
				Temp_WalkMotor_L =  p2Acc->CalCmd.VOxyz.z;
				Temp_WalkMotor_R = -p2Acc->CalCmd.VOxyz.z;
			}				
		}
		else
		{
			/*不做处理*/
		}

		p2Acc->WalkMotor[0].V =  p2Acc->SpeedPlan[p2Acc->CalCmd.WalkGear-1]/642.0f*Temp_WalkMotor_L;
		p2Acc->WalkMotor[1].V = -p2Acc->SpeedPlan[p2Acc->CalCmd.WalkGear-1]/642.0f*Temp_WalkMotor_R;
	}
	else//舵轮
	{
        //ACCMode_TypeDef TgtMode = p2Acc->CalCmd.Mode;
        ACCMode_TypeDef TgtMode = p2Acc->ModeSwitchSMT;     //使用实时状态机 作为目标模式 防止模式切换时解算CBB输出异常

		//建立输入管道(实时角度输入)
		//for (i = 0; i < p2MR->Para.MotorNumber; i++)
        //414调试更新bug，之前2、4个模组时，越界操作的是实时摇杆命令输入，对正常功能没有影响
	    for (i = 0; i < p2MR->Para.ModuleNumber; i++)
		{
			p2MR->Input.ActualAng[i]=g_WheelAngle[i].Res;
		}
		
		//建立命令管道
		p2MR->Input.Oxyz[0] = p2Acc->CalCmd.VOxyz.x;
		p2MR->Input.Oxyz[1] = p2Acc->CalCmd.VOxyz.y;
		p2MR->Input.Oxyz[2] = p2Acc->CalCmd.VOxyz.z;
		p2MR->Input.Aoxyz[0] = p2Acc->CalCmd.AOxyz.x;
		p2MR->Input.Aoxyz[1] = p2Acc->CalCmd.AOxyz.y;
		p2MR->Input.Aoxyz[2] = p2Acc->CalCmd.AOxyz.z;

        //直接通过圆心坐标解算
        if(p2Acc->CalCmd.BitPara.CentreCtrl)
        {
            p2MR->Para.Bit.CentreCtrl = 1;
            //p2MR->Input.RotPoint.X = g_CPC.IntersectionPoint.X;
            //p2MR->Input.RotPoint.Y = g_CPC.IntersectionPoint.Y;
            p2MR->Input.RotPoint = g_CPC.CenterPoint;
        }
        else
            p2MR->Para.Bit.CentreCtrl = 0;

		//行走模式
		if(TgtMode == ACCMode_PosStraight || TgtMode == ACCMode_PosFlexible 
		|| TgtMode == ACCMode_PosTrace)
		{
            p2MR->Input.WalkMode = MR_WALK_POS;					//纵向

            if(g_ShelfAutadj.out_AdjStart && g_ShelfAutadj.out_Mode == ShelfAutadj_Mode_Carturn)//自动对中功能正在执行中
            {
                p2MR->Para.Bit.WALK_CAR = 1;
            }
            else
			    p2MR->Para.Bit.WALK_CAR = 0;

			//Vy速度为负时，Ax和Az取反
			if(p2Acc->CalCmd.VOxyz.y<0 && g_ShelfAutadj.out_AdjStart == 1)
			{
				p2MR->Input.Aoxyz[0] = -p2MR->Input.Aoxyz[0];
				p2MR->Input.Aoxyz[2] = -p2MR->Input.Aoxyz[2];
				//改为取反平滑前的Axyz
			}
		}
		else if(TgtMode == ACCMode_InfStraight || TgtMode == ACCMode_InfFlexible 
		|| TgtMode == ACCMode_InfTrace)
		{
			p2MR->Input.WalkMode=	MR_WALK_INF;					//横向 
			//Vx速度为负时，Ay和Az取反
			if(p2Acc->CalCmd.VOxyz.x<0)
			{
				//p2MR->Input.Aoxyz[1] = -p2MR->Input.Aoxyz[1];
				//p2MR->Input.Aoxyz[2] = -p2MR->Input.Aoxyz[2];
				//改为取反平滑前的Axyz
			}
		}
		else if(TgtMode == ACCMode_Rotation)
			p2MR->Input.WalkMode=	MR_WALK_ROT;					//自转
		else
			p2MR->Input.WalkMode=	MR_WALK_NULL;					//0

		//p2MR->AngAdjEn = 1;

		//补丁：介于很多项目正向巡线时 车头车尾前后甩，所以将正向巡线和其他模式的差动速度分离开
		{
			if(TgtMode == ACCMode_PosTrace)
			{
				p2MR->Para.SkewSpeed = g_Para.CtrlCoef.TraceSkewSpeed;
			}
			else
			{
				p2MR->Para.SkewSpeed = g_Para.Walk.SkewSpeed;
			}
		}

		//解算缩放参数输入到CBB
		if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
		{
			p2MR->RdMax.RdMax_AsbInput = g_Asb.Output.WheelRMax;
			p2MR->WheelSpeedObj.WheelSpeedMax_AsbInput = g_Asb.Output.WheelVMax;
		}
		else
		{
			p2MR->RdMax.RdMax_AsbInput = 0;
			p2MR->WheelSpeedObj.WheelSpeedMax_AsbInput = 0;
		}
		
		//调用CBB解算接口
		MotorResolving_Main(p2MR,p2Acc->in_Ms);

		//解算对象输出到acc对象
		for (i = 0; i < p2MR->Para.MotorNumber; i++)
		{
			p2Acc->WalkMotor[i].V = p2MR->Output.WheelSpeed[i];
		}
	}
	return 1;
}

/************************************END OF FILE************************************/
