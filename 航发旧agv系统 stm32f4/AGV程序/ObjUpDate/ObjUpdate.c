/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ObjUpdate.c
** 创 建 人: 文小兵  
** 描    述: 对象更新集中到改源文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月22日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define ObjUpdate_GLOBALS

//自定义引用	
#include "ObjUpdate.h"		
#include "includes.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "Debug.h"
#include "ADDA.h"

//内部调用
#include "ObjUpdate_RFID.h"
#include "ObjUpdate_Mag.h"
#include "ObjUpdate_PGV.h"
#include "ObjUpdate_BMS.h"
#include "ObjUpdate_VoicePlayer.h"
#include "ObjUpdate_WheelAngle.h"
#include "ObjUpdate_Lift.h"
#include "ObjUpdate_ABS.h"
#include "ObjUpdate_LoadDetection.h"
#include "ObjUpdate_IO.h"
#include "ObjUpdate_Coord.h"
#include "ObjUpdate_Color_Band.h"
#include "ObjUpdate_AGV.h"
#include "ObjUpdate_Assemly.h"
#include "ObjUpdate_VirtualMagnetic.h"
#include "ObjUpdate_VirtualOdo.h"
#include "ObjUpdate_API_Slave.h"
#include "ObjUpdate_LeadShine.h"


//自动对中cbb实时输入
bool ShelfAutadj_Input(ShelfAutadj_TypeDef* p)
{
	//char i=0;
	
	//测距输入
    //p->in_DistanceLeft[0] =
	char i=0;
	
	//测距输入
	s16 temp_dis[8] = { 0 };

	if (ShelfAutadj_Liner.Num == 0)//直接乘以系数
	{
		for (i = 0; i < 8; i++)
		{
			temp_dis[i] = (EQP_AD.RD[i] / 229.94f) + 15;
		}
	}
	else//进行线性标定
	{
		for (i = 0; i < 8; i++)
		{
			temp_dis[i] = RatioStand(EQP_AD.RD[i], ShelfAutadj_Liner.Data[i].SetLowOrg,ShelfAutadj_Liner.Data[i].SetHighOrg,
				ShelfAutadj_Liner.Data[i].SetLow,ShelfAutadj_Liner.Data[i].SetHigh);

			//偏置
			temp_dis[i] += ShelfAutadj_Liner.OffSet[i];
		}
	}
	

	p->in_DistanceLeft[0] = temp_dis[0];							//左侧传感器测距值 mm
	p->in_DistanceRight[0] = temp_dis[1];							//右侧传感器测距值 mm

	p->in_DistanceLeft[1] = temp_dis[2];							//左侧传感器测距值 mm
	p->in_DistanceRight[1] = temp_dis[3];							//右侧传感器测距值 mm

	p->in_DistanceLeft[2] = temp_dis[4];							//左侧传感器测距值 mm
	p->in_DistanceRight[2] = temp_dis[5];							//右侧传感器测距值 mm

	p->in_DistanceLeft[3] = temp_dis[6];							//左侧传感器测距值 mm
	p->in_DistanceRight[3] = temp_dis[7];							//右侧传感器测距值 mm	

    //速度输入
    p->in_Velocity = AccData.CalCmd.VOxyz.y;


	return 1;
}



//底盘控制数据对象监控更新
bool MC_Monitor(void)
{
	char ErrCnt=0;

	//遥控CBB 无线数据解析
	WirelessHandle_DataProcess(&g_WHData);				

	//RFID数据更新
	ObjUpdate_RFID();

	//坐标数据更新
	ObjUpdate_Coord();

	//磁导航数据更新
	ObjUpdate_Mag();

	//二维码数据更新
	ObjUpdate_PGV();

	//BMS数据更新
	ObjUpdate_BMS();

	//色带数据更新
	ObjUpdate_Color_Band();

	//语音播报输入更新
	ObjUpdate_VoicePlayer();

	//IO输入更新，硬件到IO对象中
	ObjUpdate_IOInput();

	//ADDA更新
	if(ADDA_Update()==0)
		ErrCnt++;
	else {}

	//联动数据更新
	if(AtDbug.Bit.AsbCommOff == 0)
		ObjUpdate_Assemly();
	else {}

	//轮系角度计算更新
	ObjUpdate_WheelAngle();

	//升降输入、升降计算标定
	ObjUpdate_Lift();

	//悬挂/减震实时数据输入 计算标定
	ObjUpdate_ABS();

	//负载监测输入 数据换算
    ObjUpdate_LoadDetection();

	//自动对中输入
	ShelfAutadj_Input(&g_ShelfAutadj);

	//自动对中 数据处理调用
	ShelfAutadj_SideStatusFuc(&g_ShelfAutadj);

	//雷达与防撞触边(切区、使能控制)

	//可保存参数、临时操作参数同步到CBB中

	//虚拟里程计 数据更新 最多间隔10ms调用，最好间隔5ms调用
	ObjUpdate_VirtualOdo();

	//虚拟磁导航数据更新
	ObjUpdate_VirtualMagnetic();

	//自动部分代码 对象更新
	ObjUpdate_AGV();

	//二次开发接口 数据更新
	ObjUpdate_API_Slave();
	
	//雷赛伺服 控制数据更新
	ObjUpdate_LeadShine();

	//伺服使能控制
	SVO_EnableOperation(g_Sys_1ms_Counter);

	//IO控制更新到DHR位中
	if (!g_Para.Debug.IOOutputOff)
		ObjUpdate_IOOutput();
	else {}

	if(ErrCnt)
		return 0;
	else
		return 1;
}

//***********************************END OF FILE***********************************

