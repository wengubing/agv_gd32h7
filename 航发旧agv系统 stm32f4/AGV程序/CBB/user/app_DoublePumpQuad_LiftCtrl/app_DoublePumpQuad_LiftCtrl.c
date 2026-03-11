/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	app_DoublePumpQuad_LiftCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-10-10
	* Description:
	******************************************************************************
**/

#include "app_DoublePumpQuad_LiftCtrl.h"
#include "basecalfunc.h"
#define  app_DoublePumpQuad_LiftCtrle_GLOBALS


/***************************************************************************************
** 函数名称: DPQ_LiftCtrl_TempDataManage
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DPQ_LiftCtrl_TempDataManage(DoublePumpQuad_LiftCtrl_TypeDef *p)
{
	DoublePumpQuad_LiftCtrl_Temp_TypeDef		*CtrlTemp	= &p->DPQ_Temp;
	DoublePumpQuad_LiftCtrl_CtrlIn_TypeDef		*In			= &p->DPQ_In;

	//通用功能：获取平均高度
	{
		CtrlTemp->Average_F = (In->Distance[0]+In->Distance[1])/2.0f;
		CtrlTemp->Average_B = (In->Distance[2]+In->Distance[3])/2.0f;
		CtrlTemp->Average_L = (In->Distance[0]+In->Distance[2])/2.0f;
		CtrlTemp->Average_R = (In->Distance[1]+In->Distance[3])/2.0f;
		CtrlTemp->Average 	= (In->Distance[0]+In->Distance[1]+In->Distance[2]+In->Distance[3])/4.0f;
	}
	//通用功能：获得姿态偏差
	{
		CtrlTemp->Pitch_Diff = CtrlTemp->Average_F-CtrlTemp->Average_B;
		CtrlTemp->Roll_Diff  = CtrlTemp->Average_L-CtrlTemp->Average_R;
		//锁存偏差值
		if(p->DPQ_Cmd.in_Level == 0)
		{
			CtrlTemp->last_Pitch_Diff 	= CtrlTemp->Pitch_Diff;
			CtrlTemp->last_Roll_Diff 	= CtrlTemp->Roll_Diff;
		}
	}
	//通用功能：获取最高值、最低值，并得到偏差数据
	{
		uint8_t i = 0;
		//冒泡找到最大值
		{
			float Highest = In->Distance[0];
			float Lowest  = In->Distance[0];
			float Diff_Max= 0;
			for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
			{
				//最高值获取
				if(Highest < In->Distance[i])
				{
					Highest = In->Distance[i];
					CtrlTemp->Highest_Site = i;
				}
				//最低值获取
				if(Lowest > In->Distance[i])
				{
					Lowest = In->Distance[i];
					CtrlTemp->Lowest_Site = i;
				}
			}
			CtrlTemp->Highest 	= Highest;
			CtrlTemp->Lowest 	= Lowest;
			//存在高差的情况
			if(Highest != Lowest)
			{
				CtrlTemp->Diff_Max	= Diff_Max = Highest - Lowest;
				//求出每个点 和最高、最低的差值；
				for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
				{
					CtrlTemp->Highest_Diff[i] 	= Highest - In->Distance[i];
					CtrlTemp->Up_K[i] 			= CtrlTemp->Highest_Diff[i]/Diff_Max;

					CtrlTemp->Lowest_Diff[i]	= In->Distance[i] - Lowest;
					CtrlTemp->Down_K[i] 		= CtrlTemp->Lowest_Diff[i]/Diff_Max;
				}
			}
			else
			{
				CtrlTemp->Diff_Max	= 1;
				for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
				{
					CtrlTemp->Highest_Diff[i] 	= 0;
					CtrlTemp->Up_K[i] 			= 1;

					CtrlTemp->Lowest_Diff[i]	= 0;
					CtrlTemp->Down_K[i] 		= 1;
				}
			}
		}
	}
	//通用功能：获取升降速度
	{
		uint8_t i = 0;
		uint32_t Ms_Diff = In->In_Ms - CtrlTemp->Last_Ms;
		
		if(Ms_Diff != 0 && Ms_Diff > 1000)
		{
			CtrlTemp->Last_Ms = In->In_Ms;
			for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
			{
				CtrlTemp->High_Speed[i] = (In->Distance[i] - CtrlTemp->Last_High[i])/Ms_Diff*60.0f*1000.0f;
				CtrlTemp->Last_High[i] 	= In->Distance[i];
				
			}
		}
	}
	

}

/***************************************************************************************
** 函数名称: DPQ_CtrlTemp_OffAll
** 功能描述: 到位
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DPQ_CtrlTemp_OffAll(DoublePumpQuad_LiftCtrl_TypeDef *p)
{
	DoublePumpQuad_LiftCtrl_Temp_TypeDef		*CtrlTemp	= &p->DPQ_Temp;
	uint8_t i = 0;
	for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
	{
		CtrlTemp->Ctrl_Temp	[i]	= 0;
		CtrlTemp->Up_K		[i]	= 0;
		CtrlTemp->Down_K	[i]	= 0;
	}
}


/***************************************************************************************
** 函数名称: DPQ_Higt_LiftCtrl_CmdPrep
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DPQ_Higt_LiftCtrl_CmdPrep(DoublePumpQuad_LiftCtrl_TypeDef *p)
{
	DoublePumpQuad_LiftCtrl_Cmd_TypeDef 		*Cmd 		= &p->DPQ_Cmd;
	DoublePumpQuad_LiftCtrl_Temp_TypeDef		*CtrlTemp	= &p->DPQ_Temp;

	uint8_t i = 0;
	//获取命令挡位
	{
		//自由升降做透传处理
		if(Cmd->do_Force != 0)
		{
			Cmd->do_Gears 	= Cmd->in_Gears;
		}
		//正常升降，考虑限位前的缓冲
		else
		{
			DoublePumpQuad_LiftCtrl_CtrlPara_TypeDef		*Para	= &p->DPQ_Para;

			if(	  (CtrlTemp->Average > Para->HighLimit[0] -  Para->ShowDis)
				||(CtrlTemp->Average < Para->LowLimit[0]  +  Para->ShowDis))
			{
				Cmd->do_Gears 	= 0;
			}
			else
			{
				Cmd->do_Gears 	= Cmd->in_Gears;
			}
		}
	}
	//获取控制命令
	{
		if(Cmd->do_Force != 0)
		{
			if(Cmd->in_Up == 1)
			{
				p->PS_STM = DoublePumpQuad_LiftCtrl_Up;	//标记为升运行中
				for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
				{
					CtrlTemp->Ctrl_Temp	[i]	= 1;
					CtrlTemp->Up_K		[i]	= 1;
					CtrlTemp->Down_K	[i]	= 0;
				}
			}
			else if(Cmd->in_Down == 1)
			{
				p->PS_STM = DoublePumpQuad_LiftCtrl_Down;	//标记为降运行中
				for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
				{
					CtrlTemp->Ctrl_Temp	[i]	= 2;
					CtrlTemp->Up_K		[i]	= 0;
					CtrlTemp->Down_K	[i]	= 1;
				}
			}
			else
			{
				p->PS_STM = DoublePumpQuad_LiftCtrl_Normal;	//标记为保持状态
				DPQ_CtrlTemp_OffAll(p);
			}
			
		}
		else
		{
			DoublePumpQuad_LiftCtrl_CtrlPara_TypeDef	*Para		= &p->DPQ_Para;	
			DoublePumpQuad_LiftCtrl_CtrlIn_TypeDef		*In			= &p->DPQ_In;
			//如果需要所有的传感器都到限位才停
			if(Para->Fun_AllPlace == 1)
			{
				if(Cmd->in_Up == 1)
				{
					float limit 	= 0;
					float Distance 	= 0;
					p->PS_STM = DoublePumpQuad_LiftCtrl_LimitUp;		//标记为推到位
					/*遍历所有传感数据，将执行结果给到缓存中*/
					for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
					{
						Distance 	= In->Distance[i];
						limit	 	= Para->HighLimit[i];
						if(Distance < limit)
						{
							CtrlTemp->Ctrl_Temp[i] = 1;		//输入缓存
							p->PS_STM = DoublePumpQuad_LiftCtrl_Up;	//标记为推运行中
						}
						else
						{
							CtrlTemp->Ctrl_Temp[i] = 0;
						}
					}
				}
				else if(Cmd->in_Down == 1)
				{
					
					float limit 	= 0;
					float Distance 	= 0;
					p->PS_STM = DoublePumpQuad_LiftCtrl_LimitDown;		//标记为推到位
					
					/*遍历所有传感数据，将执行结果给到缓存中*/
					for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
					{
						Distance 	= In->Distance[i];
						limit	 	= Para->LowLimit[i];
						if(Distance > limit)
						{
							CtrlTemp->Ctrl_Temp[i] = 2;		//输入缓存
							p->PS_STM = DoublePumpQuad_LiftCtrl_Down;	//标记为推运行中
						}
						else
						{
							CtrlTemp->Ctrl_Temp[i] = 0;
						}
					}
				}
				else
				{
					p->PS_STM = DoublePumpQuad_LiftCtrl_Normal;	//标记为推运行中
					DPQ_CtrlTemp_OffAll(p);
				}
			}
			//只要有一个达到限位就停
			else
			{
				if(Cmd->in_Up == 1)
				{
					float limit 	= 0;
					float Distance 	= 0;
					p->PS_STM = DoublePumpQuad_LiftCtrl_Up;	//标记为推运行中
					/*遍历所有传感数据，将执行结果给到缓存中*/
					for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
					{
						Distance 	= In->Distance[i];
						limit	 	= Para->HighLimit[i];
						if(Distance < limit)
						{
							CtrlTemp->Ctrl_Temp[i] = 1;		//输入缓存
							
						}
						else
						{
							p->PS_STM = DoublePumpQuad_LiftCtrl_LimitUp;		//标记为推到位
							CtrlTemp->Ctrl_Temp[i] = 0;
						}
					}
				}
				else if(Cmd->in_Down == 1)
				{
					
					float limit 	= 0;
					float Distance 	= 0;
					
					p->PS_STM = DoublePumpQuad_LiftCtrl_Down;	//标记为推运行中
					/*遍历所有传感数据，将执行结果给到缓存中*/
					for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
					{
						Distance 	= In->Distance[i];
						limit	 	= Para->LowLimit[i];
						if(Distance > limit)
						{
							CtrlTemp->Ctrl_Temp[i] = 2;		//输入缓存
						}
						else
						{
							p->PS_STM = DoublePumpQuad_LiftCtrl_LimitDown;		//标记为推到位
							CtrlTemp->Ctrl_Temp[i] = 0;
						}
					}
				}
				else
				{
					p->PS_STM = DoublePumpQuad_LiftCtrl_Normal;	//标记为推运行中
					DPQ_CtrlTemp_OffAll(p);
				}
			}
		}
	}
	//基于升降状态，输出逻辑处理后的控制命令
	{
		if(p->PS_STM == DoublePumpQuad_LiftCtrl_Up)
		{
			Cmd->do_Up		= 1;
			Cmd->do_Down	= 0;
		}
		else if(p->PS_STM == DoublePumpQuad_LiftCtrl_Down)
		{
			Cmd->do_Up		= 0;
			Cmd->do_Down	= 1;
		}
		else
		{
			Cmd->do_Up		= 0;
			Cmd->do_Down	= 0;
		}
	}
}
/***************************************************************************************
** 函数名称: DPQ_Higt_LiftCtrl_TempDataManage
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DPQ_Higt_LiftCtrl_OutPut(DoublePumpQuad_LiftCtrl_TypeDef *p)
{
	static float K_NoSpeed[DoublePumpQuad_Higt_MAX] 		= {1.0f,1.0f,1.0f,1.0f};
	DoublePumpQuad_LiftCtrl_CtrlPara_TypeDef	*Para		= &p->DPQ_Para;	
	DoublePumpQuad_LiftCtrl_Cmd_TypeDef 		*Cmd 		= &p->DPQ_Cmd;
	DoublePumpQuad_LiftCtrl_Temp_TypeDef		*CtrlTemp	= &p->DPQ_Temp;
	DoublePumpQuad_LiftCtrl_CtrlOut_TypeDef		*Out		= &p->DPQ_Out;
	//电机速度
	{
		if(Cmd->do_Up == 1)
		{
			Out->MotorSpeed 			= CtrlTemp->p_SpeedPlan[Cmd->do_Gears];
		}
		else
		{
			Out->MotorSpeed 			= 0;
		}
	}
	//比例阀调节
	{
		uint8_t i = 0;
		if(Cmd->do_Up == 1)
		{
			float *p_Up_K 				= K_NoSpeed;
			//没速度控制，上升 比例阀全开
			if(Para->Fun_NoSpeedRegulation == 1)
			{
				p_Up_K =  K_NoSpeed;
			}
			else
			{
				p_Up_K =  CtrlTemp->Up_K;
			}
			//将预处理中的调节系数 转换成逻辑输出结果
			{
				float *Up_DA_Minimum 				= Para->Up_DA_Minimum;
				float *Up_DA_Maximum 				= Para->Up_DA_Maximum;
				for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
				{
					if(CtrlTemp->Ctrl_Temp[i] == 1)
					{
						Out->UpProportionalValve[i] = Up_DA_Minimum[i] + p_Up_K[i]*(Up_DA_Maximum[i] - Up_DA_Minimum[i]);
					}
					else
					{
						Out->UpProportionalValve[i] = 0;
					}
					Out->DownProportionalValve[i] = 0;
				}
			}
		}
		else if(Cmd->do_Down == 1)
		{
			float *p_Down_K 				= K_NoSpeed;
			//没速度控制，上升 比例阀全开
			if(Para->Fun_NoSpeedRegulation == 1)
			{
				p_Down_K =  K_NoSpeed;
			}
			else
			{
				p_Down_K =  CtrlTemp->Down_K;
			}
			
			//将预处理中的调节系数 转换成逻辑输出结果
			{
				float *Down_DA_Minimum 				= Para->Down_DA_Minimum;
				float *Down_DA_Maximum 				= Para->Down_DA_Maximum;
				for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
				{
					if(CtrlTemp->Ctrl_Temp[i] == 2)
					{
						Out->DownProportionalValve[i] = Down_DA_Minimum[i] + p_Down_K[i]*(Down_DA_Maximum[i] - Down_DA_Minimum[i]);
					}
					else
					{
						Out->DownProportionalValve[i] = 0;
					}
					Out->UpProportionalValve[i] = 0;
				}
			}
		}
		else
		{
			Out->MotorSpeed 			= 0;
			for(i=0;i<DoublePumpQuad_Higt_MAX;i++)
			{
				Out->UpProportionalValve	[i] = 0;
				Out->DownProportionalValve	[i] = 0;
			}
		}
	}
	//开关阀
	{
		if( Cmd->do_Up == 1)
		{
			Out->Valve_LF = 0;
			Out->Valve_RF = 0;
			Out->Valve_LB = 0;
			Out->Valve_RB = 0;
		}
		else if( Cmd->do_Down == 1)
		{
			//比例阀为0时（偏差过大），关闭开关阀
			if(Para->Fun_ValveControl == 1)
			{
				if(CtrlTemp->Lowest_Diff[0] == 0)	{Out->Valve_LF = 0;}
				else								{Out->Valve_LF = 1;}
				if(CtrlTemp->Lowest_Diff[1] == 0)	{Out->Valve_RF = 0;}
				else								{Out->Valve_RF = 1;}
				if(CtrlTemp->Lowest_Diff[2] == 0)	{Out->Valve_LB = 0;}
				else								{Out->Valve_LB = 1;}
				if(CtrlTemp->Lowest_Diff[3] == 0)	{Out->Valve_RB = 0;}
				else								{Out->Valve_RB = 1;}
			}
			else
			{
				Out->Valve_LF = 1;
				Out->Valve_RF = 1;
				Out->Valve_LB = 1;
				Out->Valve_RB = 1;
			}
		}
		else
		{
			Out->Valve_LF = 0;
			Out->Valve_RF = 0;
			Out->Valve_LB = 0;
			Out->Valve_RB = 0;
		}
	}
	//泄压阀
	{
		if( Cmd->do_Up == 1)
		{
			Out->ThrottleValve_F = 1;
			Out->ThrottleValve_B = 1;
		}
		else if( Cmd->do_Down == 1)
		{
			Out->ThrottleValve_F = 0;
			Out->ThrottleValve_B = 0;
		}
		else
		{
			Out->ThrottleValve_F = 0;
			Out->ThrottleValve_B = 0;
		}
	}
}

/***************************************************************************************
** 函数名称: DPQ_Pitch_LiftCtrl_CmdPrep
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DPQ_Pitch_LiftCtrl_CmdPrep(DoublePumpQuad_LiftCtrl_TypeDef *p)
{

}

/***************************************************************************************
** 函数名称: DPQ_Pitch_LiftCtrl_OutPut
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DPQ_Pitch_LiftCtrl_OutPut(DoublePumpQuad_LiftCtrl_TypeDef *p)
{

}

/***************************************************************************************
** 函数名称: DPQ_Roll_LiftCtrl_CmdPrep
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DPQ_Roll_LiftCtrl_CmdPrep(DoublePumpQuad_LiftCtrl_TypeDef *p)
{

}
/***************************************************************************************
** 函数名称: DPQ_Roll_LiftCtrl_OutPut
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DPQ_Roll_LiftCtrl_OutPut(DoublePumpQuad_LiftCtrl_TypeDef *p)
{
	
}

/***************************************************************************************
** 函数名称: DoublePumpQuad_LiftCtrl_Main
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void DoublePumpQuad_LiftCtrl_Main(DoublePumpQuad_LiftCtrl_TypeDef *p)
{
	//
	{
		//模式发送变化时的一次性动作
		if(p->DPQ_Temp.Last_DPQ_Mode != p->DPQ_Mode)
		{
			switch(p->DPQ_Mode)
			{
				default:
				case DoublePumpQuad_High:
				{
					p->DPQ_Temp.p_SpeedPlan = p->DPQ_Para.SpeedPlanHigh;
				}break;
				case DoublePumpQuad_Pitch:
				{
					p->DPQ_Temp.p_SpeedPlan = p->DPQ_Para.SpeedPlanPitch;
				}break;
				case DoublePumpQuad_Roll:
				{
					p->DPQ_Temp.p_SpeedPlan = p->DPQ_Para.SpeedPlanRoll;
				}break;
				case DoublePumpQuad_FloorLevel:
				{

				}break;
			}
			//清除传感器动作
			
			//更新缓存
			p->DPQ_Temp.Last_DPQ_Mode = p->DPQ_Mode;
		}
	}

	//高度数据 逻辑处理
	{
		DPQ_LiftCtrl_TempDataManage(p);
	}
	//控制     逻辑处理
	{
		switch(p->DPQ_Mode)
		{
			default:
			case DoublePumpQuad_Null:
			case DoublePumpQuad_High:
			{
				p->DPQ_Temp.p_SpeedPlan = p->DPQ_Para.SpeedPlanHigh;
				//输入命令 逻辑处理
				DPQ_Higt_LiftCtrl_CmdPrep(p);
				//输出指令解析
				DPQ_Higt_LiftCtrl_OutPut(p);
			}break;
			case DoublePumpQuad_Pitch:
			{
				p->DPQ_Temp.p_SpeedPlan = p->DPQ_Para.SpeedPlanPitch;
				//输入命令 逻辑处理
				DPQ_Pitch_LiftCtrl_CmdPrep(p);
				//输出指令解析
				DPQ_Pitch_LiftCtrl_OutPut(p);
			}break;
			case DoublePumpQuad_Roll:
			{
				p->DPQ_Temp.p_SpeedPlan = p->DPQ_Para.SpeedPlanRoll;
				//输入命令 逻辑处理
				DPQ_Roll_LiftCtrl_CmdPrep(p);
				//输出指令解析
				DPQ_Roll_LiftCtrl_OutPut(p);
			}break;
			case DoublePumpQuad_FloorLevel:
			{
				
			}break;
		}
	}
	
	//动作状态机，用于时间控制
	{
		if(p->DPQ_Temp.Last_DPQ_STM != p->PS_STM)
		{
			switch(p->PS_STM)
			{
				default:
				case DoublePumpQuad_LiftCtrl_Null:
				{

				}break;
				case DoublePumpQuad_LiftCtrl_Normal:
				{
					
				}break;
				case DoublePumpQuad_LiftCtrl_LimitUp:
				{
					
				}break;
				case DoublePumpQuad_LiftCtrl_LimitDown:
				{
					
				}break;
				case DoublePumpQuad_LiftCtrl_Up:
				{
					
				}break;
				case DoublePumpQuad_LiftCtrl_Down:
				{
					
				}break;

				case DoublePumpQuad_LiftCtrl_LevelFalish:
				{
					
				}break;
			}
			p->DPQ_Temp.Last_DPQ_STM = p->PS_STM;
		}
	}
}



/************************************END OF FILE************************************/
