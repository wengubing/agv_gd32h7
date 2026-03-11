#ifndef __APP_ATUOROTATIONCONTROLTION_H__
#define __APP_ATUOROTATIONCONTROLTION_H__



#ifdef   APP_ATUOROTATIONCONTROLTION_GLOBALS
 #define APP_ATUOROTATIONCONTROLTION_EXT
#else
 #define APP_ATUOROTATIONCONTROLTION_EXT  extern
#endif

#include <stdbool.h>
#include "common_def.h"
typedef enum
{
    Rotation_RunStep_init=0,                               //旋转开始
    Rotation_RunStep_lostsignal,                           //磁条信号全部丢失
    Rotation_RunStep_againfindsignal,                      //再次发现磁条信号
    Rotation_RunStep_arrivepostion,                        //自转到位
    Rotation_RunStep_judge_ok,                             //是否需要继续自转
    Rotation_RunStep_finished,                             //自转完成
    Rotation_RunStep_err,                                  //自转异常（超时、过转）
}_Rotation_Step;
typedef enum
{
    RotationState_null=0,           //空状态
    RotationState_finished,         //选装完成状态
    RotationState_moving,           //旋转中
    RotationState_timeout,          //旋转超时状态
    RotationState_err,              //旋转错误状态

}_Rotation_State;
//
typedef struct _Auto_RotationControl
{
f32   start_speed,                      //旋转开始速度
      standard_speed,                   //旋转中途速度
      final_speed;                      //旋转到位速度
u32   roation_timeout;                  //旋转超时时间
u16   current_towards,                  //当前朝向
      tartget_towards;                  //目标朝向
s16   fornt_postion,                    //前磁导航位置
      back_postion,                     //后磁导航位置
      left_postion,                     //左磁导航位置
      right_postion;                    //右磁导航位置
u16   forntmag_state,                   //前磁导航读取磁条状态
      backmag_state,                    //后磁导航读取磁条状态
      leftmag_state,                    //左磁导航读取磁条状态
      rightmag_state;                   //右磁导航读取磁条状态
s16   forntmag_strength,                //前磁导航磁场强度
      backmag_strength,                 //后磁导航磁场强度
      leftmag_strength,                 //左磁导航磁场强度
      rightmag_strength;                //右磁导航磁场强度
u8    rotation_state;                   //旋转状态  0：空   1：完成  2:旋转中 3:超时 4：异常
f32   rotation_speed;                   //旋转速度  -107-107;
u8    rotation_runstep;                 //旋转步骤
s16   fornt_org_postion,                    //前磁导航位置--原始值
      back_org_postion,                     //后磁导航位置--原始值
      left_org_postion,                     //左磁导航位置--原始值
      right_org_postion;                    //右磁导航位置--原始值
u16   forntmag_org_state,                   //前磁导航读取磁条状态--原始值
      backmag_org_state,                    //后磁导航读取磁条状态--原始值
      leftmag_org_state,                    //左磁导航读取磁条状态--原始值
      rightmag_org_state;                   //右磁导航读取磁条状态--原始值
s16   forntmag_org_strength,                //前磁导航磁场强度--原始值
      backmag_org_strength,                 //后磁导航磁场强度--原始值
      leftmag_org_strength,                 //左磁导航磁场强度--原始值
      rightmag_org_strength;                //右磁导航磁场强度--原始值
}_auto_rotationcontrol;

//APP_ATUOROTATIONCONTROLTION_EXT _auto_rotationcontrol;

APP_ATUOROTATIONCONTROLTION_EXT void  RotationPara_Init(_auto_rotationcontrol *p,f32 speed1,f32 speed2,f32 speed3,u32 timeout_t);
APP_ATUOROTATIONCONTROLTION_EXT u8 AutoRotationControl(_auto_rotationcontrol *p,u32 SysMs_t);
APP_ATUOROTATIONCONTROLTION_EXT void Postion_Filt(_auto_rotationcontrol *p,u32 SysMs_t);
#endif
