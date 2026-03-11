/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_VirtualMagnetic.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-09-27
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  ObjUpdate_VirtualMagnetic_GLOBALS

#include "ObjUpdate_VirtualMagnetic.h"
#include "ACC_Project.h"



//slam巡线的巡线轨迹偏差得到
void SlamTarceVirtualMagnetic(void)
{
	float RtX = g_ROKITData.ROKIT_Read.LocationX/10.0f;
	float RtY = g_ROKITData.ROKIT_Read.LocationY/10.0f;
	float RtZ = g_ROKITData.ROKIT_Read.LocationZ;
	
	static _MagneticVirtual_TypeDef *p_VirtualMag = &g_VirtualMagnetic;
	//得到巡线轨迹
	{
		/*在自动运行模式下，虚拟磁条输入数据管道构建*/
		if(g_HostData.ControlMODEL == Style_Auto)
		{	
			p_VirtualMag->StartSxy[MagVir_x] = g_HostData.CurrentPath_X;
			p_VirtualMag->StartSxy[MagVir_y] = g_HostData.CurrentPath_Y;
			
			p_VirtualMag->EndSxy[MagVir_x] = g_HostData.NextPath_X;
			p_VirtualMag->EndSxy[MagVir_y] = g_HostData.NextPath_Y;
		}
		/*在手动运行模式下，虚拟磁条输入数据管道构建*/
		else
		{
			g_SlamTrackLine.Rtx = RtX;
			g_SlamTrackLine.Rty = RtY;
			app_Remote_SlamTrackLine(&g_SlamTrackLine);
			if(g_SlamTrackLine.SlamTrackLine_STA != 0)
			{
				p_VirtualMag->StartSxy[MagVir_x] = g_SlamTrackLine.Remote_SlamTrackLine.org_X;
				p_VirtualMag->StartSxy[MagVir_y] = g_SlamTrackLine.Remote_SlamTrackLine.org_Y;
				
				p_VirtualMag->EndSxy[MagVir_x] = g_SlamTrackLine.Remote_SlamTrackLine.des_X;
				p_VirtualMag->EndSxy[MagVir_y] = g_SlamTrackLine.Remote_SlamTrackLine.des_Y;
			}
		}
	}
	//实时坐标输入管道构建
	{
		p_VirtualMag->RTxy[MagVir_x] = RtX;
		p_VirtualMag->RTxy[MagVir_y] = RtY;

		/*力士乐SLAM的RTz单位是0.01°范围（原始值是-18000~18000，转换后是） 虚拟磁条的是1°（0~360）*/
		p_VirtualMag->RTxy[MagVir_z] = RtZ/100.0f;
	}
	/*虚拟磁条的主方法调用*/
	MagneticVirtual_DataProcess(p_VirtualMag);

}

/***************************************************************************************
** 函数名称:	PutGoodsQRVirtualMagnetic
** 功能描述:	放工装的时候，将二维码偏差值映射成虚拟磁条巡线偏差值
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void PutGoodsQRVirtualMagnetic(void)
{
	static	HostCtrl_DATA			*g_AGVHostCtrl		= &g_HostData;			/*AGV框架代码对象指针	*/
	static _MagneticVirtual_TypeDef	*g_VirtualMagPGV	= &g_PGVAngle_MagneticVirtual;

	/*在二维码数据正常的前提下，计算一次放工装的零偏*/
	if(	  g_AGVHostCtrl->MagParkQRData.flag			== 0x01
		&&g_AGVHostCtrl->AGVMagParkZeroOffset.Flag 	== 0x00)
	{
		g_AGVHostCtrl->MagParkQRData.flag = 0x00;

		/*用虚拟磁导航模块，将工装二维码的X Z偏移反算得到磁条零偏*/
		{
			uint8_t Err = 0x00;
			g_VirtualMagPGV->StartSxy	[MagVir_x] = 0;
			g_VirtualMagPGV->StartSxy	[MagVir_y] = 0;
			g_VirtualMagPGV->RTxy		[MagVir_x] = g_AGVHostCtrl->MagParkQRData.QR_x;
			g_VirtualMagPGV->RTxy		[MagVir_y] = g_AGVHostCtrl->MagParkQRData.QR_y;
			//计算正向巡线的偏移结果
			{
				//输入管道构建
				{
					g_VirtualMagPGV->EndSxy		[MagVir_x] = 0;
					g_VirtualMagPGV->EndSxy		[MagVir_y] = 100;

					if(g_AGVHostCtrl->MagParkQRData.Offset_z != 0)
					{
						g_VirtualMagPGV->RTxy[MagVir_z] = 270.0f + g_AGVHostCtrl->MagParkQRData.Offset_z;
					}
					else
					{
					g_VirtualMagPGV->RTxy[MagVir_z] = 270.01f;
					}
				}

				MagneticVirtual_DataProcess(g_VirtualMagPGV);
				
				//输出管道构建
				{
					if( g_VirtualMagPGV->MagOffset[MagVir_F] != 32767)
					{g_AGVHostCtrl->AGVMagParkZeroOffset.Mag_F = g_VirtualMagPGV->MagOffset[MagVir_F]/10.0f;}
					else{g_AGVHostCtrl->AGVMagParkZeroOffset.Mag_F = 0;Err = Err|0x01;}
					if( g_VirtualMagPGV->MagOffset[MagVir_B] != 32767)
					{g_AGVHostCtrl->AGVMagParkZeroOffset.Mag_B = g_VirtualMagPGV->MagOffset[MagVir_B]/10.0f;}
					else{g_AGVHostCtrl->AGVMagParkZeroOffset.Mag_B = 0;Err = Err|0x02;}
				}
			}
			//计算横向巡线的偏移结果
			{
				//输入管道构建
				{
					g_VirtualMagPGV->EndSxy[MagVir_x] = 100;
					g_VirtualMagPGV->EndSxy[MagVir_y] = 0;
				}

				MagneticVirtual_DataProcess(g_VirtualMagPGV);

				//输出管道构建
				{
					if( g_VirtualMagPGV->MagOffset[MagVir_L] != 32767)
					{g_AGVHostCtrl->AGVMagParkZeroOffset.Mag_L = -g_VirtualMagPGV->MagOffset[MagVir_L]/10.0f;}
					else{g_AGVHostCtrl->AGVMagParkZeroOffset.Mag_L = 0;Err = Err|0x04;}
					if( g_VirtualMagPGV->MagOffset[MagVir_R] != 32767)
					{g_AGVHostCtrl->AGVMagParkZeroOffset.Mag_R = -g_VirtualMagPGV->MagOffset[MagVir_R]/10.0f;}
					else{g_AGVHostCtrl->AGVMagParkZeroOffset.Mag_R = 0;Err = Err|0x08;}
				}
			}

			//将故障情况同步至自动框架下
			{
				if(Err == 0x00)
				{
					g_AGVHostCtrl->AGVMagParkZeroOffset.Flag = 0x02;
				}
				else
				{
					g_AGVHostCtrl->AGVMagParkZeroOffset.Flag = 0x01;
				}
			}
		}
	}
	/*当末端定位生效，并且有相关数据后，对外部输出零偏结果*/
	else if(	g_AGVHostCtrl->EndPathFlag 				== 0X01
			 &&g_AGVHostCtrl->AGVMagParkZeroOffset.Flag == 0x02)
	{
		g_AGVHostCtrl->AGVMagParkZeroOffset.Flag = 0x04;
	}
	/*标志位被外部置为8后，自行清除所有变量，等待下一次计算偏差*/
	else if(g_AGVHostCtrl->AGVMagParkZeroOffset.Flag == 0x08)
	{
		memset(&g_AGVHostCtrl->AGVMagParkZeroOffset,0,sizeof(g_AGVHostCtrl->AGVMagParkZeroOffset));
	}
}
/***********************************************************
** 函数名称:		ObjUpdate_VirtualMagnetic
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_VirtualMagnetic(void)
{
	static Para_TarceType_EnumDef	*TarceType = &(g_Para.TarceType.FunType);
	//获取slam相关的偏差数据
	if(	  *TarceType == Para_TarceType_Slam
		||*TarceType == Para_TarceType_Slam_Magnetic
		||*TarceType == Para_TarceType_Slam_ColorBand)
	{
		SlamTarceVirtualMagnetic();
	}

	//放工装时，将工装二维码偏差反补至巡线上的功能
	if(g_HostData.AGVPendantFun.PutGoodsQRMag == 0X01)
	{
		PutGoodsQRVirtualMagnetic();
	}
	
}

/************************************END OF FILE************************************/
