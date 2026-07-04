/**************************************************************************		
公    司：航发液压工程有限公司													
文件名称：DeviceDefine.h																
描    述：定义公式支持的设备类型和设备地址编码														
作    者：邓洋														
版    本：V1.0																	
日    期：2015-05-10													
修改记录：2012-05-02 创建																
**************************************************************************/
#ifndef __DEVICE_DEFINE_H__
#define __DEVICE_DEFINE_H__


// 定义设备类型
typedef enum
{
    // 机器人主机，范围0x01 ~ 0x08
    DeviceType_RobotHost = 0x01,            // 机器人主机

    // 用户主机，范围0x08 ~ 0x0F
    DeviceType_Handle = 0x0F,

    // 电源控制板，范围0x10 ~ 0x1F
    DeviceType_PowerManage = 0x10,          // 电源板

    // 运动控制板，范围0x20 ~ 0x2F
    DeviceType_MotionControl = 0x20,        // 运动控制板

    // 传感器设备，范围0x30 ~ 0x6F
    DeviceType_UltrasonicInfrared = 0x31,   // 超声红外板
    DeviceType_Ultrasonic = 0x32,           // 超声波
    DeviceType_Acceleration = 0x33,         // 加速度
    DeviceType_Gyroscope = 0x34,            // 陀螺仪
    DeviceType_Compass = 0x35,              // 电子罗盘
    DeviceType_GPS = 0x36,                  // GPS
    DeviceType_Inclinometer = 0x37,         // 倾角
    DeviceType_Lidar = 0x38,                // 激光雷达
    DeviceType_EncoderPosition = 0x39,      // 码盘定位
    DeviceType_AreaPosition = 0x3A,         // 局域定位
    DeviceType_LaserInfrared = 0x3B,        // 激光红外
    DeviceType_FindMarkingLine = 0x3C,      // 巡线模块
    DeviceType_CSS = 0x3D,                  // CSS测距
    DeviceType_MNS = 0x3F,                  // 磁导航传感器
    DeviceType_AGV = 0x45,                  // AGV主板
		
    // 保留，范围0x70 ~ 0xFE

    // 广播，0xFF
    DeviceType_Broadcast = 0xFF,

}HF_DeviceType;

#define DeviceBroadcastAddress          		0xFF   // 广播地址

#endif

