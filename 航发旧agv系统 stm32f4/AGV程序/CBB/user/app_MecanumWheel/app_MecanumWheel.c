/********************************** Copyright ***********************************
 *
 *                           成都航发机器人有限公司
 *                            All Rights Reserved
 *
 * FileName   : app_MecanumWheel.c
 * Version    : V1.0
 * Author     : WangSiPing
 * Date       : 2024.02.20
 * Description: 输入控制命令，输出每个轮子的控制速度
 *******************************************************************************/

//包含头文件
#include "app_MecanumWheel.h"
#include <math.h>
#include <stdlib.h>

//外部与内部数据合并 取最大值
f32 Mec_DataMerge_Max(Mec_DataMerge_TypeDef* p)
{
	//如果外部输入无效，则返回内部数据；如外部输入有效，则取外部输入和内部数据的最大值
    if(p->ExtIn == Mec_InvalidExtIn)
        return p->Self;
    else
    {
        if(p->Self > p->ExtIn)
            return p->Self;
        else
			return p->ExtIn;
    }
}

//外部与内部数据合并 取最小值
f32 Mec_DataMerge_Min(Mec_DataMerge_TypeDef* p)
{
    //如果外部输入无效，则返回内部数据；如外部输入有效，则取外部输入和内部数据的最小值
    if(p->ExtIn == Mec_InvalidExtIn)
        return p->Self;
    else
    {
        if(p->Self < p->ExtIn)
            return p->Self;
        else
            return p->ExtIn;
    }
}

//计算电机速度符号
bool Mec_SpeedDirCal(Mecanum_TypeDef* p2Mec)
{
    u8 i = 0;
    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        //计算轮子x轴符号
        if(p2Mec->Para.Mxy[i].x > 0)
            p2Mec->SpeedDir[i].SAxis_x = 1;
        else if(p2Mec->Para.Mxy[i].x < 0)
            p2Mec->SpeedDir[i].SAxis_x = -1;
        else
			p2Mec->SpeedDir[i].SAxis_x = 0;

		//计算轮子y轴符号
        if(p2Mec->Para.Mxy[i].y > 0)
            p2Mec->SpeedDir[i].SAxis_y = 1;
        else if(p2Mec->Para.Mxy[i].y < 0)
            p2Mec->SpeedDir[i].SAxis_y = -1;
		else
			p2Mec->SpeedDir[i].SAxis_y = 0;

        //根据每个轮子小轮的倾斜方向和电机旋转轴方向判断x,y方向速度的符号
        p2Mec->SpeedDir[i].Sx = p2Mec->Para.Mxy[i].WheelType * p2Mec->Para.Mxy[i].MotorDirection;
        p2Mec->SpeedDir[i].Sy = p2Mec->Para.Mxy[i].MotorDirection;
		p2Mec->SpeedDir[i].Sz = p2Mec->SpeedDir[i].SAxis_x * p2Mec->Para.Mxy[i].MotorDirection;
    }

    return 1;
}

//减速器，轮子直径比例系数解算，寻找电机相同转速时的最小轮速，并以最小轮速为基准归一化
bool Mec_SpeedUnifCal(Mecanum_TypeDef* p2Mec)
{
    char i = 0;
    p2Mec->SpeedUnif.BlowMax = p2Mec->SpeedUnif.Blow[0];
    p2Mec->SpeedUnif.BlowMin = p2Mec->SpeedUnif.Blow[0];

    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        p2Mec->SpeedUnif.Blow[i] = p2Mec->Para.Diameter[i] / p2Mec->Para.Ratio[i];
        if (p2Mec->SpeedUnif.BlowMax < p2Mec->SpeedUnif.Blow[i])
            p2Mec->SpeedUnif.BlowMax = p2Mec->SpeedUnif.Blow[i];
        else {}

        if (p2Mec->SpeedUnif.BlowMin > p2Mec->SpeedUnif.Blow[i])
            p2Mec->SpeedUnif.BlowMin = p2Mec->SpeedUnif.Blow[i];
        else {}
    }

    //归一化每个轮子的速度系数
    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        if (p2Mec->SpeedUnif.Blow[i] != 0)
            p2Mec->SpeedUnif.Unifor[i] = p2Mec->SpeedUnif.BlowMin / p2Mec->SpeedUnif.Blow[i];
        else
            p2Mec->SpeedUnif.Unifor[i] = 0;
    }
    
    return 1;
}

//旋转中心比例系数解算，寻找最大半径，并以最大半径为基准归一化
bool Mec_RotUnifCal(Mecanum_TypeDef* p2Mec)
{
    char i = 0;
    p2Mec->RotUnif.LxyMax.Self = p2Mec->RotUnif.Lxy[0];
    p2Mec->RotUnif.LxyMin = p2Mec->RotUnif.Lxy[0];

    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        p2Mec->RotUnif.Lxy[i] = fabs(p2Mec->Para.Mxy[i].x) + fabs(p2Mec->Para.Mxy[i].y) + p2Mec->RotUnif.Asb[i];
        if (p2Mec->RotUnif.LxyMax.Self < p2Mec->RotUnif.Lxy[i])
            p2Mec->RotUnif.LxyMax.Self = p2Mec->RotUnif.Lxy[i];
        else {}

        if (p2Mec->RotUnif.LxyMin > p2Mec->RotUnif.Lxy[i])
            p2Mec->RotUnif.LxyMin = p2Mec->RotUnif.Lxy[i];
        else {}
    }

	//寻找最大半径
	p2Mec->RotUnif.LxyMax.Exe = Mec_DataMerge_Max(&p2Mec->RotUnif.LxyMax);

    //归一化每个轮子的旋转半径
    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        if (p2Mec->RotUnif.LxyMax.Exe != 0)
            p2Mec->RotUnif.Unifor[i] = p2Mec->RotUnif.Lxy[i] / p2Mec->RotUnif.LxyMax.Exe; // 归一化每个轮子的旋转半径
        else
            p2Mec->RotUnif.Unifor[i] = 0;
    }

    return 1;
}

//根据摇杆解算速度
bool Mec_SpeedCal(Mecanum_TypeDef* p2Mec)
{
	char i = 0;

    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        p2Mec->Speed[i] = p2Mec->Oxyz[0]*p2Mec->SpeedDir[i].Sx 
                        + p2Mec->Oxyz[1]*p2Mec->SpeedDir[i].Sy 
                        + p2Mec->Oxyz[2]*p2Mec->SpeedDir[i].Sz* p2Mec->RotUnif.Unifor[i];

        //乘以Ksm
        p2Mec->Speed[i] *= p2Mec->Ksm;
    }

    return 1;
}

//乘归一化每个轮子相同电机转速下的轮子速度，匹配不同轮子直径，不同减速比，最终使轮子线速度相同
bool Mec_SpeedMotor(Mecanum_TypeDef* p2Mec)
{
    char i = 0;

    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        p2Mec->SpeedMotor[i] = p2Mec->Speed[i] * p2Mec->SpeedUnif.Unifor[i];
    }

    return 1;
}

//整体速度缩放
bool Mec_Zoom(Mecanum_TypeDef* p2Mec)
{
	u8 i = 0;
    p2Mec->SpeedZoom.SpeedMax.Self = fabs(p2Mec->SpeedMotor[0]);
    p2Mec->SpeedZoom.SpeedMaxId = 0;

    //寻找最大速度
    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        if (p2Mec->SpeedZoom.SpeedMax.Self < fabs(p2Mec->SpeedMotor[i]))
        {
            p2Mec->SpeedZoom.SpeedMax.Self = fabs(p2Mec->SpeedMotor[i]);
            p2Mec->SpeedZoom.SpeedMaxId = i;
        }
    }

	//比较外部输入，取最大速度执行
	p2Mec->SpeedZoom.SpeedMax.Exe = Mec_DataMerge_Max(&p2Mec->SpeedZoom.SpeedMax);

    //计算整体缩放比例
    if (p2Mec->SpeedZoom.SpeedMax.Exe != 0 && p2Mec->Para.MotorMaxRpm != 0 && p2Mec->SpeedZoom.SpeedMax.Exe > p2Mec->Para.MotorMaxRpm)
    {
		p2Mec->SpeedZoom.K_Zoom2MaxLimit = p2Mec->Para.MotorMaxRpm / p2Mec->SpeedZoom.SpeedMax.Exe;
    }
    else
    {
        p2Mec->SpeedZoom.K_Zoom2MaxLimit = 1;
    }

    //缩放每个电机速度
    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        p2Mec->SpeedZoom.SpeedZoom[i] = p2Mec->SpeedMotor[i] * p2Mec->SpeedZoom.K_Zoom2MaxLimit;
    }

	return 1;
}

//输出
bool Mec_Output(Mecanum_TypeDef* p2Mec)
{
    char i = 0;

    for (i = 0; i < p2Mec->Para.WalkMotorNum; i++)
    {
        p2Mec->Out.Speed[i] = p2Mec->SpeedZoom.SpeedZoom[i];
    }

    return 1;
}

//速度解算
bool Mec_Main(Mecanum_TypeDef* p2Mec)
{
    //计算电机速度符号
    Mec_SpeedDirCal(p2Mec);

    //计算速度归一化系数
    Mec_SpeedUnifCal(p2Mec);

    //计算旋转中心归一化系数
    Mec_RotUnifCal(p2Mec);

    //摇杆解算速度
	Mec_SpeedCal(p2Mec);

	//乘归一化每个轮子相同电机转速下的轮子速度，匹配不同轮子直径，不同减速比，最终使轮子线速度相同
	//暂未为联动设计不同的缩放系数
	Mec_SpeedMotor(p2Mec);
    
	//速度整体缩放
	Mec_Zoom(p2Mec);

	//输出
	Mec_Output(p2Mec);
    
    return 1;
}

//cbb测试初始化
bool Mec_Test_Init(Mecanum_TypeDef* p2Mec)
{
	char i = 0;
    f32 Mxy_Y[8] = {2000, 2000, 1000, 1000, -1000, -1000, -2000, -2000};
	s8 WheelType[8] = {1, -1, 1, -1, -1, 1, -1, 1};

    //初始化参数
    p2Mec->Para.WalkMotorNum = 8;
	p2Mec->Para.MotorMaxRpm = 3000;
    p2Mec->Ksm = 1.0f; 


    //初始化轮子位置和类型
    for (i = 0; i < 8; i++)
    {
        p2Mec->Para.Mxy[i].x = (i % 2 == 0) ? -1000 : 1000;
        p2Mec->Para.Mxy[i].y = Mxy_Y[i];
        p2Mec->Para.Mxy[i].WheelType = WheelType[i];
        p2Mec->Para.Mxy[i].MotorDirection = (p2Mec->Para.Mxy[i].x < 0) ? 1 : -1;
        p2Mec->Para.Diameter[i] = 200.0f;
        p2Mec->Para.Ratio[i] = 1.0f;
    }

    //摇杆输入初始化
	p2Mec->Oxyz[0] = 30;
	p2Mec->Oxyz[1] = 60;
	p2Mec->Oxyz[2] = 90;

    //外部输入数据
    p2Mec->RotUnif.LxyMax.ExtIn = 20;
    p2Mec->SpeedZoom.SpeedMax.ExtIn = 20;

    return true;
}

//***********************************END OF FILE***********************************
