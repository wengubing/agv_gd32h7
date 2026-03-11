/********************************** Copyright ***********************************
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
*
* FileName   : app_MotorResoling.c
* Version    : V1.0
* Author     : WenXiaoBing
* Date       : 2023.12.22
* Description: 输入控制命令，输出每个轮子的控制速度
*******************************************************************************/

/*包含对象头文件*/
#include <math.h>
#include <stdlib.h>
#include "basecalfunc.h"
#include "app_MotorResolving.h"
#include "app_DataSmoothing.h"
//#include "Patch.h"

/*在外部实现的方法声明*/
extern  float32_t arm_sin_f32(float32_t x);
extern  float32_t arm_cos_f32(float32_t x);

/*外部实例对象声明
extern WalkMotorSmooth_TypeDef g_OSSM;*/

/**************************************************************************
* 函数名:  MotorResolving_GetRadiusMin()
* 功  能:分别计算横向/正向模式最大斜行角度下 允许的最小圆弧运动半径
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_GetRadiusMin(MotorResolving_TypeDef* p)
{
	char i = 0;
	p->ModuleAxisXMax = 0;
	p->ModuleAxisYMax = 0;
	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		/*找到最远的模组坐标*/
		if (fabs(p->Para.Mxy[i].X_Axis) > p->ModuleAxisXMax)
		{
			p->ModuleAxisXMax = fabs(p->Para.Mxy[i].X_Axis);
		}

		if (fabs(p->Para.Mxy[i].Y_Axis) > p->ModuleAxisYMax)
		{
			p->ModuleAxisYMax = fabs(p->Para.Mxy[i].Y_Axis);
		}
	}
	p->RadiusMinPos = fabs(p->ModuleAxisXMax) / (arm_cos_f32(p->Para.MaxSliAngPos * K_AngleToRad));
	p->RadiusMinInf = (0.707 * (fabs(p->ModuleAxisYMax) + fabs(p->ModuleAxisXMax))) / arm_sin_f32((45 - p->Para.MaxSliAngInf) * K_AngleToRad);

	/*限制半径参数*/
	if (p->Para.WheelType != MR_WheelType_EntiretyDifSpeed)/*非差速轮*/
	{
		if (p->Para.MinRotDisPos < p->RadiusMinPos)
			p->Para.MinRotDisPos = p->RadiusMinPos;

		if (p->Para.MinRotDisInf < p->RadiusMinInf)
			p->Para.MinRotDisInf = p->RadiusMinInf;
	}
}


/**************************************************************************
* 函数名:  MotorResolving_Xyz2Geometry()
* 功  能:摇杆值映射成实际复合运动参数
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_Xyz2Geometry(MotorResolving_TypeDef* p)
{
	/*汽车模式时 用斜行轴命令作为转向命令解算 复合运动Z轴的符号*/
	/*摇杆控制命令的Z轴，和转向角度进行映射，之后再由前进方向决定复合运动z轴命令符号*/
	if (p->Input.WalkMode == MR_WALK_POS && p->Para.Bit.WALK_CAR)
	{
		//汽车转向模式，暂只支持正向运动
		if (1)//p->Input.WalkMode == MR_WALK_POS)
		{
			p->Input.Aoxyz[0] = AbsFuc(p->Input.Aoxyz[2], p->Para.MaxSliAngPos);
			if (p->Input.Aoxyz[1] > 0)
				p->Input.Aoxyz[2] = (-p->Input.Aoxyz[0]);
			else
				p->Input.Aoxyz[2] = p->Input.Aoxyz[0];
		}
		else if (p->Input.WalkMode == MR_WALK_INF)
		{
			p->Input.Aoxyz[1] = AbsFuc(p->Input.Aoxyz[2], p->Para.MaxSliAngPos);
			if (p->Input.Aoxyz[0] > 0)
				p->Input.Aoxyz[2] = (-p->Input.Aoxyz[1]);
			else
				p->Input.Aoxyz[2] = p->Input.Aoxyz[1];
		}
	}

	if (p->Input.WalkMode == MR_WALK_ROT)
	{
		p->AozNew = 0;
		p->SliAng = 0;
	}
	else
	{
		/*非巡线 且 不是汽车模式 才非线性映射 圆弧半径*/
		{
			if (p->Input.GolineFlag == 0 && p->Para.Bit.WALK_CAR == 0)
				p->AozNew = atanf(fabs(p->Input.Aoxyz[2]) / (MR_K_RNew / 10)) * 1.2738f * K_RadToAngle;
			else
				p->AozNew = fabs(p->Input.Aoxyz[2]);

			if (p->AozNew > MR_MaxSkewHandle)
				p->AozNew = MR_MaxSkewHandle;
		}

		/*不同模式下解算复合运动 具体分量*/
		if (p->Input.WalkMode == MR_WALK_POS)
		{
			p->SliAng = -(p->Input.Aoxyz[0] * p->Para.MaxSliAngPos) / MR_MaxSkewHandle;
			p->RotDis = p->Para.MaxRotDisPos - ((p->Para.MaxRotDisPos - p->Para.MinRotDisPos) * ((p->AozNew) / MR_MaxSkewHandle));
		}
		else
		{
			p->SliAng = (p->Input.Aoxyz[1] * p->Para.MaxSliAngInf) / MR_MaxSkewHandle;
			p->RotDis = p->Para.MaxRotDisInf - ((p->Para.MaxRotDisInf - p->Para.MinRotDisInf) * ((p->AozNew) / MR_MaxSkewHandle));
		}

		//汽车模式时，圆弧半径计算
		if (p->Input.WalkMode == MR_WALK_POS && p->Para.Bit.WALK_CAR)
		{
			p->RotDis = p->ModuleAxisYMax / arm_sin_f32(fabs(p->SliAng) * K_AngleToRad);
			//p->RotDis = p->ModuleAxisXMax / arm_sin_f32(p->SliAng*K_AngleToRad);//暂不考虑横向
		}

		/*限制范围*/
		if (p->Input.WalkMode == MR_WALK_POS)
		{
			p->SliAng = AbsFuc(p->SliAng, p->Para.MaxSliAngPos);

			if (p->Input.Aoxyz[2] == 0)//无旋转分量
				p->RotDis = InvalidRotDis;
			else
			{
				p->RotDis = AbsFuc(p->RotDis, p->Para.MaxRotDisPos);
				p->RotDis = AbsFucLimitMin(p->RotDis, p->Para.MinRotDisPos);
			}
		}
		else if (p->Input.WalkMode == MR_WALK_INF)
		{
			p->SliAng = AbsFuc(p->SliAng, p->Para.MaxSliAngInf);

			if (p->Input.Aoxyz[2] == 0)//无旋转分量
				p->RotDis = InvalidRotDis;
			else
			{
				p->RotDis = AbsFuc(p->RotDis, p->Para.MaxRotDisInf);
				p->RotDis = AbsFucLimitMin(p->RotDis, p->Para.MinRotDisInf);
			}
		}
	}
}

/**************************************************************************
* 函数名:MotorResolving_RotPointAxisCal_Comp
* 功  能:计算圆弧运动中心坐标
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_RotPointAxisCal_Comp(MotorResolving_TypeDef* p)
{
	f32	TriangleAdj = 0.0;//直角三角形邻边
	f32 TriangleOpp = 0.0;//直角三角形对边

	TriangleAdj = arm_cos_f32(fabs(p->SliAng) * K_AngleToRad) * p->RotDis;
	TriangleOpp = arm_sin_f32(fabs(p->SliAng) * K_AngleToRad) * p->RotDis;
	if (p->Input.Aoxyz[2] >= 0)//以车体左侧为旋转半径  
	{
		if (p->Input.WalkMode == MR_WALK_POS)
		{
			if (p->SliAng >= 0)//当p->SliAng>=0时，旋转半径位于第三象限
			{
				p->RotPointX = TriangleAdj * (-1);
				p->RotPointY = TriangleOpp * (-1);
			}
			else//当p->SliAng<0时，旋转半径位于第二象限
			{
				p->RotPointX = TriangleAdj * (-1);
				p->RotPointY = TriangleOpp * (+1);
			}
		}
		else
		{
			if (p->SliAng >= 0)
			{
				p->RotPointX = TriangleOpp * (-1);
				p->RotPointY = TriangleAdj * (+1);
			}
			else
			{
				p->RotPointX = TriangleOpp * (+1);
				p->RotPointY = TriangleAdj * (+1);
			}
		}
	}
	else  //以车体右侧为旋转半径
	{
		if (p->Input.WalkMode == MR_WALK_POS)
		{
			if (p->SliAng >= 0)    //当p->SliAng>=0时，旋转半径位于第一象限
			{
				p->RotPointX = TriangleAdj * (+1);
				p->RotPointY = TriangleOpp * (+1);
			}
			else                  //当p->SliAng<0时，旋转半径位于第四象限
			{
				p->RotPointX = TriangleAdj * (+1);
				p->RotPointY = TriangleOpp * (-1);
			}
		}
		else
		{
			if (p->SliAng >= 0)
			{
				p->RotPointX = TriangleOpp * (+1);
				p->RotPointY = TriangleAdj * (-1);
			}
			else
			{
				p->RotPointX = TriangleOpp * (-1);
				p->RotPointY = TriangleAdj * (-1);
			}
		}
	}
}

/**************************************************************************
* 函数名:MotorResolving_WhellRdAndTgtAngCal_Comp
* 功  能:计算各轮子运动半径、各模组目标角度
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_WhellRdAndTgtAngCal_Comp(MotorResolving_TypeDef* p)
{
	f32
		Rd_Buffer_X = 0.0,										//解算轮系距离1
		Rd_Buffer_Y = 0.0,										//解算轮系距离2		
		Rd_Buffer_XY = 0.0,										//解算轮系距离3
		ClacBuffer = 0;                     					//计算缓存
	char i = 0;
	s8 sign_temp = 1, sign_temp2 = 1;

	//根据转弯方向确定轮子在圆弧运动中的内外侧
	if (p->Input.Aoxyz[2] > 0)
		sign_temp = 1;
	else
		sign_temp = -1;

	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		Rd_Buffer_X = (p->Para.Mxy[i].X_Axis - p->RotPointX);
		Rd_Buffer_Y = (p->Para.Mxy[i].Y_Axis - p->RotPointY);
		Rd_Buffer_XY = powf(fabs(Rd_Buffer_X), 2) + powf(fabs(Rd_Buffer_Y), 2);
		ClacBuffer = sqrtf(Rd_Buffer_XY);

		//横向模式下，如果目标角度反向，则内外侧相反
		if (p->Input.WalkMode == MR_WALK_INF && p->Para.Mxy[1].Inf_Invflg)
			sign_temp2 = (-sign_temp);
		else
			sign_temp2 = sign_temp;

		p->WheelRd[i * 2] = ClacBuffer + (p->PosModeRdSign[i * 2]) * p->Para.Lc * sign_temp2;
		p->WheelRd[i * 2 + 1] = ClacBuffer + (p->PosModeRdSign[i * 2 + 1]) * p->Para.Lc * sign_temp2;

		/*符号变换*/
		if (p->Input.WalkMode == MR_WALK_POS)
		{
			p->TgtAng[i] = asinf(fabs(Rd_Buffer_Y) / ClacBuffer) * K_RadToAngle;

			if (Rd_Buffer_Y >= 0)
				p->TgtAng[i] *= (+1);
			else
				p->TgtAng[i] *= (-1);

			if (p->Input.Aoxyz[2] < 0)
				p->TgtAng[i] *= (-1);
		}
		else if (p->Input.WalkMode == MR_WALK_INF)
		{
			p->TgtAng[i] = asinf(fabs(Rd_Buffer_X) / ClacBuffer) * K_RadToAngle;

			if (Rd_Buffer_X >= 0)
				p->TgtAng[i] *= (-1);
			else
				p->TgtAng[i] *= (+1);

			if (p->Input.Aoxyz[2] > 0)
				p->TgtAng[i] *= (-1);

			/*横向角度偏移*/
			p->TgtAng[i] = (MR_InfeedSkewAng + p->TgtAng[i]);
		}
	}
}

/**************************************************************************
* 函数名:MotorResolving_WhellRdAndTgtAngCal_Rot
* 功  能:自转模式计算rd和tgtang
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_WhellRdAndTgtAngCal_Rot(MotorResolving_TypeDef* p)
{
	f32 ClacBuffer = 0;//计算缓存
	char i = 0;
	for (i = 0; i < p->Para.MotorNumber; i++)
	{
		ClacBuffer = sqrtf(powf(fabs(p->Para.Mxy[i / 2].X_Axis), 2) +
			powf(fabs(p->Para.Mxy[i / 2].Y_Axis), 2));
		p->WheelRd[i] = ClacBuffer + (p->RotModeRdSign[i]) * p->Para.Lc;
	}

	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		if (p->Para.Mxy[i].Quadrant == MR_Quadrant_YPositive || p->Para.Mxy[i].Quadrant == MR_Quadrant_YMinus)
		{
			//如果横向方向是反的，只能往90度转
			if (p->Para.Mxy[i].Inf_Invflg)
				p->TgtAng[i] = 90;
			else
				p->TgtAng[i] = -90;//x axis = 0
		}
		else if (p->Para.Mxy[i].Quadrant == MR_Quadrant_XPositive || p->Para.Mxy[i].Quadrant == MR_Quadrant_XMinus || p->Para.Mxy[i].Quadrant == MR_Quadrant_Origin)
			p->TgtAng[i] = 0;//y axis = 0 或 原点
		else
			p->TgtAng[i] = atanf(fabs(p->Para.Mxy[i].Y_Axis) / fabs(p->Para.Mxy[i].X_Axis)) * K_RadToAngle * p->RotModeAngSign[i];
	}
}

/**************************************************************************
* 函数名:MotorResolving_WhellRdAndTgtAngCal_Sli
* 功  能:平移模式计算rd和tgtang
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_WhellRdAndTgtAngCal_Sli(MotorResolving_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		p->WheelRd[i * 2] = p->RotDis;
		p->WheelRd[i * 2 + 1] = p->RotDis;
		if (p->Input.WalkMode == MR_WALK_INF)
		{
			p->TgtAng[i] = p->SliAng + MR_InfeedSkewAng;

			//如果横向方向取反
			if (p->Para.Mxy[i].Inf_Invflg)
				p->TgtAng[i] = p->SliAng + 90;
			else {}
		}
		else
			p->TgtAng[i] = p->SliAng;
	}
}

/**************************************************************************
* 函数名:MotorResolving_WheelRdZoom
* 功  能:半径缩放
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_WheelRdZoom(MotorResolving_TypeDef* p)
{
	char i = 0;
	/*find max*/
	p->RdMax.RdMax_Self = p->WheelRd[0];
	for (i = 0; i < p->Para.MotorNumber; i++)
	{
		if (p->WheelRd[i] > p->RdMax.RdMax_Self)
			p->RdMax.RdMax_Self = p->WheelRd[i];
		else {}
	}

	//和联动输入的最大值比较，取较大值
	if (p->RdMax.RdMax_Self > p->RdMax.RdMax_AsbInput || p->RdMax.RdMax_AsbInput == InvalidRotDis)
		p->RdMax.RdMax_Exe = p->RdMax.RdMax_Self;
	else
		p->RdMax.RdMax_Exe = p->RdMax.RdMax_AsbInput;

	/*all zoom*/
	for (i = 0; i < p->Para.MotorNumber; i++)
	{
		if (p->Input.WalkMode != MR_WALK_ROT && p->Input.Aoxyz[2] == 0)
			p->WheelRdUnifor[i] = 1;
		else
			p->WheelRdUnifor[i] = p->WheelRd[i] / p->RdMax.RdMax_Exe;

	}
}

/**************************************************************************
* 函数名:MotorResolving_MotorSpeedCal
* 功  能:计算电机速度
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_MotorSpeedCal(MotorResolving_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->Para.MotorNumber; i++)
	{
		/*前进速度解析*/
		if (p->Input.WalkMode == MR_WALK_POS)
		{
			if (p->Para.WheelType == MR_WheelType_EntiretyDifSpeed && p->Input.Oxyz[1] == 0 && p->Para.Bit.EntiretyDifSpeedRotModeEn == 0)
			{
				p->Output.WheelLineSpeed[i] = (p->WheelDirRot[i] * p->Input.Oxyz[2]) * p->Para.Ksm;
			}
			else
				p->Output.WheelLineSpeed[i] = (p->WheelDirPos[i] * p->Input.Oxyz[1]) * p->Para.Ksm;
		}
		else if (p->Input.WalkMode == MR_WALK_INF)
			p->Output.WheelLineSpeed[i] = (p->WheelDirInf[i] * p->Input.Oxyz[0]) * p->Para.Ksm;
		else if (p->Input.WalkMode == MR_WALK_ROT)
			p->Output.WheelLineSpeed[i] = (p->WheelDirRot[i] * p->Input.Oxyz[2]) * p->Para.Ksm;

		/*差动速度计算*/
		p->Output.WheelDiffSpeed[i] = MotorResolving_OffsetModCalc(p->TgtAng[i/2],p->Input.ActualAng[i/2],MR_ChangeWalk,p,i)*p->Para.SkewSpeed*p->AngAdjEn; 
		
		/*装入差动平滑输入
		g_OSSM.SM[i].in_RealTimeValue = MotorResolving_OffsetModCalc(p->TgtAng[i / 2], p->Input.ActualAng[i / 2], MR_ChangeWalk, p, i) * p->Para.SkewSpeed * p->AngAdjEn;
		//未使能差动 直接赋值
		if (g_OSSM.En == 0)
			p->Output.WheelDiffSpeed[i] = g_OSSM.SM[i].in_RealTimeValue;
		else//平滑处理
		{
			p->Output.WheelDiffSpeed[i] = DataSmoothing_Main(&(g_OSSM.SM[i]), p->Ms, g_OSSM.SM[i].in_RealTimeValue) * p->AngAdjEn;
			g_OSSM.RunFlg = 1;
		}*/

		/*电机合成*/
		p->Output.WheelSpeed[i] = MotorResolving_WheelSpeedUpDate(p->Output.WheelLineSpeed[i], p->Output.WheelDiffSpeed[i], p->WheelRdUnifor[i], i);
	}
}

/**************************************************************************
* 函数名:MotorResolving_WheelSpeedZoom
* 功  能:速度缩放
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_WheelSpeedZoom(MotorResolving_TypeDef* p)
{
	char i = 0;
	/*找到绝对值最大的速度参数*/
	p->WheelSpeedObj.WheelSpeedMax_Self = 0;
	for (i = 0; i < p->Para.MotorNumber; i++)
	{
		if (fabs(p->Output.WheelSpeed[i]) > p->WheelSpeedObj.WheelSpeedMax_Self)
		{
			p->WheelSpeedObj.WheelSpeedMax_Self = fabs(p->Output.WheelSpeed[i]);
		}
	}

	//和联动输入的最大值比较，取较大值
	if (p->WheelSpeedObj.WheelSpeedMax_Self > p->WheelSpeedObj.WheelSpeedMax_AsbInput)
		p->WheelSpeedObj.WheelSpeedMax_Exe = p->WheelSpeedObj.WheelSpeedMax_Self;
	else
		p->WheelSpeedObj.WheelSpeedMax_Exe = p->WheelSpeedObj.WheelSpeedMax_AsbInput;

	/*如果轮子最大速度参数超过范围则求出系数,然后所有轮速比例缩小
	此过程问题：前向速度较大时，最大差动性能被限制
	可优化：在保证差动性能前提下，计算并缩放前向速度，影响是差动时前向速度变化会更明显*/
	if (p->WheelSpeedObj.WheelSpeedMax_Exe > p->Para.MotorMaxRpm)
	{
		p->WheelSpeedObj.K2WheelZoom = p->Para.MotorMaxRpm / p->WheelSpeedObj.WheelSpeedMax_Exe;

		for (i = 0; i < p->Para.MotorNumber; i++)
		{
			p->Output.WheelSpeed[i] *= p->WheelSpeedObj.K2WheelZoom;
		}
	}
	else
		p->WheelSpeedObj.K2WheelZoom = 1;
}

/**************************************************************************
* 函数名:MotorResolving_WheelSpeedUpDate
* 功  能:速度合成函数
* 参  数:Vls：前向分速度，Vls1：差动分速度，Rd：圆弧运动半径
* 返回值:WheelSpeed：合成速度
**************************************************************************/
f32 MotorResolving_WheelSpeedUpDate(f32 Vls, f32 Vls1, f32 Rd, u8 ModeN)
{
	f32 WheelSpeed = 0;

	if (1)
	{//先缩放Vls
		Vls = Vls * Rd;
		WheelSpeed = Vls + Vls1;
	}
	else//一起缩放
		WheelSpeed = (Vls + Vls1) * Rd;
	return WheelSpeed;
}

/**************************************************************************
* 函数名:MotorResolving_OffsetModCalc
* 功  能:根据设定角度与当前角度计算偏差系数
* 参  数:SetAng.要转向的角度    CurAng.当前角度   Mode.转向模式类型 i.电机序号
* 返回值:带符号的、归一化的差动控制量
**************************************************************************/
float MotorResolving_OffsetModCalc(float SetAng, float CurAng, MR_Change_TypeEnum Mode, MotorResolving_TypeDef* p, u8 i)
{
	float OffsetMod = 0.0f;//计算后的偏差系数
	float AngDiffe = 0.0f;//设定值与当前值的差值				
	short  Sign = 0;
	f32 InPlaceMin = 0.0f;//到位最小值
	f32 AngSlowDown = 0.0f;//减速角度

	if (Mode == MR_ChangeIng)
	{
		InPlaceMin = p->Para.AngInPlaceMin_ChangeIng;
		AngSlowDown =p->Para.AngSlowDown_ChangeIng;
	}
	else
	{
		InPlaceMin = p->Para.AngInPlaceMin;
		AngSlowDown =p->Para.AngSlowDown;
	}

	if (SetAng - CurAng > 0)//向左转
	{
		Sign = 1;
	}
	else if (SetAng - CurAng < 0)//向右转
	{
		Sign = -1;
	}
	else
	{
		Sign = 0;
	}

	AngDiffe = fabs(SetAng - CurAng);

	if (AngDiffe < InPlaceMin)//到位
	{
		OffsetMod = 0;
	}
	else if (AngDiffe < AngSlowDown)//减速
	{
		OffsetMod = AngDiffe / AngSlowDown;
		if (Mode == MR_ChangeIng) //原地转向时保持最小速度，避免停滞无法响应或效率低
		{
			if (OffsetMod < 0.1f)
				OffsetMod = 0.1f;
		}
	}
	else//最大速度差动
	{
		OffsetMod = 1;
	}

	if (SetAng > p->Para.Mxy[i / 2].CtrlAngMax_Left || SetAng < p->Para.Mxy[i / 2].CtrlAngMax_Right)
	{
		p->Output.CtrlAngLimit_Flg = 1;//会发生报警
		OffsetMod = 0;
	}
	else
	{
		//p->Output.CtrlAngLimit_Flg=0;//移除报警
	}
	return OffsetMod * Sign;
}

/**************************************************************************
* 函数名:MotorResolving_RotPointAxisCal
* 功  能:计算圆心坐标
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_RotPointAxisCal(MotorResolving_TypeDef* p)
{
	if (p->Input.WalkMode == MR_WALK_ROT)
	{
		p->RotPointX = 0;
		p->RotPointY = 0;
	}
	else
	{
		if (p->Input.Aoxyz[2] == 0)//无旋转分量
		{
			p->RotPointX = InvalidRotDis;
			p->RotPointY = InvalidRotDis;
		}
		else
			MotorResolving_RotPointAxisCal_Comp(p);
	}
}

/**************************************************************************
* 函数名:MotorResolving_WhellRdAndTgtAngCal
* 功  能:轮系圆弧运动半径、目标角度计算
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_WhellRdAndTgtAngCal(MotorResolving_TypeDef* p)
{
	if (p->Input.WalkMode == MR_WALK_ROT)//自转     
	{
		MotorResolving_WhellRdAndTgtAngCal_Rot(p);
	}
	else
	{
		if (p->Input.Aoxyz[2] != 0)
			MotorResolving_WhellRdAndTgtAngCal_Comp(p);
		else
			MotorResolving_WhellRdAndTgtAngCal_Sli(p);//无旋转分量
	}
}

/**************************************************************************
* 函数名:MotorResolving_GetAngAdjEn
* 功  能:解算调用主方法
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_GetAngAdjEn(MotorResolving_TypeDef* p)
{
	if (p->Input.WalkMode == MR_WALK_POS)
		p->DrivingAxis = 1;
	else if (p->Input.WalkMode == MR_WALK_INF)
		p->DrivingAxis = 0;
	else if (p->Input.WalkMode == MR_WALK_ROT)
		p->DrivingAxis = 2;

	if (p->Input.Oxyz[p->DrivingAxis] == 0)
		p->AngAdjEn = 0;
	else
		p->AngAdjEn = 1;
}

//输入更新
bool MR_Input(MotorResolving_TypeDef* p, u32 in_Ms)
{
	//ms传递
	p->Ms = in_Ms;

	return 1;
}

//非行走模式时清除输出 并 提前退出
bool MR_WalkMode_NULL(MotorResolving_TypeDef* p)
{
	u8 i = 0;
	bool res = 0;

	if (p->Input.WalkMode == MR_WALK_POS
		|| p->Input.WalkMode == MR_WALK_INF
		|| p->Input.WalkMode == MR_WALK_ROT
		)//行走模式 什么都不做
	{
		res = 0;
	}
	else//非行走模式 清除输出 并反回1 告诉调用者提前退出解算
	{
		for (i = 0; i < p->Para.MotorNumber; i++)
		{
			p->Output.WheelSpeed[i] = 0;
		}
		res = 1;
	}
	return res;
}

//获取圆心坐标
bool MotorResolving_GitRotPointAxis(MotorResolving_TypeDef* p)
{
	//正向模式下 直接使用输入的坐标作为圆心坐标 参数有效时
	if (p->Input.WalkMode == MR_WALK_POS && p->Para.Bit.CentreCtrl)
	{
		p->RotPointX = p->Input.RotPoint.X;
		p->RotPointY = p->Input.RotPoint.Y;
	}
	else
		MotorResolving_RotPointAxisCal(p);

	return 1;
}

//整车差动 计算轮子运动半径
bool MotorResolving_GitWheelRd_EntiretyDifSpeed(MotorResolving_TypeDef* p)
{
	if (p->Input.WalkMode == MR_WALK_POS)
	{
		p->RotDis = p->Para.MaxRotDisPos - ((p->Para.MaxRotDisPos - p->Para.MinRotDisPos) * ((fabs(p->Input.Aoxyz[2]) / MR_MaxSkewHandle)));
		if (p->Input.Aoxyz[2] > 0)
		{
			p->WheelRd[0] = p->RotDis - p->Para.Lc;
			p->WheelRd[1] = p->RotDis + p->Para.Lc;
		}
		else if (p->Input.Aoxyz[2] < 0)
		{
			p->WheelRd[0] = p->RotDis + p->Para.Lc;
			p->WheelRd[1] = p->RotDis - p->Para.Lc;
		}
	}
	else if (p->Input.WalkMode == MR_WALK_ROT)/*差速轮自转模式半径特殊处理都为1*/
	{
		p->WheelRd[0] = 1;
		p->WheelRd[1] = 1;
	}
	else {}

	return 1;
}

/**************************************************************************
* 函数名:MotorResolving_Main
* 功  能:解算调用主方法
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
bool MotorResolving_Main(MotorResolving_TypeDef* p, u32 in_Ms)
{
	//u8  i = 0; 

	//输入更新
	MR_Input(p, in_Ms);

	//非行走模式时清除输出 并 提前退出
	if (MR_WalkMode_NULL(p))
		return 1;
	else {}

	if (p->Para.WheelType == MR_WheelType_DifSpeed || p->Para.WheelType == MR_WheelType_TurnIndependent)//差速或独立转向
	{
		/*处理差动速度使能*/
		MotorResolving_GetAngAdjEn(p);

		/*计算复合运动参数*/
		MotorResolving_Xyz2Geometry(p);

		//获取圆心坐标
		MotorResolving_GitRotPointAxis(p);

		/*计算半径和目标角度*/
		MotorResolving_WhellRdAndTgtAngCal(p);
	}
	else if (p->Para.WheelType == MR_WheelType_EntiretyDifSpeed)//整车差动
		MotorResolving_GitWheelRd_EntiretyDifSpeed(p);
	else {}

	/*半径缩放*/
	MotorResolving_WheelRdZoom(p);

	/*电机速度计算*/
	MotorResolving_MotorSpeedCal(p);

	/*电机速度缩放*/
	MotorResolving_WheelSpeedZoom(p);

	/*虚拟差动
	MotorResolving_VirAngRun(p);
	for(i=0;i<p->Para.ModuleNumber;i++)
		p->Input.ActualAng[i]=p->VirAng.VirAng[i];*/

	return (true);
}

//象限判断
bool MRSI_QuadrantJudge(MotorResolving_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		//象限判断
		//20240905更新，327项目现场测试发现3好模组自转异常，原因是象限判断错误
		//目前使用此版的392项目模组不在3出问题的象限，所以不会出现问题
		//之前的项目用的CBB版本，没有此象限判断，所以不会出现问题
		if (p->Para.Mxy[i].X_Axis == 0 && p->Para.Mxy[i].Y_Axis == 0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_Origin;
		else if (p->Para.Mxy[i].X_Axis > 0 && p->Para.Mxy[i].Y_Axis > 0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_First;
		else if (p->Para.Mxy[i].X_Axis < 0 && p->Para.Mxy[i].Y_Axis>0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_Second;
		else if (p->Para.Mxy[i].X_Axis < 0 && p->Para.Mxy[i].Y_Axis < 0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_Third;
		else if (p->Para.Mxy[i].X_Axis > 0 && p->Para.Mxy[i].Y_Axis < 0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_Fourth;
		else if (p->Para.Mxy[i].X_Axis == 0 && p->Para.Mxy[i].Y_Axis > 0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_YPositive;
		else if (p->Para.Mxy[i].X_Axis == 0 && p->Para.Mxy[i].Y_Axis < 0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_YMinus;
		else if (p->Para.Mxy[i].X_Axis > 0 && p->Para.Mxy[i].Y_Axis == 0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_XPositive;
		else if (p->Para.Mxy[i].X_Axis < 0 && p->Para.Mxy[i].Y_Axis == 0)
			p->Para.Mxy[i].Quadrant = MR_Quadrant_XMinus;
		else {}
	}
	return 1;
}

//电机速度方向初始化
bool MRSI_MotorDirInit(MotorResolving_TypeDef* p)
{
	char i = 0;
	MR_QuadrantEnum Quadrant = MR_Quadrant_NULL;

	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		Quadrant = p->Para.Mxy[i].Quadrant;

		//正向模式 电机速度方向初始化
		p->WheelDirPos[i * 2] = -1;
		p->WheelDirPos[i * 2 + 1] = 1;

		//横向模式 电机速度方向初始化
		if (p->Para.Mxy[i].Inf_Invflg == 0)
		{
			p->WheelDirInf[i * 2] = -1;
			p->WheelDirInf[i * 2 + 1] = 1;
		}
		else//如果横向方向取反,对应象限模组横向速度符号取反
		{
			p->WheelDirInf[i * 2] = 1;
			p->WheelDirInf[i * 2 + 1] = -1;
		}

		//自转模式 电机速度方向初始化
		if (
			Quadrant == MR_Quadrant_Second || Quadrant == MR_Quadrant_Third
			|| Quadrant == MR_Quadrant_YPositive  || Quadrant == MR_Quadrant_XMinus
			)//第二象限、第三象限、Y正轴、x负轴上，左边轮子在外侧
		{
			p->WheelDirRot[i * 2] = 1;
			p->WheelDirRot[i * 2 + 1] = -1;
		}
		else if (
			Quadrant == MR_Quadrant_First || Quadrant == MR_Quadrant_Fourth
			|| Quadrant == MR_Quadrant_YMinus || Quadrant == MR_Quadrant_XPositive
			)//第一象限、第四象限、Y负轴上、
		{
			p->WheelDirRot[i * 2] = -1;
			p->WheelDirRot[i * 2 + 1] = 1;
		}
		else//在原点上，模组朝前，原地自转
		{
			p->WheelDirRot[i * 2] = 1;
			p->WheelDirRot[i * 2 + 1] = 1;
		}

		//如果在y轴上，横向方向又取反，则模组目标角度符号反，电机方向反
		if (
			(Quadrant == MR_Quadrant_YMinus ||  Quadrant == MR_Quadrant_YPositive)
			&& p->Para.Mxy[i].Inf_Invflg
		)
		{
			p->WheelDirRot[i * 2] = -p->WheelDirRot[i * 2];
			p->WheelDirRot[i * 2 + 1] = -p->WheelDirRot[i * 2 + 1];
		}
		else {}
	}
	return 1;
}

//轮子Lc参数符号初始化
bool MRSI_RdSignInit(MotorResolving_TypeDef* p)
{
	char i = 0;
	MR_QuadrantEnum Quadrant = MR_Quadrant_NULL;

	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		Quadrant = p->Para.Mxy[i].Quadrant;

		//非自转模式 轮子Lc参数符号
		p->PosModeRdSign[i * 2] = -1;
		p->PosModeRdSign[i * 2 + 1] = 1;

		//自转模式 轮子Lc参数符号
		//if(p->Para.Mxy[i].X_Axis <= 0)//位于第2 3象限、及y轴上的模组，左边轮在外侧
		if (
			Quadrant == MR_Quadrant_Second || Quadrant == MR_Quadrant_Third
			|| Quadrant == MR_Quadrant_YPositive || Quadrant == MR_Quadrant_XMinus
			)//第二象限、第三象限、Y正轴、X负轴上，左边轮在外侧
		{
			p->RotModeRdSign[i * 2] = 1;
			p->RotModeRdSign[i * 2 + 1] = -1;
		}
		else if (
			Quadrant == MR_Quadrant_First || Quadrant == MR_Quadrant_Fourth
			|| Quadrant == MR_Quadrant_YMinus || Quadrant == MR_Quadrant_XPositive
			)//第一象限、第四象限、Y负轴、X正轴上，右边轮在内侧
		{
			p->RotModeRdSign[i * 2] = -1;
			p->RotModeRdSign[i * 2 + 1] = 1;
		}
		else//在原点上，两边轮都在外侧
		{
			p->RotModeRdSign[i * 2] = 1;
			p->RotModeRdSign[i * 2 + 1] = 1;
		}

		//如果在y轴上，横向方向又取反，则模组目标角度符号反，轮子Rd参数符号反
		if (p->Para.Mxy[i].X_Axis == 0 && p->Para.Mxy[i].Inf_Invflg == 1)
		{
			p->PosModeRdSign[i * 2] = -p->PosModeRdSign[i * 2];
			p->PosModeRdSign[i * 2 + 1] = -p->PosModeRdSign[i * 2 + 1];
		}
		else {}
	}
	return 1;
}

//自转模式 目标角度符号初始化
bool MRSI_RotModeAngSignInit(MotorResolving_TypeDef* p)
{
	char i = 0;
	MR_QuadrantEnum Quadrant = MR_Quadrant_NULL;

	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		Quadrant = p->Para.Mxy[i].Quadrant;
		//自转模式 目标角度符号
		/*坐标
		if (
			(p->Para.Mxy[i].X_Axis<0 && p->Para.Mxy[i].Y_Axis>0)
			||(p->Para.Mxy[i].X_Axis>0 && p->Para.Mxy[i].Y_Axis<0)
		)*/
		/*位置枚举*/
		if (Quadrant == MR_Quadrant_Second || Quadrant == MR_Quadrant_Fourth)//第2 4象限,目标角度向右,在坐标轴和原点时目标角度0度或90度特殊处理
			p->RotModeAngSign[i] = -1;
		else
			p->RotModeAngSign[i] = 1;
	}

	//差速轮自转时符号特殊处理
	if (p->Para.WheelType == MR_WheelType_EntiretyDifSpeed)
	{
		p->WheelDirRot[0] = 1;
		p->WheelDirRot[1] = 1;
	}
	else {}

	return 1;
}

/**************************************************************************
* 函数名:MotorResolving_SignInit
* 功  能:解算数据符号初始化
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_SignInit(MotorResolving_TypeDef* p)
{
	//象限判断
	MRSI_QuadrantJudge(p);

	//电机速度方向初始化
	MRSI_MotorDirInit(p);

	//轮子Lc参数符号初始化
	MRSI_RdSignInit(p);

	//自转模式 目标角度符号初始化
	MRSI_RotModeAngSignInit(p);
}

/**************************************************************************
* 函数名:MotorResolving_VirAngRun
* 功  能:模块仿真时虚拟角度 差速积分
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
void MotorResolving_VirAngRun(MotorResolving_TypeDef* p, u32 in_Ms)
{
	char i = 0;

	//计算时间差
	p->VirAng.CallDelayMs = in_Ms - p->VirAng.LastTimeMs;
	p->VirAng.LastTimeMs = in_Ms;

	//差速积分
	for (i = 0; i < p->Para.ModuleNumber; i++)
	{
		//模块内部 最终电机速度积分
		//p->VirAng.DiffSpeed[i]=p->Output.WheelSpeed[i*2]+p->Output.WheelSpeed[i*2+1];

		//模块内部 电机差速速度积分，避免圆弧运动时把线速度当成了差动速度
		p->VirAng.DiffSpeed[i] = p->Output.WheelDiffSpeed[i * 2] + p->Output.WheelDiffSpeed[i * 2 + 1];

		//p->VirAng.DiffW[i]=(p->VirAng.DiffSpeed[i]/(p->Para.MotorMaxRpm*2))*p->VirAng.Wmax
		//	/(1000/p->VirAng.CallDelayMs);

		p->VirAng.DiffW[i] = (p->VirAng.DiffSpeed[i] / (p->Para.MotorMaxRpm * 2)) * p->VirAng.Wmax
			* (p->VirAng.CallDelayMs / 1000.0);

		if (p->VirAng.DiffW[i] > fabs(p->VirAng.VirAng[i] - p->TgtAng[i])) //如果积分超过目标角度，则直接赋值目标角度
			p->VirAng.VirAng[i] = p->TgtAng[i];
		else
			p->VirAng.VirAng[i] = p->VirAng.VirAng[i] + p->VirAng.DiffW[i];
	}
}

/************************************END OF FILE************************************/

