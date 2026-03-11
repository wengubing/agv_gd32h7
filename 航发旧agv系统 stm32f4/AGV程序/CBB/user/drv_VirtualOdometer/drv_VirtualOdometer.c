/********************************** Copyright ***********************************
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : drv_VirtualOdometer.c
  * Version    : V1.0
  * Author     : BBZZ、Master
  * Date       : 2023.12.20
  * Description: {描述}
  *******************************************************************************/
#include "drv_VirtualOdometer.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"


#define StatisticTime    (1)  	/*里程统计时间 单位ms*/

static OdometerData_t* g_odo_Data = NULL;

static float	RunEfficiency = 1;	  /*积分时间效率*/
static float	DriveEfficiency = 1;  /*驱动器执行效率*/

static float 	K_MotorToWheelRes = 1.0f;/*减速比*轮子周长*/
static float 	x_k = 1.0f;
static float 	y_k = 1.0f;

static float 	z_k2 = 1.0f;
static float 	z_k3 = 1.0f;

static float	Speed_X = 0;/*世界坐标X轴电机速度方向*/
static float	Speed_Y = 0;/*世界坐标Y轴电机速度方向*/
static float	Speed_Z = 0;/*世界坐标Z轴电机速度方向*/
/*规定：     90°
	*        ↑Vy
	*   \ +  |  - /
	*    \   |   /
	*     \  |  /
	*      \ | /
	*       \|/
	* --------------→Vx 0°   世界坐标、车体坐标如图所示，左偏为正，右偏为负。
*/

//static float 	Correction_Dis = 5.89343596f,			/*里程X轴*/
//				Correction_X = 5.89343596f,			/*里程X轴*/
//				Correction_Y = 5.89343596f;			/*里程Y轴*/
//static float 	Correction_Z = 272.414307f;			/*里程Z轴*/
//static float 	Correction_Z2 = 1.0f;			/*里程Z轴*/
//static float 	Correction_Z3 = 1.0f;			/*里程Z轴*/

static float 	Mileage_X = 0;	 	 /*里程X轴*/
static float 	Mileage_Y = 0;		  /*里程Y轴*/
static float 	Mileage_Z = 0;		  /*里程Z轴*/

//static uint32_t LastSysCount = 0;	  /*系统计数存储*/
static uint32_t g_Integrate_cnt = 0;	  /*统计时间计数*/
static uint32_t g_DelayTime_cnt = 0;	  /*统计次数*/

float GyroYawTempBuffer = 0.0f;

int32_t Remix_Integration ( Wheel_Type_t WheelType );


/**********************************************************************************************************
*	函 数 名:OdometerInit
*	功能说明:里程计初始化
*	形    参: 初始化结构体指针
*	返 回 值: 初始化结果
*   其    他：无
**********************************************************************************************************/
bool OdometerInit ( OdometerData_t* p_Init )
{
    OdometerData_t* pH = p_Init;
    g_odo_Data = p_Init;
    if ( p_Init == NULL )
        return false;

    if ( pH->InitData.WheelNum == 0 || pH->InitData.WheelNum > 16 )
        return false;

	K_MotorToWheelRes = g_odo_Data->InitData.GBRatio * Pai * g_odo_Data->InitData.Wheelradius/60;

	x_k = K_MotorToWheelRes;
	y_k = K_MotorToWheelRes;

    if ( pH->InitData.WheelType == Steering || pH->InitData.WheelType == DiffSteerings )
    {
//        CarAngelA = atan ( tan ( pH->InitData.CarLRTread / pH->InitData.CarFBTread ) ); /*A角大小*/
//        Hypotenuse = sqrt ( pH->InitData.CarLRTread * pH->InitData.CarLRTread + pH->InitData.CarFBTread * pH->InitData.CarFBTread ); /*斜边大小*/
    }
    return true;
}

/**********************************************************************************************************
*	函 数 名:OdometerRefresh
*	功能说明:里程计起始点刷新函数
*	形    参: 无
*	返 回 值: 无
*   其    他：无
**********************************************************************************************************/
void OdometerRefresh ( void )
{
	g_odo_Data->RTx = g_odo_Data->FixX;
	g_odo_Data->RTy = g_odo_Data->FixY;
	g_odo_Data->RTz = g_odo_Data->FixZ;
	Mileage_X = g_odo_Data->FixX/g_odo_Data->InitData.Correction_X;
	Mileage_Y = g_odo_Data->FixY/g_odo_Data->InitData.Correction_Y;

	if(g_odo_Data->InitData.GyroType == 0)
	{
		/*自转时的z轴积分*/
		if(g_odo_Data->WorkMode == OdoWalkMode_GoRotation)
		{
			Mileage_Z = g_odo_Data->FixZ/g_odo_Data->InitData.Correction_Z;
		}
		/*正向运行时的z轴积分*/
		else if(g_odo_Data->WorkMode == OdoWalkMode_GoLine)
		{
			Mileage_Z = g_odo_Data->FixZ/g_odo_Data->InitData.Correction_Z2;
		}
		/*横向运行时的z轴积分*/
		else if(g_odo_Data->WorkMode == OdoWalkMode_GoTransverse)
		{
			Mileage_Z = g_odo_Data->FixZ/g_odo_Data->InitData.Correction_Z3;
		}
	}
	else
	{
		GyroYawTempBuffer = g_odo_Data->GyroYaw;
		Mileage_Z = g_odo_Data->FixZ; 
	}
	
	g_odo_Data->FixFlag = 0;
}
/**********************************************************************************************************
*	函 数 名:OdometerMain
*	功能说明:里程计主函数
*	形    参: 无
*	返 回 值: 无
*   其    他：无
**********************************************************************************************************/
void OdometerMain ( void )
{
    OdometerData_t* pH = g_odo_Data;
    odo_Init_t* pI = &pH->InitData;

    g_DelayTime_cnt++;/*进来一次加一次，此函数放在1ms任务中，理论上每次统计时该值应该正好等于StatisticTime*/

    /*时间效率计算，当OdometerMain不是1ms间隔调用时，虽然精度有所下降，但至少不会漏积*/
    if ( ( *pI->SysCount - g_Integrate_cnt ) >= StatisticTime ) /*理论上StatisticTime ms进入一次*/
    {
        RunEfficiency = (( float ) g_DelayTime_cnt ) / ( *pI->SysCount - g_Integrate_cnt );
		g_Integrate_cnt = *pI->SysCount;
        g_DelayTime_cnt = 0;
    }

	/*如果刷新标志置位 直接刷新*/
    if(pH->FixFlag)
    {
        OdometerRefresh();
    }
	/*否则进行实时积分*/
	else
	{
		Remix_Integration ( pI->WheelType );
		pH->RTx = Mileage_X * g_odo_Data->InitData.Correction_X;
		pH->RTy = Mileage_Y * g_odo_Data->InitData.Correction_Y;
		pH->RTxy = sqrt(pow(pH->RTx, 2) + pow(pH->RTy, 2 ));
		if(g_odo_Data->InitData.GyroType == 0)
		{
			/*自转时的z轴积分*/
			if(g_odo_Data->WorkMode == OdoWalkMode_GoRotation)
			{
				float temp =  Mileage_Z * g_odo_Data->InitData.Correction_Z;
				
				/*角度变换为0 -360°*/
				while(temp<0.0f || temp >36000.0f)
				{
					if(temp < 0.0f)
					{
						temp += 36000.0f;
					}
					else if(temp >= 36000.0f)
					{
						temp -= 36000.0f;
					}
				}
				pH->RTz = temp;
			}
			/*正向运行时的z轴积分*/
			else if(g_odo_Data->WorkMode == OdoWalkMode_GoLine)
			{
				pH->RTz = Mileage_Z * g_odo_Data->InitData.Correction_Z2;
			}
			/*横向运行时的z轴积分*/
			else if(g_odo_Data->WorkMode == OdoWalkMode_GoTransverse)
			{
				pH->RTz = Mileage_Z * g_odo_Data->InitData.Correction_Z3;
			}
		}
		else
		{
			pH->RTz = Mileage_Z;
		}
	}
}
/**********************************************************************************************************
*	函 数 名:Mecanum_Integration
*	功能说明:麦轮复合里程积分
*	形    参: 无
*	返 回 值: 里程
*   其    他：此函数适用于麦轮复合导航
**********************************************************************************************************/
float Mecanum_Integration ( void )
{
    return -1;
}

/**********************************************************************************************************
*	函 数 名:Differential_Integration
*	功能说明:差速轮复合里程积分
*	形    参: 无
*	返 回 值: 里程
*   其    他：此函数适用于差速论复合导航
**********************************************************************************************************/
#define MoveOffset	1.05f

float Differential_Integration ( void )
{
    return -1;
}

/**********************************************************************************************************
*	函 数 名:Steering_Integration
*	功能说明:单舵轮复合里程积分
*	形    参: 无
*	返 回 值: 里程
*   其    他：此函数适用于单舵轮复合导航
**********************************************************************************************************/
float Steering_Integration ( void )
{
    return 0;
}


/**********************************************************************************************************
*	函 数 名:DiffSteerings_Integration
*	功能说明:差速舵轮复合里程积分
*	形    参: 无
*	返 回 值: 里程
*   其    他：此函数适用于差速舵轮复合导航
**********************************************************************************************************/
void DiffSteerings_Integration (void) /*与单舵轮解算方法一致，区别是要先算出整个模组的速度矢量*/
{
    OdometerData_t* pH = g_odo_Data;
    odo_Init_t* pI = &(pH->InitData);

    int16_t ModuleSpeed[8];/*模组速度*/

    {
        int16_t i = 0;
        for (; i < 8; i++)
        {
            ModuleSpeed[i] = (pH->SpeedActual[2*i] - pH->SpeedActual[2*i+1]) / 2;
        }
    }
	Speed_X = 0;/*世界坐标X轴电机速度方向*/
	Speed_Y = 0;/*世界坐标Y轴电机速度方向*/
	Speed_Z = 0;/*世界坐标Z轴电机速度方向*/

    {
        int32_t SpeedSum = 0,  /*整体运动线速度*/
                AngleSum = 0;  /*整体朝向角度*/
        if ( pH->Voz == 0 ) /*直线运动，此时各个模组速度矢量平行*/
        {
            float Temp = 0;
            float AverageSpeed = 0,
                  AverageAngle = 0;
            uint8_t i = 0;
            for ( ; i < (pI->WheelNum / 2) ; i++ )
            {
                SpeedSum += ModuleSpeed[i];
                AngleSum += pH->ModuleAngle[i];/*单位0.01°*/
            }
            AverageSpeed = (float)SpeedSum / (pI->WheelNum / 2) * K_MotorToWheel;/*算出所有轮子的平均速度*/
            AverageAngle = (float)AngleSum / (pI->WheelNum / 2);/*算出所有轮子的平均角度*//*单位0.01°*/

            /*此时的角度是车体坐标系的偏角，要转换成世界坐标*/
            Temp = cos ( ( AverageAngle + Mileage_Z ) / 100.0f * K_AngleToRad );/*单位转换成1°*/
            Speed_X = AverageSpeed * Temp;
            Temp = sin ( ( AverageAngle + Mileage_Z ) / 100.0f * K_AngleToRad );
            Speed_Y = AverageSpeed * Temp;
            Speed_Z = 0;/*直线运动Z轴无速度*/
            pH->Distance += pI->Correction_Dis*fabs(AverageSpeed) * DriveEfficiency / ( 1000.0f * RunEfficiency ) / 1000.0f;
        }
        else if ( pH->Voz != 0 && ( pH->Vox != 0 || pH->Voy != 0 ) ) /*圆周运动，整体绕某个原点运动*/
        {

        }
        else if ( pH->Voz != 0 && pH->Vox == 0 && pH->Voy == 0 ) /*自转运动，绕车体中心旋转*/
        {
            float R_o = 0;/*整车运动半径*/
            Speed_X = 0;
            Speed_Y = 0;
            R_o = sqrt ( pow ( pI->CarLRTread, 2 ) + pow ( pI->CarFBTread, 2 ) );
            Speed_Z = (fabs( ModuleSpeed[0]) + fabs(ModuleSpeed[1]) + fabs(ModuleSpeed[2]) + fabs(ModuleSpeed[3] )) / 4 
					  * K_MotorToWheel / R_o * K_RadToAngle; /*Z轴速度 = 角速度*/
            if ( pH->Voz > 0 ) /*逆时针旋转，朝向角增大*/
            {
                Speed_Z =  Speed_Z;
            }
            else if ( pH->Voz < 0 ) /*顺时针旋转，朝向角减小*/
            {
                Speed_Z = -Speed_Z;
            }
        }
        Mileage_X += Speed_X * DriveEfficiency / ( 1000.0f * RunEfficiency );
        Mileage_Y += Speed_Y * DriveEfficiency / ( 1000.0f * RunEfficiency );
        Mileage_Z += Speed_Z * DriveEfficiency / ( 1000.0f * RunEfficiency );
		
    }
}
/**********************************************************************************************************
*	函 数 名:DiffSteerings_IntegrationOZ
*	功能说明:差速舵轮用摇杆值进行z轴积分；当GyroType不为0时，不会调用
*	形    参: 无
*	返 回 值: 里程
*   其    他：此函数适用于差速舵轮复合导航
**********************************************************************************************************/
void DiffSteerings_IntegrationOZ (void)
{
	/*自转时的z轴积分*/
	if(g_odo_Data->WorkMode == OdoWalkMode_GoRotation)
	{
		Speed_Z = g_odo_Data->Voz;
		Mileage_Z += Speed_Z / ( 1000.0f * RunEfficiency );
	}
	/*正向运行时的z轴积分*/
	else if(g_odo_Data->WorkMode == OdoWalkMode_GoLine)
	{
		if(g_odo_Data->RotDis != 0)
		{
			Speed_Z = z_k2*g_odo_Data->Voy/g_odo_Data->RotDis;
		}
		else
		{
			Speed_Z = z_k2*g_odo_Data->Voy/276447232.0f;
		}
		Mileage_Z += Speed_Z / ( 1.0f * RunEfficiency );
	}
	/*横向运行时的z轴积分*/
	else if(g_odo_Data->WorkMode == OdoWalkMode_GoTransverse)
	{
		if(g_odo_Data->RotDis != 0)
		{
			Speed_Z = z_k3*g_odo_Data->Vox/g_odo_Data->RotDis;
		}
		else
		{
			Speed_Z = z_k3*g_odo_Data->Vox/276447232.0f;
		}
		Mileage_Z += Speed_Z / ( 1.0f * RunEfficiency );
	}
}

void DiffSteerings_IntegrationOXYZ (void) /*与单舵轮解算方法一致，区别是要先算出整个模组的速度矢量*/
{
		/*车体坐标系y轴和世界坐标系y轴夹角*/
		static double sina   = 0.0f,cosa   = 0.0f;
		/*车体坐标系x轴和世界坐标系x轴夹角 等于y轴夹角减去90°*/
		static double sinb   = 0.0f,cosb   = 0.0f;
		/*车体轮系和车体坐标系y轴的夹角*/
		static double sinc   = 0.0f,cosc   = 0.0f;

		static double SpeedXY_buff = 1.0f,SpeedX_buff = 1.0f,SpeedY_buff = 1.0f;
		double AverageRad   = 0.0f;
		uint8_t i = 0;

		/*车体坐标系角度对应系数 */
		for (i = 0; i < (g_odo_Data->InitData.WheelNum / 2) ; i++ )
		{
			AverageRad += g_odo_Data->ModuleAngle[i];/*单位0.01°*/
		}
		AverageRad = ((AverageRad/(g_odo_Data->InitData.WheelNum / 2))+90.0f)*K_AngleToRad;
		sinc = sin(AverageRad);
		cosc = cos(AverageRad);

		/*正向运行时，用Vy来做速度的积分*/
		if(g_odo_Data->WorkMode == OdoWalkMode_GoLine)
		{
			SpeedX_buff = g_odo_Data->Voy*cosc;
			SpeedY_buff = g_odo_Data->Voy*sinc;
		}
		/*横向运行时，用Vx来做速度的积分*/
		else if(g_odo_Data->WorkMode == OdoWalkMode_GoTransverse)
		{
			SpeedX_buff = g_odo_Data->Vox*cosc;
			SpeedY_buff = g_odo_Data->Vox*sinc;
		}
		else
		{
			SpeedX_buff = 0;
			SpeedY_buff = 0;
		}

		/*计算正坐标交变换系数*/
		AverageRad = (g_odo_Data->RTz/100.0f)*K_AngleToRad;
		sina = sin(AverageRad);
		cosa = cos(AverageRad);
		AverageRad = ((g_odo_Data->RTz/100.0f)-90.0f)*K_AngleToRad;
		sinb = sin(AverageRad);
		cosb = cos(AverageRad);

		/*将车体坐标系的速度 坐标正交变化到世界坐标系下*/
		Speed_X = x_k*SpeedX_buff*cosb + y_k*SpeedY_buff*cosa;
		Speed_Y = x_k*SpeedX_buff*sinb + y_k*SpeedY_buff*sina;

		SpeedXY_buff = sqrt ( pow ( Speed_X, 2 ) + pow ( Speed_Y, 2 ) );

		g_odo_Data->Distance += g_odo_Data->InitData.Correction_Dis * SpeedXY_buff / ( 1000.0f * RunEfficiency ) / 1000.0f;

		if(g_odo_Data->InitData.GyroType == 0)
		{
			DiffSteerings_IntegrationOZ();
		}
		else
		{
			Mileage_Z = g_odo_Data->FixZ+(g_odo_Data->GyroYaw-GyroYawTempBuffer)*100.0f; 
		}
		

        Mileage_X += Speed_X / ( 1000.0f * RunEfficiency );
        Mileage_Y += Speed_Y / ( 1000.0f * RunEfficiency );
        

}
/**********************************************************************************************************
*	函 数 名:Remix_Integration
*	功能说明:复合里程积分
*	形    参: 轮系类型
*	返 回 值: 无
*   其    他：此函数适用于复合导航
**********************************************************************************************************/
int32_t Remix_Integration ( Wheel_Type_t WheelType )
{
    if ( WheelType > DiffSteerings || WheelType <= NullWheel ) /*若参数不合法，返回-1*/
        return -1;

    switch ( WheelType )
    {
        case Mecanum : /*麦轮*/
        {
            Mecanum_Integration();
        }
        break;

        case Differential :/*差速轮*/
        {
            Differential_Integration();
        }
        break;

        case Steering:/*单舵轮*/
        {
            Steering_Integration();
        }
        break;

        case DiffSteerings:/*差速舵轮*/
        {
            //DiffSteerings_Integration();
			DiffSteerings_IntegrationOXYZ();
        }
        break;

        default :
            break;
    }
    return 0;
}





/************************************END OF FILE************************************/
