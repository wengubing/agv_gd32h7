/********************************** Copyright ***********************************
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_DiscreteMagNav.c
  * Version    : V2.0
  * Author     : MT
  * Date       : 2025.5.14
  * Description: 巡磁钉导航模块
  *******************************************************************************/
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "app_DiscreteMagNav.h"

#define MATHTYPE double

STATIC DisMagNavTypedef* DisMagNavP;/*模块实例指针*/
DisMagNavStepTypedef DisMagNavStep = DisMagNav_Init;/*主函数状态机*/
STATIC RuningStepTypedef RuningStep = Init_Step;/*曲线绘制状态机*/
MagNavTypedef FMagNav;/*行进方向前磁导航对象*/
STATIC MagNavTypedef BMagNav;/*行进方向后磁导航对象*/

typedef struct Error
{
    float Current_Error;/*当前误差*/
    float Last_Error;/*上一次误差*/
    float Previous_Error;/*上上次误差*/
} Error_t;


STATIC Error_t PIDError_F;

/***************************************************************************************
** 函数名称: PID_Increase
** 功能描述: 增量式PID
** 参    数: Sptr 误差结构体指针 PID PID参数指针 NowPlace 实际值 Point 期望值
** 返 回 值: 增量
****************************************************************************************/
double PID_Increase ( Error_t *Sptr, PID_t *PID, int32_t NowPlace, int32_t Point )
{

    int32_t iError;  //当前误差
    double Increase;  //最后得出的实际增量

    iError = Point - NowPlace;  // 计算当前误差

    Increase = PID->Kp * iError //E[k]项
               + PID->Ki * Sptr->Last_Error //E[k－1]项
               + PID->Kd * Sptr->Previous_Error; //E[k－2]项

    Sptr->Previous_Error = Sptr->Last_Error;  // 更新前次误差
    Sptr->Last_Error = iError;        // 更新上次误差

    return Increase;  // 返回增量
}

/***************************************************************************************
** 函数名称: DisMagNavInit
** 功能描述: 初始化参数赋值完成后执行，传入实例对象，初始化状态机
** 参    数: Pd 实例指针
** 返 回 值: true 初始化成功 false 初始化失败
****************************************************************************************/
bool DisMagNavInit ( DisMagNavTypedef* Pd )
{
    DisMagNavP = Pd;

    Pd->Output.FixFlag = Pd->Init.FixFlag;
    Pd->Output.FixX = Pd->Init.FixX;
    Pd->Output.FixY = Pd->Init.FixY;
    Pd->Output.FixZ = Pd->Init.FixZ;

    RuningStep = Init_Step;
    if ( Pd->Init.SensorDisFB != 0)
        return true;
    else
        return false;
}

/***************************************************************************************
** 函数名称: DisMagNavReInit
** 功能描述: 运动方向变化，或报错时复位使用
** 参    数: 无
** 返 回 值: true 初始化成功 false 初始化失败
****************************************************************************************/
bool DisMagNavReInit ( void )
{
    DisMagNavStep = DisMagNav_Init;/*复位状态机*/
    RuningStep = Init_Step;
	DisMagNavP->Output.Error.ErrorWord = 0;/*清空错误码*/
    return true;
}
/***************************************************************************************
** 函数名称: Function_Init
** 功能描述: 状态机初始化函数
** 参    数: 无
** 返 回 值: 无
****************************************************************************************/
extern uint8_t GoLine_Mode, Last_GoLine_Mode;	/*巡线模式*/
void Function_Init ( void )
{

    if ( DisMagNavP->Input.MovingDirection == Forward_Mode ) /*向前运动*/
    {
        DisMagNavP->P_FMagOffset = &DisMagNavP->Input.FrontMagOffset;    /*选定偏移数据来源*/
        DisMagNavP->P_BMagOffset = &DisMagNavP->Input.BackMagOffset;

        DisMagNavP->P_FMagStrength = &DisMagNavP->Input.FrontMagStrength;/*选定强度数据来源*/
        DisMagNavP->P_BMagStrength = &DisMagNavP->Input.BackMagStrength;
		
		DisMagNavP->Input.LineWalking_Mode_Last = DisMagNavP->Input.LineWalking_Mode;	/*记录上一次运行方向*/
		
			
    }
    else if ( DisMagNavP->Input.MovingDirection == Backward_Mode ) /*向后运动*/
    {
        DisMagNavP->P_FMagOffset = &DisMagNavP->Input.BackMagOffset;/*选定偏移数据来源*/
        DisMagNavP->P_BMagOffset = &DisMagNavP->Input.FrontMagOffset;

        DisMagNavP->P_FMagStrength = &DisMagNavP->Input.BackMagStrength;/*选定强度数据来源*/
        DisMagNavP->P_BMagStrength = &DisMagNavP->Input.FrontMagStrength;
		
		DisMagNavP->Input.LineWalking_Mode_Last = DisMagNavP->Input.LineWalking_Mode; /*记录上一次运行方向*/
			
    }
    else
    {
Error:  DisMagNavStep = DisMagNav_Error;/*错误态*/
        DisMagNavP->Output.Error.ErrorBit.InitDataError = true;/*置初始化数据错误*/
        return;
    }
	
	if(DisMagNavP->Init.PID_Arc == NULL 
	  || DisMagNavP->Init.PID_Str == NULL
	  || DisMagNavP->Init.P_systemCNT == NULL
	  || DisMagNavP->Init.FixFlag == NULL
	  || DisMagNavP->Init.FixX == NULL
	  || DisMagNavP->Init.FixY == NULL
	  || DisMagNavP->Init.FixZ == NULL
	  || DisMagNavP->Init.PID_Arc == NULL
	  || DisMagNavP->Init.PID_Str == NULL
	  )
		goto Error;
	
		
    DisMagNavP->P_SensorDis = &DisMagNavP->Init.SensorDisFB;/*传感器距离数据来源*/

    *DisMagNavP->Output.FixFlag = true;/*里程计数据清空*/
    *DisMagNavP->Output.FixX = 0;
    *DisMagNavP->Output.FixY = 0;
    *DisMagNavP->Output.FixZ = 9000;
    DisMagNavStep = DisMagNav_Running;/*运行态*/
}

/***************************************************************************************
** 函数名称: OffsetCalculate
** 功能描述: 根据里程计预测此刻偏移
** 参    数: X 当前里程计X轴数据，Z 当前里程计Z轴数据,Offset偏移，Pos方位
** 返 回 值: 偏移
** 说    明: 简化模型，都当初直线来寻，
**           X轴的变化直接作用在偏移上，Z轴的变化需要根据尺寸转换一下
****************************************************************************************/

uint32_t Delay_Deceleration = 0;
float FixRatio = 200.0f;	//纠偏时间系数

typedef enum
{
		Deceleration_Running,
		Deceleration_Idel,
}Decelerat_t;
Decelerat_t Decelerat;

void OffsetCalculate ( void ) 
{
	float Ratio = 0;
	
	Ratio = DisMagNavP->Input.AverageSpeed / 30000.0f;/*整体速度占比*/
	
	if(Delay_Fuc(*DisMagNavP->Init.P_systemCNT, Delay_Deceleration, 100))
	{
		Delay_Deceleration = *DisMagNavP->Init.P_systemCNT;
		
		switch(Decelerat)
		{
			case Deceleration_Running:
			{
				FMagNav.CarOffsetA = (DisMagNavP->P_FMagOffset - DisMagNavP->P_BMagOffset) /1502;
				
					if(FMagNav.MagOffset > 0)
					{
						FMagNav.CarOffset -= FMagNav.MagOffset / 1000.0f * Ratio * FixRatio;
						
						if(FMagNav.CarOffset <= 0)
						{
								FMagNav.CarOffset = 0;
								Decelerat = Deceleration_Idel;
						}
					}
					if(FMagNav.MagOffset < 0)
					{
						FMagNav.CarOffset -= FMagNav.MagOffset / 1000.0f * Ratio * FixRatio;
						if(FMagNav.CarOffset >= 0)
						{
								FMagNav.CarOffset = 0;
								Decelerat = Deceleration_Idel;
						}
					}
					if(DisMagNavP->P_FMagOffset - DisMagNavP->P_BMagOffset == 0)
					if(FMagNav.MagOffset == 0)
					{
						FMagNav.CarOffset = 0;
						Decelerat = Deceleration_Idel;
					}
					
					
			}break;
			
			case Deceleration_Idel:
				break;
		
		
		}
	}
	
}

/***************************************************************************************
** 函数名称: CurvePlotting
** 功能描述: 控制输出
** 参    数: float *SRatio,bool *InArcFlag
** 返 回 值: 无
****************************************************************************************/
STATIC float SpeedRatio = 0;  /*速度比例，用于限速*/
STATIC bool IsInArc = false;  /*是否在弯道*/

STATIC MATHTYPE FDistanceLast_Record, BDistanceLast_Record, /*磁钉间距记录*/
                FDistance_Last, BDistance_Last;


STATIC double F_PIDOffset,F_PIDAngleOffset;

STATIC double HandleRatio = 0;
STATIC PID_t PID_Para;
void CurvePlotting ( float *SRatio, bool *InArcFlag )
{
    if ( *InArcFlag ) /*在弯道*/ /*8字转向模式*/
    {
        
		PID_Para.Kp = DisMagNavP->Init.PID_Arc->Kp * SpeedRatio;
		PID_Para.Ki = DisMagNavP->Init.PID_Arc->Ki * SpeedRatio;
		PID_Para.Kd = DisMagNavP->Init.PID_Arc->Kd * SpeedRatio;

        F_PIDOffset = PID_Increase ( &PIDError_F, &PID_Para, FMagNav.CarOffset, 0 );
    }
    else
    {

		PID_Para.Kp = DisMagNavP->Init.PID_Str->Kp * SpeedRatio;
		PID_Para.Ki = DisMagNavP->Init.PID_Str->Ki * SpeedRatio;
		PID_Para.Kd = DisMagNavP->Init.PID_Str->Kd * SpeedRatio;

//        F_PIDOffset = PID_Increase ( &PIDError_F, &PID_Para, FMagNav.CarOffset, 0 );
		F_PIDOffset = PID_Increase ( &PIDError_F, &PID_Para, FMagNav.CarOffset, 0 );
		F_PIDAngleOffset = PID_Increase ( &PIDError_F, &PID_Para, FMagNav.CarOffsetA, 0 );
    }

    HandleRatio = F_PIDOffset / 200.0f;

    if ( HandleRatio > 1.0f ) /*限幅*/
        HandleRatio = 1.0f;
    if ( HandleRatio < -1.0f )
        HandleRatio = -1.0f;
	if(abs(FMagNav.CarOffset) > 10)
	{		
		DisMagNavP->Output.ControlX = HandleRatio;
		DisMagNavP->Output.ControlY = *SRatio * DisMagNavP->Input.Hy;
		DisMagNavP->Output.ControlZ = F_PIDAngleOffset;
	}else
	{
		DisMagNavP->Output.ControlX = 0;
		DisMagNavP->Output.ControlY = *SRatio * DisMagNavP->Input.Hy;
		DisMagNavP->Output.ControlZ = 0;		
	}
	
//    DisMagNavP->Output.ControlX = 0;
//	DisMagNavP->Output.ControlY = *SRatio * DisMagNavP->Input.Hy;
//    DisMagNavP->Output.ControlZ = HandleRatio * 127.0f;
}
/***************************************************************************************
** 函数名称: IsInArcJudge
** 功能描述: 是否在弯道判定
** 参    数: bool *InArcFlag, MATHTYPE *FRecord, MATHTYPE *BRecord
** 返 回 值: 无
****************************************************************************************/
STATIC int16_t ArcMagCount = 0;
void IsInArcJudge ( bool *InArcFlag, MATHTYPE *FRecord, MATHTYPE *BRecord )
{
    if ( FMagNav.IsFound ) /*找到磁钉*/
    {
        if ( FDistanceLast_Record < DisMagNavP->Init.ArcDistance ) /*且间距小于阈值*/
            ArcMagCount++;
        else
            ArcMagCount--;

        if ( ArcMagCount < 0 )
            ArcMagCount = 0;
    }


    if ( ArcMagCount >= 2 ) /*扫到两颗密集磁钉，判断为进弯道*/
        *InArcFlag = true;
    else
        *InArcFlag = false;

    if ( FDistanceLast_Record > DisMagNavP->Init.ExitArcDis ) /*间距大于阈值退出*/
    {

        *InArcFlag = false;
        ArcMagCount = 0;
    }
	
	if ( FDistanceLast_Record > DisMagNavP->Init.LostMagDis && FMagNav.MagCount > 1) /*丢失磁钉判断*/
	{
		DisMagNavP->Output.Error.ErrorBit.NotFoundMag = true;
        DisMagNavStep = DisMagNav_Error;/*进入错误态*/
	}
	
	*InArcFlag = false;/*纯直线*/
}
/***************************************************************************************
** 函数名称: SlowDownJudge
** 功能描述: 减速判定
** 参    数: float *SRatio 速率百分比指针
** 返 回 值: 无
****************************************************************************************/
typedef union
{
    uint8_t SDLevel;
    struct
    {
        uint8_t High_Speed: 1,   /*不限速*/
                Middle_Speed: 1, /*中档*/
                Low_Speed: 1,    /*低档*/
                bit3: 1,
                bit4: 1,
                bit5: 1,
                bit6: 1,
                bit7: 1;
    } SDLevelBits;
} SlowDownLevel_t;

STATIC SlowDownLevel_t SlowDownLevel, SlowDownLevel;

void SlowDownJudge ( float *SRatio, bool *InArcFlag )
{
    FDistanceLast_Record = ( DisMagNavP->Input.Distance - FDistance_Last ) * 1000.0f;
    BDistanceLast_Record = ( DisMagNavP->Input.Distance - BDistance_Last ) * 1000.0f;

    IsInArcJudge ( InArcFlag, &FDistanceLast_Record, &BDistanceLast_Record );

    if ( FMagNav.IsFound )
    {
        FMagNav.IsFound = false;
        FDistance_Last = DisMagNavP->Input.Distance;
    }
    if ( BMagNav.IsFound )
    {
        BMagNav.IsFound = false;
        BDistance_Last = DisMagNavP->Input.Distance;
    }

    if ( FMagNav.MagOffset > DisMagNavP->Init.OffsetSlowDownLevel_1 ) /*拖拽只关注前磁导航*/
        SlowDownLevel.SDLevelBits.Middle_Speed = true;
    else
        SlowDownLevel.SDLevelBits.Middle_Speed = false;

    if ( FMagNav.MagOffset > DisMagNavP->Init.OffsetSlowDownLevel_2 ) /*拖拽只关注前磁导航*/
        SlowDownLevel.SDLevelBits.Low_Speed = true;
    else
        SlowDownLevel.SDLevelBits.Low_Speed = false;

    if ( *InArcFlag )
        SlowDownLevel.SDLevelBits.Low_Speed = true;
    else
        SlowDownLevel.SDLevelBits.Low_Speed = false;/*解除低速*/

    if ( SlowDownLevel.SDLevel <= 0x01 )
        *SRatio = DisMagNavP->Init.SlowDownLevel[2];
    if ( SlowDownLevel.SDLevelBits.Middle_Speed )
        *SRatio = DisMagNavP->Init.SlowDownLevel[1];
    if ( SlowDownLevel.SDLevelBits.Low_Speed )
        *SRatio = DisMagNavP->Init.SlowDownLevel[0];

}

/***************************************************************************************
** 函数名称: Mag_Filter
** 功能描述: 磁导航数据滤波器，由于磁导航数据会持续一段时间，并不是瞬时的，而需要的是瞬时数据，
**   		 所以，通过滤波取得强度最大时的瞬时数据。
** 参    数: 无
** 返 回 值: 无
****************************************************************************************/
STATIC bool FMagEdgeCheck = false;/*检测入口标志*/

void Mag_Filter ( void )
{
    if ( *DisMagNavP->P_FMagStrength != 0) /*前进方向前磁导航有强度*/
    {
        if ( *DisMagNavP->P_FMagStrength > 0 && DisMagNavP->Init.MagOrientation ) /*磁钉布置为正强度*/
        {
            if ( *DisMagNavP->P_FMagStrength > DisMagNavP->Init.MagnetStrengthMin && !FMagEdgeCheck ) /*若大于阈值，取用，关闭入口*/
            {
                FMagNav.MagCount++;
                FMagNav.MagOffset = *DisMagNavP->P_FMagOffset;
                FMagNav.IsFound = true;
                FMagEdgeCheck = true;/*关闭入口*/
            }
        }
        else if ( *DisMagNavP->P_FMagStrength < 0 && !DisMagNavP->Init.MagOrientation ) /*磁钉布置为负强度*/
        {
            if ( abs ( *DisMagNavP->P_FMagStrength ) > DisMagNavP->Init.MagnetStrengthMin && !FMagEdgeCheck ) /*若大于阈值，取用，关闭入口*/
            {
                FMagNav.MagCount++;
                FMagNav.MagOffset = *DisMagNavP->P_FMagOffset;
                FMagNav.IsFound = true;
                FMagEdgeCheck = true;/*关闭入口*/
            }
        }
    }
    else/*前进方向前磁导航无强度*/
    {
        if ( FMagEdgeCheck )
        {
            FMagEdgeCheck = false;
        }
    }
}

/***************************************************************************************
** 函数名称: Function_Running
** 功能描述: 运行
** 参    数: 无
** 返 回 值: 无
****************************************************************************************/
void Function_Running ( void )
{

    Mag_Filter();/*磁导航数据处理，持续数据转换瞬时数据*//*此函数得到最新磁导航数据*//*得到  MagnetOffset  */
	
    switch ( RuningStep )
    {
        case Init_Step:/*起步时，未获得磁钉数据，有一段限速盲走*/
        {
            DisMagNavP->Output.ControlX = 0;
            DisMagNavP->Output.ControlZ = 0;/*盲走*/

            DisMagNavP->Output.ControlY = DisMagNavP->Input.Hy * 0.5f;

            if ( FMagNav.MagCount != FMagNav.MagCount_Last ) /*发现新前磁钉，重置里程计*/
            {
                FMagNav.MagCount_Last = FMagNav.MagCount;
                *DisMagNavP->Output.FixFlag = true;/*里程计数据清空*/
                *DisMagNavP->Output.FixX = 0;
                *DisMagNavP->Output.FixY = 0;
                *DisMagNavP->Output.FixZ = 9000;
                RuningStep = Runing_Step;
            }
        }
        break;

        case Runing_Step:/*根据里程计数据，绘制偏移曲线*/
        {
            if ( FMagNav.MagCount != FMagNav.MagCount_Last ) /*当发现新磁钉时，校准里程计*/
            {
                FMagNav.MagCount_Last = FMagNav.MagCount;
                *DisMagNavP->Output.FixFlag = true;/*里程计数据清空*/
                *DisMagNavP->Output.FixX = 0;
                *DisMagNavP->Output.FixY = 0;
                *DisMagNavP->Output.FixZ = 9000;
				
				FMagNav.CarOffset = FMagNav.MagOffset;
				Decelerat = Deceleration_Running;
            }

            OffsetCalculate ( ); /*根据里程计，推算现在的偏移*/

            SlowDownJudge ( &SpeedRatio, &IsInArc );
            CurvePlotting ( &SpeedRatio, &IsInArc ); /*根据里程计模拟前后磁导航偏移,Output结构体输出给巡磁条模块*/
        }
        break;
    }
}
/***************************************************************************************
** 函数名称: Function_Error
** 功能描述: 状态机函数
** 参    数: 无
** 返 回 值: 无
****************************************************************************************/
void Function_Error ( void )
{
    DisMagNavP->Output.ControlX = 0;
	DisMagNavP->Output.ControlY = 0;
	DisMagNavP->Output.ControlZ = 0;
}
/***************************************************************************************
** 函数名称: DisMagNav_Main
** 功能描述: 模块主函数，
** 参    数: 无
** 返 回 值: 无
****************************************************************************************/
void DisMagNav_Main ( void )
{
//	Function_Running();
    switch ( DisMagNavStep )
    {
        case DisMagNav_Init:/*初始化*/
        {
            Function_Init();
        }
        break;
        case DisMagNav_Running:/*运行*/
        {
            Function_Running();
        }
        break;
        case DisMagNav_Error:/*错误*/
        {
            Function_Error();
        }
        break;
        case DisMagNav_Standby:/*待机*/
        {
            __nop();
        }
        break;
        default:
            break;
    }
}

/************************************END OF FILE*****************************************/
