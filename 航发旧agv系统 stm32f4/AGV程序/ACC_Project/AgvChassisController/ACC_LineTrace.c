/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_LineTrace.c
** 创 建 人: Master 
** 描    述: 
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年4月22日
**--------------------------------------------------------------------------------------
***************************************************************************************/  

//声明对象在此文件编译时实例化
#define  ACC_LineTrace_GLOBALS
#include "AgvChassisController.h"
#include "ACC_LineTrace.h"

#include "ACC_AssemlyCtrl.h"

#include "app_SlamTrackLine.h"


/**************************************************************************
* 函数名:ACC_ModeSMTIsTrace
* 功  能:是巡线模式判断
* 参  数:
* 返回值:bool
**************************************************************************/
bool ACC_ModeSMTIsTrace(ACC_DATA_TypeDef *p)
{
	if(
		p->ModeSwitchSMT==ACCMode_PosTrace
		||p->ModeSwitchSMT==ACCMode_InfTrace
	)
		return 1;
	else
		return 0;
}

/**************************************************************************
* 函数名:ACC_LineTrace_Input
* 功  能:
* 参  数:
* 返回值:
**************************************************************************/
void ACC_LineTrace_Input(ACC_DATA_TypeDef *p)
{
	MagneticTrace_TypeDef *p2LT=&g_MagneticTrace;
	
	float in_MagValue_F = 0.0f;
	float in_MagValue_B = 0.0f;
	float in_MagValue_L = 0.0f;
	float in_MagValue_R = 0.0f;
	//磁条偏移量输入
	{
		//确定当前应该用什么传感器的结果，送入到磁条巡线模块中
		switch(g_Para.TarceType.FunType)
		{
			default:
			case Para_TarceType_Null_Fun:{}break;

			case Para_TarceType_Magnetic:
			{
				in_MagValue_F =  g_MNData[0].MN_Read.Distance;
				in_MagValue_B = -g_MNData[1].MN_Read.Distance;
				in_MagValue_L = -g_MNData[2].MN_Read.Distance;
				in_MagValue_R =  g_MNData[3].MN_Read.Distance;

			}break;

			case Para_TarceType_ColorBand:
			{
				in_MagValue_F =  g_ColorBand[0].ColorBand_Deviation.Distance;
				in_MagValue_B =  g_ColorBand[1].ColorBand_Deviation.Distance;
				in_MagValue_L = -g_ColorBand[2].ColorBand_Deviation.Distance;
				in_MagValue_R = -g_ColorBand[3].ColorBand_Deviation.Distance;
			}break;

			case Para_TarceType_Slam:
			{
				in_MagValue_F = -0.1f*(g_VirtualMagnetic.MagOffset[0]);
				in_MagValue_B =  0.1f*(g_VirtualMagnetic.MagOffset[1]);
				in_MagValue_L = -0.1f*(g_VirtualMagnetic.MagOffset[2]);
				in_MagValue_R =  0.1f*(g_VirtualMagnetic.MagOffset[3]);
			}break;
			case Para_TarceType_Slam_Magnetic:
			{
				uint8_t Mag_F_sta = g_MNData[0].MN_Read.Status;
				uint8_t Mag_B_sta = g_MNData[1].MN_Read.Status;
				uint8_t Mag_L_sta = g_MNData[2].MN_Read.Status;
				uint8_t Mag_R_sta = g_MNData[3].MN_Read.Status;
				if(Mag_F_sta==1)	{in_MagValue_F = g_MNData[0].MN_Read.Distance;}    
				else				{in_MagValue_F = -0.1f*(g_VirtualMagnetic.MagOffset[0]);}

				if(Mag_B_sta==1)	{in_MagValue_B = -g_MNData[1].MN_Read.Distance;}
				else				{in_MagValue_B =  0.1f*(g_VirtualMagnetic.MagOffset[1]);}

				if(Mag_L_sta==1)	{in_MagValue_L = -g_MNData[2].MN_Read.Distance;}
				else				{in_MagValue_L =  -0.1f*g_VirtualMagnetic.MagOffset[2];}

				if(Mag_R_sta==1)    {in_MagValue_R = g_MNData[3].MN_Read.Distance;}
				else				{in_MagValue_R =  0.1f*g_VirtualMagnetic.MagOffset[3];}

			}break;
			case Para_TarceType_Slam_ColorBand:
			{
				uint8_t ColorBand_F_sta = g_ColorBand[0].ColorBand_Deviation.Status;
				uint8_t ColorBand_B_sta = g_ColorBand[1].ColorBand_Deviation.Status;
				uint8_t ColorBand_L_sta = g_ColorBand[2].ColorBand_Deviation.Status;
				uint8_t ColorBand_R_sta = g_ColorBand[3].ColorBand_Deviation.Status;
				if(ColorBand_F_sta==1)	{in_MagValue_F = g_ColorBand[0].ColorBand_Deviation.Distance;}    
				else					{in_MagValue_F = -0.1f*(g_VirtualMagnetic.MagOffset[0]);}

				if(ColorBand_B_sta==1)	{in_MagValue_B = g_ColorBand[1].ColorBand_Deviation.Distance;}
				else					{in_MagValue_B =  0.1f*(g_VirtualMagnetic.MagOffset[1]);}

				if(ColorBand_L_sta==1)	{in_MagValue_L = -g_ColorBand[3].ColorBand_Deviation.Distance;}
				else					{in_MagValue_L =  -0.1f*g_VirtualMagnetic.MagOffset[2];}

				if(ColorBand_R_sta==1)	{in_MagValue_R = -g_ColorBand[2].ColorBand_Deviation.Distance;}
				else					{in_MagValue_R =  0.1f*g_VirtualMagnetic.MagOffset[3];}

			}break;

			case Para_TarceType_QR:
			{
				
			}break;
		}
		//CBB输入要求为：人与车的前后方向一致，前后磁条偏左时给正值；左右磁条偏后时给正值
		{
			p2LT->in_MagValue[0] = in_MagValue_F;
			p2LT->in_MagValue[1] = in_MagValue_B;
			p2LT->in_MagValue[2] = in_MagValue_L;
			p2LT->in_MagValue[3] = in_MagValue_R;
		}
	}
	//运行模式和前进速度输入
	{
		if(p->CalCmd.Mode == ACCMode_PosTrace)
		{
			p2LT->in_Mode = MagneticTraceMode_Pos;
			p2LT->in_Velocity = p->CalCmd.Vxyz.y;//前向速度输入
		}
		else if(p->CalCmd.Mode == ACCMode_InfTrace)
		{
			p2LT->in_Mode = MagneticTraceMode_Inf;
			p2LT->in_Velocity = p->CalCmd.Vxyz.x;//前向速度输入
		}
	}

}

float k_TraceX = 0;
float k_TraceZ = 0;

/**************************************************************************
* 函数名:ACC_LineTrace_Output
* 功  能:
* 参  数:
* 返回值:
**************************************************************************/
void ACC_LineTrace_Output(ACC_DATA_TypeDef *p)
{
	float k_TraceXMin_temp = g_Para.CtrlCoef.k_TraceXMin/1000.0f;
	float k_TraceXMax_temp = g_Para.CtrlCoef.k_TraceXMax/1000.0f;
	float k_TraceZMin_temp = g_Para.CtrlCoef.k_TraceZMin/1000.0f;
	float k_TraceZMax_temp = g_Para.CtrlCoef.k_TraceZMax/1000.0f;

	MagneticTrace_TypeDef *p2LT = &g_MagneticTrace;
	//纠正量变比例控制计算,速度大时比例小！
	if (abs(p2LT->in_Velocity) < g_Para.CtrlCoef.VMin)
	{
		k_TraceX = k_TraceXMax_temp;
		k_TraceZ = k_TraceZMax_temp;
	}
	else if(abs(p2LT->in_Velocity) > g_Para.CtrlCoef.VMax)
	{
		k_TraceX = k_TraceXMin_temp;
		k_TraceZ = k_TraceZMin_temp;
	}
	else
	{
		k_TraceX = RatioStand(abs(p2LT->in_Velocity),
			g_Para.CtrlCoef.VMin, g_Para.CtrlCoef.VMax,
			k_TraceXMax_temp, k_TraceXMin_temp);
		k_TraceZ = RatioStand(abs(p2LT->in_Velocity),
			g_Para.CtrlCoef.VMin, g_Para.CtrlCoef.VMax,
			k_TraceZMax_temp, k_TraceZMin_temp);
	}

	//获取纠正控制量
	if(p->ChassisType ==ACC_ChassisType_Ant)//转向舵轮
	{
		if(p->CalCmd.Mode == ACCMode_PosTrace)
		{
			p->CalCmd.Axyz.x = p2LT->out_CmdAngle * k_TraceX;
			p->CalCmd.Axyz.z = p2LT->out_CmdRadius * k_TraceZ;
		}
		else if(p->CalCmd.Mode == ACCMode_InfTrace)
		{
			p->CalCmd.Axyz.y = p2LT->out_CmdAngle * k_TraceX;
			p->CalCmd.Axyz.z = p2LT->out_CmdRadius * k_TraceZ;
		}
	}
	else if(p->ChassisType==ACC_ChassisType_Titan || p->ChassisType==ACC_ChassisType_Rhino)//差速舵轮
	{
		if(p->CalCmd.Mode == ACCMode_PosTrace)
		{
			if(p2LT->in_Velocity > 0)
			{
				p->CalCmd.Axyz.x = p2LT->out_CmdAngle * k_TraceX;
				p->CalCmd.Axyz.z = p2LT->out_CmdRadius * k_TraceZ;
			}
			else
			{
				p->CalCmd.Axyz.x = p2LT->out_CmdAngle * -k_TraceX;
				p->CalCmd.Axyz.z = p2LT->out_CmdRadius * -k_TraceZ;
			}
			
		}
		else if(p->CalCmd.Mode == ACCMode_InfTrace)
		{
			if(p2LT->in_Velocity > 0)
			{
				p->CalCmd.Axyz.y = p2LT->out_CmdAngle * k_TraceX;
				p->CalCmd.Axyz.z = p2LT->out_CmdRadius * k_TraceZ;
			}
			else
			{
				p->CalCmd.Axyz.y = p2LT->out_CmdAngle * -k_TraceX;
				p->CalCmd.Axyz.z = p2LT->out_CmdRadius * -k_TraceZ;
			}
		}
	}
	else if(p->ChassisType==ACC_ChassisType_Turtle)//麦轮
	{
		if(p->CalCmd.Mode == ACCMode_PosTrace)
		{
			if(p->CalCmd.Vxyz.y > 1 || p->CalCmd.Vxyz.y< -1)
			{
				p->CalCmd.Vxyz.x = p2LT->out_CmdAngle;
				p->CalCmd.Vxyz.z = p2LT->out_CmdRadius;
			}
			else
			{
				p->CalCmd.Vxyz.x = 0;
				p->CalCmd.Vxyz.z = 0;
			}
			
		}
		else if(p->CalCmd.Mode == ACCMode_InfTrace)
		{
			if(p->CalCmd.Vxyz.x > 1 || p->CalCmd.Vxyz.x< -1)
			{
				p->CalCmd.Vxyz.y = p2LT->out_CmdAngle;
				p->CalCmd.Vxyz.z = p2LT->out_CmdRadius;
			}
			else
			{
				p->CalCmd.Vxyz.y = 0;
				p->CalCmd.Vxyz.z = 0;
			}
			
		}
		else
		{
			p->CalCmd.Vxyz.x = 0;
			p->CalCmd.Vxyz.y = 0;
			p->CalCmd.Vxyz.z = 0;
		}
	}
}


/**************************************************************************
* 函数名:ACC_LineTrace_Process_General
* 功  能:
* 参  数:
* 返回值:
**************************************************************************/
bool ACC_LineTrace_Process_General(ACC_DATA_TypeDef *p)
{
	//输入管道创建
	ACC_LineTrace_Input(p);

	//巡线CBB调用
	MagneticTrace_Main(&g_MagneticTrace);

	//输出管道创建
	ACC_LineTrace_Output(p);

	return 1;
}


/***********************************************************
** 函数名称:	ACC_LineTrace_Process_WheelFollow
** 功能描述: 轮系跟随巡线处理
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//轮系跟随巡线处理
bool ACC_LineTrace_Process_WheelFollow(ACC_DATA_TypeDef* p)
{
	static int16_t MagF_Dis_Temp = 0;
	static int16_t MagB_Dis_Temp = 0;
	static int16_t MagL_Dis_Temp = 0;
	static int16_t MagR_Dis_Temp = 0;

	float k_TraceXMin_temp = g_Para.CtrlCoef.k_TraceXMin / 1000.0f;
	float k_TraceXMax_temp = g_Para.CtrlCoef.k_TraceXMax / 1000.0f;
	float k_TraceZMin_temp = g_Para.CtrlCoef.k_TraceZMin / 1000.0f;
	float k_TraceZMax_temp = g_Para.CtrlCoef.k_TraceZMax / 1000.0f;

	//实时输入
	/*向前时*/
	if (p->CalCmd.VOxyz.y >= 0)
	{
		int16_t MagF = g_MNData[0].MN_Read.Status*g_MNData[0].Diff;
		int16_t MagL = g_MNData[2].MN_Read.Status*g_MNData[2].Diff;
		if(MagF_Dis_Temp != MagF && MagF != 0)
		{
			MagF_Dis_Temp = MagF;
		}
		if(MagL_Dis_Temp != MagL && MagL != 0)
		{
			MagL_Dis_Temp = MagL;
		}
		g_CPC.In_MagValue[CPC_TraceFront]	= MagF_Dis_Temp;
		g_CPC.In_MagValue[CPC_TraceBack]	= MagL_Dis_Temp;
	}
	else//向后时
	{
		
		int16_t MagB = g_MNData[1].MN_Read.Status*g_MNData[1].Diff;
		int16_t MagR = g_MNData[3].MN_Read.Status*g_MNData[3].Diff;
		if(MagB_Dis_Temp != MagB && MagB != 0)
		{
			MagB_Dis_Temp = MagB;
		}
		if(MagR_Dis_Temp != MagR && MagR != 0)
		{
			MagR_Dis_Temp = MagR;
		}
		g_CPC.In_MagValue[CPC_TraceFront]	= MagB_Dis_Temp;
		g_CPC.In_MagValue[CPC_TraceBack]	= MagR_Dis_Temp;
	}

	//速度输入
	g_CPC.In_Velocity = p->CalCmd.VOxyz.y;

	//巡线处理
	CenterPointCal_Run(&g_CPC);

	//无纠正时
    if (g_CPC.Status == CPC_ST_NoCorrect)
	{
		p->CalCmd.Axyz.x = 0;
		p->CalCmd.Axyz.z = 0;
	}
	else if (g_CPC.Status == CPC_ST_SlopeCorrect)//纯平移纠正时
	{
		//纠正量变比例控制计算,速度大时比例小！
		if (abs(g_CPC.In_Velocity) < g_Para.CtrlCoef.VMin)
		{
			k_TraceX = k_TraceXMax_temp;
			k_TraceZ = k_TraceZMax_temp;
		}
		else if (abs(g_CPC.In_Velocity) > g_Para.CtrlCoef.VMax)
		{
			k_TraceX = k_TraceXMin_temp;
			k_TraceZ = k_TraceZMin_temp;
		}
		else
		{
			k_TraceX = RatioStand(abs(g_CPC.In_Velocity),
				g_Para.CtrlCoef.VMin, g_Para.CtrlCoef.VMax,
				k_TraceXMax_temp, k_TraceXMin_temp);
			k_TraceZ = RatioStand(abs(g_CPC.In_Velocity),
				g_Para.CtrlCoef.VMin, g_Para.CtrlCoef.VMax,
				k_TraceZMax_temp, k_TraceZMin_temp);
		}

		//获取纠正控制量
		if (p->ChassisType == ACC_ChassisType_Titan || p->ChassisType == ACC_ChassisType_Rhino || p->ChassisType == ACC_ChassisType_Ant)//舵轮
		{
			if (p->CalCmd.Mode == ACCMode_PosTrace)
			{
				p->CalCmd.Axyz.x = g_CPC.TraceAng[0] * k_TraceX;
				p->CalCmd.Axyz.z = 0;
			}
			else {}
		}
	}
	else if(g_CPC.Status == CPC_ST_CarCorrect || g_CPC.Status == CPC_ST_ArcCorrect || g_CPC.Status == CPC_ST_CompositeCorrect)
	{//有交点的圆弧运动纠正
		//判断圆弧运动半径半径 位置是否超出范围

		//向解算CBB输入信号
		if (p->ChassisType == ACC_ChassisType_Titan || p->ChassisType == ACC_ChassisType_Rhino || p->ChassisType == ACC_ChassisType_Ant)//舵轮
		{
			if (p->CalCmd.Mode == ACCMode_PosTrace)
			{
                p->CalCmd.BitPara.CentreCtrl = 1;

                //根据运动圆心坐标 计算遥杆x、z的符号，不宜过大，解算中只取其符号
                if(g_CPC.CenterPoint.X > 0)//圆心在右侧
                {
                    if(p->CalCmd.Vxyz.y > 0)//前进
                    {
                        p->CalCmd.Axyz.z = -1.0f;
                        if(g_CPC.CenterPoint.Y > 0)//圆心在右上方
                            p->CalCmd.Axyz.x = 1.0f;
                        else//圆心在右下方
                            p->CalCmd.Axyz.x = -1.0f;
                    }
                    else//后退
                    {
                        p->CalCmd.Axyz.z = 1.0f;
                        if(g_CPC.CenterPoint.Y > 0)//圆心在右上方
                            p->CalCmd.Axyz.x = -1.0f;
                        else//圆心在右下方
                            p->CalCmd.Axyz.x = 1.0f;
                    }
                }
                else//圆心在左侧
                {
                    if(p->CalCmd.Vxyz.y > 0)//前进
                    {
                        p->CalCmd.Axyz.z = 1.0f;
                        if(g_CPC.CenterPoint.Y > 0)//圆心在左上方
                            p->CalCmd.Axyz.x = -1.0f;
                        else//圆心在左下方
                            p->CalCmd.Axyz.x = 1.0f;
                    }
                    else//后退
                    {
                        p->CalCmd.Axyz.z = -1.0f;
                        if(g_CPC.CenterPoint.Y > 0)//圆心在左上方
                            p->CalCmd.Axyz.x = -1.0f;
                        else//圆心在左下方
                            p->CalCmd.Axyz.x = 1.0f;
                    }
                }
			}
			else {}//p->CalCmd.BitPara.CentreCtrl = 0; 在前面已经清零了
		}
		else {}
	}
    else {}

	return 1;
}


/***********************************************************
** 函数名称:	ACC_Magnetic_Patrol_Nail
** 功能描述: 巡磁钉控制函数
** 参    数: None
** 返 回 值:	None
***********************************************************/
DisMagNavStepTypedef 	DisMagNavigation;		
RuningStepTypedef		DisMagNav_Step;			/*巡磁钉模块运行状态机*/
bool ACC_Magnetic_Patrol_Nail(ACC_DATA_TypeDef *p)
{
	//巡线方向改变 状态机初始化
	if((DisMagNavData.Input.LineWalking_Mode_Last != DisMagNavData.Input.LineWalking_Mode))
	{
		DisMagNavigation = DisMagNav_Init;
		DisMagNavReInit();
	}
	
    switch ( DisMagNav_Step )
    {
        case Init_Step:
        {
			DisMagNavInit(&DisMagNavData);/*初始化*/
			
			if(g_Sys_1ms_Counter > 3000)
			{
				*DisMagNavData.Output.FixFlag = true;/*重置里程计*/
				*DisMagNavData.Output.FixX = 0;
				*DisMagNavData.Output.FixY = 0;
				*DisMagNavData.Output.FixZ = 9000;/*与里程计坐标不一致，坐标变换*/
				DisMagNav_Step = Runing_Step;
			}
			
        }
		break;

        case Runing_Step:
        {
			if(p->CalCmd.Mode == ACCMode_PosTrace)	/*正向巡线*/	
			{
				/*实时传入磁导航数据*/
				DisMagNavData.Input.FrontMagOffset = g_MNData[0].MN_Read.Distance;			//前磁钉位置
				DisMagNavData.Input.FrontMagStrength = g_MNData[0].MN_Read.Strength;		//前磁钉强度
							
				DisMagNavData.Input.BackMagOffset = g_MNData[1].MN_Read.Distance;			//后磁钉位置
				DisMagNavData.Input.BackMagStrength = g_MNData[1].MN_Read.Strength;			//后磁钉强度
							
				/*传入里程计数据*/
				if(DisMagNavData.Input.LineWalking_Mode == Forward)
				{
					
					DisMagNavData.Input.RTx = g_VirtualOdoData_MG.RTx; //* 2
					DisMagNavData.Input.RTy = g_VirtualOdoData_MG.RTy;
					DisMagNavData.Input.RTz = (g_VirtualOdoData_MG.RTz - 9000) * 4.23f;			
					DisMagNavData.Input.MovingDirection = Forward_Mode;/*正向前进*/
								
				}
				else if(DisMagNavData.Input.LineWalking_Mode == Backward)
				{
					
					DisMagNavData.Input.RTx = -g_VirtualOdoData_MG.RTx;
					DisMagNavData.Input.RTy = -g_VirtualOdoData_MG.RTy;
					DisMagNavData.Input.RTz = (g_VirtualOdoData_MG.RTz - 9000) * 4.23f;
					DisMagNavData.Input.MovingDirection = Backward_Mode;/*正向后退*/
								
				}
				if(p->CalCmd.Mode == ACCMode_PosTrace)
					DisMagNavData.Input.Hy = p->CalCmd.Vxyz.y;/*正向巡线模式*/
							
			}else if(p->CalCmd.Mode == ACCMode_InfTrace)	/*横向巡线*/									
			{
				DisMagNavData.Input.FrontMagOffset = g_MNData[2].MN_Read.Distance;			//前（左）磁钉位置
				DisMagNavData.Input.FrontMagStrength = g_MNData[2].MN_Read.Strength;		//前磁钉强度
								
				DisMagNavData.Input.BackMagOffset = g_MNData[3].MN_Read.Distance;			//后磁钉位置
				DisMagNavData.Input.BackMagStrength = g_MNData[3].MN_Read.Strength;		//后磁钉强度
							
				/*传入里程计数据*/
				if(DisMagNavData.Input.LineWalking_Mode == MoveLeft)
				{
					DisMagNavData.Input.RTx = g_VirtualOdoData_MG.RTy;
					DisMagNavData.Input.RTy = -g_VirtualOdoData_MG.RTx;
					DisMagNavData.Input.RTz = (g_VirtualOdoData_MG.RTz - 9000) * 4.23f;
									
					DisMagNavData.Input.MovingDirection = Forward_Mode;/*前进*//*横向向左*/
								
				}else if(DisMagNavData.Input.LineWalking_Mode == MoveRight)
				{
					DisMagNavData.Input.RTx = -g_VirtualOdoData_MG.RTy;
					DisMagNavData.Input.RTy = g_VirtualOdoData_MG.RTx;
					DisMagNavData.Input.RTz = (g_VirtualOdoData_MG.RTz - 9000 ) * 4.23f; 
									
					DisMagNavData.Input.MovingDirection = Backward_Mode;/*后退*//*横向向右*/
								
				}
				if(p->CalCmd.Mode == ACCMode_InfTrace)
					DisMagNavData.Input.Hy = p->CalCmd.Vxyz.x;/*横向巡线模式*/
			}
				
			DisMagNavData.Input.Distance = g_VirtualOdoData.Distance;					
			//巡磁钉主函数调用
			DisMagNav_Main();
			//获取纠正控制量
			if(p->ChassisType==ACC_ChassisType_Titan || p->ChassisType==ACC_ChassisType_Rhino || p->ChassisType ==ACC_ChassisType_Ant)//舵轮
			{
				if(p->CalCmd.Mode == ACCMode_PosTrace)
				{
					if(DisMagNavData.Input.LineWalking_Mode == Forward)
					{
						p->CalCmd.Vxyz.y = DisMagNavData.Output.ControlY;
						p->CalCmd.Axyz.x = DisMagNavData.Output.ControlX;
						p->CalCmd.Axyz.z = DisMagNavData.Output.ControlZ;
					}
					
					if(DisMagNavData.Input.LineWalking_Mode == Backward)
					{
						p->CalCmd.Vxyz.y = DisMagNavData.Output.ControlY;
						p->CalCmd.Axyz.x = (-1)* DisMagNavData.Output.ControlX;
						p->CalCmd.Axyz.z = (-1)* DisMagNavData.Output.ControlZ;
					}
				}
				else if(p->CalCmd.Mode == ACCMode_InfTrace)
				{
					if(DisMagNavData.Input.LineWalking_Mode == MoveLeft)
					{
						p->CalCmd.Vxyz.x = DisMagNavData.Output.ControlY;
						p->CalCmd.Axyz.y = (-1)* DisMagNavData.Output.ControlX;
						p->CalCmd.Axyz.z = (-1)* DisMagNavData.Output.ControlZ;
					}
					
					if(DisMagNavData.Input.LineWalking_Mode == MoveRight)
					{
						p->CalCmd.Vxyz.x = DisMagNavData.Output.ControlY;
						p->CalCmd.Axyz.y = DisMagNavData.Output.ControlX;
						p->CalCmd.Axyz.z = DisMagNavData.Output.ControlZ;
					}
				}
			}

        }
		break;

        default:
            break;
    }
	
	return 1;
}

/***********************************************************
** 函数名称:	ACC_Magnetic_DisMagNav_Input
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/

void ACC_Magnetic_DisMagNav_Input(ACC_DATA_TypeDef *p)
{
	u8 i;
	for(i = 0; i < g_Para.Walk.MotorNum; i++)
	{
		DisMagNavData.Input.AverageSpeed += fabs(SVO[i].Speed);
	}
		DisMagNavData.Input.AverageSpeed /= g_Para.Walk.MotorNum;
	
	if(p->CalCmd.Mode == ACCMode_PosTrace)
	{
		/*正向巡线*/
		if(p->CalCmd.Vxyz.y < 0)
			DisMagNavData.Input.LineWalking_Mode = Backward;
		else if(p->CalCmd.Vxyz.y > 0)
			DisMagNavData.Input.LineWalking_Mode = Forward;
	}
	else if(p->CalCmd.Mode == ACCMode_InfTrace)
	{
		/*横向巡线*/
		if(p->CalCmd.Vxyz.x < 0)
			DisMagNavData.Input.LineWalking_Mode = MoveLeft;
		else if(p->CalCmd.Vxyz.x > 0)
			DisMagNavData.Input.LineWalking_Mode = MoveRight;
	}
}
/***********************************************************
** 函数名称:	ACC_LineTrace_Process
** 功能描述: 	磁钉巡线主函数入口
** 参    数:
** 返 回 值:	None
***********************************************************/
void ACC_LineTrace_Process(ACC_DATA_TypeDef *p)
{

	//磁钉模块的依赖输入
	ACC_Magnetic_DisMagNav_Input(p);
	
	if(ACC_ModeSMTIsTrace(p) != 0)
    {
        if(g_Para.CtrlCoef.BitPara.WheelFollow)//选择了轮系跟随
		{
            ACC_LineTrace_Process_WheelFollow(p);
		}
		else if(g_Para.CtrlCoef.BitPara.MagneticNail)	//使能了磁钉巡线
		{
			ACC_Magnetic_Patrol_Nail(p);	
		}
        else//标准巡线功能
		{
		    ACC_LineTrace_Process_General(p);
		}
    }
    else
	{
		/*无需执行*/
	}
}

//***********************************END OF FILE***********************************
