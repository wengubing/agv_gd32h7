/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_MagneticTrace.c   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2023.12.21-20240706         
* Description: 输入磁条偏移，输出整车控制进行巡线
*******************************************************************************/

/********************************** 设计笔记 ***********************************
* 前桥转向巡线模式实现ok，仿真模拟测试
* 差速轮前侧传感器巡线模式实现ok，仿真模拟测试
* 麦轮前后巡线ok,仿真模拟测试
* 麦轮前侧巡线ok,仿真模拟测试
*
*******************************************************************************/

/*包含以下头文件*/
#include <math.h>
#include "..\..\CBB\user\basecalfunc\basecalfunc.h"
#include "app_MagneticTrace.h"

/*在外部实现的方法声明*/
extern  float32_t arm_sin_f32(float32_t x);
extern  float32_t arm_cos_f32(float32_t x);

/**************************************************************************
* 函数名:  MagneticTrace_ModeInit()
* 功  能:区分正向横向进行控制初始化
* 参  数:MagneticTrace_TypeDef *p
* 返回值:void
**************************************************************************/
void MagneticTrace_ModeInit(MagneticTrace_TypeDef *p)
{	
	//char i=0;
	if(p->in_Mode==MagneticTraceMode_Pos)
	{
		p->GoMode=1;//传入模式
		p->FSpeed=p->in_Velocity;//传入前向速度
		p->MagDiffCtrl[MT_NoOfMagCtrlF]=p->in_MagValue[0];//据模式传入磁条偏移
		p->MagDiffCtrl[MT_NoOfMagCtrlB]=p->in_MagValue[1];
		p->MaxMagLimit=p->in_MagMax[0];
	}
	else if(p->in_Mode==MagneticTraceMode_Inf)
	{
		p->GoMode=2;
		p->FSpeed=p->in_Velocity;		
		p->MagDiffCtrl[MT_NoOfMagCtrlF]=p->in_MagValue[2];
		p->MagDiffCtrl[MT_NoOfMagCtrlB]=p->in_MagValue[3];
		p->MaxMagLimit=p->in_MagMax[2];
	}
	else
	{
		p->GoMode=0;
		p->FSpeed=0;
	}
}

/**************************************************************************
* 函数名:  MagneticTrace_LimitInit()
* 功  能:初始化 巡线方向偏移 最大斜行角度 最小半径限制 纠正角度计算
* 参  数:MagneticTrace_TypeDef *p
* 返回值:void
**************************************************************************/
void MagneticTrace_LimitInit(MagneticTrace_TypeDef *p)
{	
	char i=0;

	if(p->GoMode==1)
	{
		p->MaxAngLimit=p->in_AngPosMax;
		p->MinRLimit=p->in_RadiusPosMin;
	}
	else if(p->GoMode==2)
	{
		p->MaxAngLimit=p->in_AngInfMax;
		p->MinRLimit=p->in_RadiusInfMin;
	}
	else
	{
		p->MagDiffCtrl[MT_NoOfMagCtrlF]=0;
		p->MagDiffCtrl[MT_NoOfMagCtrlB]=0;
	}	

	for(i=0;i<MT_NumOfMagCtrl;i++)
	{
		p->GolineAng[i]=RatioFuc(p->MagDiffCtrl[i],0,//计算纠正角度
								p->MaxMagLimit,0,p->MaxAngLimit);
		p->GolineAng[i]=AbsFuc(p->GolineAng[i], p->MaxAngLimit);//限制纠正角度至最大斜行角度
	}

	if(p->FSpeed>0)
		p->TglCalTemp.Sign4=+1;
	else
		p->TglCalTemp.Sign4=-1;
}

/**************************************************************************
* 函数名:  MagneticTrace_CalCmd()
* 功  能:计算控制输出
* 参  数:MagneticTrace_TypeDef *p
* 返回值:void
**************************************************************************/
void MagneticTrace_CalCmd(MagneticTrace_TypeDef *p)
{	
	//Ang---> Vx
	if(fabs(p->OutCtrlAng)>p->MaxAngLimit)
	{
		p->OutCtrlAng=AbsFuc(p->OutCtrlAng,p->MaxAngLimit);
		p->flg_AngMaxLmt=1;
	}
	else
	{
		p->flg_AngMaxLmt=0;
	}

	if (p->GolineAng[MT_NoOfMagCtrlF] == -p->GolineAng[MT_NoOfMagCtrlB])
	{//关闭斜行
		p->out_CmdAngle = 0;
	}
	else
	{//斜行与圆弧都调整
		p->out_CmdAngle = -p->OutCtrlAng * MT_MaxSkewHandle / p->MaxAngLimit;
	}

	//R --->Vz
	if (p->GolineAng[MT_NoOfMagCtrlF] == p->GolineAng[MT_NoOfMagCtrlB])
	{//姿态完全0° 仅斜行
		p->out_CmdRadius = 0;
	}
	else if (p->OutCtrlR > MT_MaxRotDis_All)
	{
		//p->out_CmdRadius = 0;									//仅平移 可能稳态误差较大
		p->out_CmdRadius = p->TglCalTemp.Signr * 1;				//以最小摇杆值纠正
	}
	else
	{
		if(p->OutCtrlR<p->MinRLimit)
		{
			p->OutCtrlR=p->MinRLimit;
			p->flg_RMinLmt=1;
		}
		else
		{
			p->flg_RMinLmt=0;
		}
		
		p->out_CmdRadius = p->TglCalTemp.Signr
							*(MT_MaxRotDis_All-p->OutCtrlR)/(MT_MaxRotDis_All-p->MinRLimit)
							*MT_MaxSkewHandle;
	}
}	

/**************************************************************************
* 函数名:  MagneticTrace_SignInit()
* 功  能:确定符号
* 参  数:MagneticTrace_TypeDef *p
* 返回值:void
**************************************************************************/
void MagneticTrace_SignInit(MagneticTrace_TypeDef *p)
{
	if(p->GolineAng[MT_NoOfMagCtrlF]>=0)
	{//前方向右
		if(fabs(p->GolineAng[MT_NoOfMagCtrlF])>=fabs(p->GolineAng[MT_NoOfMagCtrlB]))
		{
			p->TglCalTemp.Sign1=-1;
			p->TglCalTemp.Sign2=+1;
			p->TglCalTemp.Sign3=-1;
			p->TglCalTemp.Signr=+1;
		}
		else if(fabs(p->GolineAng[MT_NoOfMagCtrlF])<fabs(p->GolineAng[MT_NoOfMagCtrlB]))
		{
			if(p->GolineAng[MT_NoOfMagCtrlB]>0)
			{
				p->TglCalTemp.Sign1=+1;
				p->TglCalTemp.Sign2=-1;
				p->TglCalTemp.Sign3=+1;
				p->TglCalTemp.Signr=-1;
			}
			else if(p->GolineAng[MT_NoOfMagCtrlB]<0)
			{
				p->TglCalTemp.Sign1=-1;
				p->TglCalTemp.Sign2=+1;
				p->TglCalTemp.Sign3=-1;
				p->TglCalTemp.Signr=+1;
			}
		}
	}
	else if(p->GolineAng[MT_NoOfMagCtrlF]<0)
	{//前方向左
		if(fabs(p->GolineAng[MT_NoOfMagCtrlF])>=fabs(p->GolineAng[MT_NoOfMagCtrlB]))
		{
			p->TglCalTemp.Sign1=+1;
			p->TglCalTemp.Sign2=-1;
			p->TglCalTemp.Sign3=+1;
			p->TglCalTemp.Signr=-1;
		}
		else if(fabs(p->GolineAng[MT_NoOfMagCtrlF])<fabs(p->GolineAng[MT_NoOfMagCtrlB]))
		{
			if(p->GolineAng[MT_NoOfMagCtrlB]>0)
			{
				p->TglCalTemp.Sign1=+1;
				p->TglCalTemp.Sign2=-1;
				p->TglCalTemp.Sign3=+1;
				p->TglCalTemp.Signr=-1;
			}
			else if(p->GolineAng[MT_NoOfMagCtrlB]<0)
			{
				p->TglCalTemp.Sign1=-1;
				p->TglCalTemp.Sign2=+1;
				p->TglCalTemp.Sign3=-1;
				p->TglCalTemp.Signr=+1;
			}
		}
	}
}

/**************************************************************************
* 函数名:  MagneticTrace_CalAngAndRadius()
* 功  能:复合运动拟合
* 参  数:MagneticTrace_TypeDef *p
* 返回值:void
**************************************************************************/
void MagneticTrace_CalAngAndRadius(MagneticTrace_TypeDef *p)
{
	p->TglCalTemp.A_AngF=90+p->TglCalTemp.Sign1*p->GolineAng[MT_NoOfMagCtrlF];
	p->TglCalTemp.A_AngB=90+p->TglCalTemp.Sign2*p->GolineAng[MT_NoOfMagCtrlB];

	p->TglCalTemp.L_FB=2*p->in_DistancePos;

	p->TglCalTemp.A_FOB=180-fabs(p->TglCalTemp.A_AngF)-fabs(p->TglCalTemp.A_AngB);

	//正弦定理，由于限制前后调整角度小于45°，所以不会出现钝角三角形解
	p->TglCalTemp.Temp[0]=arm_sin_f32(p->TglCalTemp.A_AngB*K_AngleToRad);
	p->TglCalTemp.Temp[1]=arm_sin_f32(p->TglCalTemp.A_FOB*K_AngleToRad);
	p->TglCalTemp.L_AngB=2*p->in_DistancePos*p->TglCalTemp.Temp[0]/p->TglCalTemp.Temp[1];

	//余弦定理，唯一解
	p->TglCalTemp.Temp[2]=arm_cos_f32(p->TglCalTemp.A_AngF*K_AngleToRad);
	p->OutCtrlR=sqrt(pow(p->in_DistancePos,2)+pow(p->TglCalTemp.L_AngB,2)
										-2*p->TglCalTemp.L_AngB*p->in_DistancePos*p->TglCalTemp.Temp[2]);

	//正弦定理
	p->TglCalTemp.Temp[3]=arm_sin_f32(p->TglCalTemp.A_AngF*K_AngleToRad);
	p->TglCalTemp.Temp[4]=p->TglCalTemp.L_AngB*p->TglCalTemp.Temp[3]
										/p->OutCtrlR;
	p->TglCalTemp.A_L_AngB=asin(p->TglCalTemp.Temp[4])*K_RadToAngle;

	//解判定
	if(fabs(p->GolineAng[MT_NoOfMagCtrlF])>fabs(p->GolineAng[MT_NoOfMagCtrlB]))
		p->TglCalTemp.A_L_AngB=180-p->TglCalTemp.A_L_AngB;
	
	p->OutCtrlAng=p->TglCalTemp.Sign3*(90-p->TglCalTemp.A_L_AngB);
}

/**************************************************************************
* 函数名:  MagneticTrace_Main()
* 功  能: 磁条巡线模块主方法
* 参  数: 模块对象地址
* 返回值: 正常返回0，异常返回1
**************************************************************************/
bool MagneticTrace_Main(MagneticTrace_TypeDef *p)
{	
	short signtemp_x=0,signtemp_z=0;

	MagneticTrace_ModeInit(p);

	MagneticTrace_LimitInit(p);
	
	if(p->in_TraceMode==MagneticTrace_TraceMode_FrontAndBack)
	{
		if(p->GoMode!=0)
		{
			if(p->GolineAng[MT_NoOfMagCtrlF]==0 && p->GolineAng[MT_NoOfMagCtrlB]==0)
			{//直行
				p->OutCtrlAng=0;
				p->OutCtrlR=MT_MaxRotDis_All+1;
			}
			else if(0)//p->GolineAng[MT_NoOfMagCtrlF]==0)
			{//仅后侧调整

			}
			else if(0)//p->GolineAng[MT_NoOfMagCtrlB]==0)
			{//仅前侧调整

			}
			else if(p->GolineAng[MT_NoOfMagCtrlF]==p->GolineAng[MT_NoOfMagCtrlB])
			{//仅斜行
				p->OutCtrlAng=p->GolineAng[MT_NoOfMagCtrlF];
				p->OutCtrlR=MT_MaxRotDis_All+1;
			}
			else
			{//斜行与圆弧都调整,复合运动拟合
				MagneticTrace_SignInit(p);
				MagneticTrace_CalAngAndRadius(p);
			}
			
		}

		MagneticTrace_CalCmd(p);//复合运动反算到遥控控制命令
	}
	else if(p->in_TraceMode==MagneticTrace_TraceMode_FrontOnly)
	{
		if(p->FSpeed>0)
			p->out_CmdCarAngle = -(p->MagDiffCtrl[MT_NoOfMagCtrlF] / p->MaxMagLimit) * p->in_CmdVxMax;
		else if(p->FSpeed<0)
			p->out_CmdCarAngle = (p->MagDiffCtrl[MT_NoOfMagCtrlB] / p->MaxMagLimit) * p->in_CmdVxMax;
		else
			p->out_CmdCarAngle=0;
	}
	else if(p->in_TraceMode==MagneticTrace_TraceMode_DiffVCar)
	{
		if(p->FSpeed>0)
			p->out_CmdRadius = -(p->MagDiffCtrl[MT_NoOfMagCtrlF] / p->MaxMagLimit) * p->in_CmdVzMax;
		else if(p->FSpeed<0)
			p->out_CmdRadius = (p->MagDiffCtrl[MT_NoOfMagCtrlB] / p->MaxMagLimit) * p->in_CmdVzMax;
		else
			p->out_CmdRadius=0;
	}
	else if(p->in_TraceMode==MagneticTrace_TraceMode_MecanumFrontAndBack)
	{
		if(p->FSpeed>0)
		{
			signtemp_x=-1;
			signtemp_z=1;
		}
		else if(p->FSpeed<0)
		{
			signtemp_x=-1;
			signtemp_z=1;
		}
		else
		{
			signtemp_x=0;
			signtemp_z=0;
		}

		p->out_CmdAngle = signtemp_x*((p->MagDiffCtrl[MT_NoOfMagCtrlF]+p->MagDiffCtrl[MT_NoOfMagCtrlB]) / p->MaxMagLimit*2)
							* p->in_CmdVxMax;
		p->out_CmdRadius = signtemp_z*((p->MagDiffCtrl[MT_NoOfMagCtrlF]-p->MagDiffCtrl[MT_NoOfMagCtrlB]) / p->MaxMagLimit*2)
							* p->in_CmdVzMax;

		
	}
	else if(p->in_TraceMode==MagneticTrace_TraceMode_MecanumFrontOnly)
	{
		if(p->FSpeed>0)
			p->out_CmdCarAngle = -(p->MagDiffCtrl[MT_NoOfMagCtrlF] / p->MaxMagLimit) * p->in_CmdVxMax;
		else if(p->FSpeed<0)
			p->out_CmdCarAngle = (p->MagDiffCtrl[MT_NoOfMagCtrlB] / p->MaxMagLimit) * p->in_CmdVxMax;
		else
			p->out_CmdCarAngle=0;
	}

	return 0;
}

/**************************************************************************
* 函数名:MagneticTrace_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:MotorResolving_TypeDef *p
* 返回值:无
**************************************************************************/
bool MagneticTrace_InitExample(MagneticTrace_TypeDef *p)
{
	char i=0;
	for(i=0;i<4;i++)
	{
		p->in_MagMax[i]=100;
	}
	p->in_AngPosMax=20;
	p->in_AngInfMax=10;
	p->in_RadiusPosMax=60000;
	p->in_RadiusPosMin=5000;
	p->in_RadiusInfMax=60000;
	p->in_RadiusInfMin=6000;
	p->in_DistancePos=5000;
	p->in_DistanceInf=5000;

	/*实时输入*/
	p->in_MagValue[0]=100;
	p->in_Mode=MagneticTraceMode_Pos;
	p->in_Velocity=100;
	
	p->in_TraceMode=MagneticTrace_TraceMode_DiffVCar;
	return 0;
}

/************************************END OF FILE************************************/


