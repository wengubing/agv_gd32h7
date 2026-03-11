/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	app_UnitePump_LiftCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 	2026.01.17
	* Description:
	******************************************************************************
**/
#define  app_UnitePump_LiftCtrl_GLOBALS
#include "app_UnitePump_LiftCtrl.h"
#include "basecalfunc.h"



/***************************************************************************************
** 函数名称: UPL_LiftCtrl_TempDataManage
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_LiftCtrl_TempDataManage(UnitePump_LiftCtrl_TypeDef *p)
{
	UnitePump_LiftCtrl_Temp_TypeDef		*p_Temp	= &p->UPL_Temp;
	UnitePump_LiftCtrl_CtrlIn_TypeDef	*p_In	= &p->UPL_In;
	
	//通用功能：获取平均高度
	{
		p_Temp->Average_F = (p_In->Distance[UnitePump_LF]+p_In->Distance[UnitePump_RF])/2.0f;
		p_Temp->Average_B = (p_In->Distance[UnitePump_LB]+p_In->Distance[UnitePump_RB])/2.0f;
		p_Temp->Average_L = (p_In->Distance[UnitePump_LF]+p_In->Distance[UnitePump_LB])/2.0f;
		p_Temp->Average_R = (p_In->Distance[UnitePump_RF]+p_In->Distance[UnitePump_RB])/2.0f;

		p_Temp->Average	  = (p_In->Distance[UnitePump_LF]
							+p_In->Distance[UnitePump_RF]
							+p_In->Distance[UnitePump_LB]
							+p_In->Distance[UnitePump_RB])/4.0f;
	}
	//通用功能：获得姿态偏差
	{
		p_Temp->Pitch_Diff = p_Temp->Average_F-p_Temp->Average_B;
		p_Temp->Roll_Diff  = p_Temp->Average_L-p_Temp->Average_R;
	}
	//通用功能：获取最高值、最低值，并得到偏差数据；是否抵达限位\限速
	{
		UnitePump_Orientation_TypeDef i = UnitePump_LF;
		float Highest 		= p_In->Distance[UnitePump_LF];
		float Lowest  		= p_In->Distance[UnitePump_LF];
		float HighShowDis	= p->UPL_Para.HighShowDis;

		//冒泡找到最大值、最小；是否达到限位值;
		{
			float Distance		= 0;
			float HighLimit		= 0;
			float LowLimit		= 0;
			for(i=UnitePump_LF;i<UnitePump_MAX;i++)
			{
				//获取高度数据
				Distance	= p_In->Distance[i];
				//最高值获取
				if(Highest < Distance)
				{
					Highest = p_In->Distance[i];
					p_Temp->Highest_Site = i;
				}else{}
				//最低值获取
				if(Lowest > Distance)
				{
					Lowest = p_In->Distance[i];
					p_Temp->Lowest_Site = i;
				}else{}

				HighLimit		= p->UPL_Para.HighLimit[i];
				LowLimit		= p->UPL_Para.LowLimit[i];
				//抵达限位或者限速 判断
				if(Distance >= HighLimit)						{p_Temp->High_Limit_Sta[i] = 1;}
				else if(Distance >= HighLimit - HighShowDis)	{p_Temp->High_Limit_Sta[i] = 3;}
				else if(Distance <= LowLimit)					{p_Temp->High_Limit_Sta[i] = 2;}
				else if(Distance <= HighLimit + HighShowDis)	{p_Temp->High_Limit_Sta[i] = 4;}
				else											{p_Temp->High_Limit_Sta[i] = 0;}
				
				
			}
			p_Temp->Highest		= Highest;
			p_Temp->Lowest		= Lowest;
			p_Temp->High_Diff 	= Highest - Lowest;
		}
		//计算高差
		{
			//存在高差的情况
			if(p_Temp->Highest_Site != p_Temp->Lowest_Site)
			{
				//求出每个点 和最高、最低的差值；
				for(i=UnitePump_LF;i<UnitePump_MAX;i++)
				{
					p_Temp->Highest_Diff[i]		= Highest - p_In->Distance[i];
					p_Temp->Lowest_Diff[i]		= p_In->Distance[i] - Lowest;
				}
			}
			else
			{
				for(i=UnitePump_LF;i<UnitePump_MAX;i++)
				{
					p_Temp->Highest_Diff[i] 	= 0;
					p_Temp->Lowest_Diff[i]		= 0;
				}
			}
		}
	}
	//通用功能：获取升降速度
	{
		uint32_t Ms_Diff = p->UPL_Ms - p_Temp->Last_Ms;
		if(Ms_Diff != 0 && Ms_Diff > 1000)
		{
			UnitePump_Orientation_TypeDef i = UnitePump_LF;
			p_Temp->Last_Ms = p->UPL_Ms;
			for(i=UnitePump_LF;i<UnitePump_MAX;i++)
			{
				p_Temp->High_Speed[i] = (p_In->Distance[i] - p_Temp->Last_High[i])/Ms_Diff*60.0f*1000.0f;
				p_Temp->Last_High[i] 	= p_In->Distance[i];
			}
		}
	}
	//通用功能：获取每个检测点是否达到限位
	
}

/***************************************************************************************
** 函数名称: UPL_CtrlTemp_OffAll
** 功能描述: 到位
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_CtrlTemp_OffAll(UnitePump_LiftCtrl_TypeDef *p)
{
	UnitePump_LiftCtrl_CtrlOut_TypeDef		*CtrlTemp	= &p->UPL_Temp.Temp_CtrlOut;

	CtrlTemp->MotorSpeed  =  0;

	CtrlTemp->Up_Valve_LF	=  0;
	CtrlTemp->Up_Valve_RF	=  0;
	CtrlTemp->Up_Valve_LB	=  0;
	CtrlTemp->Up_Valve_RB	=  0;

	CtrlTemp->Down_Valve_LF	=  0;
	CtrlTemp->Down_Valve_RF	=  0;
	CtrlTemp->Down_Valve_LB	=  0;
	CtrlTemp->Down_Valve_RB	=  0;
}



/***************************************************************************************
** 函数名称: UPL_Higt_LiftCtrl_TempDataPrep
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_Higt_LiftCtrl_TempDataPrep(UnitePump_LiftCtrl_TypeDef *p)
{
	//基于中间变量和限位值，判断每个控制点是否需要上升
	{
		UnitePump_LiftCtrl_CtrlPara_TypeDef	*p_Para	= &p->UPL_Para;
		//UnitePump_LiftCtrl_CtrlIn_TypeDef	*p_In	= &p->UPL_In;
		UnitePump_LiftCtrl_Temp_TypeDef		*p_Temp	= &p->UPL_Temp;
		UnitePump_Orientation_TypeDef 		i 		= UnitePump_LF;

		//确定每个控制点是否需要吸合电磁阀、是否抵达限速
		{
			//float HighShowDis	= p_Para->HighShowDis;
			float Highest_Diff	= 0;
			uint8_t High_Limit_Sta = 0;
			uint8_t Ctrl_Temp 	= 0;
			if(p->UPL_Cmd.in_Up == 1)
			{
				float High_Up_DeadZne	= p_Para->High_Up_DeadZne;
				float High_Up_Limit		= p_Para->High_Up_Limit;
				//标记各个控制点是否需要上升，
				for(i=UnitePump_LF;i<UnitePump_MAX;i++)
				{
					Ctrl_Temp		= 0;
					High_Limit_Sta	= p_Temp->High_Limit_Sta[i];
					Highest_Diff	= p_Temp->Highest_Diff[i];

					do
					{
						//抵达限位了的不再上升
						if(High_Limit_Sta == 1)				{Ctrl_Temp = 1;break;}
						//和最高的比，差的过大 过大的不再上升
						if(Highest_Diff >= High_Up_DeadZne)	{Ctrl_Temp = 2;break;}
						//小于回差控制的结果，需要恢复上升
						if(Highest_Diff <= High_Up_Limit)	{Ctrl_Temp = 11;break;}
						
						//执行到这里，介于回差控制结果之间，需要保持当前的状态，
						//如果是开启的继续开启，如果是关闭的，继续关闭，直到触发下一轮的回差控制
						if(p_Temp->Ctrl_Temp[i] == 1)		{Ctrl_Temp = 3;}
						else								{Ctrl_Temp = 12;}
						
					}while(0);

					//根据判定结果控制上升电磁阀
					if(Ctrl_Temp > 10)	{p_Temp->Ctrl_Temp[i] = 1;}
					else				{p_Temp->Ctrl_Temp[i] = 0;}

					//抵达限速
					if(High_Limit_Sta == 3)	{p_Temp->ArrivalHighShow  = 1;}
				}
			}
			else if(p->UPL_Cmd.in_Down == 1)
			{
				float High_Down_DeadZne		= p_Para->High_Down_DeadZne;
				float High_Down_Limit		= p_Para->High_Down_Limit;
				//标记各个控制点是否需要下降
				for(i=UnitePump_LF;i<UnitePump_MAX;i++)
				{
					Ctrl_Temp		= 0;
					High_Limit_Sta	= p_Temp->High_Limit_Sta[i];
					Highest_Diff	= p_Temp->Highest_Diff[i];

					do
					{
						//抵达限位了的不再上升
						if(High_Limit_Sta == 1)					{Ctrl_Temp = 1;break;}
						//和最低的比，差的过大 过大的不再上升
						if(Highest_Diff >= High_Down_DeadZne)	{Ctrl_Temp = 2;break;}
						//小于回差控制的结果，需要恢复上升
						if(Highest_Diff <= High_Down_Limit)		{Ctrl_Temp = 11;break;}
						
						//执行到这里，介于回差控制结果之间，需要保持当前的状态，
						//如果是开启的继续开启，如果是关闭的，继续关闭，直到触发下一轮的回差控制
						if(p_Temp->Ctrl_Temp[i] == 2)			{Ctrl_Temp = 3;}
						else									{Ctrl_Temp = 12;}

					}while(0);

					//根据判定结果控制上升电磁阀
					if(Ctrl_Temp > 10)	{p_Temp->Ctrl_Temp[i] = 2;}
					else				{p_Temp->Ctrl_Temp[i] = 0;}

					//抵达限速
					if(High_Limit_Sta == 3)	{p_Temp->ArrivalHighShow  = 1;}
				}
			}
			else
			{
				for(i=UnitePump_LF;i<UnitePump_MAX;i++)
				{
					p_Temp->Ctrl_Temp[i] = 0;
				}
			}
		}
		//判断是否到位，到位后；所有执行器会停止动作
		{
			switch(p_Para->Fun_Arrival)
			{
				default:
				case UnitePump_Arrival_Average:
				{
					if(p->UPL_Cmd.in_Up == 1)
					{
						p_Temp->ArrivalLowLimit = 0;
						if(p_Temp->Average > p_Para->HighLimit[0])	{p_Temp->ArrivalHighLimit = 1;}
						else										{p_Temp->ArrivalHighLimit = 0;}
					}
					else if(p->UPL_Cmd.in_Down == 1)
					{
						p_Temp->ArrivalHighLimit = 0;
						if(p_Temp->Average > p_Para->LowLimit[0])	{p_Temp->ArrivalLowLimit = 1;}
						else										{p_Temp->ArrivalLowLimit = 0;}
					}
					else
					{
						p_Temp->ArrivalHighLimit= 0;
						p_Temp->ArrivalLowLimit	= 0;
					}

				}break;
				case UnitePump_Arrival_All:
				{
					if(p->UPL_Cmd.in_Up == 1)
					{
						p_Temp->ArrivalHighLimit= 1;
						p_Temp->ArrivalLowLimit	= 0;
						for(i=UnitePump_LF;i<UnitePump_MAX;i++)
						{
							//只要存在一个点没抵达限位，判定不到位
							if(p_Temp->High_Limit_Sta[i] != 1)	{p_Temp->ArrivalHighLimit = 0;}
							else{}
						}
					}
					else if(p->UPL_Cmd.in_Down == 1)
					{
						p_Temp->ArrivalHighLimit= 0;
						p_Temp->ArrivalLowLimit	= 1;
						for(i=UnitePump_LF;i<UnitePump_MAX;i++)
						{
							//只要存在一个点没抵达限位，判定不到位
							if(p_Temp->High_Limit_Sta[i] != 2)	{p_Temp->ArrivalLowShow = 0;}
							else{}
						}
					}
					else
					{
						p_Temp->ArrivalHighLimit= 0;
						p_Temp->ArrivalLowLimit	= 0;
					}

				}break;
				case UnitePump_Arrival_One:
				{
					p_Temp->ArrivalHighLimit = 0;
					p_Temp->ArrivalLowLimit	 = 0;
					if(p->UPL_Cmd.in_Up == 1)
					{
						for(i=UnitePump_LF;i<UnitePump_MAX;i++)
						{
							//只要有一个抵达限位，判定到位
							if(p_Temp->High_Limit_Sta[i] == 1)	{p_Temp->ArrivalHighLimit = 1;}
							else{}
						}
					}
					else if(p->UPL_Cmd.in_Down == 1)
					{
						for(i=UnitePump_LF;i<UnitePump_MAX;i++)
						{
							//只要有一个抵达限位，判定到位
							if(p_Temp->High_Limit_Sta[i] == 2)	{p_Temp->ArrivalLowShow = 1;}
							else{}
						}
					}
					else{}

				}break;

			}
		}

	}
}
/***************************************************************************************
** 函数名称: UPL_Higt_LiftCtrl_CmdPrep
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_Higt_LiftCtrl_CmdPrep(UnitePump_LiftCtrl_TypeDef *p)
{
	UnitePump_LiftCtrl_Cmd_TypeDef 		*p_Cmd	= &p->UPL_Cmd;
	UnitePump_LiftCtrl_Temp_TypeDef		*p_Temp	= &p->UPL_Temp;

	//获取输入控制命令 得到执行状态
	{
		uint8_t Temp_STM = 0;
		//强制升降的情况下
		if(p_Cmd->do_Force != 0)
		{
			//自由升降做透传处理
			p_Cmd->do_Gears 	= p_Cmd->in_Gears;
			//上升、下降 指令解析
			if		(p_Cmd->in_Up 	== 1)	{Temp_STM = 1;}
			else if	(p_Cmd->in_Down	== 1)	{Temp_STM = 3;}
			else {}
		}
		//正常情况下
		else
		{
			//UnitePump_LiftCtrl_CtrlIn_TypeDef	*In	= &p->UPL_In;

			if(p_Cmd->in_Up == 1)
			{
				//考虑限位前的缓冲
				if(p_Temp->ArrivalHighShow == 1)	{p_Cmd->do_Gears 	= 0;}
				else								{p_Cmd->do_Gears 	= p_Cmd->in_Gears;}
				//上升 指令解析
				if(p_Temp->ArrivalHighLimit == 1)	{Temp_STM = 2;}
				else								{Temp_STM = 1;}
			}
			else if(p_Cmd->in_Down == 1)
			{
				//考虑限位前的缓冲
				if(p_Temp->ArrivalLowShow == 1)		{p_Cmd->do_Gears 	= 0;}
				else								{p_Cmd->do_Gears 	= p_Cmd->in_Gears;}
				//下降 指令解析
				if(p_Temp->ArrivalLowLimit == 1)	{Temp_STM = 4;}
				else								{Temp_STM = 3;}
			}
			else{}
		}
		//状态更新
		{
			if(Temp_STM == 2)		//标记为升限位
			{
				p->UPL_STM = UnitePump_LiftCtrl_LimitUp;	
			}
			else if(Temp_STM == 1)	//标记为升运行中
			{
				p->UPL_STM = UnitePump_LiftCtrl_Up;	
			}
			else if(Temp_STM == 4)	//标记为降限位
			{
				p->UPL_STM = UnitePump_LiftCtrl_LimitDown;	
			}
			else if(Temp_STM == 3)	//标记为降运行中
			{
				p->UPL_STM = UnitePump_LiftCtrl_Down;	
			}
			else					//标记为保持状态
			{
				p->UPL_STM = UnitePump_LiftCtrl_Normal;	
			}
		}
	}
	//基于执行状态得到 实际执行指令
	{
		//得到实际指令
		{
			//上升
			if(p->UPL_STM == UnitePump_LiftCtrl_Up)
			{
				p_Cmd->do_Up 		= 1;
				p_Cmd->do_Down 	= 0;
			}
			//下降
			else if(p->UPL_STM == UnitePump_LiftCtrl_Down)
			{
				p_Cmd->do_Up 		= 0;
				p_Cmd->do_Down 	= 1;
			}
			else
			{
				p_Cmd->do_Up 		= 0;
				p_Cmd->do_Down 	= 0;
			}
		}
	}

}
/***************************************************************************************
** 函数名称: UPL_Higt_LiftCtrl_TempDataManage
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_Higt_LiftCtrl_OutPut(UnitePump_LiftCtrl_TypeDef *p)
{
	UnitePump_LiftCtrl_CtrlOut_TypeDef	*p_Out		= &p->UPL_Out;
	UnitePump_LiftCtrl_Temp_TypeDef		*p_Temp		= &p->UPL_Temp;
	
	//基于控制命令do得到 输出缓存
	{
		UnitePump_LiftCtrl_Cmd_TypeDef 		*p_Cmd		= &p->UPL_Cmd;
		UnitePump_LiftCtrl_CtrlOut_TypeDef	*p_Temp_Out	= &p->UPL_Temp.Temp_CtrlOut;
		//先清除所有输出缓存指令
		UPL_CtrlTemp_OffAll(p);
		//得到实际指令
		{
			//上升
			if(p_Cmd->do_Up == 1)
			{
				if(p_Temp ->Ctrl_Temp[UnitePump_LF] == 1){p_Temp_Out->Up_Valve_LF = 1;}
				if(p_Temp ->Ctrl_Temp[UnitePump_RF] == 1){p_Temp_Out->Up_Valve_RF = 1;}
				if(p_Temp ->Ctrl_Temp[UnitePump_LB] == 1){p_Temp_Out->Up_Valve_LB = 1;}
				if(p_Temp ->Ctrl_Temp[UnitePump_RB] == 1){p_Temp_Out->Up_Valve_RB = 1;}

				p_Temp_Out->MotorSpeed = p->UPL_Para.HighSpeedPlan[p_Cmd->do_Gears];
			}
			//下降
			else if(p_Cmd->do_Down == 1)
			{
				if(p_Temp ->Ctrl_Temp[UnitePump_LF] == 2){p_Temp_Out->Down_Valve_LF = 1;}
				if(p_Temp ->Ctrl_Temp[UnitePump_RF] == 2){p_Temp_Out->Down_Valve_RF = 1;}
				if(p_Temp ->Ctrl_Temp[UnitePump_LB] == 2){p_Temp_Out->Down_Valve_LB = 1;}
				if(p_Temp ->Ctrl_Temp[UnitePump_RB] == 2){p_Temp_Out->Down_Valve_RB = 1;}

				p_Temp_Out->MotorSpeed = p->UPL_Para.HighSpeedPlan[p_Cmd->do_Gears];
			}
			else{}
		}
	}
	//基于输出缓存 得到实际对外部输出
	{
		UnitePump_LiftCtrl_CtrlOut_TypeDef	*p_Temp_Out	= &p->UPL_Temp.Temp_CtrlOut;
		//电磁阀控制
		{
			p_Out->Up_Valve_LF	=  p_Temp_Out->Up_Valve_LF;
			p_Out->Up_Valve_RF	=  p_Temp_Out->Up_Valve_RF;
			p_Out->Up_Valve_LB	=  p_Temp_Out->Up_Valve_LB;
			p_Out->Up_Valve_RB	=  p_Temp_Out->Up_Valve_RB;
			
			p_Out->Down_Valve_LF	=  p_Temp_Out->Down_Valve_LF;
			p_Out->Down_Valve_RF	=  p_Temp_Out->Down_Valve_RF;
			p_Out->Down_Valve_LB	=  p_Temp_Out->Down_Valve_LB;
			p_Out->Down_Valve_RB	=  p_Temp_Out->Down_Valve_RB;
		}
		//电机控制
		{
			p_Out->MotorSpeed		=  p_Temp_Out->MotorSpeed;
		}
	}


}

/***************************************************************************************
** 函数名称: UPL_Pitch_LiftCtrl_CmdPrep
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_Pitch_LiftCtrl_CmdPrep(UnitePump_LiftCtrl_TypeDef *p)
{

}

/***************************************************************************************
** 函数名称: UPL_Pitch_LiftCtrl_OutPut
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_Pitch_LiftCtrl_OutPut(UnitePump_LiftCtrl_TypeDef *p)
{

}

/***************************************************************************************
** 函数名称: UPL_Roll_LiftCtrl_CmdPrep
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_Roll_LiftCtrl_CmdPrep(UnitePump_LiftCtrl_TypeDef *p)
{

}
/***************************************************************************************
** 函数名称: UPL_Roll_LiftCtrl_OutPut
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UPL_Roll_LiftCtrl_OutPut(UnitePump_LiftCtrl_TypeDef *p)
{

}

/***************************************************************************************
** 函数名称: UnitePump_LiftCtrl_Main
** 功能描述: 
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void UnitePump_LiftCtrl_Main(UnitePump_LiftCtrl_TypeDef *p)
{

	//高度数据 逻辑处理
	{
		UPL_LiftCtrl_TempDataManage(p);
	}
	//控制     逻辑处理
	{
		switch(p->UPL_Mode)
		{
			default:
			case UnitePump_High:
			{
				//输入命令 逻辑处理
				UPL_Higt_LiftCtrl_CmdPrep(p);
				//输出指令解析
				UPL_Higt_LiftCtrl_OutPut(p);
			}break;
			case UnitePump_Pitch:
			{
				//输入命令 逻辑处理
				UPL_Pitch_LiftCtrl_CmdPrep(p);
				//输出指令解析
				UPL_Pitch_LiftCtrl_OutPut(p);
			}break;
			case UnitePump_Roll:
			{
				//输入命令 逻辑处理
				UPL_Roll_LiftCtrl_CmdPrep(p);
				//输出指令解析
				UPL_Roll_LiftCtrl_OutPut(p);
			}break;

		}
	}
}



/************************************END OF FILE************************************/
