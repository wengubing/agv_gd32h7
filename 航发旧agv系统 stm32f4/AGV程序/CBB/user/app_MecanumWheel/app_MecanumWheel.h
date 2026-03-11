/********************************** Copyright ***********************************
 *
 *                           成都航发机器人有限公司
 *                            All Rights Reserved
 *
 * FileName   : app_MecanumWheel.h
 * Version    : V1.0
 * Author     : WangSiPing
 * Date       : 2024.02.20
 * Description: 输入控制命令，输出每个轮子的控制速度
 *******************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __MECANUMWHEEL_H__
#define __MECANUMWHEEL_H__

//确保仅在该文件编译时实例化一次对象
#ifdef app_MecanumWheel_GLOBALS
#define app_MecanumWheel_EXT
#else
#define app_MecanumWheel_EXT  extern
#endif

 // 包含头文件
#include "basecalfunc.h"

// 宏定义
#define MOTOR_NUM 24                                        // 行走电机数量定义,用于数组初始化，定义一个麦轮最大的电机数
#define V_Max 642                                           // 最大摇杆值
#ifndef PI
#define PI 3.1415926535                                     // 圆周率
#endif
#define Mec_InvalidExtIn 0								    // 无效的外部输入值

//外部与内部数据合并对象
typedef struct
{
    f32 Self;                                               //自己
    f32 ExtIn;                                              //外部输入
    f32 Exe;                                                //执行
}Mec_DataMerge_TypeDef;

// 轮系解算方向定义
typedef struct
{
    // 输入
    f32 x;                                                  // x坐标
    f32 y;                                                  // y坐标
	//小轮倾斜方向，1代表小轮倾斜方向为左斜，即常规4个麦轮的车，俯视时米字的左上角小轮倾斜方向
    s16 WheelType;

    //电机安装方向，人与车前进方向同方向，1代表电机旋转轴朝右，-1代表电机旋转轴朝左
    //根本为1电机速度命令正时，轮子转动方向为车体向前运动方向
    s16 MotorDirection;                                     
}Mec_AxisXY_TypeDef;

//参数定义
typedef struct
{
    u16 WalkMotorNum;                                       //电机个数
    f32 MotorMaxRpm;                                        //电机最大速度
    Mec_AxisXY_TypeDef Mxy[MOTOR_NUM];                      //车子几何参数
    f32 Ratio[MOTOR_NUM];                                   //电机减速器减速比
    f32 Diameter[MOTOR_NUM];                                //轮子直径
}Mec_Para_TypeDef;

//摇杆命令计算电机速度方向符号
typedef struct
{
    s8 Sx;      								            //x轴速度符号
    s8 Sy;                                                  //
    s8 Sz;                                                  //
    s8 SAxis_x;                                             //轮子位置 x轴符号
    s8 SAxis_y;                                             //轮子位置 y轴符号
}Mec_SpeedDir_TypeDef;

//轮子线速度大小归一化系数
typedef struct
{
    f32 Blow[MOTOR_NUM];                                    //轮子线速度放大结果
    f32 BlowMax;                                            //轮子线速度放大结果最大值
    f32 BlowMin;                                            //轮子线速度放大结果最小值
    f32 Unifor[MOTOR_NUM];                                  //归一化系数 反比于轮子线速度放大结果
}Mec_SpeedUnif_TypeDef;

//自转速度归一化系数
typedef struct
{
	f32 Asb[MOTOR_NUM];                                    //联动参数
    f32 Lxy[MOTOR_NUM];
    Mec_DataMerge_TypeDef LxyMax;
    f32 LxyMin;
	f32 Unifor[MOTOR_NUM];                                  //归一化系数
}Mec_RotUnif_TypeDef;

//整体缩放
typedef struct
{
    Mec_DataMerge_TypeDef SpeedMax;                         //所有电机的最大速度
    u16 SpeedMaxId;                                         //最大速度所在位置
    //f32 Unifor[MOTOR_NUM];                                //归一化缩放比例 (0~1]
	f32 K_Zoom2MaxLimit;                                    //最大速度缩放系数
    f32 SpeedZoom[MOTOR_NUM];                               //缩放后的速度
}Mec_SpeedZoom_TypeDef;

//输出
typedef struct
{
    f32 Speed[MOTOR_NUM];                                   //最终电机速度
}Mec_Out_TypeDef;

typedef struct
{
    // 初始化
	Mec_Para_TypeDef Para;                                   //结构等参数
    
    // 实时输入
    f32 Oxyz[3];                                            //堵塞后的摇杆值

    //中间过程量
    f32 Ksm;                                                // 电机最大速度与摇杆最大值的比值
	Mec_SpeedDir_TypeDef SpeedDir[MOTOR_NUM];               //摇杆命令计算电机速度方向符号
    Mec_SpeedUnif_TypeDef SpeedUnif;                        //轮径等机械参数计算归一化速度缩放系数
    Mec_RotUnif_TypeDef RotUnif;                            //z轴命令自转半径归一化系数计算

    f32 Speed[MOTOR_NUM];                                   //标准轮速计算结果
    f32 SpeedMotor[MOTOR_NUM];                              //机械参数归一化速度计算结果
    Mec_SpeedZoom_TypeDef SpeedZoom;                        //整体缩放计算

    //输出
	Mec_Out_TypeDef Out;                                     //输出
} Mecanum_TypeDef;

//方法外部声明
app_MecanumWheel_EXT bool Mec_SpeedDirCal(Mecanum_TypeDef* p2Mec);
app_MecanumWheel_EXT bool Mec_SpeedUnifCal(Mecanum_TypeDef* p2Mec);
app_MecanumWheel_EXT bool Mec_RotUnifCal(Mecanum_TypeDef* p2Mec);
app_MecanumWheel_EXT bool Mec_Main(Mecanum_TypeDef* p2Mec);

app_MecanumWheel_EXT bool Mec_Test_Init(Mecanum_TypeDef* p2Mec);

#endif
//***********************************END OF FILE***********************************
