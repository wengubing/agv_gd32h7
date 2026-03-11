/********************************** Copyright ***********************************
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_VoicePlayer.c
  * Version    : V1.0
  * Author     : CDHF
  * Date       : 2024.5.13
  * Description: 实现对雷赛伺服控制器组成的升降模组的控制
  *******************************************************************************/


#include "app_LeadShineServoControl.h"
#include "arm_math.h"


unsigned int   g_LeadShineServo_Runtime_cnt=0,g_LiftServo_timeout_cnt=0;


/***************************************************************************************
** 函数名称: LeadShine_SoftReset
** 功能描述: 软件复位
** 参    数: 伺服控制结构体指针
** 返 回 值: 无
****************************************************************************************/
void LeadShine_SoftReset(LeadShineServoControl_TypeDef *p)
{
		if(*(p->SoftReset_flag)==1)
		{
			p->Servo_CtrlCmd.SoftResetCmd=0x6666;//伺服软件复位
			*(p->SoftReset_flag)=0;
			p->Current_RunStep=RunStep_Reset; 
		}
}

/***************************************************************************************
** 函数名称: LeadShine_AlarmStausJudge
** 功能描述: 报警状态反馈
** 参    数: 伺服控制结构体指针
** 返 回 值: res  0 :无报警  1：存在报警
****************************************************************************************/
void LeadShine_AlarmStausJudge(LeadShineServoControl_TypeDef *p)
{
	unsigned char i=0,res=0,Alarm_Flag=0;
	for(i=0;i<Servo_Max_Num4;i++)
	{
		if(p->Servo_FeedbackData[i].Servo_AlarmStatus!=0)
		{
			Alarm_Flag++;
			p->Servo_CtrlCmd.EnableCmd=0x00;     //使能状态
			p->Servo_CtrlCmd.RunStatusCmd=0x00;  //运行状态
		}		
	}
	if(*(p->OffCtrlCheck)==1)//若开启强制则认为无强制认为无报警
	{
		Alarm_Flag=0;
	}
	if(Alarm_Flag)	
	res=1;	
	return res;
}
/***************************************************************************************
** 函数名称: LeadShine_PostionClearZeroJudge
** 功能描述: 升降伺服位置复位到位状态反馈
** 参    数: 伺服控制结构体指针
** 返 回 值: res  0 :到位  1：未到位
****************************************************************************************/
u8 LeadShine_PostionClearZeroJudge(void)
{
	unsigned char i=0,res=0,OK_flag=0;
	for(i=0;i<Servo_Max_Num;i++)
	{
		if(fabs(NavgHeightSensor_Data.LiftServo_FeedbackData[i].Servo_ActivePostion)<1500)
		{
			OK_flag++;
		}		
	}
	if(OK_flag>=Servo_Max_Num)	
	res=1;		
	return res;
}
/***************************************************************************************
** 函数名称: app_LeadShineServoControl
** 功能描述: 伺服控制参数
** 参    数: 伺服控制结构体指针   系统ms计数
** 返 回 值: 无
****************************************************************************************/
void app_LeadShineServoControl(LeadShineServoControl_TypeDef *p,u32 Sysms_counter)
{
	  unsigned char  i=0;
	  static unsigned char Potionclear_Err_Cnt=0; //位置清除错误计数
		LeadShine_SoftReset(p);
		if(LeadShine_AlarmStausJudge(p))//报警判断
		{
			p->Current_RunStep=RunStep_AlarmStaus;
		}
    switch(p->Current_RunStep)
    {
      case RunStep_Reset :
        p->Servo_CtrlCmd.SoftResetCmd=0x6666;
        Potionclear_Err_Cnt=0;
        if(*(p->OffCtrlCheck))
        {
          p->Current_RunStep=RunStep_StopRun; 
        }else
        {
          p->Current_RunStep=RunStep_ResetOk; 
        }
        g_LeadShineServo_Runtime_cnt=Sysms_counter;
      break;
      case RunStep_ResetOk :
      if(Servo_delay_fuction(Sys_Ms, g_LeadShineServo_Runtime_cnt,1000))
			{	
				p->Servo_CtrlCmd.SoftResetCmdp->Servo_CtrlCmd.SoftResetCmd=0x0000;//软件复位						
			}
			if(Servo_delay_fuction(Sys_Ms, g_LeadShineServo_Runtime_cnt,2500))
			{
					p->Servo_CtrlCmd.EnableCmd=0x83;     //使能状态
					p->Servo_CtrlCmd.RunStatusCmd=0x10;  //运行状态
					p->Servo_CtrlCmd.RunModeCmd=0x03;    //回零模式
					p->Servo_CtrlCmd.SoftResetCmd=0x0000;//软件复位 
					p->Current_RunStep=RunStep_WaitInitOk;
					g_LeadShineServo_Runtime_cnt=Sysms_counter;
			}
      break;
      case RunStep_WaitInitOk :
      	for(i=0;i<Servo_Max_Num;i++)
		    {
			    if(Servo_delay_fuction(Sysms_counter, g_LeadShineServo_Runtime_cnt,20000))//回零操作超时，进入报警模式
			    {				
				   p->Current_RunStep=RunStep_AlarmStaus;
			    }
			    if((p->Servo_FeedbackData[i].Servo_IOInputStatus&0x10)!=0x10)
			    {				
				    break;	
			    }
			    if(i>=(Servo_Max_Num-1))
			    {				
					  Potionclear_Err_Cnt++;
            p->Servo_CtrlCmd.EnableCmd=0x00;     //使能状态
					  p->Servo_CtrlCmd.RunStatusCmd=0x21;  //运行状态---清除位置信息
					  p->Servo_CtrlCmd.RunModeCmd=0x00;    //停止运行
					  g_LeadShineServo_Runtime_cnt=Sysms_counter;
					  p->Current_RunStep=RunStep_SetRunMode;
			    }
				}
			break;
			case RunStep_ClearPostionOperationesetOk :
				if(Delay_Fuction(Sysms_counter,g_LeadShineServo_Runtime_cnt,1500))//增加这一步骤的原因是怕位置初始化不成功
				{
						NavgHeightSensor_Data.NavgHeightSensor_RunStatusCmd=0x21;//运行状态--清零
						NavgHeightSensor_Data.NavgHeightSensor_EnableCmd=0x83;  //使能状态	
				}	
				if(Delay_Fuction(Sysms_counter,g_LeadShineServo_Runtime_cnt,3000))//进行
				{
					if(LeadShine_PostionClearZeroJudge(p))
					{
							p->Current_RunStep=RunStep_SetRunMode;
						  g_LeadShineServo_Runtime_cnt=Sysms_counter;
					}else
					{
						g_LeadShineServo_Runtime_cnt=Sysms_counter;
						if(Potionclear_Err_Cnt<3)//若超过3次未清零成功，则直接进入位置模式
						{
							p->Current_RunStep=RunStep_WaitInitOk;							
						}
						else
						{
							p->Current_RunStep=RunStep_SetRunMode;
							for(i=0;i<Servo_Max_Num;i++)
							{
								p->Servo_RecordPostion[i]=p->Servo_FeedbackData[i].Servo_ActivePostion;//记录当前位置，避免由于清零不成功导致伺服乱转
							}
						}					
					}
						for(i=0;i<Servo_Max_Num;i++)
						{
							p->Servo_RecordPostion[i]=p->Servo_FeedbackData[i].Servo_ActivePostion;//记录当前位置，避免由于清零不成功导致伺服乱转
						}
						p->Servo_CtrlCmd.RunStatusCmd=0x21;//手动清零
						p->Servo_CtrlCmd.EnableCmd=0x00;  //失能状态	
				}
      break;
    case RunStep_SetRunMode :
			if(Servo_delay_fuction(Sys_Ms, g_LeadShineServo_Runtime_cnt,1500))
			{
					p->Servo_CtrlCmd.EnableCmd=0x83;     //使能状态
					p->Servo_CtrlCmd.RunStatusCmd=0x10;  //运行状态
					p->Servo_CtrlCmd.RunModeCmd=0x01;    //回零模式
					p->Servo_CtrlCmd.SoftResetCmd=0x0000;//软件复位 
					p->Servo_CtrlCmd.SpeedCtrCmd=*p.RunSpeed_High;
					p->Current_RunStep=RunStep_NormaLRun;
					g_LeadShineServo_Runtime_cnt=Sysms_counter;
			}
      break;
		case RunStep_NormaLRun ://处于运行模式后则可以进行升降伺服的控制与位置移动							
			for(i=0;i<Servo_Max_Num;i++)
			{
				if(p.HeightCtrCmd<p.LiftHeightLowValue)
				{
					p->Servo_CtrlCmd.PostionCtrCmd[i]=-2000;				
				}
				else
				{								
					if(p.HeightCtrCmd>p.LiftHeightMaxValue)
					{
						p->Servo_CtrlCmd.PostionCtrCmd[i]=p->Servo_RecordPostion[i][i]+(s32)((p.LiftHeightMaxValue+p.PostionOffset[i])*3333);			
					}else
					{						
						if(p.HeightCtrCmd==0)
						{
							p->Servo_CtrlCmd.PostionCtrCmd[i]=-2000;		
						}else
						{							
							p->Servo_CtrlCmd.PostionCtrCmd[i]=p->Servo_RecordPostion[i][i]+(s32)((p.HeightCtrCmd+p.PostionOffset[i])*3333);
						}							
					}				
				}
			}
				if(fabs(((f32)(p->Servo_CtrlCmd.PostionCtrCmd[i]-p->Servo_FeedbackData[i].Servo_ActivePostion)))<15000)
				p->Servo_CtrlCmd.SpeedCtrCmd=*p.RunSpeed_Low;
				else
				p->Servo_CtrlCmd.SpeedCtrCmd=*p.RunSpeed_High;
      break;
    case  RunStep_AlarmStaus :
					p->Servo_CtrlCmd.EnableCmd=0x00;     //使能状态
					p->Servo_CtrlCmd.RunStatusCmd=0x00;  //运行状态
					p->Servo_CtrlCmd.RunModeCmd=0x00;    //回零模式
					p->Servo_CtrlCmd.SoftResetCmd=0x0000;//软件复位 
					for(i=0;i<Servo_Max_Num;i++)
					p->Servo_CtrlCmd.PostionCtrCmd[i]=0;
      break;
    case RunStep_StopRun :
					p->Servo_CtrlCmd.EnableCmd=0x00;     //使能状态
					p->Servo_CtrlCmd.RunStatusCmd=0x00;  //运行状态
					p->Servo_CtrlCmd.RunModeCmd=0x00;    //回零模式
					p->Servo_CtrlCmd.SoftResetCmd=0x0000;//软件复位 
					for(i=0;i<Servo_Max_Num;i++)
					p->Servo_CtrlCmd.PostionCtrCmd[i]=0;
      break;
		}

}
