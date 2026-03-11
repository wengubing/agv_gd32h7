/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_VirtualOdo.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-09-30
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  ObjUpdate_VirtualOdo_GLOBALS

#include "ObjUpdate_VirtualOdo.h"
#include "ACC_Project.h"


/*陀螺仪 CBB没弄出来之前，暂时这么搞*/
union GyroA0030SFlaot{
	uint8_t byte_8bit[4];
	float	data_32bit;
};
typedef struct
{
	union GyroA0030SFlaot	GyroYaw;
	bool 					FixFlag;			/*数据修正标志*/
}GyroA0030S_TypeDef;

/*陀螺仪 CBB没弄出来之前，暂时这么搞*/
GyroA0030S_TypeDef			g_GyroA0030SData;

/***********************************************************
** 函数名称:		ObjUpdate_VirtualOdo
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_VirtualOdo(void)
{
	uint8_t res = 0x00;/*0-不需要更新里程计；其他-标定里程计*/
	OdometerData_t  			*g_VirtualOdoAGV		= &g_VirtualOdoData;	/*虚拟里程计对象指针		*/
	HostCtrl_DATA 				*g_AGVHostCtrl			= &g_HostData;			/*AGV框架代码对象指针	*/

	/*虚拟里程计输入数据管道构建*/
	{
		MotorResolving_TypeDef	*g_MRDataAGV			= &g_MotorResolving;	/*运动解算对象指针，等底盘控制API完成后，再做替换*/
		g_VirtualOdoAGV->Aox = g_MRDataAGV->Input.Aoxyz[0];
		g_VirtualOdoAGV->Aoy = g_MRDataAGV->Input.Aoxyz[1];
		g_VirtualOdoAGV->Aoz = g_MRDataAGV->Input.Aoxyz[2];
		g_VirtualOdoAGV->Vox = g_MRDataAGV->Input.Oxyz[0];
		g_VirtualOdoAGV->Voy = g_MRDataAGV->Input.Oxyz[1];
		g_VirtualOdoAGV->Voz = g_MRDataAGV->Input.Oxyz[2];
		g_VirtualOdoAGV->SliAng = g_MRDataAGV->SliAng;
		g_VirtualOdoAGV->RotDis = g_MRDataAGV->RotDis;
		/*实际运动模式输入虚拟里程计*/
		if(g_AGVHostCtrl->ActualAGVWorkMode == AGVMode_GoTransverse)
		{
			g_VirtualOdoAGV->WorkMode = OdoWalkMode_GoTransverse;
		}
		else if(g_AGVHostCtrl->ActualAGVWorkMode == AGVMode_GoRotation)
		{
			g_VirtualOdoAGV->WorkMode = OdoWalkMode_GoRotation;
		}
		else
		{
			g_VirtualOdoAGV->WorkMode = OdoWalkMode_GoLine;
		}
		/*模组角度输入*/
		{
			uint8_t i = 0;
			for(i=0;i<(g_VirtualOdoAGV->InitData.WheelNum/2);i++)
			{
				g_VirtualOdoAGV->ModuleAngle[i] = g_MRDataAGV->TgtAng[i];
			}
		}
	}
	/*陀螺仪数据接入虚拟里程计数据管道*/
	{
		/*A0030S陀螺仪航向角清零 未完成的过程中，都默认模块输出为0；*/
		if(g_GyroA0030SData.FixFlag == 1)
		{
			g_VirtualOdoAGV->GyroYaw = 0;
		}
		else
		{
			g_VirtualOdoAGV->GyroYaw = g_GyroA0030SData.GyroYaw.data_32bit;
		}
	}
	/*自动运行时，行走过程中的虚拟里程计的标定*/
	switch(g_AGVHostCtrl->AGVNavigateMode)
	{
		default:
		case AGVNavigateMode_NULL:		/*初始化*/
		case AGVNavigateMode_SLAM:		/*激光导航*/
		case AGVNavigateMode_GPS:		/*GPS导航*/
		case AGVNavigateMode_Magnetic:	/*磁条导航*/
		{
			
			if(	  g_AGVHostCtrl->ActualAGVWorkMode == AGVMode_GoLine
				||g_AGVHostCtrl->ActualAGVWorkMode == AGVMode_GoTransverse)
			{
				
				/*有任务的时候，到位就标定更新一下虚拟里程计;没任务的时候在点位上更新一下*/
				if(	  (g_AGVHostCtrl->ControlMODEL == Style_Auto && g_AGVHostCtrl->AGV_FuncSta == AGV_FunctionSta_Init && g_AGVHostCtrl->ExecutionPathState == ExePathState_HaveTask)
					||(g_AGVHostCtrl->PointUpdateFlag != 0 && g_AGVHostCtrl->ExecutionPathState != ExePathState_HaveTask)
					)
				{
					/*二维码停车的情况下，用地标二维码来标定*/
					if(  g_AGVHostCtrl->AGVWalkParkMode == AGVParking_ParkQR
						||(g_AGVHostCtrl->AGVWalkParkMode == AGVParking_QR && g_AGVHostCtrl->ParkQRData.PintSta != 0X00))
					{
							g_VirtualOdoAGV->FixX = g_AGVHostCtrl->ParkQRData.QR_x;
							g_VirtualOdoAGV->FixY = g_AGVHostCtrl->ParkQRData.QR_y;
							g_VirtualOdoAGV->FixZ = g_AGVHostCtrl->ParkQRData.QR_z*100.0f;
					}
					/*有RFID的情况下，按0来标定*/
					else if(  g_AGVHostCtrl->AGVWalkParkMode == AGVParking_RFID
						||g_AGVHostCtrl->AGVWalkParkMode == AGVParking_RFIDSignal)
					{
							g_VirtualOdoAGV->FixX = 0;
							g_VirtualOdoAGV->FixY = 0;
							//g_VirtualOdoAGV->FixZ = g_AGVHostCtrl->TargetToward*100.0f;
					}
					else
					{
						g_VirtualOdoAGV->FixX = 0;
						g_VirtualOdoAGV->FixY = 0;
					}
					res = 0x02;
				}
			}
			/*自转后（主要是十字磁条上的姿态控制）中的虚拟里程计的标定*/
			if(g_HostData.PosturRunSta == 4)
			{
				g_HostData.PosturRunSta = 0;
				if(	  g_AGVHostCtrl->AGVPosture == AGVPosture_Magnetic
					||g_AGVHostCtrl->AGVPosture == AGVPosture_ODO_Mag)
				{
					g_VirtualOdoAGV->FixX = 0;
					g_VirtualOdoAGV->FixY = 0;
					g_VirtualOdoAGV->FixZ = g_AGVHostCtrl->TargetToward*100.0f;
					res = 0x03;
				}
			}
		}
		break;
		case AGVNavigateMode_QR:		/*二维码导航，需要虚拟磁条功能调用*/
		{
			/*二维码停车的情况下，用地标二维码来标定*/
			if((g_AGVHostCtrl->ParkQRData.PintSta != 0X00))
			{
				g_VirtualOdoAGV->FixX = g_AGVHostCtrl->ParkQRData.QR_x;
				g_VirtualOdoAGV->FixY = g_AGVHostCtrl->ParkQRData.QR_y;
				g_VirtualOdoAGV->FixZ = g_AGVHostCtrl->ParkQRData.QR_z*100.0f;
				res = 0x04;
			}
		}
		break;
	}

	/*外部接口进行校订,或者 自动运行达到校订条件时*/
	if(g_AGVHostCtrl->VirtualOdoFixFlag != 0x00 || res != 0X00)
	{
		g_VirtualOdoAGV->FixFlag = 1;
		g_VirtualOdoAGV->Distance = 0;
		g_GyroA0030SData.FixFlag = 1;
	}

	/*虚拟里程计主方法调用*/
	OdometerMain();

	/*同步校订结果至自动接口中*/
	if(g_AGVHostCtrl->VirtualOdoFixFlag != 0x00 && g_VirtualOdoAGV->FixFlag == 0x00)
	{
		g_AGVHostCtrl->VirtualOdoFixFlag = 0X00;
	}
}

/************************************END OF FILE************************************/
