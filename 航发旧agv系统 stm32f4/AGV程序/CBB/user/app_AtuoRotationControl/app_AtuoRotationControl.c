/**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: app_AtuoRotationControl.c
** 创 建 人: HEBIN
** 描    述: 实现磁条导航自转到位判断以及对于磁条数据过滤


**------------- 版本信息 ---------------------------------------------------------------
** 版    本:  V1.0.0
** 创建日期:  2023.12.19
** 描    述:  新建

**--------------------------------------------------------------------------------------   
**/




#define   APP_ATUOROTATIONCONTROLTION_GLOBALS

#include "app_AtuoRotationControl.h"
#include "includes.h"


#define delay_fuction(A, B, C)		((s32)(A - B) > (s32)(C))

#define delaytime_t       25
#define Filt_Number   8
//延时等待操作A:系统计数器,B：需要延时等待的时间计数器,C:等待的时间值
u32 g_filttime_cnt=0;
u32 g_Rotationtimeout_cnt=0;
/***************************************************************************************
** 函数名称: RotationPara_Init
** 功能描述: 对自转参数进行初始化
** 参    数:	speed1		开始速度
							speed2		中途速度
							speed3		结束速度
							timeout_t	超时时间
							*p        自转对象结构体
** 返 回 值: None
****************************************************************************************/
void  RotationPara_Init(_auto_rotationcontrol *p,f32 speed1,f32 speed2,f32 speed3,u32 timeout_t)
{
	p->start_speed=speed1;
	p->standard_speed=speed2;
	p->final_speed=speed3;
	p->roation_timeout=timeout_t;
}
/***************************************************************************************
** 函数名称: AutoRotationControl
** 功能描述: 自转过程控制函数
** 参    数:
						*p        自转对象结构体
						SysMs_t		系统ms计数

							
** 返 回 值: None
****************************************************************************************/
u8 AutoRotationControl(_auto_rotationcontrol *p,u32 SysMs_t)
{
	u8 res=0;
	float outspeed=0;
	static bool rotation_continue_falg=true;
	s8	 dir=0;
	u16  num=0;
	s16  ang;

	if(p->forntmag_strength)
		num++;	
	if(p->backmag_strength)
		num++;	
	if(!p->leftmag_strength)
		num++;	
	if(p->rightmag_strength)
		num++;
		
	switch (p->rotation_runstep)
	{
			case Rotation_RunStep_init:
				if(p->forntmag_strength&&p->backmag_strength&&p->leftmag_strength&&p->rightmag_strength)
				{
					outspeed=p->start_speed;
					p->rotation_runstep=Rotation_RunStep_lostsignal;
				}else
				{
					p->rotation_runstep=Rotation_RunStep_err;
					p->rotation_state=RotationState_err;
				}
				p->rotation_state=RotationState_null;
				g_Rotationtimeout_cnt=SysMs_t;
			break;
			case Rotation_RunStep_lostsignal:
				if(!p->forntmag_strength&&!p->backmag_strength&&!p->leftmag_strength&&!p->rightmag_strength)
				{
					outspeed = p->standard_speed;
					p->rotation_runstep=Rotation_RunStep_lostsignal;
				}
				p->rotation_state=RotationState_moving;
			break;
			case Rotation_RunStep_againfindsignal:
				if(num>=2)
				{
					outspeed=p->final_speed;
					p->rotation_runstep=Rotation_RunStep_arrivepostion;
				}
				p->rotation_state=RotationState_moving;
			break;
			case Rotation_RunStep_arrivepostion:
				 if(num<2)
				{
					outspeed=0;
					p->rotation_state=RotationState_err;
				}
				if(((abs(p->fornt_postion)<40&&abs(p->back_postion)<45)||(abs(p->back_postion)<40&&abs(p->fornt_postion)<45))
					&&p->forntmag_strength<-200&&p->backmag_strength<-200					
				)
				{
					outspeed=0;
					p->rotation_runstep=Rotation_RunStep_judge_ok;
					p->rotation_state=RotationState_moving;
				}
			break;
			case Rotation_RunStep_judge_ok:
				if(abs((p->tartget_towards)-(p->current_towards))==180&&rotation_continue_falg==true)
				{
					p->rotation_runstep=Rotation_RunStep_init;
					rotation_continue_falg=false;
				}else
				{
					p->rotation_runstep=Rotation_RunStep_finished;
					outspeed=0;					
				}
			break;
			case Rotation_RunStep_finished:
			{				
				p->rotation_state=RotationState_finished;
				p->rotation_runstep=Rotation_RunStep_init;
				rotation_continue_falg=true;
				outspeed=0;
				res=1;
			}
			break;
			case Rotation_RunStep_err:
				p->rotation_state=RotationState_err;
				outspeed=0;
			break;
			default:
				outspeed=0;
			break;
	}
	if(delay_fuction(SysMs_t,g_Rotationtimeout_cnt,p->roation_timeout))
	{
		p->rotation_state=RotationState_timeout;
		outspeed=0;
	}
	ang=p->tartget_towards-p->current_towards;
 //速度处理
  if(ang==90 || ang== 180 || ang== -270)  
  {
     dir = 1; 
  }   
  if(ang==-90 || ang== -180 || ang== 270)
  {
    dir = -1;     
  }
	p->rotation_speed=outspeed*dir;
	return res;
}
/***************************************************************************************
** 函数名称: Postion_Filt
** 功能描述: 数据过滤处理
** 参    数:
						*p        自转数据对象结构体
						SysMs_t		系统ms计数

							
** 返 回 值: None
****************************************************************************************/
void Postion_Filt(_auto_rotationcontrol *p,u32 SysMs_t)
{
	u8 i=0,j=0,k=0;
	s16 temp_postion[4][8]={0},
	temp_strength[4][8]={0};
	s16 postion_temp=0,
		strength_temp;
	s16	postion_tempbuf[4]={0},
		strength_tempbuf[4]={0};
	if(delay_fuction(SysMs_t,g_filttime_cnt,delaytime_t))
	{

		temp_postion[0][0]=p->fornt_org_postion;
		temp_postion[1][0]=p->back_org_postion;
		temp_postion[2][0]=p->left_org_postion;
		temp_postion[3][0]=p->right_org_postion;
		temp_strength[0][0]=p->forntmag_org_strength;
		temp_strength[1][0]=p->backmag_org_strength;
		temp_strength[2][0]=p->leftmag_org_strength;
		temp_strength[3][0]=p->rightmag_org_strength;
		for(i=0;i<(Filt_Number -1);i++)
		{
			temp_postion[0][Filt_Number-1-i]=temp_postion[0][Filt_Number-1-1-i];
			temp_postion[1][Filt_Number-1-i]=temp_postion[1][Filt_Number-1-1-i]; 
			temp_postion[2][Filt_Number-1-i]=temp_postion[2][Filt_Number-1-1-i];
			temp_postion[3][Filt_Number-1-i]=temp_postion[3][Filt_Number-1-1-i];
			temp_strength[0][Filt_Number-1-i]=temp_strength[0][Filt_Number-1-1-i];
			temp_strength[1][Filt_Number-1-i]=temp_strength[1][Filt_Number-1-1-i]; 
			temp_strength[2][Filt_Number-1-i]=temp_strength[2][Filt_Number-1-1-i];
			temp_strength[3][Filt_Number-1-i]=temp_strength[3][Filt_Number-1-1-i]; 
		}
			for(j=0;j<7;j++)
			{
				for(k=0;k<7-j;k++)
				{
					if(temp_postion[0][k]>temp_postion[0][k+1])
					{
						postion_temp=temp_postion[0][k];
						temp_postion[0][k]=temp_postion[0][k+1];
						temp_postion[0][k+1]=postion_temp;
					}
					if(temp_postion[1][k]>temp_postion[1][k+1])
					{
						postion_temp=temp_postion[1][k];
						temp_postion[1][k]=temp_postion[1][k+1];
						temp_postion[1][k+1]=postion_temp;

					}
					if(temp_postion[2][k]>temp_postion[2][k+1])
					{
						postion_temp=temp_postion[2][k];
						temp_postion[2][k]=temp_postion[2][k+1];
						temp_postion[2][k+1]=postion_temp;

					}
					if(temp_postion[3][k]>temp_postion[3][k+1])
					{
						postion_temp=temp_postion[3][k];
						temp_postion[3][k]=temp_postion[3][k+1];
						temp_postion[3][k+1]=postion_temp;
					}
					if(temp_strength[0][k]>temp_strength[0][k+1])
					{
						strength_temp=temp_postion[0][k];
						temp_strength[0][k]=temp_strength[0][k+1];
						temp_strength[0][k+1]=strength_temp;
					}
					if(temp_strength[1][k]>temp_strength[1][k+1])
					{
						strength_temp=temp_postion[1][k];
						temp_strength[1][k]=temp_strength[1][k+1];
						temp_strength[1][k+1]=strength_temp;
					}
					if(temp_strength[2][k]>temp_strength[2][k+1])
					{
						strength_temp=temp_postion[2][k];
						temp_strength[2][k]=temp_strength[2][k+1];
						temp_strength[2][k+1]=strength_temp;
					}
					if(temp_strength[3][k]>temp_strength[3][k+1])
					{
						strength_temp=temp_postion[4][k];
						temp_strength[4][k]=temp_strength[4][k+1];
						temp_strength[4][k+1]=strength_temp;
					}

				}	
			}
				for(i=3;i<5;i++)
				{	
					postion_tempbuf[0] += temp_postion[0][i];
					postion_tempbuf[1] += temp_postion[1][i];	
					postion_tempbuf[2] += temp_postion[2][i];	
					postion_tempbuf[3] += temp_postion[3][i];
					strength_tempbuf[0] += temp_strength[0][i];
					strength_tempbuf[1] += temp_strength[1][i];	
					strength_tempbuf[2] += temp_strength[2][i];	
					strength_tempbuf[3] += temp_strength[3][i];	
				}
				p->fornt_postion = postion_tempbuf[0]>>1;// 
				p->back_postion  = postion_tempbuf[1]>>1;// 
				p->left_postion  = postion_tempbuf[2]>>1;// 
				p->right_postion = postion_tempbuf[3]>>1;//
				p->forntmag_strength = strength_tempbuf[0]>>1;// 
				p->backmag_strength  = strength_tempbuf[1]>>1;// 
				p->leftmag_strength  = strength_tempbuf[2]>>1;// 
				p->rightmag_strength = strength_tempbuf[3]>>1;//
	}	
}  