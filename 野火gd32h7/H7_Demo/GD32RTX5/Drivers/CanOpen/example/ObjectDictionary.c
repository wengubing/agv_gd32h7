
/*

dy dengyang 2015.12.25

1.对Slave_OD的修改；
2.PDO通讯参数、映射参数的编辑和修改；
3.

by biyabc   2017.10.15
1.增加can节点至32点
2.修改对象字典

*/

#include "gd32h7xx.h"
#include <stdint.h>
#include <stdbool.h>

#include "LinkCanopenMaster.h"
#include "ObjectDictionary.h"

enum
{
    As_Master = 0,	    //主机使用
    As_Slave = 1,		//作为从机使用	
}canopen_type;


#define valueRange_EMC					0x9F /* Type for index 0x1003 subindex 0x00 (only set of value 0 is possible) */



TIMER_HANDLE CanObjectDict_heartBeatTimers[MAX_NodeId_NUM] = {TIMER_NONE,};//消费者心跳timers:暂存timers[]的row编号

const UNS8 CanObjectDict_iam_a_slave = As_Master; //(0:master;1:slave)

UNS8 CanObjectDict_bDeviceNodeId = 0;/* node_id default value.*/







/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

                               OBJECT DICTIONARY

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/* index 0x1000 :   Device Type. */
    UNS32 CanObjectDict_obj1000 = 0;
    subindex CanObjectDict_Index1000[] = 
    {
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1000 }
    };

/* index 0x1001 :   Error Register. */
    UNS8 CanObjectDict_obj1001 = 0;
    subindex CanObjectDict_Index1001[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_obj1001 }
    };

/* index 0x1002 :   Mapped variable MasterRxMap15 */
    UNS32 CanObjectDict_Rx_Map15 = 0;
    ODCallback_t CanObjectDict_0x1002_callbacks[] = 
    {
        NULL
    };
    subindex CanObjectDict_Index1002[] = 
    {
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_Rx_Map15 }
    };

/* index 0x1003 :   Pre-defined Error Field */
    UNS8 CanObjectDict_highestSubIndex_obj1003 = 0x07; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1003[] = 
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };
    ODCallback_t CanObjectDict_Index1003_callbacks[] = 
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };
    subindex CanObjectDict_Index1003[] = 
    {
        { RW, valueRange_EMC, sizeof (UNS8),  (void*)&CanObjectDict_highestSubIndex_obj1003 },
        { RO, uint32,         sizeof (UNS32), (void*)&CanObjectDict_obj1003[0] },
        { RO, uint32,         sizeof (UNS32), (void*)&CanObjectDict_obj1003[1] },
        { RO, uint32,         sizeof (UNS32), (void*)&CanObjectDict_obj1003[2] },
        { RO, uint32,         sizeof (UNS32), (void*)&CanObjectDict_obj1003[3] },
        { RO, uint32,         sizeof (UNS32), (void*)&CanObjectDict_obj1003[4] },
        { RO, uint32,         sizeof (UNS32), (void*)&CanObjectDict_obj1003[5] },
        { RO, uint32,         sizeof (UNS32), (void*)&CanObjectDict_obj1003[6] },
    };

/* index 0x1005 :   SYNC COB ID. 同步报文cobid标识符*/
    UNS32 CanObjectDict_obj1005 = 0x40000080ul;
    ODCallback_t CanObjectDict_Index1005_callbacks[] = 
    {
        NULL,
    };
    subindex CanObjectDict_Index1005[] = 
    {
        { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1005 }
    };

/* index 0x1006 :   Communication Cycle Period. 同步帧循环周期---单位:us*/
    UNS32 CanObjectDict_obj1006 = MS_TO_TIMEVAL(SYNC_CommunicationCyclePeriod);
    ODCallback_t CanObjectDict_Index1006_callbacks[] = 
    {
        NULL,
    };
    subindex CanObjectDict_Index1006[] = 
    {
        { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1006 }
    };

/* index 0x1008 :   Amplifier model number */
    UNS64 CanObjectDict_obj1008 = 0x0;	/* 0 */
    subindex CanObjectDict_Index1008[] = 
    {
        { RO, uint64, sizeof (UNS64), (void*)&CanObjectDict_obj1008 }
    };

/* index 0x1009 :   Hardware version string */
    UNS64 CanObjectDict_obj1009 = 0x0;	/* 0 */
    subindex CanObjectDict_Index1009[] = 
    {
        { RO, uint64, sizeof (UNS64), (void*)&CanObjectDict_obj1009 }
    };

/* index 0x100A :   Manufacturer Software Version */
    UNS64 CanObjectDict_obj100A = 0x0;	/* 0 */
    subindex CanObjectDict_Index100A[] = 
    {
        { RO, uint64, sizeof (UNS64), (void*)&CanObjectDict_obj100A }
    };

/* index 0x100C :   Node Guarding time 节点守护时间,unit:ms*/
    UNS16 CanObjectDict_obj100C = NMT_GuardTime;//0 - 65535
    subindex CanObjectDict_Index100C[] = 
    {
        { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_obj100C }
    };

/* index 0x100D :   Life Time Factor 节点寿命因子*/
    UNS8 CanObjectDict_obj100D = NMT_LifeTimeFactor;//0 - 255
    subindex CanObjectDict_Index100D[] = 
    {
        { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_obj100D }
    };

/* index 0x1010 :   Store parameters. */
    UNS8 CanObjectDict_highestSubIndex_obj1010 = 4; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1010_Save_All_Parameters = 0x0;	/* 0 */
    UNS32 CanObjectDict_obj1010_Save_Communication_Parameters = 0x0;	/* 0 */
    UNS32 CanObjectDict_obj1010_Save_Application_Parameters = 0x0;	/* 0 */
    UNS32 CanObjectDict_obj1010_Save_manufacturer_defined_parameters = 0x0;	/* 0 */
    subindex CanObjectDict_Index1010[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1010 },
        { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1010_Save_All_Parameters },
        { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1010_Save_Communication_Parameters },
        { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1010_Save_Application_Parameters },
        { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1010_Save_manufacturer_defined_parameters }
    };

/* index 0x1013 :   High resolution time stamp */
    UNS32 CanObjectDict_obj1013 = 0;	/* 0 */
    subindex CanObjectDict_Index1013[] = 
    {
        { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1013 }
    };

/* index 0x1014 :   Emergency COB ID. */
    UNS32 CanObjectDict_obj1014 = 0;	/* 0 */
    subindex CanObjectDict_Index1014[] = 
    {
        { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1014 }
    };

/* index 0x1015 :   Emergency object inhibit time. */
    UNS16 CanObjectDict_obj1015 = 0;	/* 0 */
    subindex CanObjectDict_Index1015[] = 
    {
        { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_obj1015 }
    };

/* index 0x1016 :   Consumer Heartbeat Time. 消费者心跳时间间隔*/
    UNS8 CanObjectDict_highestSubIndex_obj1016 = MAX_NodeId_NUM;//消费者心跳数量
    UNS32 CanObjectDict_obj1016[MAX_NodeId_NUM];//消费者心跳录
    subindex CanObjectDict_Index1016[] = //作为主机需要配置的....!!!....其数值在 can_master_init()中配置
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1016 },
				
#if (MAX_NodeId_NUM == 1)		
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },                     

#elif (MAX_NodeId_NUM == 2)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },                     

#elif (MAX_NodeId_NUM == 3)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },                      
				
#elif (MAX_NodeId_NUM == 4)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },                     
				
#elif (MAX_NodeId_NUM == 5)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },                      
				
#elif (MAX_NodeId_NUM == 6)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },                     
				
#elif (MAX_NodeId_NUM == 7)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },                      
				
#elif (MAX_NodeId_NUM == 8)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },                     
				
#elif (MAX_NodeId_NUM == 9)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },                      
				
#elif (MAX_NodeId_NUM == 10)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },                     
				
#elif (MAX_NodeId_NUM == 11)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },                     
				
#elif (MAX_NodeId_NUM == 12)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },                      
				
#elif (MAX_NodeId_NUM == 13)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },                      
				
#elif (MAX_NodeId_NUM == 14)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },                      
				
#elif (MAX_NodeId_NUM == 15)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
                     			
#elif (MAX_NodeId_NUM == 16)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },                     
				
#elif (MAX_NodeId_NUM == 17)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },                    
				
#elif (MAX_NodeId_NUM == 18)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },                     
				
#elif (MAX_NodeId_NUM == 19)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },                      
				
#elif (MAX_NodeId_NUM == 20)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },                   
							
#elif (MAX_NodeId_NUM == 21)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },                     
				
#elif (MAX_NodeId_NUM == 22)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },                    
				
#elif (MAX_NodeId_NUM == 23)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },                       
				
#elif (MAX_NodeId_NUM == 24)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },                     
				
#elif (MAX_NodeId_NUM == 25)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[24] },                     
				
#elif (MAX_NodeId_NUM == 26)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[24] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[25] },                    
				
#elif (MAX_NodeId_NUM == 27)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[24] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[25] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[26] },                      
				
#elif (MAX_NodeId_NUM == 28)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[24] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[25] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[26] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[27] },                      
				
#elif (MAX_NodeId_NUM == 29)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[24] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[25] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[26] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[27] },                      
				
#elif (MAX_NodeId_NUM == 30)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[24] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[25] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[26] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[27] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[28] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[29] },                     
				
#elif (MAX_NodeId_NUM == 31)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[24] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[25] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[26] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[27] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[28] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[29] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[30] },                      
				
#elif (MAX_NodeId_NUM == 32)	
				
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[0] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[1] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[2] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[3] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[4] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[5] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[6] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[7] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[8] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[9] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[10] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[11] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[12] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[13] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[14] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[15] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[16] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[17] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[18] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[19] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[20] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[21] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[22] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[23] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[24] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[25] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[26] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[27] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[28] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[29] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[30] },
				{ RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1016[31] },                       
				
#endif			
                     
    };

/* index 0x1017 :   Producer Heartbeat Time */ 
    UNS16 CanObjectDict_obj1017 = Producer_Heartbeat_Times;//ProducerHeartBeatTime 生产心跳间隔时间值
    subindex CanObjectDict_Index1017[] =
    {
        { RW, uint16, sizeof (uint16), (void*)&CanObjectDict_obj1017 }
    };

/* index 0x1018 :   Identity */ 
    UNS16 CanObjectDict_obj1018 = 4;
    UNS32 ADP_SN[MAX_NodeId_NUM];
    UNS32 IdentityObject[4];
    ODCallback_t CanObjectDict_Index1018_callbacks[] = 
    {
    NULL,
    };
    subindex CanObjectDict_Index1018[] = 
    {
        { RO, uint8, sizeof (uint8), (void*)&CanObjectDict_obj1018 },
        { RO, uint32, sizeof (UNS32), (void*)&IdentityObject[0] },
        { RO, uint32, sizeof (UNS32), (void*)&IdentityObject[1] },
        { RO, uint32, sizeof (UNS32), (void*)&IdentityObject[2] },
        { RO, uint32, sizeof (UNS32), (void*)&IdentityObject[3] },
    };
										 
/* index 0x1200 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1200 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1200_COB_ID_Client_to_Server_Transmit_SDO = 0x601;	/* 1537 */
    UNS32 CanObjectDict_obj1200_COB_ID_Server_to_Client_Receive_SDO = 0x581;	/* 1409 */
    subindex CanObjectDict_Index1200[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1200 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1200_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1200_COB_ID_Client_to_Server_Transmit_SDO }
    };

/* index 0x1201 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1201 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1201_COB_ID_Client_to_Server_Transmit_SDO = 0x602;	
    UNS32 CanObjectDict_obj1201_COB_ID_Server_to_Client_Receive_SDO = 0x582;	
    subindex CanObjectDict_Index1201[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1201 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1201_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1201_COB_ID_Client_to_Server_Transmit_SDO }
    };

/* index 0x1202 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1202 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1202_COB_ID_Client_to_Server_Transmit_SDO = 0x603;	
    UNS32 CanObjectDict_obj1202_COB_ID_Server_to_Client_Receive_SDO = 0x583;	
    subindex CanObjectDict_Index1202[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1202 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1202_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1202_COB_ID_Client_to_Server_Transmit_SDO }
    };

/* index 0x1203 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1203 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1203_COB_ID_Client_to_Server_Transmit_SDO = 0x604;	
    UNS32 CanObjectDict_obj1203_COB_ID_Server_to_Client_Receive_SDO = 0x584;	
    subindex CanObjectDict_Index1203[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1203 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1203_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1203_COB_ID_Client_to_Server_Transmit_SDO }
    };
    
/* index 0x1204 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1204 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1204_COB_ID_Client_to_Server_Transmit_SDO = 0x605;	
    UNS32 CanObjectDict_obj1204_COB_ID_Server_to_Client_Receive_SDO = 0x585;	
    subindex CanObjectDict_Index1204[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1204 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1204_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1204_COB_ID_Client_to_Server_Transmit_SDO }
    };
    
/* index 0x1205 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1205 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1205_COB_ID_Client_to_Server_Transmit_SDO = 0x606;	
    UNS32 CanObjectDict_obj1205_COB_ID_Server_to_Client_Receive_SDO = 0x586;	
    subindex CanObjectDict_Index1205[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1205 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1205_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1205_COB_ID_Client_to_Server_Transmit_SDO }
    };    

/* index 0x1206 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1206 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1206_COB_ID_Client_to_Server_Transmit_SDO = 0x607;	
    UNS32 CanObjectDict_obj1206_COB_ID_Server_to_Client_Receive_SDO = 0x587;	
    subindex CanObjectDict_Index1206[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1206 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1206_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1206_COB_ID_Client_to_Server_Transmit_SDO }
    };  
    
/* index 0x1207 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1207 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1207_COB_ID_Client_to_Server_Transmit_SDO = 0x608;	
    UNS32 CanObjectDict_obj1207_COB_ID_Server_to_Client_Receive_SDO = 0x588;	
    subindex CanObjectDict_Index1207[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1207 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1207_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1207_COB_ID_Client_to_Server_Transmit_SDO }
    };  

/* index 0x1208 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1208 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1208_COB_ID_Client_to_Server_Transmit_SDO = 0x609;	
    UNS32 CanObjectDict_obj1208_COB_ID_Server_to_Client_Receive_SDO = 0x589;	
    subindex CanObjectDict_Index1208[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1208 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1208_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1208_COB_ID_Client_to_Server_Transmit_SDO }
    };

/* index 0x1209 :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj1209 = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj1209_COB_ID_Client_to_Server_Transmit_SDO = 0x60A;	
    UNS32 CanObjectDict_obj1209_COB_ID_Server_to_Client_Receive_SDO = 0x58A;	
    subindex CanObjectDict_Index1209[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj1209 },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1209_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj1209_COB_ID_Client_to_Server_Transmit_SDO }
    };
    
/* index 0x120A :   Server1 SDO parameter. */
    UNS8 CanObjectDict_highestSubIndex_obj120A = 2; /* number of subindex - 1*/
    UNS32 CanObjectDict_obj120A_COB_ID_Client_to_Server_Transmit_SDO = 0x60B;	
    UNS32 CanObjectDict_obj120A_COB_ID_Server_to_Client_Receive_SDO = 0x58B;	
    subindex CanObjectDict_Index120A[] = 
    {
        { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_obj120A },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj120A_COB_ID_Server_to_Client_Receive_SDO },
        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_obj120A_COB_ID_Client_to_Server_Transmit_SDO }
    };    
    
/* -------------------------------------------------------- 分割线------------------------------------------------------------------- */

/* index 0x1280 :   Client1 SDO Parameter. */
    subindex CanObjectDict_Index1280[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1281 :   Client2 SDO Parameter. */
    subindex CanObjectDict_Index1281[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1282 :   Client3 SDO Parameter. */
    subindex CanObjectDict_Index1282[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1283 :   Client4 SDO Parameter. */
    subindex CanObjectDict_Index1283[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1284 :   Client5 SDO Parameter. */
    subindex CanObjectDict_Index1284[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1285 :   Client6 SDO Parameter. */
    subindex CanObjectDict_Index1285[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1286 :   Client7 SDO Parameter. */
    subindex CanObjectDict_Index1286[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1287 :   Client8 SDO Parameter. */
    subindex CanObjectDict_Index1287[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1288 :   Client9 SDO Parameter. */
    subindex CanObjectDict_Index1288[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1289 :   Client10 SDO Parameter. */
    subindex CanObjectDict_Index1289[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x128A :   Client11 SDO Parameter. */
    subindex CanObjectDict_Index128A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x128B :   Client12 SDO Parameter. */
    subindex CanObjectDict_Index128B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x128C :   Client13 SDO Parameter. */
    subindex CanObjectDict_Index128C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x128D :   Client14 SDO Parameter. */
    subindex CanObjectDict_Index128D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x128E :   Client15 SDO Parameter. */
    subindex CanObjectDict_Index128E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x128F :   Client16 SDO Parameter. */
    subindex CanObjectDict_Index128F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1290 :   Client17 SDO Parameter. */
    subindex CanObjectDict_Index1290[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };     

/* index 0x1291 :   Client18 SDO Parameter. */
    subindex CanObjectDict_Index1291[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1292 :   Client19 SDO Parameter. */
    subindex CanObjectDict_Index1292[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1293 :   Client20 SDO Parameter. */
    subindex CanObjectDict_Index1293[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1294 :   Client21 SDO Parameter. */
    subindex CanObjectDict_Index1294[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1295 :   Client22 SDO Parameter. */
    subindex CanObjectDict_Index1295[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1296 :   Client23 SDO Parameter. */
    subindex CanObjectDict_Index1296[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1297 :   Client24 SDO Parameter. */
    subindex CanObjectDict_Index1297[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1298 :   Client25 SDO Parameter. */
    subindex CanObjectDict_Index1298[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x1299 :   Client26 SDO Parameter. */
    subindex CanObjectDict_Index1299[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x129A :   Client27 SDO Parameter. */
    subindex CanObjectDict_Index129A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x129B :   Client28 SDO Parameter. */
    subindex CanObjectDict_Index129B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x129C :   Client29 SDO Parameter. */
    subindex CanObjectDict_Index129C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x129D :   Client30 SDO Parameter. */
    subindex CanObjectDict_Index129D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x129E :   Client31 SDO Parameter. */
    subindex CanObjectDict_Index129E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };

/* index 0x129F :   Client32 SDO Parameter. */
    subindex CanObjectDict_Index129F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { RW, uint8, sizeof (UNS8), 0 }
    };                     

/*-------------------------从机x的TPDO通讯参数--------------------分割线2------------------------------------------*/
    
/*-------------------------从机x的TPDO1通讯参数--------------------*/										 
/* index 0x1400 :   从机节点1的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1400[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1401 :   从机节点2的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1401[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1402 :   从机节点3的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1402[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1403 :   从机节点4的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1403[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1404 :   从机节点5的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1404[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1405 :   从机节点6的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1405[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1406 :   从机节点7的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1406[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1407 :   从机节点8的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1407[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1408 :   从机节点9的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1408[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1409 :   从机节点10的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1409[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x140A :   从机节点11的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index140A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x140B :   从机节点12的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index140B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x140C :   从机节点13的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index140C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x140D :   从机节点14的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index140D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x140E :   从机节点15的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index140E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x140F :   从机节点16的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index140F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };
    
/* index 0x1410 :   从机节点17的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1410[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1411 :   从机节点18的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1411[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1412 :   从机节点19的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1412[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x1413 :   从机节点20的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1413[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     

/* index 0x1414 :   从机节点21的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1414[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1415 :   从机节点22的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1415[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1416 :   从机节点23的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1416[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1417 :   从机节点24的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1417[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1418 :   从机节点25的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1418[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };
    
/* index 0x1419 :   从机节点26的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index1419[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };   
    
/* index 0x141A :   从机节点27的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index141A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x141B :   从机节点28的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index141B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x141C :   从机节点29的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index141C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x141D :   从机节点30的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index141D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x141E :   从机节点31的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index141E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x141F :   从机节点32的TPDO1映射到本主节点的RPDO1*/
    subindex CanObjectDict_Index141F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     

  

/*-------------------------从机x的TPDO2通讯参数--------------------*/											 
/* index 0x1420 :   从机节点1的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1420[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1421 :   从机节点2的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1421[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1422 :   从机节点3的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1422[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1423 :   从机节点4的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1423[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1424 :   从机节点5的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1424[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1425 :   从机节点6的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1425[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1426 :   从机节点7的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1426[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1427 :   从机节点8的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1427[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1428 :   从机节点9的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1428[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1429 :   从机节点10的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1429[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x142A :   从机节点11的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index142A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x142B :   从机节点12的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index142B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x142C :   从机节点13的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index142C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x142D :   从机节点14的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index142D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x142E :   从机节点15的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index142E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x142F :   从机节点16的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index142F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };
    
/* index 0x1430 :   从机节点17的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1430[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1431 :   从机节点18的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1431[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1432 :   从机节点19的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1432[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x1433 :   从机节点20的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1433[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     

/* index 0x1434 :   从机节点21的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1434[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1435 :   从机节点22的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1435[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1436 :   从机节点23的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1436[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1437 :   从机节点24的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1437[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1438 :   从机节点25的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1438[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };
    
/* index 0x1439 :   从机节点26的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index1439[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };   
    
/* index 0x143A :   从机节点27的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index143A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x143B :   从机节点28的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index143B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x143C :   从机节点29的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index143C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x143D :   从机节点30的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index143D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x143E :   从机节点31的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index143E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x143F :   从机节点32的TPDO2映射到本主节点的RPDO2*/
    subindex CanObjectDict_Index143F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     

  

/*-------------------------从机x的TPDO3通讯参数--------------------*/											 
/* index 0x1440 :   从机节点1的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1440[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1441 :   从机节点2的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1441[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1442 :   从机节点3的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1442[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1443 :   从机节点4的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1443[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1444 :   从机节点5的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1444[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1445 :   从机节点6的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1445[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1446 :   从机节点7的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1446[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1447 :   从机节点8的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1447[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1448 :   从机节点9的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1448[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1449 :   从机节点10的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1449[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x144A :   从机节点11的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index144A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x144B :   从机节点12的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index144B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x144C :   从机节点13的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index144C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x144D :   从机节点14的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index144D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x144E :   从机节点15的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index144E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x144F :   从机节点16的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index144F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };
    
/* index 0x1450 :   从机节点17的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1450[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1451 :   从机节点18的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1451[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1452 :   从机节点19的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1452[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x1453 :   从机节点20的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1453[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     

/* index 0x1454 :   从机节点21的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1454[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1455 :   从机节点22的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1455[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1456 :   从机节点23的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1456[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1457 :   从机节点24的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1457[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1458 :   从机节点25的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1458[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };
    
/* index 0x1459 :   从机节点26的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index1459[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };   
    
/* index 0x145A :   从机节点27的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index145A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x145B :   从机节点28的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index145B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x145C :   从机节点29的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index145C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x145D :   从机节点30的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index145D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x145E :   从机节点31的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index145E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x145F :   从机节点32的TPDO3映射到本主节点的RPDO3*/
    subindex CanObjectDict_Index145F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     

  

/*-------------------------从机x的TPDO4通讯参数--------------------*/											 
/* index 0x1460 :   从机节点1的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1460[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1461 :   从机节点2的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1461[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1462 :   从机节点3的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1462[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1463 :   从机节点4的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1463[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1464 :   从机节点5的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1464[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1465 :   从机节点6的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1465[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1466 :   从机节点7的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1466[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1467 :   从机节点8的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1467[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1468 :   从机节点9的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1468[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1469 :   从机节点10的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1469[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x146A :   从机节点11的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index146A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x146B :   从机节点12的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index146B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x146C :   从机节点13的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index146C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x146D :   从机节点14的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index146D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x146E :   从机节点15的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index146E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x146F :   从机节点16的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index146F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };
    
/* index 0x1470 :   从机节点17的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1470[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1471 :   从机节点18的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1471[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1472 :   从机节点19的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1472[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x1473 :   从机节点20的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1473[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     

/* index 0x1474 :   从机节点21的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1474[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x1475 :   从机节点22的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1475[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1476 :   从机节点23的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1476[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1477 :   从机节点24的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1477[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };

/* index 0x1478 :   从机节点25的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1478[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };
    
/* index 0x1479 :   从机节点26的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index1479[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };   
    
/* index 0x147A :   从机节点27的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index147A[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x147B :   从机节点28的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index147B[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
    
/* index 0x147C :   从机节点29的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index147C[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x147D :   从机节点30的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index147D[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x147E :   从机节点31的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index147E[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };    
    
/* index 0x147F :   从机节点32的TPDO4映射到本主节点的RPDO4*/
    subindex CanObjectDict_Index147F[] = 
    {
        { RO, uint8, sizeof (UNS8), 0 },
        { RW, uint32, sizeof (UNS32), 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { RW, uint32, sizeof (UNS32), 0 }
    };     
										 
/* ----------------------------------从机RPDO通讯参数(PDO_TRS)--------------- 分割线3---------------------------------------------------- */
/*-------------------------从机x的RPDO1通讯参数--------------------*/	
/* index 0x1800 :   Slave1 RPDO 1 communication parameters. */
                    ODCallback_t CanObjectDict_Index1800_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1800[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1801 :   Slave2 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1801_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1801[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1802 :   Slave3 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1802_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1802[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1803 :   Slave4 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1803_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1803[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1804 :   Slave5 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1804_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1804[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1805 :   Slave6 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1805_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1805[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1806 :   Slave7 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1806_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1806[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1807 :   Slave8 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1807_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1807[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1808 :   Slave9 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1808_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1808[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1809 :   Slave10 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1809_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1809[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x180A :   Slave11 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index180A_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index180A[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x180B :   Slave12 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index180B_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index180B[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x180C :   Slave13 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index180C_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index180C[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x180D :   Slave14 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index180D_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index180D[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x180E :   Slave15 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index180E_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index180E[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x180F :   Slave16 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index180F_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index180F[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x1810 :   Slave17 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1810_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1810[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
										 
/* index 0x1811 :   Slave18 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1811_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1811[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1812 :   Slave19 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1812_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1812[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1813 :   Slave20 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1813_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1813[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1814 :   Slave21 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1814_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1814[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1815 :   Slave22 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1815_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1815[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1816 :   Slave23 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1816_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1816[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1817 :   Slave24 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1817_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1817[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1818 :   Slave25 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1818_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1818[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1819 :   Slave26 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1819_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1819[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x181A :   Slave27 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index181A_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index181A[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x181B :   Slave28 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index181B_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index181B[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x181C :   Slave29 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index181C_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index181C[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x181D :   Slave30 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index181D_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index181D[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x181E :   Slave31 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index181E_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index181E[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x181F :   Slave32 RPDO 1 communication parameters.  */
                    ODCallback_t CanObjectDict_Index181F_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index181F[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/*-------------------------从机x的RPDO2通讯参数--------------------*/
/* index 0x1820 :   Slave1 RPDO 2 communication parameters. */
                    ODCallback_t CanObjectDict_Index1820_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1820[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1821 :   Slave2 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1821_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1821[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1822 :   Slave3 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1822_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1822[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1823 :   Slave4 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1823_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1823[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1824 :   Slave5 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1824_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1824[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1825 :   Slave6 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1825_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1825[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1826 :   Slave7 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1826_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1826[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1827 :   Slave8 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1827_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1827[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1828 :   Slave9 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1828_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1828[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1829 :   Slave10 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1829_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1829[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x182A :   Slave11 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index182A_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index182A[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x182B :   Slave12 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index182B_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index182B[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x182C :   Slave13 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index182C_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index182C[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x182D :   Slave14 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index182D_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index182D[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x182E :   Slave15 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index182E_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index182E[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x182F :   Slave16 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index182F_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index182F[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x1830 :   Slave17 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1830_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1830[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
										 
/* index 0x1831 :   Slave18 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1831_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1831[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1832 :   Slave19 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1832_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1832[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1833 :   Slave20 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1833_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1833[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1834 :   Slave21 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1834_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1834[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1835 :   Slave22 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1835_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1835[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1836 :   Slave23 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1836_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1836[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1837 :   Slave24 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1837_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1837[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1838 :   Slave25 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1838_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1838[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1839 :   Slave26 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1839_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1839[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x183A :   Slave27 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index183A_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index183A[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x183B :   Slave28 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index183B_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index183B[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x183C :   Slave29 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index183C_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index183C[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x183D :   Slave30 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index183D_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index183D[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x183E :   Slave31 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index183E_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index183E[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x183F :   Slave32 RPDO 2 communication parameters.  */
                    ODCallback_t CanObjectDict_Index183F_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index183F[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
                     

/*-------------------------从机x的RPDO3通讯参数--------------------*/
/* index 0x1840 :   Slave1 RPDO 3 communication parameters. */
                    ODCallback_t CanObjectDict_Index1840_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1840[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1841 :   Slave2 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1841_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1841[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1842 :   Slave3 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1842_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1842[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1843 :   Slave4 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1843_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1843[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1844 :   Slave5 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1844_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1844[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1845 :   Slave6 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1845_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1845[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1846 :   Slave7 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1846_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1846[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1847 :   Slave8 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1847_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1847[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1848 :   Slave9 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1848_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1848[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1849 :   Slave10 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1849_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1849[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x184A :   Slave11 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index184A_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index184A[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x184B :   Slave12 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index184B_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index184B[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x184C :   Slave13 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index184C_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index184C[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x184D :   Slave14 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index184D_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index184D[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x184E :   Slave15 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index184E_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index184E[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x184F :   Slave16 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index184F_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index184F[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x1850 :   Slave17 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1850_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1850[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
										 
/* index 0x1851 :   Slave18 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1851_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1851[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1852 :   Slave19 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1852_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1852[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1853 :   Slave20 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1853_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1853[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1854 :   Slave21 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1854_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1854[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1855 :   Slave22 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1855_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1855[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1856 :   Slave23 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1856_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1856[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1857 :   Slave24 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1857_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1857[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1858 :   Slave25 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1858_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1858[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1859 :   Slave26 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1859_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1859[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x185A :   Slave27 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index185A_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index185A[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x185B :   Slave28 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index185B_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index185B[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x185C :   Slave29 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index185C_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index185C[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x185D :   Slave30 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index185D_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index185D[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x185E :   Slave31 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index185E_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index185E[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x185F :   Slave32 RPDO 3 communication parameters.  */
                    ODCallback_t CanObjectDict_Index185F_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index185F[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };      


/*-------------------------从机x的RPDO4通讯参数--------------------*/
/* index 0x1860 :   Slave1 RPDO 4 communication parameters. */
                    ODCallback_t CanObjectDict_Index1860_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1860[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1861 :   Slave2 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1861_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1861[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1862 :   Slave3 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1862_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1862[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1863 :   Slave4 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1863_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1863[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1864 :   Slave5 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1864_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1864[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1865 :   Slave6 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1865_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1865[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1866 :   Slave7 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1866_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1866[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1867 :   Slave8 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1867_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1867[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1868 :   Slave9 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1868_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1868[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };

/* index 0x1869 :   Slave10 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1869_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1869[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x186A :   Slave11 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index186A_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index186A[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x186B :   Slave12 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index186B_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index186B[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x186C :   Slave13 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index186C_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index186C[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x186D :   Slave14 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index186D_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index186D[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x186E :   Slave15 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index186E_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index186E[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x186F :   Slave16 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index186F_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index186F[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
	 
/* index 0x1870 :   Slave17 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1870_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1870[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
										 
/* index 0x1871 :   Slave18 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1871_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1871[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1872 :   Slave19 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1872_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1872[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1873 :   Slave20 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1873_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1873[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1874 :   Slave21 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1874_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1874[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1875 :   Slave22 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1875_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1875[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1876 :   Slave23 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1876_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1876[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1877 :   Slave24 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1877_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1877[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1878 :   Slave25 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1878_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1878[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x1879 :   Slave26 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index1879_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index1879[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x187A :   Slave27 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index187A_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index187A[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x187B :   Slave28 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index187B_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index187B[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x187C :   Slave29 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index187C_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index187C[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x187D :   Slave30 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index187D_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index187D[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x187E :   Slave31 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index187E_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index187E[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };
																				 
/* index 0x187F :   Slave32 RPDO 4 communication parameters.  */
                    ODCallback_t CanObjectDict_Index187F_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     };
                    subindex CanObjectDict_Index187F[] = 
                     {
                       { RO, uint8, sizeof (UNS8), NULL },
                       { RW, uint32, sizeof (UNS32), NULL },
                       { RW, uint8, sizeof (UNS8), NULL },
                       { RW, uint16, sizeof (UNS16), NULL },
                       { RW, uint16, sizeof (UNS16), NULL},
                       { RW, uint8, sizeof (UNS8), NULL}
                     };                        
                     
/*----------从机的TPDO映射参数------------分割线3---------------------------------*/
/*----------从机的TPDO映射参数------------TPDO1-----------------------------------*/		
/*----------从机的TPDO映射参数------------步科驱动器-------------------------------*/												 
			UNS32 CanObjectDict_TPDO1_Kinco_Map[] = 
			{
				0x60790020,//母线电压
				0x60780010,//电机实际电流
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo1_Kinco = CountOf(CanObjectDict_TPDO1_Kinco_Map);		
			subindex CanObjectDict_TPDO1_Map_Kinco_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo1_Kinco },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO1_Kinco_Map[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO1_Kinco_Map[1] },
			 };		

/*----------从机的TPDO映射参数------------台达驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO1_Delta_Map[] = 
			{
				0x20090020,//P0_09:母线电压
				0x200A0020,//P0_10:IGBT温度
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo1_Delta = CountOf(CanObjectDict_TPDO1_Delta_Map);		
			subindex CanObjectDict_TPDO1_Map_Delta_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo1_Delta },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO1_Delta_Map[0] },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO1_Delta_Map[1] },
			 };		

/*----------从机的TPDO映射参数------------风得控驱动器-------------------------------*/				 
			UNS32 CanObjectDict_TPDO1_Windcon_Map[] = 
			{
				0x60780010,//实际电流
				0x20030010,//母线电压
				0x22A20010,//驱动器温度
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo1_Windcon = CountOf(CanObjectDict_TPDO1_Windcon_Map);		
			subindex CanObjectDict_TPDO1_Map_Windcon_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo1_Windcon },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO1_Windcon_Map[0] },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO1_Windcon_Map[1] },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO1_Windcon_Map[2] },				 
			 };			

/*----------从机的TPDO映射参数------------惠斯通驱动器-------------------------------*/				 
			UNS32 CanObjectDict_TPDO1_Wheatstone_Map[] = 
			{
        0x20090520,//母线电压
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo1_Wheatstone = CountOf(CanObjectDict_TPDO1_Wheatstone_Map);		
			subindex CanObjectDict_TPDO1_Map_Wheatstone_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo1_Wheatstone },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO1_Wheatstone_Map[0] },		 
			 };		

/*----------从机的TPDO映射参数------------科亚驱动器-------------------------------*/				 
			UNS32 CanObjectDict_TPDO1_Keya_Map[] = 
			{
        0x20120010,//A驱动器转速
        0x20100010,//A驱动器电流
        0x20160020,//A驱动器位置		
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo1_Keya = CountOf(CanObjectDict_TPDO1_Keya_Map);		
			subindex CanObjectDict_TPDO1_Map_Keya_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo1_Keya },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO1_Keya_Map[0] },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO1_Keya_Map[1] },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO1_Keya_Map[2] },				 
			 };				 

/*---------从机的TPDO映射参数--------------Copley驱动器-------------------------------*/
			UNS32 CanObjectDict_TPDO1_Copley_Map[] = 
			{
				0x22010010,//母线电压
				0x221C0010,//电机实际电流
				0x22020010,//温度
				0x21900010,//输入pin状态
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo1_Copley = CountOf(CanObjectDict_TPDO1_Copley_Map);		
			subindex CanObjectDict_TPDO1_Map_Copley_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo1_Copley },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO1_Copley_Map[0] },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO1_Copley_Map[1] },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO1_Copley_Map[2] },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO1_Copley_Map[3] },
			 };			 

/*----------从机的TPDO映射参数------------TPDO2------------------------------------*/	
/*----------从机的TPDO映射参数------------步科驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO2_Kinco_Map[] = 
			{
				0x60410010,//状态字(RO)
				0x60610008,//工作模式(RO)
        0x60630020,//电机实际位置				
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo2_Kinco = CountOf(CanObjectDict_TPDO2_Kinco_Map);		
			subindex CanObjectDict_TPDO2_Map_Kinco_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo2_Kinco },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO2_Kinco_Map[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO2_Kinco_Map[1] },	
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO2_Kinco_Map[2] },				 
			 };			

/*----------从机的TPDO映射参数------------台达驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO2_Delta_Map[] = 
			{
				0x606C0020,//电机转速
				0x60780010,//实际电流
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo2_Delta = CountOf(CanObjectDict_TPDO2_Delta_Map);		
			subindex CanObjectDict_TPDO2_Map_Delta_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo2_Delta },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO2_Delta_Map[0] },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO2_Delta_Map[1] },
			 };			

/*----------从机的TPDO映射参数------------风得控驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO2_Windcon_Map[] = 
			{
				0x60410010,//状态字(RO)
				0x60610008,//工作模式(RO)
				0x603F0010,//错误代码(RO)				
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo2_Windcon = CountOf(CanObjectDict_TPDO2_Windcon_Map);		
			subindex CanObjectDict_TPDO2_Map_Windcon_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo2_Windcon },
				 { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO2_Windcon_Map[0] },
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_TPDO2_Windcon_Map[1] },	
				 { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO2_Windcon_Map[2] },				 
			 };		

/*----------从机的TPDO映射参数------------惠斯通驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO2_Wheatstone_Map[] = 
			{
				0x606C0020,//电机转速
        0x20090420,//实际电流					
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo2_Wheatstone = CountOf(CanObjectDict_TPDO2_Wheatstone_Map);		
			subindex CanObjectDict_TPDO2_Map_Wheatstone_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo2_Wheatstone },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO2_Wheatstone_Map[0] },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO2_Wheatstone_Map[1] },				 
			 };			

/*----------从机的TPDO映射参数------------科亚驱动器-------------------------------*/				 
			UNS32 CanObjectDict_TPDO2_Keya_Map[] = 
			{
        0x20220010,//B驱动器转速
        0x20200010,//B驱动器电流
        0x20260020,//B驱动器位置		
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo2_Keya = CountOf(CanObjectDict_TPDO2_Keya_Map);		
			subindex CanObjectDict_TPDO2_Map_Keya_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo2_Keya },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO2_Keya_Map[0] },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO2_Keya_Map[1] },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO2_Keya_Map[2] },				 
			 };				 
	 
/*---------从机的TPDO映射参数--------------Copley驱动器-------------------------------*/
			UNS32 CanObjectDict_TPDO2_Copley_Map[] = 
			{
				0x60410010,//状态字(RO)
				0x10020020,//事件字(RO)
				0x60610008,//工作模式(RO)
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo2_Copley = CountOf(CanObjectDict_TPDO2_Copley_Map);		
			subindex CanObjectDict_TPDO2_Map_Copley_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo2_Copley },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO2_Copley_Map[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO2_Copley_Map[1] },
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_TPDO2_Copley_Map[2] },
			 };	
			 			 
/*----------从机的TPDO映射参数------------TPDO3-----------------------------------*/	
/*----------从机的TPDO映射参数------------步科驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO3_Kinco_Map[] = 
			{
				0x606C0020,//实际电机转速(RO)
				0x26010010,//错误锁存 --- 错误状态字(RO)    
				0x26020010,//错误锁存 --- 错误状态字2(RO)   				
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo3_Kinco = CountOf(CanObjectDict_TPDO3_Kinco_Map);		
			subindex CanObjectDict_TPDO3_Map_Kinco_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo3_Kinco },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO3_Kinco_Map[0] },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO3_Kinco_Map[1] },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO3_Kinco_Map[2] },				 
			 };		

/*----------从机的TPDO映射参数------------台达驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO3_Delta_Map[] = 
			{
				0x60410010,//状态字(RO)
				0x603F0010,//错误代码(RO)
				0x60610008,//工作模式(RO)
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo3_Delta = CountOf(CanObjectDict_TPDO3_Delta_Map);		
			subindex CanObjectDict_TPDO3_Map_Delta_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo3_Delta },
				 { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO3_Delta_Map[0] },
				 { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO3_Delta_Map[1] },
				 { RO, int8, sizeof (INTEGER8), (void*)&CanObjectDict_TPDO3_Delta_Map[2] },
			 };			 
	
/*----------从机的TPDO映射参数------------风得控驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO3_Windcon_Map[] = 
			{
				0x606C0020,//电机转速
				0x20000010,//驱动器错误码  
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo3_Windcon = CountOf(CanObjectDict_TPDO3_Windcon_Map);		
			subindex CanObjectDict_TPDO3_Map_Windcon_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo3_Windcon },
				 { RW, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO3_Windcon_Map[0] },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO3_Windcon_Map[1] },
			 };	

/*----------从机的TPDO映射参数------------惠斯通驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO3_Wheatstone_Map[] = 
			{
				0x60410010,//状态字(RO)
				0x603F0010,//错误代码(RO)
				0x60610008,//工作模式(RO)
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo3_Wheatstone = CountOf(CanObjectDict_TPDO3_Wheatstone_Map);		
			subindex CanObjectDict_TPDO3_Map_Wheatstone_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo3_Wheatstone },
				 { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO3_Wheatstone_Map[0] },
				 { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_TPDO3_Wheatstone_Map[1] },
				 { RO, int8, sizeof (INTEGER8), (void*)&CanObjectDict_TPDO3_Wheatstone_Map[2] },
			 };			

/*----------从机的TPDO映射参数------------科亚驱动器-------------------------------*/				 
			UNS32 CanObjectDict_TPDO3_Keya_Map[] = 
			{
        0x20110010,//A驱动器故障
        0x20210010,//B驱动器故障
        0x20130010,//电压
        0x203C0010,//温度				
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo3_Keya = CountOf(CanObjectDict_TPDO3_Keya_Map);		
			subindex CanObjectDict_TPDO3_Map_Keya_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo3_Keya },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO3_Keya_Map[0] },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO3_Keya_Map[1] },
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO3_Keya_Map[2] },	
				 { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_TPDO3_Keya_Map[3] },				 
			 };		
			 		 
/*---------从机的TPDO映射参数--------------Copley驱动器-------------------------------*/
			UNS32 CanObjectDict_TPDO3_Copley_Map[] = 
			{
				0x60690020,//实际电机转速(RO)
				0x21830020,//错误锁存(RO) 
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo3_Copley = CountOf(CanObjectDict_TPDO3_Copley_Map);		
			subindex CanObjectDict_TPDO3_Map_Copley_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo3_Copley },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO3_Copley_Map[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_TPDO3_Copley_Map[1] },
			 };	
			 			 
/*----------从机的TPDO映射参数------------TPDO4------------------------------------*/		
/*----------从机的TPDO映射参数------------步科驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO4_Kinco_Map[] = 
			{
				NULL,
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo4_Kinco = CountOf(CanObjectDict_TPDO4_Kinco_Map);
			subindex CanObjectDict_TPDO4_Map_Kinco_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo4_Kinco },
			 };

/*----------从机的TPDO映射参数------------台达驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO4_Delta_Map[] = 
			{
				0x200B0020,//P0_11:驱动器错误码
				0x200C0020,//P0_12:保护计数
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo4_Delta = CountOf(CanObjectDict_TPDO4_Delta_Map);		
			subindex CanObjectDict_TPDO4_Map_Delta_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo4_Delta },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO4_Delta_Map[0] },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO4_Delta_Map[1] },
			 };
			 
/*----------从机的TPDO映射参数------------风得控驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO4_Windcon_Map[] = 
			{
        0x60640020,   //电机实际位置
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo4_Windcon = CountOf(CanObjectDict_TPDO4_Windcon_Map);		
			subindex CanObjectDict_TPDO4_Map_Windcon_index[] = 
			 {
				 { RW, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo4_Windcon },
				 { RW, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO4_Windcon_Map[0] },
			 };		

/*----------从机的TPDO映射参数------------惠斯通驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO4_Wheatstone_Map[] = 
			{
				NULL,
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo4_Wheatstone = CountOf(CanObjectDict_TPDO4_Wheatstone_Map);
			subindex CanObjectDict_TPDO4_Map_Wheatstone_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo4_Wheatstone },
			 };	

/*----------从机的TPDO映射参数------------科亚驱动器-------------------------------*/				 
			UNS32 CanObjectDict_TPDO4_Keya_Map[] = 
			{
        0x20030020,//A驱动器状态
        0x20040020,//B驱动器状态		
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo4_Keya = CountOf(CanObjectDict_TPDO4_Keya_Map);		
			subindex CanObjectDict_TPDO4_Map_Keya_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo4_Keya },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO4_Keya_Map[0] },
				 { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_TPDO4_Keya_Map[1] },				 
			 };					 	 

/*----------从机的TPDO映射参数------------Copley驱动器-------------------------------*/			 
			UNS32 CanObjectDict_TPDO4_Copley_Map[] = 
			{
				NULL,
			};
			UNS8 CanObjectDict_highestSubIndex_tpdo4_Copley = CountOf(CanObjectDict_TPDO4_Copley_Map);
			subindex CanObjectDict_TPDO4_Map_Copley_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_tpdo4_Copley},
			 };			 
			 
/* ----------------------------------从机RPDO映射参数(PDO_TRS_MAP)----------- 分割线3---------------------------------------------------- */
			UNS32 CanObjectDict_RPDO1_Map[] = 
			{
				0x60400010,//输入控制字
				0x60FF0020,//速度命令输入_Commanded velocity	
				0x210C0010,//P1-12 S16 力矩限制暂存器				
			};
			UNS8 CanObjectDict_highestSubIndex_RPDO1_Map = CountOf(CanObjectDict_RPDO1_Map);
			subindex CanObjectDict_RPDO1_Map_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_RPDO1_Map },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_RPDO1_Map[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO1_Map[1] },
				 { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_RPDO1_Map[2] },			 
			 };
			UNS32 CanObjectDict_RPDO1_Map_FPMode[] = 
			{
				0x60400010,//输入控制字
				0x607A0020,//目标位置
			};
			UNS8 CanObjectDict_highestSubIndex_FPRPDO1_Map = CountOf(CanObjectDict_RPDO1_Map_FPMode);
			subindex CanObjectDict_RPDO1_Map_ForPosationMode_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_FPRPDO1_Map },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO1_Map_FPMode[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO1_Map_FPMode[1] },
			 };				 
			 
			UNS32 CanObjectDict_RPDO2_Map[] = 
			{
				NULL,
			};
			UNS8 CanObjectDict_highestSubIndex_RPDO2_Map = CountOf(CanObjectDict_RPDO2_Map);
			subindex CanObjectDict_RPDO2_Map_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_RPDO2_Map },

			 };
             
			UNS32 CanObjectDict_RPDO3_Map[] = 
			{
				NULL,
			};
			UNS8 CanObjectDict_highestSubIndex_RPDO3_Map = CountOf(CanObjectDict_RPDO3_Map);
			subindex CanObjectDict_RPDO3_Map_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_RPDO3_Map },

			 };     

			UNS32 CanObjectDict_RPDO4_Map[] = 
			{
				NULL,
			};
			UNS8 CanObjectDict_highestSubIndex_RPDO4_Map = CountOf(CanObjectDict_RPDO4_Map);
			subindex CanObjectDict_RPDO4_Map_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_RPDO4_Map },

			 };    

/* ----------------------------------从机RPDO映射参数(PDO_TRS_MAP)----------- 分割线4---------------------------------------------------- */			 
			UNS32 CanObjectDict_RPDO1_Keya_Map[] = 
			{
				0x607A0020,//A驱动器使能
				0x60810020,//A驱动器速度命令输入_Commanded velocity				
			};
			UNS8 CanObjectDict_highestSubIndex_RPDO1_Keya_Map = CountOf(CanObjectDict_RPDO1_Keya_Map);
			subindex CanObjectDict_RPDO1_Map_Keya_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_RPDO1_Keya_Map },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO1_Keya_Map[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO1_Keya_Map[1] },		 
			 };
			 			
			UNS32 CanObjectDict_RPDO2_Keya_Map[] = 
			{
				0x60830020,//B驱动器使能
				0x60840020,//B驱动器速度命令输入_Commanded velocity				
			};
			UNS8 CanObjectDict_highestSubIndex_RPDO2_Keya_Map = CountOf(CanObjectDict_RPDO2_Keya_Map);
			subindex CanObjectDict_RPDO2_Map_Keya_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_RPDO2_Keya_Map },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO2_Keya_Map[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO2_Keya_Map[1] },		 
			 };			 

/* ----------------------------------从机RPDO映射参数(PDO_TRS_MAP)----------- 分割线5---------------------------------------------------- */			 
			UNS32 CanObjectDict_RPDO1_Copley_Map[] = 
			{
#if (CopleyWorkMode==0)
				0x21910010,//INPUT PIN CONFIG REGISTER (16 BIT)
				0x23410020,//速度命令输入_Commanded velocity		
#elif  (CopleyWorkMode==1)
				0x60400010,//输入控制字
				0x60FF0020,//速度命令输入_Commanded velocity	
#elif  (CopleyWorkMode==2)
				0x21910010,//INPUT PIN CONFIG REGISTER (16 BIT)
				0x23400010,//电流命令输入_Current Loop
#endif	
			};
			UNS8 CanObjectDict_highestSubIndex_RPDO1_Copley_Map = CountOf(CanObjectDict_RPDO1_Copley_Map);
			subindex CanObjectDict_RPDO1_Map_Copley_index[] = 
			 {
				 { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_highestSubIndex_RPDO1_Copley_Map },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO1_Copley_Map[0] },
				 { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_RPDO1_Copley_Map[1] },		 
			 };			 

/* -------------------------自定义接收PDOmapping映射区域------------------------------ 分割线-------------------------------------------------------- */
/* index 0x2000 :   Mapped variable CanObjectDict_Rx_Map58 */
										INTEGER16 CanObjectDict_Rx_Map58;
                    ODCallback_t CanObjectDict_0x2000_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2000[] = 
                     {
                       { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_Rx_Map58 }
                     };	

#if (SVO_Type == 4)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器	       5：Copley驱动器	

/* index 0x2003 :   Mapped variable CanObjectDict_Rx_Map59 */
										INTEGER32 CanObjectDict_Rx_Map59;
                    ODCallback_t CanObjectDict_0x2003_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2003[] = 
                     {
                       { RW, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map59 }
                     };	
									 
#else

/* index 0x2003 :   Mapped variable CanObjectDict_Rx_Map59 */
										INTEGER16 CanObjectDict_Rx_Map59;
                    ODCallback_t CanObjectDict_0x2003_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2003[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map59 }
                     };											 
										 
#endif										 
										 
										 
/* index 0x2004 :   Mapped variable CanObjectDict_Rx_Map59 */
										INTEGER32 CanObjectDict_Rx_Map60;
                    ODCallback_t CanObjectDict_0x2004_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2004[] = 
                     {
                       { RW, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map60 }
                     };											 

										 

/* index 0x2009 :   Mapped variable CanObjectDict_Rx_Map40 */
					          INTEGER32 CanObjectDict_Rx_Map40;
                    ODCallback_t CanObjectDict_0x2009_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2009[] = 
                     {
                       { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map40 }
                     };
										 
/* index 0x200A :   Mapped variable CanObjectDict_Rx_Map41 */
					          INTEGER32 CanObjectDict_Rx_Map41;
                    ODCallback_t CanObjectDict_0x200A_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index200A[] = 
                     {
                       { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map41 }
                     };			
										 
/* index 0x200B :   Mapped variable CanObjectDict_Rx_Map42 */
					          INTEGER32 CanObjectDict_Rx_Map42;
                    ODCallback_t CanObjectDict_0x200B_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index200B[] = 
                     {
                       { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map42 }
                     };

/* index 0x200C :   Mapped variable CanObjectDict_Rx_Map43 */
					          INTEGER32 CanObjectDict_Rx_Map43;
                    ODCallback_t CanObjectDict_0x200C_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index200C[] = 
                     {
                       { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map43 }
                     };
										 
/* index 0x2010 :   Mapped variable CanObjectDict_Rx_Map61 */
										INTEGER16 CanObjectDict_Rx_Map61;
                    ODCallback_t CanObjectDict_0x2010_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2010[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map61 }
                     };			
										 
										 
/* index 0x2011 :   Mapped variable CanObjectDict_Rx_Map62 */
										INTEGER16 CanObjectDict_Rx_Map62;
                    ODCallback_t CanObjectDict_0x2011_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2011[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map62 }
                     };			

/* index 0x2012 :   Mapped variable CanObjectDict_Rx_Map63 */
										INTEGER16 CanObjectDict_Rx_Map63;
                    ODCallback_t CanObjectDict_0x2012_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2012[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map63 }
                     };				

/* index 0x2013 :   Mapped variable CanObjectDict_Rx_Map64 */
										INTEGER16 CanObjectDict_Rx_Map64;
                    ODCallback_t CanObjectDict_0x2013_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2013[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map64 }
                     };			

/* index 0x2016 :   Mapped variable CanObjectDict_Rx_Map65 */
										INTEGER32 CanObjectDict_Rx_Map65;
                    ODCallback_t CanObjectDict_0x2016_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2016[] = 
                     {
                       { RW, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map65 }
                     };			

/* index 0x2020 :   Mapped variable CanObjectDict_Rx_Map66 */
										INTEGER16 CanObjectDict_Rx_Map66;
                    ODCallback_t CanObjectDict_0x2020_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2020[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map66 }
                     };		

/* index 0x2021 :   Mapped variable CanObjectDict_Rx_Map67 */
										INTEGER16 CanObjectDict_Rx_Map67;
                    ODCallback_t CanObjectDict_0x2021_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2021[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map67 }
                     };												 
										 
/* index 0x2022 :   Mapped variable CanObjectDict_Rx_Map68 */
										INTEGER16 CanObjectDict_Rx_Map68;
                    ODCallback_t CanObjectDict_0x2022_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2022[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map68 }
                     };				

/* index 0x2026 :   Mapped variable CanObjectDict_Rx_Map69 */
										INTEGER32 CanObjectDict_Rx_Map69;
                    ODCallback_t CanObjectDict_0x2026_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2026[] = 
                     {
                       { RW, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map69 }
                     };			

/* index 0x203C :   Mapped variable CanObjectDict_Rx_Map70 */
										INTEGER16 CanObjectDict_Rx_Map70;
                    ODCallback_t CanObjectDict_0x203C_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index203C[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map70 }
                     };												 
										 
/* index 0x210C :   Mapped variable MasterTxMap12 */
					          INTEGER16 CanObjectDict_Tx_Map12[MAX_NodeId_NUM] = {0};
                    ODCallback_t CanObjectDict_0x210C_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index210C[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)CanObjectDict_Tx_Map12 }
                     };												 										 
										              
/* index 0x2183 :   Mapped variable MasterRxMap18 */
                    UNS32 CanObjectDict_Rx_Map18 = 0;
                    ODCallback_t CanObjectDict_0x2183_callbacks[] = 
                    {
                        NULL
                    };
                    subindex CanObjectDict_Index2183[] = 
                    {
                        { RO, uint32, sizeof (UNS32), (void*)&CanObjectDict_Rx_Map18 }
                    };     
									 
/* index 0x2190 :   Mapped variable CanObjectDict_Rx_Map13 */
										INTEGER16 CanObjectDict_Rx_Map13;
                    ODCallback_t CanObjectDict_0x2190_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2190[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map13 }
                     };
										 
/* index 0x2191 :   Mapped variable MasterTxMap10 */
                    ODCallback_t CanObjectDict_0x2191_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2191[] = 
                     {
                       { RW, uint16, sizeof (UNS16), (void*)CanObjectDict_Tx_Map10 }
                     };										 											

/* index 0x2201 :   Mapped variable CanObjectDict_Rx_Map10 */
										UNS32 CanObjectDict_Rx_Map10;
                    ODCallback_t CanObjectDict_0x2201_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2201[] = 
                     {
                       { RW, uint16, sizeof (UNS16), (void*)&CanObjectDict_Rx_Map10 }
                     };
										 
/* index 0x221C :   Mapped variable CanObjectDict_Rx_Map11 */
										INTEGER16 CanObjectDict_Rx_Map11;
                    ODCallback_t CanObjectDict_0x221C_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index221C[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map11 }
                     };			
										 
/* index 0x2202 :   Mapped variable CanObjectDict_Rx_Map12 */
										INTEGER16 CanObjectDict_Rx_Map12;
                    ODCallback_t CanObjectDict_0x2202_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2202[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map12 }
                     };
								 
/* index 0x22A2 :   Mapped variable CanObjectDict_Rx_Map52 */
										INTEGER16 CanObjectDict_Rx_Map52;
                    ODCallback_t CanObjectDict_0x22A2_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index22A2[] = 
                     {
                       { RO, int16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map52 }
                     };
										 										 																 
/* index 0x2300 :   Mapped variable MasterRxMap27 */
										UNS8 CanObjectDict_Rx_Map27 = 0;										 
                    ODCallback_t CanObjectDict_0x2300_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2300[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_Rx_Map27 }
                     };

/* index 0x2340 :   Mapped variable MasterTxMap11 */
                    ODCallback_t CanObjectDict_0x2340_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2340[] = 
                     {
                       { RW, int16, sizeof (INTEGER16), (void*)CanObjectDict_Tx_Map11 }
                     };
		
/* index 0x2341 :   Mapped variable MasterTxMap11 */
                    ODCallback_t CanObjectDict_0x2341_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2341[] = 
                     {
                       { RW, int32, sizeof (INTEGER32), (void*)CanObjectDict_Tx_Map11 }
                     };
		
/* index 0x2601 :   Mapped variable MasterRxMap22 */
										UNS16 CanObjectDict_Rx_Map22 = 0;
                    ODCallback_t CanObjectDict_0x2601_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2601[] = 
                     {
                       { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_Rx_Map22 }
                     };	

/* index 0x2602 :   Mapped variable MasterRxMap22 */
										UNS16 CanObjectDict_Rx_Map30 = 0;
                    ODCallback_t CanObjectDict_0x2602_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index2602[] = 
                     {
                       { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_Rx_Map30 }
                     };												 

/* index 0x603F :   Mapped variable MasterRxMap16 */
                    UNS16 CanObjectDict_Rx_Map46 = 0;           
                    ODCallback_t CanObjectDict_0x603F_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index603F[] = 
                     {
                       { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_Rx_Map46 }
                     };                     

/* index 0x6040 :   Mapped variable MasterTxMap10 */
					UNS16 CanObjectDict_Tx_Map10[MAX_NodeId_NUM];
                    ODCallback_t CanObjectDict_0x6040_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6040[] = 
                     {
                       { RW, uint16, sizeof (UNS16), (void*)CanObjectDict_Tx_Map10 }
                     };

/* index 0x6041 :   Mapped variable MasterRxMap14 */
										UNS16 CanObjectDict_Rx_Map14 = 0;
                    ODCallback_t CanObjectDict_0x6041_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6041[] = 
                     {
                       { RO, uint16, sizeof (UNS16), (void*)&CanObjectDict_Rx_Map14 }
                     };

/* index 0x6063 :   Mapped variable MasterRxMap19 */
		            INTEGER32 CanObjectDict_Rx_Map32 = 0;
                    ODCallback_t CanObjectDict_0x6063_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6063[] = 
                     {
                       { RO, uint8, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map32 }
                     };	
										 
/* index 0x6064 :   Mapped variable MasterRxMap26 */
		            INTEGER32 CanObjectDict_Rx_Map26[MAX_NodeId_NUM] ;
                    ODCallback_t CanObjectDict_0x6064_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6064[] = 
                     {
                       { RO, uint8, sizeof (INTEGER32), (void*)CanObjectDict_Rx_Map26 }
                     };	
										 
/* index 0x6069 :   Mapped variable MasterRxMap16 */
										INTEGER32 CanObjectDict_Rx_Map16 = 0;
                    ODCallback_t CanObjectDict_0x6069_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6069[] = 
                     {
                       { RO, int32, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map16 }
                     };

/* index 0x6061 :   Mapped variable MasterRxMap17 */
										UNS8 CanObjectDict_Rx_Map17 = 0;
                    ODCallback_t CanObjectDict_0x6061_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6061[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&CanObjectDict_Rx_Map17 }
                     };

/* index 0x606C :   Mapped variable MasterRxMap14 */
										INTEGER32 CanObjectDict_Rx_Map23 = 0;
                    ODCallback_t CanObjectDict_0x606C_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index606C[] = 
                     {
                       { RO, uint16, sizeof (INTEGER32), (void*)&CanObjectDict_Rx_Map23 }
                     };										 

/* index 0x6078 :   Mapped variable CanObjectDict_Rx_Map11 */
										INTEGER16 CanObjectDict_Rx_Map21;
                    ODCallback_t CanObjectDict_0x6078_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6078[] = 
                     {
                       { RW, uint16, sizeof (INTEGER16), (void*)&CanObjectDict_Rx_Map21 }
                     };										 

/* index 0x6079 :   Mapped variable MasterRxMap20 */
										UNS32 CanObjectDict_Rx_Map20 = 0;
                    ODCallback_t CanObjectDict_0x6079_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6079[] = 
                     {
                       { RW, uint32, sizeof (UNS32), (void*)&CanObjectDict_Rx_Map20 }
                     };		
		
/* index 0x607A :   Mapped variable MasterRxMap18 */
										INTEGER32 CanObjectDict_Tx_Map18[MAX_NodeId_NUM] ;
                    ODCallback_t CanObjectDict_0x607A_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index607A[] = 
                     {
                       { RO, uint8, sizeof (INTEGER32), (void*)&CanObjectDict_Tx_Map18 }
                     };		
										 
/* index 0x6081 :   Mapped variable MasterTxMap21 */
										INTEGER32 CanObjectDict_Tx_Map20[MAX_NodeId_NUM];
                    ODCallback_t CanObjectDict_0x6081_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6081[] = 
                     {
                       { RW, int32, sizeof (INTEGER32), (void*)CanObjectDict_Tx_Map20 }
                     };										 

/* index 0x6083 :   Mapped variable MasterRxMap19 */
										INTEGER32 CanObjectDict_Tx_Map19[MAX_NodeId_NUM] ;
                    ODCallback_t CanObjectDict_0x6083_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6083[] = 
                     {
                       { RO, uint8, sizeof (INTEGER32), (void*)&CanObjectDict_Tx_Map19 }
                     };				

/* index 0x6084 :   Mapped variable MasterTxMap22 */
										INTEGER32 CanObjectDict_Tx_Map21[MAX_NodeId_NUM];
                    ODCallback_t CanObjectDict_0x6084_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index6084[] = 
                     {
                       { RW, int32, sizeof (INTEGER32), (void*)CanObjectDict_Tx_Map21 }
                     };				

/* index 0x60FF :   Mapped variable MasterTxMap11 */
										INTEGER32 CanObjectDict_Tx_Map11[MAX_NodeId_NUM];
                    ODCallback_t CanObjectDict_0x60FF_callbacks[] = 
                     {
                       NULL
                     };
                    subindex CanObjectDict_Index60FF[] = 
                     {
                       { RW, int32, sizeof (INTEGER32), (void*)CanObjectDict_Tx_Map11 }
                     };
										 												 
/**************************************************************************/
/* Declaration of pointed variables                                       */
/**************************************************************************/

const indextable CanObjectDict_objdict[] = 
{
    { (subindex*)CanObjectDict_Index1000,CountOf(CanObjectDict_Index1000), 0x1000},
    { (subindex*)CanObjectDict_Index1001,CountOf(CanObjectDict_Index1001), 0x1001},
    { (subindex*)CanObjectDict_Index1002,CountOf(CanObjectDict_Index1002), 0x1002},
    { (subindex*)CanObjectDict_Index1003,CountOf(CanObjectDict_Index1003), 0x1003},
    { (subindex*)CanObjectDict_Index1005,CountOf(CanObjectDict_Index1005), 0x1005},
    { (subindex*)CanObjectDict_Index1006,CountOf(CanObjectDict_Index1006), 0x1006},
    { (subindex*)CanObjectDict_Index1008,CountOf(CanObjectDict_Index1008), 0x1008},
    { (subindex*)CanObjectDict_Index1009,CountOf(CanObjectDict_Index1009), 0x1009},
    { (subindex*)CanObjectDict_Index100A,CountOf(CanObjectDict_Index100A), 0x100A},
    { (subindex*)CanObjectDict_Index100C,CountOf(CanObjectDict_Index100C), 0x100C},
    { (subindex*)CanObjectDict_Index100D,CountOf(CanObjectDict_Index100D), 0x100D},
    { (subindex*)CanObjectDict_Index1010,CountOf(CanObjectDict_Index1010), 0x1010},
    { (subindex*)CanObjectDict_Index1013,CountOf(CanObjectDict_Index1013), 0x1013},
    { (subindex*)CanObjectDict_Index1014,CountOf(CanObjectDict_Index1014), 0x1014},
    { (subindex*)CanObjectDict_Index1015,CountOf(CanObjectDict_Index1015), 0x1015},
    { (subindex*)CanObjectDict_Index1016,CountOf(CanObjectDict_Index1016), 0x1016},
    { (subindex*)CanObjectDict_Index1017,CountOf(CanObjectDict_Index1017), 0x1017},
    { (subindex*)CanObjectDict_Index1018,CountOf(CanObjectDict_Index1018), 0x1018},
    { (subindex*)CanObjectDict_Index1200,CountOf(CanObjectDict_Index1200), 0x1200},
    { (subindex*)CanObjectDict_Index1201,CountOf(CanObjectDict_Index1201), 0x1201},
    { (subindex*)CanObjectDict_Index1202,CountOf(CanObjectDict_Index1202), 0x1202},
    { (subindex*)CanObjectDict_Index1203,CountOf(CanObjectDict_Index1203), 0x1203},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},    
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},      
    { (subindex*)CanObjectDict_Index1280,CountOf(CanObjectDict_Index1280), 0x1280},
    { (subindex*)CanObjectDict_Index1281,CountOf(CanObjectDict_Index1281), 0x1281},
    { (subindex*)CanObjectDict_Index1282,CountOf(CanObjectDict_Index1282), 0x1282},
    { (subindex*)CanObjectDict_Index1283,CountOf(CanObjectDict_Index1283), 0x1283},
    { (subindex*)CanObjectDict_Index1284,CountOf(CanObjectDict_Index1284), 0x1284},
    { (subindex*)CanObjectDict_Index1285,CountOf(CanObjectDict_Index1285), 0x1285},
    { (subindex*)CanObjectDict_Index1286,CountOf(CanObjectDict_Index1286), 0x1286},
    { (subindex*)CanObjectDict_Index1287,CountOf(CanObjectDict_Index1287), 0x1287},
    { (subindex*)CanObjectDict_Index1288,CountOf(CanObjectDict_Index1288), 0x1288},
    { (subindex*)CanObjectDict_Index1289,CountOf(CanObjectDict_Index1289), 0x1289},
    { (subindex*)CanObjectDict_Index128A,CountOf(CanObjectDict_Index128A), 0x128A},
    { (subindex*)CanObjectDict_Index128B,CountOf(CanObjectDict_Index128B), 0x128B},
    { (subindex*)CanObjectDict_Index128C,CountOf(CanObjectDict_Index128C), 0x128C},
    { (subindex*)CanObjectDict_Index128D,CountOf(CanObjectDict_Index128D), 0x128D},
    { (subindex*)CanObjectDict_Index128E,CountOf(CanObjectDict_Index128E), 0x128E},
    { (subindex*)CanObjectDict_Index128F,CountOf(CanObjectDict_Index128F), 0x128F},
    { (subindex*)CanObjectDict_Index1290,CountOf(CanObjectDict_Index1290), 0x1290},
    { (subindex*)CanObjectDict_Index1291,CountOf(CanObjectDict_Index1291), 0x1291},
    { (subindex*)CanObjectDict_Index1292,CountOf(CanObjectDict_Index1292), 0x1292},
    { (subindex*)CanObjectDict_Index1293,CountOf(CanObjectDict_Index1293), 0x1293},
    { (subindex*)CanObjectDict_Index1294,CountOf(CanObjectDict_Index1294), 0x1294},
    { (subindex*)CanObjectDict_Index1295,CountOf(CanObjectDict_Index1295), 0x1295},
    { (subindex*)CanObjectDict_Index1296,CountOf(CanObjectDict_Index1296), 0x1296},
    { (subindex*)CanObjectDict_Index1297,CountOf(CanObjectDict_Index1297), 0x1297},
    { (subindex*)CanObjectDict_Index1298,CountOf(CanObjectDict_Index1298), 0x1298},
    { (subindex*)CanObjectDict_Index1299,CountOf(CanObjectDict_Index1299), 0x1299},
    { (subindex*)CanObjectDict_Index129A,CountOf(CanObjectDict_Index129A), 0x129A},
    { (subindex*)CanObjectDict_Index129B,CountOf(CanObjectDict_Index129B), 0x129B},
    { (subindex*)CanObjectDict_Index129C,CountOf(CanObjectDict_Index129C), 0x129C},
    { (subindex*)CanObjectDict_Index129D,CountOf(CanObjectDict_Index129D), 0x129D},
    { (subindex*)CanObjectDict_Index129E,CountOf(CanObjectDict_Index129E), 0x129E},
    { (subindex*)CanObjectDict_Index129F,CountOf(CanObjectDict_Index129F), 0x129F},  
    { (subindex*)CanObjectDict_Index1400,CountOf(CanObjectDict_Index1400), 0x1400},
    { (subindex*)CanObjectDict_Index1401,CountOf(CanObjectDict_Index1401), 0x1401},
    { (subindex*)CanObjectDict_Index1402,CountOf(CanObjectDict_Index1402), 0x1402},
    { (subindex*)CanObjectDict_Index1403,CountOf(CanObjectDict_Index1403), 0x1403},
    { (subindex*)CanObjectDict_Index1404,CountOf(CanObjectDict_Index1404), 0x1404},
    { (subindex*)CanObjectDict_Index1405,CountOf(CanObjectDict_Index1405), 0x1405},
    { (subindex*)CanObjectDict_Index1406,CountOf(CanObjectDict_Index1406), 0x1406},
    { (subindex*)CanObjectDict_Index1407,CountOf(CanObjectDict_Index1407), 0x1407},
    { (subindex*)CanObjectDict_Index1408,CountOf(CanObjectDict_Index1408), 0x1408},
    { (subindex*)CanObjectDict_Index1409,CountOf(CanObjectDict_Index1409), 0x1409},
    { (subindex*)CanObjectDict_Index140A,CountOf(CanObjectDict_Index140A), 0x140A},
    { (subindex*)CanObjectDict_Index140B,CountOf(CanObjectDict_Index140B), 0x140B},
    { (subindex*)CanObjectDict_Index140C,CountOf(CanObjectDict_Index140C), 0x140C},
    { (subindex*)CanObjectDict_Index140D,CountOf(CanObjectDict_Index140D), 0x140D},
    { (subindex*)CanObjectDict_Index140E,CountOf(CanObjectDict_Index140E), 0x140E},
    { (subindex*)CanObjectDict_Index140F,CountOf(CanObjectDict_Index140F), 0x140F},
    { (subindex*)CanObjectDict_Index1410,CountOf(CanObjectDict_Index1410), 0x1410},
    { (subindex*)CanObjectDict_Index1411,CountOf(CanObjectDict_Index1411), 0x1411},
    { (subindex*)CanObjectDict_Index1412,CountOf(CanObjectDict_Index1412), 0x1412},
    { (subindex*)CanObjectDict_Index1413,CountOf(CanObjectDict_Index1413), 0x1413},
    { (subindex*)CanObjectDict_Index1414,CountOf(CanObjectDict_Index1414), 0x1414},
    { (subindex*)CanObjectDict_Index1415,CountOf(CanObjectDict_Index1415), 0x1415},
    { (subindex*)CanObjectDict_Index1416,CountOf(CanObjectDict_Index1416), 0x1416},
    { (subindex*)CanObjectDict_Index1417,CountOf(CanObjectDict_Index1417), 0x1417},
    { (subindex*)CanObjectDict_Index1418,CountOf(CanObjectDict_Index1418), 0x1418},
    { (subindex*)CanObjectDict_Index1419,CountOf(CanObjectDict_Index1419), 0x1419},
    { (subindex*)CanObjectDict_Index141A,CountOf(CanObjectDict_Index141A), 0x141A},
    { (subindex*)CanObjectDict_Index141B,CountOf(CanObjectDict_Index141B), 0x141B},
    { (subindex*)CanObjectDict_Index141C,CountOf(CanObjectDict_Index141C), 0x141C},
    { (subindex*)CanObjectDict_Index141D,CountOf(CanObjectDict_Index141D), 0x141D},
    { (subindex*)CanObjectDict_Index141E,CountOf(CanObjectDict_Index141E), 0x141E},
    { (subindex*)CanObjectDict_Index141F,CountOf(CanObjectDict_Index141F), 0x141F},
    { (subindex*)CanObjectDict_Index1420,CountOf(CanObjectDict_Index1420), 0x1420},
    { (subindex*)CanObjectDict_Index1421,CountOf(CanObjectDict_Index1421), 0x1421},
    { (subindex*)CanObjectDict_Index1422,CountOf(CanObjectDict_Index1422), 0x1422},
    { (subindex*)CanObjectDict_Index1423,CountOf(CanObjectDict_Index1423), 0x1423},
    { (subindex*)CanObjectDict_Index1424,CountOf(CanObjectDict_Index1424), 0x1424},
    { (subindex*)CanObjectDict_Index1425,CountOf(CanObjectDict_Index1425), 0x1425},
    { (subindex*)CanObjectDict_Index1426,CountOf(CanObjectDict_Index1426), 0x1426},
    { (subindex*)CanObjectDict_Index1427,CountOf(CanObjectDict_Index1427), 0x1427},
    { (subindex*)CanObjectDict_Index1428,CountOf(CanObjectDict_Index1428), 0x1428},
    { (subindex*)CanObjectDict_Index1429,CountOf(CanObjectDict_Index1429), 0x1429},
    { (subindex*)CanObjectDict_Index142A,CountOf(CanObjectDict_Index142A), 0x142A},
    { (subindex*)CanObjectDict_Index142B,CountOf(CanObjectDict_Index142B), 0x142B},
    { (subindex*)CanObjectDict_Index142C,CountOf(CanObjectDict_Index142C), 0x142C},
    { (subindex*)CanObjectDict_Index142D,CountOf(CanObjectDict_Index142D), 0x142D},
    { (subindex*)CanObjectDict_Index142E,CountOf(CanObjectDict_Index142E), 0x142E},
    { (subindex*)CanObjectDict_Index142F,CountOf(CanObjectDict_Index142F), 0x142F},
    { (subindex*)CanObjectDict_Index1430,CountOf(CanObjectDict_Index1430), 0x1430},
    { (subindex*)CanObjectDict_Index1431,CountOf(CanObjectDict_Index1431), 0x1431},
    { (subindex*)CanObjectDict_Index1432,CountOf(CanObjectDict_Index1432), 0x1432},
    { (subindex*)CanObjectDict_Index1433,CountOf(CanObjectDict_Index1433), 0x1433},
    { (subindex*)CanObjectDict_Index1434,CountOf(CanObjectDict_Index1434), 0x1434},
    { (subindex*)CanObjectDict_Index1435,CountOf(CanObjectDict_Index1435), 0x1435},
    { (subindex*)CanObjectDict_Index1436,CountOf(CanObjectDict_Index1436), 0x1436},
    { (subindex*)CanObjectDict_Index1437,CountOf(CanObjectDict_Index1437), 0x1437},
    { (subindex*)CanObjectDict_Index1438,CountOf(CanObjectDict_Index1438), 0x1438},
    { (subindex*)CanObjectDict_Index1439,CountOf(CanObjectDict_Index1439), 0x1439},
    { (subindex*)CanObjectDict_Index143A,CountOf(CanObjectDict_Index143A), 0x143A},
    { (subindex*)CanObjectDict_Index143B,CountOf(CanObjectDict_Index143B), 0x143B},
    { (subindex*)CanObjectDict_Index143C,CountOf(CanObjectDict_Index143C), 0x143C},
    { (subindex*)CanObjectDict_Index143D,CountOf(CanObjectDict_Index143D), 0x143D},
    { (subindex*)CanObjectDict_Index143E,CountOf(CanObjectDict_Index143E), 0x143E},
    { (subindex*)CanObjectDict_Index143F,CountOf(CanObjectDict_Index143F), 0x143F},
    { (subindex*)CanObjectDict_Index1440,CountOf(CanObjectDict_Index1440), 0x1440},
    { (subindex*)CanObjectDict_Index1441,CountOf(CanObjectDict_Index1441), 0x1441},
    { (subindex*)CanObjectDict_Index1442,CountOf(CanObjectDict_Index1442), 0x1442},
    { (subindex*)CanObjectDict_Index1443,CountOf(CanObjectDict_Index1443), 0x1443},
    { (subindex*)CanObjectDict_Index1444,CountOf(CanObjectDict_Index1444), 0x1444},
    { (subindex*)CanObjectDict_Index1445,CountOf(CanObjectDict_Index1445), 0x1445},
    { (subindex*)CanObjectDict_Index1446,CountOf(CanObjectDict_Index1446), 0x1446},
    { (subindex*)CanObjectDict_Index1447,CountOf(CanObjectDict_Index1447), 0x1447},
    { (subindex*)CanObjectDict_Index1448,CountOf(CanObjectDict_Index1448), 0x1448},
    { (subindex*)CanObjectDict_Index1449,CountOf(CanObjectDict_Index1449), 0x1449},
    { (subindex*)CanObjectDict_Index144A,CountOf(CanObjectDict_Index144A), 0x144A},
    { (subindex*)CanObjectDict_Index144B,CountOf(CanObjectDict_Index144B), 0x144B},
    { (subindex*)CanObjectDict_Index144C,CountOf(CanObjectDict_Index144C), 0x144C},
    { (subindex*)CanObjectDict_Index144D,CountOf(CanObjectDict_Index144D), 0x144D},
    { (subindex*)CanObjectDict_Index144E,CountOf(CanObjectDict_Index144E), 0x144E},
    { (subindex*)CanObjectDict_Index144F,CountOf(CanObjectDict_Index144F), 0x144F},  
    { (subindex*)CanObjectDict_Index1450,CountOf(CanObjectDict_Index1450), 0x1450},
    { (subindex*)CanObjectDict_Index1451,CountOf(CanObjectDict_Index1451), 0x1451},
    { (subindex*)CanObjectDict_Index1452,CountOf(CanObjectDict_Index1452), 0x1452},
    { (subindex*)CanObjectDict_Index1453,CountOf(CanObjectDict_Index1453), 0x1453},
    { (subindex*)CanObjectDict_Index1454,CountOf(CanObjectDict_Index1454), 0x1454},
    { (subindex*)CanObjectDict_Index1455,CountOf(CanObjectDict_Index1455), 0x1455},
    { (subindex*)CanObjectDict_Index1456,CountOf(CanObjectDict_Index1456), 0x1456},
    { (subindex*)CanObjectDict_Index1457,CountOf(CanObjectDict_Index1457), 0x1457},
    { (subindex*)CanObjectDict_Index1458,CountOf(CanObjectDict_Index1458), 0x1458},
    { (subindex*)CanObjectDict_Index1459,CountOf(CanObjectDict_Index1459), 0x1459},
    { (subindex*)CanObjectDict_Index145A,CountOf(CanObjectDict_Index145A), 0x145A},
    { (subindex*)CanObjectDict_Index145B,CountOf(CanObjectDict_Index145B), 0x145B},
    { (subindex*)CanObjectDict_Index145C,CountOf(CanObjectDict_Index145C), 0x145C},
    { (subindex*)CanObjectDict_Index145D,CountOf(CanObjectDict_Index145D), 0x145D},
    { (subindex*)CanObjectDict_Index145E,CountOf(CanObjectDict_Index145E), 0x145E},
    { (subindex*)CanObjectDict_Index145F,CountOf(CanObjectDict_Index145F), 0x145F},    
    { (subindex*)CanObjectDict_Index1460,CountOf(CanObjectDict_Index1460), 0x1460},
    { (subindex*)CanObjectDict_Index1461,CountOf(CanObjectDict_Index1461), 0x1461},
    { (subindex*)CanObjectDict_Index1462,CountOf(CanObjectDict_Index1462), 0x1462},
    { (subindex*)CanObjectDict_Index1463,CountOf(CanObjectDict_Index1463), 0x1463},
    { (subindex*)CanObjectDict_Index1464,CountOf(CanObjectDict_Index1464), 0x1464},
    { (subindex*)CanObjectDict_Index1465,CountOf(CanObjectDict_Index1465), 0x1465},
    { (subindex*)CanObjectDict_Index1466,CountOf(CanObjectDict_Index1466), 0x1466},
    { (subindex*)CanObjectDict_Index1467,CountOf(CanObjectDict_Index1467), 0x1467},
    { (subindex*)CanObjectDict_Index1468,CountOf(CanObjectDict_Index1468), 0x1468},
    { (subindex*)CanObjectDict_Index1469,CountOf(CanObjectDict_Index1469), 0x1469},
    { (subindex*)CanObjectDict_Index146A,CountOf(CanObjectDict_Index146A), 0x146A},
    { (subindex*)CanObjectDict_Index146B,CountOf(CanObjectDict_Index146B), 0x146B},
    { (subindex*)CanObjectDict_Index146C,CountOf(CanObjectDict_Index146C), 0x146C},
    { (subindex*)CanObjectDict_Index146D,CountOf(CanObjectDict_Index146D), 0x146D},
    { (subindex*)CanObjectDict_Index146E,CountOf(CanObjectDict_Index146E), 0x146E},
    { (subindex*)CanObjectDict_Index146F,CountOf(CanObjectDict_Index146F), 0x146F},   
    { (subindex*)CanObjectDict_Index1470,CountOf(CanObjectDict_Index1470), 0x1470},
    { (subindex*)CanObjectDict_Index1471,CountOf(CanObjectDict_Index1471), 0x1471},
    { (subindex*)CanObjectDict_Index1472,CountOf(CanObjectDict_Index1472), 0x1472},
    { (subindex*)CanObjectDict_Index1473,CountOf(CanObjectDict_Index1473), 0x1473},
    { (subindex*)CanObjectDict_Index1474,CountOf(CanObjectDict_Index1474), 0x1474},
    { (subindex*)CanObjectDict_Index1475,CountOf(CanObjectDict_Index1475), 0x1475},
    { (subindex*)CanObjectDict_Index1476,CountOf(CanObjectDict_Index1476), 0x1476},
    { (subindex*)CanObjectDict_Index1477,CountOf(CanObjectDict_Index1477), 0x1477},
    { (subindex*)CanObjectDict_Index1478,CountOf(CanObjectDict_Index1478), 0x1478},
    { (subindex*)CanObjectDict_Index1479,CountOf(CanObjectDict_Index1479), 0x1479},
    { (subindex*)CanObjectDict_Index147A,CountOf(CanObjectDict_Index147A), 0x147A},
    { (subindex*)CanObjectDict_Index147B,CountOf(CanObjectDict_Index147B), 0x147B},
    { (subindex*)CanObjectDict_Index147C,CountOf(CanObjectDict_Index147C), 0x147C},
    { (subindex*)CanObjectDict_Index147D,CountOf(CanObjectDict_Index147D), 0x147D},
    { (subindex*)CanObjectDict_Index147E,CountOf(CanObjectDict_Index147E), 0x147E},
    { (subindex*)CanObjectDict_Index147F,CountOf(CanObjectDict_Index147F), 0x147F},    
		
#if (SVO_Type == 0)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器		5：Copley驱动器			 		 			

    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1600},      		
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1601},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1602},	
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1603},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1604},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1605},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1606},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1607},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1608},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1609},					
		{ (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x160A},
		{ (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x160B},		
		{ (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x160C},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x160D},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x160E},		
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x160F},   
		{ (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1610},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1611},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1612},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1613},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1614},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1615},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1616},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1617},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1618},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x1619},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x161A},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x161B},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x161C},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x161D},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x161E},
    { (subindex*)CanObjectDict_TPDO1_Map_Kinco_index,CountOf(CanObjectDict_TPDO1_Map_Kinco_index), 0x161F},  
		
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1620},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1621},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1622},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1623},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1624},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1625},		
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1626},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1627},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1628},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1629},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x162A},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x162B},	
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x162C},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x162D},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x162E},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x162F},		
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1630},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1631},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1632},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1633},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1634},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1635},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1636},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1637},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1638},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x1639},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x163A},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x163B},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x163C},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x163D},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x163E},
    { (subindex*)CanObjectDict_TPDO2_Map_Kinco_index,CountOf(CanObjectDict_TPDO2_Map_Kinco_index), 0x163F}, 
		
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1640},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1641},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1642},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1643},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1644},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1645},		
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1646},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1647},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1648},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1649},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x164A},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x164B},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x164C},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x164D},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x164E},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x164F},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1650},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1651},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1652},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1653},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1654},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1655},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1656},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1657},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1658},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x1659},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x165A},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x165B},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x165C},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x165D},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x165E},
    { (subindex*)CanObjectDict_TPDO3_Map_Kinco_index,CountOf(CanObjectDict_TPDO3_Map_Kinco_index), 0x165F},  
		
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1660},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1661},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1662},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1663},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1664},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1665},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1666},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1667},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1668},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1669},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x166A},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x166B},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x166C},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x166D},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x166E},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x166F},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1670},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1671},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1672},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1673},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1674},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1675},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1676},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1677},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1678},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x1679},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x167A},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x167B},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x167C},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x167D},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x167E},
    { (subindex*)CanObjectDict_TPDO4_Map_Kinco_index,CountOf(CanObjectDict_TPDO4_Map_Kinco_index), 0x167F},  		
	
#elif (SVO_Type == 1)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器		5：Copley驱动器			 			
		
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1600},      		
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1601},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1602},	
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1603},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1604},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1605},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1606},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1607},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1608},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1609},					
		{ (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x160A},
		{ (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x160B},		
		{ (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x160C},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x160D},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x160E},		
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x160F},   
		{ (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1610},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1611},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1612},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1613},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1614},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1615},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1616},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1617},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1618},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x1619},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x161A},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x161B},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x161C},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x161D},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x161E},
    { (subindex*)CanObjectDict_TPDO1_Map_Delta_index,CountOf(CanObjectDict_TPDO1_Map_Delta_index), 0x161F},  
		
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1620},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1621},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1622},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1623},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1624},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1625},		
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1626},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1627},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1628},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1629},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x162A},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x162B},	
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x162C},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x162D},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x162E},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x162F},		
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1630},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1631},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1632},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1633},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1634},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1635},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1636},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1637},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1638},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x1639},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x163A},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x163B},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x163C},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x163D},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x163E},
    { (subindex*)CanObjectDict_TPDO2_Map_Delta_index,CountOf(CanObjectDict_TPDO2_Map_Delta_index), 0x163F}, 
		
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1640},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1641},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1642},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1643},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1644},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1645},		
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1646},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1647},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1648},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1649},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x164A},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x164B},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x164C},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x164D},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x164E},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x164F},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1650},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1651},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1652},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1653},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1654},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1655},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1656},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1657},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1658},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x1659},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x165A},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x165B},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x165C},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x165D},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x165E},
    { (subindex*)CanObjectDict_TPDO3_Map_Delta_index,CountOf(CanObjectDict_TPDO3_Map_Delta_index), 0x165F},  
		
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1660},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1661},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1662},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1663},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1664},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1665},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1666},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1667},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1668},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1669},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x166A},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x166B},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x166C},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x166D},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x166E},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x166F},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1670},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1671},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1672},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1673},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1674},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1675},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1676},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1677},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1678},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x1679},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x167A},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x167B},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x167C},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x167D},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x167E},
    { (subindex*)CanObjectDict_TPDO4_Map_Delta_index,CountOf(CanObjectDict_TPDO4_Map_Delta_index), 0x167F},  
		
#elif (SVO_Type == 2)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器		5：Copley驱动器			 			
		
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1600},      		
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1601},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1602},	
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1603},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1604},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1605},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1606},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1607},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1608},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1609},					
		{ (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x160A},
		{ (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x160B},		
		{ (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x160C},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x160D},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x160E},		
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x160F},   
		{ (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1610},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1611},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1612},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1613},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1614},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1615},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1616},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1617},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1618},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x1619},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x161A},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x161B},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x161C},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x161D},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x161E},
    { (subindex*)CanObjectDict_TPDO1_Map_Windcon_index,CountOf(CanObjectDict_TPDO1_Map_Windcon_index), 0x161F},  
		
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1620},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1621},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1622},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1623},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1624},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1625},		
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1626},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1627},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1628},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1629},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x162A},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x162B},	
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x162C},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x162D},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x162E},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x162F},		
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1630},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1631},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1632},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1633},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1634},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1635},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1636},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1637},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1638},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x1639},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x163A},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x163B},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x163C},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x163D},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x163E},
    { (subindex*)CanObjectDict_TPDO2_Map_Windcon_index,CountOf(CanObjectDict_TPDO2_Map_Windcon_index), 0x163F}, 
		
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1640},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1641},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1642},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1643},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1644},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1645},		
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1646},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1647},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1648},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1649},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x164A},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x164B},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x164C},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x164D},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x164E},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x164F},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1650},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1651},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1652},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1653},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1654},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1655},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1656},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1657},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1658},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x1659},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x165A},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x165B},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x165C},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x165D},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x165E},
    { (subindex*)CanObjectDict_TPDO3_Map_Windcon_index,CountOf(CanObjectDict_TPDO3_Map_Windcon_index), 0x165F},  
		
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1660},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1661},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1662},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1663},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1664},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1665},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1666},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1667},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1668},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1669},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x166A},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x166B},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x166C},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x166D},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x166E},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x166F},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1670},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1671},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1672},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1673},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1674},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1675},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1676},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1677},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1678},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x1679},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x167A},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x167B},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x167C},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x167D},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x167E},
    { (subindex*)CanObjectDict_TPDO4_Map_Windcon_index,CountOf(CanObjectDict_TPDO4_Map_Windcon_index), 0x167F},  
		
#elif (SVO_Type == 3)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器		5：Copley驱动器			 			
		
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1600},      		
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1601},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1602},	
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1603},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1604},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1605},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1606},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1607},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1608},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1609},					
		{ (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x160A},
		{ (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x160B},		
		{ (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x160C},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x160D},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x160E},		
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x160F},   
		{ (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1610},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1611},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1612},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1613},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1614},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1615},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1616},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1617},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1618},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x1619},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x161A},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x161B},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x161C},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x161D},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x161E},
    { (subindex*)CanObjectDict_TPDO1_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO1_Map_Wheatstone_index), 0x161F},  
		
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1620},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1621},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1622},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1623},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1624},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1625},		
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1626},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1627},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1628},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1629},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x162A},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x162B},	
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x162C},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x162D},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x162E},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x162F},		
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1630},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1631},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1632},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1633},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1634},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1635},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1636},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1637},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1638},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x1639},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x163A},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x163B},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x163C},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x163D},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x163E},
    { (subindex*)CanObjectDict_TPDO2_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO2_Map_Wheatstone_index), 0x163F}, 
		
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1640},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1641},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1642},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1643},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1644},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1645},		
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1646},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1647},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1648},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1649},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x164A},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x164B},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x164C},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x164D},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x164E},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x164F},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1650},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1651},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1652},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1653},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1654},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1655},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1656},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1657},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1658},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x1659},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x165A},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x165B},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x165C},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x165D},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x165E},
    { (subindex*)CanObjectDict_TPDO3_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO3_Map_Wheatstone_index), 0x165F},  
		
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1660},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1661},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1662},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1663},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1664},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1665},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1666},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1667},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1668},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1669},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x166A},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x166B},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x166C},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x166D},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x166E},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x166F},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1670},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1671},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1672},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1673},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1674},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1675},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1676},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1677},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1678},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x1679},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x167A},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x167B},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x167C},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x167D},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x167E},
    { (subindex*)CanObjectDict_TPDO4_Map_Wheatstone_index,CountOf(CanObjectDict_TPDO4_Map_Wheatstone_index), 0x167F},  		
		
#elif (SVO_Type == 4)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器		5：Copley驱动器			 			
		
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1600},      		
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1601},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1602},	
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1603},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1604},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1605},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1606},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1607},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1608},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1609},					
		{ (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x160A},
		{ (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x160B},		
		{ (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x160C},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x160D},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x160E},		
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x160F},   
		{ (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1610},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1611},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1612},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1613},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1614},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1615},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1616},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1617},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1618},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x1619},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x161A},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x161B},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x161C},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x161D},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x161E},
    { (subindex*)CanObjectDict_TPDO1_Map_Keya_index,CountOf(CanObjectDict_TPDO1_Map_Keya_index), 0x161F},  
		
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1620},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1621},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1622},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1623},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1624},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1625},		
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1626},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1627},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1628},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1629},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x162A},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x162B},	
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x162C},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x162D},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x162E},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x162F},		
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1630},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1631},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1632},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1633},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1634},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1635},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1636},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1637},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1638},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x1639},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x163A},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x163B},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x163C},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x163D},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x163E},
    { (subindex*)CanObjectDict_TPDO2_Map_Keya_index,CountOf(CanObjectDict_TPDO2_Map_Keya_index), 0x163F}, 
		
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1640},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1641},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1642},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1643},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1644},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1645},		
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1646},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1647},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1648},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1649},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x164A},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x164B},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x164C},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x164D},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x164E},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x164F},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1650},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1651},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1652},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1653},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1654},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1655},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1656},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1657},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1658},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x1659},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x165A},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x165B},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x165C},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x165D},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x165E},
    { (subindex*)CanObjectDict_TPDO3_Map_Keya_index,CountOf(CanObjectDict_TPDO3_Map_Keya_index), 0x165F},  
		
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1660},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1661},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1662},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1663},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1664},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1665},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1666},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1667},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1668},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1669},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x166A},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x166B},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x166C},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x166D},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x166E},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x166F},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1670},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1671},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1672},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1673},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1674},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1675},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1676},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1677},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1678},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x1679},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x167A},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x167B},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x167C},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x167D},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x167E},
    { (subindex*)CanObjectDict_TPDO4_Map_Keya_index,CountOf(CanObjectDict_TPDO4_Map_Keya_index), 0x167F},  		
		
#elif (SVO_Type == 5)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器		5：Copley驱动器			 			
		
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1600},      		
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1601},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1602},	
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1603},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1604},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1605},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1606},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1607},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1608},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1609},					
		{ (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x160A},
		{ (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x160B},		
		{ (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x160C},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x160D},	
		{ (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x160E},		
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x160F},   
		{ (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1610},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1611},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1612},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1613},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1614},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1615},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1616},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1617},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1618},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x1619},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x161A},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x161B},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x161C},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x161D},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x161E},
    { (subindex*)CanObjectDict_TPDO1_Map_Copley_index,CountOf(CanObjectDict_TPDO1_Map_Copley_index), 0x161F},  
		
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1620},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1621},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1622},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1623},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1624},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1625},		
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1626},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1627},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1628},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1629},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x162A},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x162B},	
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x162C},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x162D},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x162E},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x162F},		
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1630},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1631},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1632},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1633},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1634},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1635},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1636},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1637},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1638},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x1639},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x163A},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x163B},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x163C},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x163D},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x163E},
    { (subindex*)CanObjectDict_TPDO2_Map_Copley_index,CountOf(CanObjectDict_TPDO2_Map_Copley_index), 0x163F}, 
		
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1640},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1641},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1642},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1643},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1644},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1645},		
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1646},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1647},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1648},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1649},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x164A},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x164B},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x164C},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x164D},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x164E},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x164F},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1650},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1651},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1652},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1653},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1654},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1655},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1656},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1657},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1658},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x1659},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x165A},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x165B},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x165C},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x165D},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x165E},
    { (subindex*)CanObjectDict_TPDO3_Map_Copley_index,CountOf(CanObjectDict_TPDO3_Map_Copley_index), 0x165F},  
		
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1660},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1661},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1662},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1663},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1664},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1665},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1666},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1667},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1668},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1669},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x166A},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x166B},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x166C},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x166D},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x166E},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x166F},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1670},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1671},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1672},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1673},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1674},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1675},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1676},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1677},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1678},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x1679},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x167A},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x167B},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x167C},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x167D},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x167E},
    { (subindex*)CanObjectDict_TPDO4_Map_Copley_index,CountOf(CanObjectDict_TPDO4_Map_Copley_index), 0x167F},  		
		
#endif	

    { (subindex*)CanObjectDict_Index1800,CountOf(CanObjectDict_Index1800), 0x1800},
    { (subindex*)CanObjectDict_Index1801,CountOf(CanObjectDict_Index1801), 0x1801},
    { (subindex*)CanObjectDict_Index1802,CountOf(CanObjectDict_Index1802), 0x1802},
    { (subindex*)CanObjectDict_Index1803,CountOf(CanObjectDict_Index1803), 0x1803},
    { (subindex*)CanObjectDict_Index1804,CountOf(CanObjectDict_Index1804), 0x1804},
    { (subindex*)CanObjectDict_Index1805,CountOf(CanObjectDict_Index1805), 0x1805},
    { (subindex*)CanObjectDict_Index1806,CountOf(CanObjectDict_Index1806), 0x1806},
    { (subindex*)CanObjectDict_Index1807,CountOf(CanObjectDict_Index1807), 0x1807},
    { (subindex*)CanObjectDict_Index1808,CountOf(CanObjectDict_Index1808), 0x1808},
    { (subindex*)CanObjectDict_Index1809,CountOf(CanObjectDict_Index1809), 0x1809},
    { (subindex*)CanObjectDict_Index180A,CountOf(CanObjectDict_Index180A), 0x180A},
    { (subindex*)CanObjectDict_Index180B,CountOf(CanObjectDict_Index180B), 0x180B},
    { (subindex*)CanObjectDict_Index180C,CountOf(CanObjectDict_Index180C), 0x180C},
    { (subindex*)CanObjectDict_Index180D,CountOf(CanObjectDict_Index180D), 0x180D},
    { (subindex*)CanObjectDict_Index180E,CountOf(CanObjectDict_Index180E), 0x180E},
    { (subindex*)CanObjectDict_Index180F,CountOf(CanObjectDict_Index180F), 0x180F},
    { (subindex*)CanObjectDict_Index1810,CountOf(CanObjectDict_Index1810), 0x1810},
    { (subindex*)CanObjectDict_Index1811,CountOf(CanObjectDict_Index1811), 0x1811},
    { (subindex*)CanObjectDict_Index1812,CountOf(CanObjectDict_Index1812), 0x1812},
    { (subindex*)CanObjectDict_Index1813,CountOf(CanObjectDict_Index1813), 0x1813},
    { (subindex*)CanObjectDict_Index1814,CountOf(CanObjectDict_Index1814), 0x1814},
    { (subindex*)CanObjectDict_Index1815,CountOf(CanObjectDict_Index1815), 0x1815},
    { (subindex*)CanObjectDict_Index1816,CountOf(CanObjectDict_Index1816), 0x1816},
    { (subindex*)CanObjectDict_Index1817,CountOf(CanObjectDict_Index1817), 0x1817},
    { (subindex*)CanObjectDict_Index1818,CountOf(CanObjectDict_Index1818), 0x1818},
    { (subindex*)CanObjectDict_Index1819,CountOf(CanObjectDict_Index1819), 0x1819},
    { (subindex*)CanObjectDict_Index181A,CountOf(CanObjectDict_Index181A), 0x181A},
    { (subindex*)CanObjectDict_Index181B,CountOf(CanObjectDict_Index181B), 0x181B},
    { (subindex*)CanObjectDict_Index181C,CountOf(CanObjectDict_Index181C), 0x181C},
    { (subindex*)CanObjectDict_Index181D,CountOf(CanObjectDict_Index181D), 0x181D},
    { (subindex*)CanObjectDict_Index181E,CountOf(CanObjectDict_Index181E), 0x181E},
    { (subindex*)CanObjectDict_Index181F,CountOf(CanObjectDict_Index181F), 0x181F},
    { (subindex*)CanObjectDict_Index1820,CountOf(CanObjectDict_Index1820), 0x1820},
    { (subindex*)CanObjectDict_Index1821,CountOf(CanObjectDict_Index1821), 0x1821},
    { (subindex*)CanObjectDict_Index1822,CountOf(CanObjectDict_Index1822), 0x1822},
    { (subindex*)CanObjectDict_Index1823,CountOf(CanObjectDict_Index1823), 0x1823},
    { (subindex*)CanObjectDict_Index1824,CountOf(CanObjectDict_Index1824), 0x1824},
    { (subindex*)CanObjectDict_Index1825,CountOf(CanObjectDict_Index1825), 0x1825},
    { (subindex*)CanObjectDict_Index1826,CountOf(CanObjectDict_Index1826), 0x1826},
    { (subindex*)CanObjectDict_Index1827,CountOf(CanObjectDict_Index1827), 0x1827},
    { (subindex*)CanObjectDict_Index1828,CountOf(CanObjectDict_Index1828), 0x1828},
    { (subindex*)CanObjectDict_Index1829,CountOf(CanObjectDict_Index1829), 0x1829},
    { (subindex*)CanObjectDict_Index182A,CountOf(CanObjectDict_Index182A), 0x182A},
    { (subindex*)CanObjectDict_Index182B,CountOf(CanObjectDict_Index182B), 0x182B},
    { (subindex*)CanObjectDict_Index182C,CountOf(CanObjectDict_Index182C), 0x182C},
    { (subindex*)CanObjectDict_Index182D,CountOf(CanObjectDict_Index182D), 0x182D},
    { (subindex*)CanObjectDict_Index182E,CountOf(CanObjectDict_Index182E), 0x182E},
    { (subindex*)CanObjectDict_Index182F,CountOf(CanObjectDict_Index182F), 0x182F},  
    { (subindex*)CanObjectDict_Index1830,CountOf(CanObjectDict_Index1830), 0x1830},
    { (subindex*)CanObjectDict_Index1831,CountOf(CanObjectDict_Index1831), 0x1831},
    { (subindex*)CanObjectDict_Index1832,CountOf(CanObjectDict_Index1832), 0x1832},
    { (subindex*)CanObjectDict_Index1833,CountOf(CanObjectDict_Index1833), 0x1833},
    { (subindex*)CanObjectDict_Index1834,CountOf(CanObjectDict_Index1834), 0x1834},
    { (subindex*)CanObjectDict_Index1835,CountOf(CanObjectDict_Index1835), 0x1835},
    { (subindex*)CanObjectDict_Index1836,CountOf(CanObjectDict_Index1836), 0x1836},
    { (subindex*)CanObjectDict_Index1837,CountOf(CanObjectDict_Index1837), 0x1837},
    { (subindex*)CanObjectDict_Index1838,CountOf(CanObjectDict_Index1838), 0x1838},
    { (subindex*)CanObjectDict_Index1839,CountOf(CanObjectDict_Index1839), 0x1839},
    { (subindex*)CanObjectDict_Index183A,CountOf(CanObjectDict_Index183A), 0x183A},
    { (subindex*)CanObjectDict_Index183B,CountOf(CanObjectDict_Index183B), 0x183B},
    { (subindex*)CanObjectDict_Index183C,CountOf(CanObjectDict_Index183C), 0x183C},
    { (subindex*)CanObjectDict_Index183D,CountOf(CanObjectDict_Index183D), 0x183D},
    { (subindex*)CanObjectDict_Index183E,CountOf(CanObjectDict_Index183E), 0x183E},
    { (subindex*)CanObjectDict_Index183F,CountOf(CanObjectDict_Index183F), 0x183F},   
    { (subindex*)CanObjectDict_Index1840,CountOf(CanObjectDict_Index1840), 0x1840},
    { (subindex*)CanObjectDict_Index1841,CountOf(CanObjectDict_Index1841), 0x1841},
    { (subindex*)CanObjectDict_Index1842,CountOf(CanObjectDict_Index1842), 0x1842},
    { (subindex*)CanObjectDict_Index1843,CountOf(CanObjectDict_Index1843), 0x1843},
    { (subindex*)CanObjectDict_Index1844,CountOf(CanObjectDict_Index1844), 0x1844},
    { (subindex*)CanObjectDict_Index1845,CountOf(CanObjectDict_Index1845), 0x1845},
    { (subindex*)CanObjectDict_Index1846,CountOf(CanObjectDict_Index1846), 0x1846},
    { (subindex*)CanObjectDict_Index1847,CountOf(CanObjectDict_Index1847), 0x1847},
    { (subindex*)CanObjectDict_Index1848,CountOf(CanObjectDict_Index1848), 0x1848},
    { (subindex*)CanObjectDict_Index1849,CountOf(CanObjectDict_Index1849), 0x1849},
    { (subindex*)CanObjectDict_Index184A,CountOf(CanObjectDict_Index184A), 0x184A},
    { (subindex*)CanObjectDict_Index184B,CountOf(CanObjectDict_Index184B), 0x184B},
    { (subindex*)CanObjectDict_Index184C,CountOf(CanObjectDict_Index184C), 0x184C},
    { (subindex*)CanObjectDict_Index184D,CountOf(CanObjectDict_Index184D), 0x184D},
    { (subindex*)CanObjectDict_Index184E,CountOf(CanObjectDict_Index184E), 0x184E},
    { (subindex*)CanObjectDict_Index184F,CountOf(CanObjectDict_Index184F), 0x184F},     
    { (subindex*)CanObjectDict_Index1850,CountOf(CanObjectDict_Index1850), 0x1850},
    { (subindex*)CanObjectDict_Index1851,CountOf(CanObjectDict_Index1851), 0x1851},
    { (subindex*)CanObjectDict_Index1852,CountOf(CanObjectDict_Index1852), 0x1852},
    { (subindex*)CanObjectDict_Index1853,CountOf(CanObjectDict_Index1853), 0x1853},
    { (subindex*)CanObjectDict_Index1854,CountOf(CanObjectDict_Index1854), 0x1854},
    { (subindex*)CanObjectDict_Index1855,CountOf(CanObjectDict_Index1855), 0x1855},
    { (subindex*)CanObjectDict_Index1856,CountOf(CanObjectDict_Index1856), 0x1856},
    { (subindex*)CanObjectDict_Index1857,CountOf(CanObjectDict_Index1857), 0x1857},
    { (subindex*)CanObjectDict_Index1858,CountOf(CanObjectDict_Index1858), 0x1858},
    { (subindex*)CanObjectDict_Index1859,CountOf(CanObjectDict_Index1859), 0x1859},
    { (subindex*)CanObjectDict_Index185A,CountOf(CanObjectDict_Index185A), 0x185A},
    { (subindex*)CanObjectDict_Index185B,CountOf(CanObjectDict_Index185B), 0x185B},
    { (subindex*)CanObjectDict_Index185C,CountOf(CanObjectDict_Index185C), 0x185C},
    { (subindex*)CanObjectDict_Index185D,CountOf(CanObjectDict_Index185D), 0x185D},
    { (subindex*)CanObjectDict_Index185E,CountOf(CanObjectDict_Index185E), 0x185E},
    { (subindex*)CanObjectDict_Index185F,CountOf(CanObjectDict_Index185F), 0x185F},  
    { (subindex*)CanObjectDict_Index1860,CountOf(CanObjectDict_Index1860), 0x1860},
    { (subindex*)CanObjectDict_Index1861,CountOf(CanObjectDict_Index1861), 0x1861},
    { (subindex*)CanObjectDict_Index1862,CountOf(CanObjectDict_Index1862), 0x1862},
    { (subindex*)CanObjectDict_Index1863,CountOf(CanObjectDict_Index1863), 0x1863},
    { (subindex*)CanObjectDict_Index1864,CountOf(CanObjectDict_Index1864), 0x1864},
    { (subindex*)CanObjectDict_Index1865,CountOf(CanObjectDict_Index1865), 0x1865},
    { (subindex*)CanObjectDict_Index1866,CountOf(CanObjectDict_Index1866), 0x1866},
    { (subindex*)CanObjectDict_Index1867,CountOf(CanObjectDict_Index1867), 0x1867},
    { (subindex*)CanObjectDict_Index1868,CountOf(CanObjectDict_Index1868), 0x1868},
    { (subindex*)CanObjectDict_Index1869,CountOf(CanObjectDict_Index1869), 0x1869},
    { (subindex*)CanObjectDict_Index186A,CountOf(CanObjectDict_Index186A), 0x186A},
    { (subindex*)CanObjectDict_Index186B,CountOf(CanObjectDict_Index186B), 0x186B},
    { (subindex*)CanObjectDict_Index186C,CountOf(CanObjectDict_Index186C), 0x186C},
    { (subindex*)CanObjectDict_Index186D,CountOf(CanObjectDict_Index186D), 0x186D},
    { (subindex*)CanObjectDict_Index186E,CountOf(CanObjectDict_Index186E), 0x186E},
    { (subindex*)CanObjectDict_Index186F,CountOf(CanObjectDict_Index186F), 0x186F}, 
    { (subindex*)CanObjectDict_Index1870,CountOf(CanObjectDict_Index1870), 0x1870},
    { (subindex*)CanObjectDict_Index1871,CountOf(CanObjectDict_Index1871), 0x1871},
    { (subindex*)CanObjectDict_Index1872,CountOf(CanObjectDict_Index1872), 0x1872},
    { (subindex*)CanObjectDict_Index1873,CountOf(CanObjectDict_Index1873), 0x1873},
    { (subindex*)CanObjectDict_Index1874,CountOf(CanObjectDict_Index1874), 0x1874},
    { (subindex*)CanObjectDict_Index1875,CountOf(CanObjectDict_Index1875), 0x1875},
    { (subindex*)CanObjectDict_Index1876,CountOf(CanObjectDict_Index1876), 0x1876},
    { (subindex*)CanObjectDict_Index1877,CountOf(CanObjectDict_Index1877), 0x1877},
    { (subindex*)CanObjectDict_Index1878,CountOf(CanObjectDict_Index1878), 0x1878},
    { (subindex*)CanObjectDict_Index1879,CountOf(CanObjectDict_Index1879), 0x1879},
    { (subindex*)CanObjectDict_Index187A,CountOf(CanObjectDict_Index187A), 0x187A},
    { (subindex*)CanObjectDict_Index187B,CountOf(CanObjectDict_Index187B), 0x187B},
    { (subindex*)CanObjectDict_Index187C,CountOf(CanObjectDict_Index187C), 0x187C},
    { (subindex*)CanObjectDict_Index187D,CountOf(CanObjectDict_Index187D), 0x187D},
    { (subindex*)CanObjectDict_Index187E,CountOf(CanObjectDict_Index187E), 0x187E},
    { (subindex*)CanObjectDict_Index187F,CountOf(CanObjectDict_Index187F), 0x187F}, 

#if (SVO_Type == 4)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器		5：Copley驱动器			 	
		
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A00},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A01},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A02},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A03},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A04},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A05},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A06},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A07},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A08},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A09},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A0A},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A0B},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A0C},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A0D},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A0E},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A0F},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A10},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A11},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A12},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A13},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A14},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A15},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A16},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A17},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A18},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A19},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A1A},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A1B},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A1C},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A1D},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A1E},
    { (subindex*)CanObjectDict_RPDO1_Map_Keya_index,CountOf(CanObjectDict_RPDO1_Map_Keya_index), 0x1A1F},
		
#elif (SVO_Type == 5)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器		5：Copley驱动器			 	
		
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A00},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A01},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A02},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A03},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A04},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A05},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A06},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A07},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A08},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A09},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A0A},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A0B},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A0C},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A0D},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A0E},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A0F},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A10},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A11},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A12},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A13},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A14},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A15},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A16},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A17},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A18},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A19},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A1A},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A1B},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A1C},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A1D},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A1E},
    { (subindex*)CanObjectDict_RPDO1_Map_Copley_index,CountOf(CanObjectDict_RPDO1_Map_Copley_index), 0x1A1F},		

#else

    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A00},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A01},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A02},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A03},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A04},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A05},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A06},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A07},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A08},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A09},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A0A},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A0B},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A0C},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A0D},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A0E},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A0F},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A10},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A11},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A12},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A13},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A14},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A15},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A16},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A17},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A18},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A19},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A1A},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A1B},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A1C},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A1D},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A1E},
    { (subindex*)CanObjectDict_RPDO1_Map_index,CountOf(CanObjectDict_RPDO1_Map_index), 0x1A1F},		
		
#endif		
		
		
#if (SVO_Type == 4)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器	 	5：Copley驱动器

    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A20},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A21},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A22},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A23},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A24},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A25},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A26},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A27},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A28},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A29},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A2A},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A2B},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A2C},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A2D},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A2E},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A2F},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A30},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A31},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A32},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A33},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A34},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A35},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A36},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A37},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A38},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A39},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A3A},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A3B},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A3C},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A3D},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A3E},
    { (subindex*)CanObjectDict_RPDO2_Map_Keya_index,CountOf(CanObjectDict_RPDO2_Map_Keya_index), 0x1A3F}, 		
		
#else		
		
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A20},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A21},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A22},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A23},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A24},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A25},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A26},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A27},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A28},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A29},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A2A},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A2B},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A2C},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A2D},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A2E},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A2F},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A30},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A31},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A32},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A33},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A34},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A35},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A36},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A37},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A38},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A39},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A3A},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A3B},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A3C},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A3D},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A3E},
    { (subindex*)CanObjectDict_RPDO2_Map_index,CountOf(CanObjectDict_RPDO2_Map_index), 0x1A3F},    
		
#endif		
			
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A40},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A41},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A42},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A43},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A44},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A45},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A46},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A47},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A48},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A49},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A4A},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A4B},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A4C},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A4D},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A4E},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A4F},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A50},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A51},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A52},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A53},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A54},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A55},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A56},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A57},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A58},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A59},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A5A},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A5B},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A5C},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A5D},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A5E},
    { (subindex*)CanObjectDict_RPDO3_Map_index,CountOf(CanObjectDict_RPDO3_Map_index), 0x1A5F},  
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A60},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A61},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A62},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A63},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A64},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A65},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A66},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A67},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A68},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A69},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A6A},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A6B},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A6C},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A6D},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A6E},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A6F},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A70},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A71},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A72},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A73},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A74},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A75},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A76},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A77},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A78},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A79},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A7A},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A7B},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A7C},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A7D},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A7E},
    { (subindex*)CanObjectDict_RPDO4_Map_index,CountOf(CanObjectDict_RPDO4_Map_index), 0x1A7F},
    { (subindex*)CanObjectDict_Index2201,CountOf(CanObjectDict_Index2201), 0x2201},
    { (subindex*)CanObjectDict_Index221C,CountOf(CanObjectDict_Index221C), 0x221C},
    { (subindex*)CanObjectDict_Index2202,CountOf(CanObjectDict_Index2202), 0x2202},
    { (subindex*)CanObjectDict_Index2190,CountOf(CanObjectDict_Index2190), 0x2190},
    { (subindex*)CanObjectDict_Index6040,CountOf(CanObjectDict_Index6040), 0x6040},
    { (subindex*)CanObjectDict_Index60FF,CountOf(CanObjectDict_Index60FF), 0x60FF},
    { (subindex*)CanObjectDict_Index6041,CountOf(CanObjectDict_Index6041), 0x6041},
    { (subindex*)CanObjectDict_Index6069,CountOf(CanObjectDict_Index6069), 0x6069},
    { (subindex*)CanObjectDict_Index6061,CountOf(CanObjectDict_Index6061), 0x6061},
    { (subindex*)CanObjectDict_Index2300,CountOf(CanObjectDict_Index2300), 0x2300},
    { (subindex*)CanObjectDict_Index2341,CountOf(CanObjectDict_Index2341), 0x2341},
    { (subindex*)CanObjectDict_Index2191,CountOf(CanObjectDict_Index2191), 0x2191},
    { (subindex*)CanObjectDict_Index2340,CountOf(CanObjectDict_Index2340), 0x2340},
    { (subindex*)CanObjectDict_Index2183,CountOf(CanObjectDict_Index2183), 0x2183},    
    { (subindex*)CanObjectDict_Index6063,CountOf(CanObjectDict_Index6063), 0x6063},	
    { (subindex*)CanObjectDict_Index6079,CountOf(CanObjectDict_Index6079), 0x6079},
    { (subindex*)CanObjectDict_Index6078,CountOf(CanObjectDict_Index6078), 0x6078},
    { (subindex*)CanObjectDict_Index2601,CountOf(CanObjectDict_Index2601), 0x2601},  
    { (subindex*)CanObjectDict_Index606C,CountOf(CanObjectDict_Index606C), 0x606C},	 		
    { (subindex*)CanObjectDict_Index607A,CountOf(CanObjectDict_Index607A), 0x607A},		
    { (subindex*)CanObjectDict_Index2602,CountOf(CanObjectDict_Index2602), 0x2602},					
    { (subindex*)CanObjectDict_Index2009,CountOf(CanObjectDict_Index2009), 0x2009},  
    { (subindex*)CanObjectDict_Index200A,CountOf(CanObjectDict_Index200A), 0x200A},	 		
    { (subindex*)CanObjectDict_Index200B,CountOf(CanObjectDict_Index200B), 0x200B},		
    { (subindex*)CanObjectDict_Index603F,CountOf(CanObjectDict_Index603F), 0x603F},					
    { (subindex*)CanObjectDict_Index210C,CountOf(CanObjectDict_Index210C), 0x210C},	
    { (subindex*)CanObjectDict_Index200C,CountOf(CanObjectDict_Index200C), 0x200C},
		{ (subindex*)CanObjectDict_Index2003,CountOf(CanObjectDict_Index2003), 0x2003},		
		{ (subindex*)CanObjectDict_Index22A2,CountOf(CanObjectDict_Index22A2), 0x22A2},		
		{ (subindex*)CanObjectDict_Index2000,CountOf(CanObjectDict_Index2000), 0x2000},		
    { (subindex*)CanObjectDict_Index6064,CountOf(CanObjectDict_Index6064), 0x6064},		
    { (subindex*)CanObjectDict_Index6081,CountOf(CanObjectDict_Index6081), 0x6081},			
    { (subindex*)CanObjectDict_Index6083,CountOf(CanObjectDict_Index6083), 0x6083},		
    { (subindex*)CanObjectDict_Index6084,CountOf(CanObjectDict_Index6084), 0x6084},		
    { (subindex*)CanObjectDict_Index2004,CountOf(CanObjectDict_Index2004), 0x2004},	
    { (subindex*)CanObjectDict_Index2010,CountOf(CanObjectDict_Index2010), 0x2010},		
    { (subindex*)CanObjectDict_Index2011,CountOf(CanObjectDict_Index2011), 0x2011},			
    { (subindex*)CanObjectDict_Index2012,CountOf(CanObjectDict_Index2012), 0x2012},		
    { (subindex*)CanObjectDict_Index2013,CountOf(CanObjectDict_Index2013), 0x2013},		
    { (subindex*)CanObjectDict_Index2016,CountOf(CanObjectDict_Index2016), 0x2016},		
    { (subindex*)CanObjectDict_Index2020,CountOf(CanObjectDict_Index2020), 0x2020},			
    { (subindex*)CanObjectDict_Index2021,CountOf(CanObjectDict_Index2021), 0x2021},		
    { (subindex*)CanObjectDict_Index2022,CountOf(CanObjectDict_Index2022), 0x2022},	
    { (subindex*)CanObjectDict_Index2026,CountOf(CanObjectDict_Index2026), 0x2026},		
    { (subindex*)CanObjectDict_Index203C,CountOf(CanObjectDict_Index203C), 0x203C},				
};

const indextable * CanObjectDict_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks)
{
	int i;
	*callbacks = NULL;
	switch(wIndex)
	{
		case 0x1000: i = 0;break;
		case 0x1001: i = 1;break;
		case 0x1002: i = 2;*callbacks = CanObjectDict_0x1002_callbacks; break;
		case 0x1003: i = 3;*callbacks = CanObjectDict_Index1003_callbacks; break;
		case 0x1005: i = 4;*callbacks = CanObjectDict_Index1005_callbacks; break;
		case 0x1006: i = 5;*callbacks = CanObjectDict_Index1006_callbacks; break;
		case 0x1008: i = 6;break;
		case 0x1009: i = 7;break;
		case 0x100A: i = 8;break;
		case 0x100C: i = 9;break;
		case 0x100D: i = 10;break;
		case 0x1010: i = 11;break;
		case 0x1013: i = 12;break;
		case 0x1014: i = 13;break;
		case 0x1015: i = 14;break;
		case 0x1016: i = 15;break;//NONE,MUST!!
		case 0x1017: i = 16;break;
		case 0x1018: i = 17;*callbacks = CanObjectDict_Index1018_callbacks; break;
/*Server SDO Parameter*/         
		case 0x1200: i = 18;break;//Server1 SDO Parameter
		case 0x1201: i = 19;break;//Server2 SDO Parameter
		case 0x1202: i = 20;break;//Server3 SDO Parameter
		case 0x1203: i = 21;break;//Server4 SDO Parameter
		case 0x1204: i = 22;break;//Server5 SDO Parameter
		case 0x1205: i = 23;break;//Server6 SDO Parameter
		case 0x1206: i = 24;break;//Server7 SDO Parameter
		case 0x1207: i = 25;break;//Server8 SDO Parameter
		case 0x1208: i = 26;break;//Server9 SDO Parameter
		case 0x1209: i = 27;break;//Server10 SDO Parameter
		case 0x120A: i = 28;break;//Server11 SDO Parameter
		case 0x120B: i = 29;break;//Server12 SDO Parameter
		case 0x120C: i = 30;break;//Server13 SDO Parameter
		case 0x120D: i = 31;break;//Server14 SDO Parameter
		case 0x120E: i = 32;break;//Server15 SDO Parameter
		case 0x120F: i = 33;break;//Server16 SDO Parameter
		case 0x1210: i = 34;break;//Server17 SDO Parameter
		case 0x1211: i = 35;break;//Server18 SDO Parameter
		case 0x1212: i = 36;break;//Server19 SDO Parameter
		case 0x1213: i = 37;break;//Server20 SDO Parameter        
		case 0x1214: i = 38;break;//Server21 SDO Parameter
		case 0x1215: i = 39;break;//Server22 SDO Parameter
		case 0x1216: i = 40;break;//Server23 SDO Parameter
		case 0x1217: i = 41;break;//Server24 SDO Parameter
		case 0x1218: i = 42;break;//Server25 SDO Parameter
		case 0x1219: i = 43;break;//Server26 SDO Parameter
		case 0x121A: i = 44;break;//Server27 SDO Parameter        
		case 0x121B: i = 45;break;//Server28 SDO Parameter        
		case 0x121C: i = 46;break;//Server29 SDO Parameter        
		case 0x121D: i = 47;break;//Server30 SDO Parameter        
		case 0x121E: i = 48;break;//Server31 SDO Parameter
		case 0x121F: i = 49;break;//Server32 SDO Parameter  
/*Client SDO Parameter*/        
		case 0x1280: i = 50;break;//Client1 SDO Parameter
		case 0x1281: i = 51;break;//Client2 SDO Parameter
		case 0x1282: i = 52;break;//Client3 SDO Parameter
		case 0x1283: i = 53;break;//Client4 SDO Parameter
		case 0x1284: i = 54;break;//Client5 SDO Parameter
		case 0x1285: i = 55;break;//Client6 SDO Parameter
		case 0x1286: i = 56;break;//Client7 SDO Parameter
		case 0x1287: i = 57;break;//Client8 SDO Parameter
		case 0x1288: i = 58;break;//Client9 SDO Parameter
		case 0x1289: i = 59;break;//Client10 SDO Parameter
		case 0x128A: i = 60;break;//Client11 SDO Parameter
		case 0x128B: i = 61;break;//Client12 SDO Parameter
		case 0x128C: i = 62;break;//Client13 SDO Parameter
		case 0x128D: i = 63;break;//Client14 SDO Parameter
		case 0x128E: i = 64;break;//Client15 SDO Parameter
		case 0x128F: i = 65;break;//Client16 SDO Parameter
		case 0x1290: i = 66;break;//Client17 SDO Parameter
		case 0x1291: i = 67;break;//Client18 SDO Parameter
		case 0x1292: i = 68;break;//Client19 SDO Parameter
		case 0x1293: i = 69;break;//Client20 SDO Parameter
		case 0x1294: i = 70;break;//Client21 SDO Parameter
		case 0x1295: i = 71;break;//Client22 SDO Parameter
		case 0x1296: i = 72;break;//Client23 SDO Parameter
		case 0x1297: i = 73;break;//Client24 SDO Parameter
		case 0x1298: i = 74;break;//Client25 SDO Parameter
		case 0x1299: i = 75;break;//Client26 SDO Parameter
		case 0x129A: i = 76;break;//Client27 SDO Parameter
		case 0x129B: i = 77;break;//Client28 SDO Parameter
		case 0x129C: i = 78;break;//Client29 SDO Parameter
		case 0x129D: i = 79;break;//Client30 SDO Parameter
		case 0x129E: i = 80;break;//Client31 SDO Parameter
		case 0x129F: i = 81;break;//Client32 SDO Parameter        
/*Rcv_PDO_Communication Para*/
		case 0x1400: i = 82;break;//client1_TPOD1_181
		case 0x1401: i = 83;break;
		case 0x1402: i = 84;break;
		case 0x1403: i = 85;break;
		case 0x1404: i = 86;break;
		case 0x1405: i = 87;break;
		case 0x1406: i = 88;break;
		case 0x1407: i = 89;break;
		case 0x1408: i = 90;break;
		case 0x1409: i = 91;break;
		case 0x140A: i = 92;break;
		case 0x140B: i = 93;break;
		case 0x140C: i = 94;break;
		case 0x140D: i = 95;break;
		case 0x140E: i = 96;break;
		case 0x140F: i = 97;break;
		case 0x1410: i = 98;break;
		case 0x1411: i = 99;break;
		case 0x1412: i = 100;break;
		case 0x1413: i = 101;break;
		case 0x1414: i = 102;break;
		case 0x1415: i = 103;break;
		case 0x1416: i = 104;break;
		case 0x1417: i = 105;break;
		case 0x1418: i = 106;break;
		case 0x1419: i = 107;break;
		case 0x141A: i = 108;break;
		case 0x141B: i = 109;break;
		case 0x141C: i = 110;break;
		case 0x141D: i = 111;break;
		case 0x141E: i = 112;break;
		case 0x141F: i = 113;break;//client32__TPOD1_19F
		case 0x1420: i = 114;break;//client1_TPOD2_281
		case 0x1421: i = 115;break;
		case 0x1422: i = 116;break;
		case 0x1423: i = 117;break;
		case 0x1424: i = 118;break;
		case 0x1425: i = 119;break;
		case 0x1426: i = 120;break;
		case 0x1427: i = 121;break;
		case 0x1428: i = 122;break;
		case 0x1429: i = 123;break;
		case 0x142A: i = 124;break;
		case 0x142B: i = 125;break;
		case 0x142C: i = 126;break;
		case 0x142D: i = 127;break;
		case 0x142E: i = 128;break;
		case 0x142F: i = 129;break;
		case 0x1430: i = 130;break;
		case 0x1431: i = 131;break;
		case 0x1432: i = 132;break;
		case 0x1433: i = 133;break;
		case 0x1434: i = 134;break;
		case 0x1435: i = 135;break;
		case 0x1436: i = 136;break;
		case 0x1437: i = 137;break;
		case 0x1438: i = 138;break;
		case 0x1439: i = 139;break;
		case 0x143A: i = 140;break;
		case 0x143B: i = 141;break;
		case 0x143C: i = 142;break;
		case 0x143D: i = 143;break;
		case 0x143E: i = 144;break;
		case 0x143F: i = 145;break;//client32__TPOD2_29F
		case 0x1440: i = 146;break;//client1_TPOD3_381
		case 0x1441: i = 147;break;
		case 0x1442: i = 148;break;
		case 0x1443: i = 149;break;
		case 0x1444: i = 150;break;
		case 0x1445: i = 151;break;
		case 0x1446: i = 152;break;
		case 0x1447: i = 153;break;
		case 0x1448: i = 154;break;
		case 0x1449: i = 155;break;
		case 0x144A: i = 156;break;
		case 0x144B: i = 157;break;
		case 0x144C: i = 158;break;
		case 0x144D: i = 159;break;
		case 0x144E: i = 160;break;
		case 0x144F: i = 161;break;
		case 0x1450: i = 162;break;
		case 0x1451: i = 163;break;
		case 0x1452: i = 164;break;
		case 0x1453: i = 165;break;
		case 0x1454: i = 166;break;
		case 0x1455: i = 167;break;
		case 0x1456: i = 168;break;
		case 0x1457: i = 169;break;
		case 0x1458: i = 170;break;
		case 0x1459: i = 171;break;
		case 0x145A: i = 172;break;
		case 0x145B: i = 173;break;
		case 0x145C: i = 174;break;
		case 0x145D: i = 175;break;
		case 0x145E: i = 176;break;
		case 0x145F: i = 177;break;//client32__TPOD3_39F        
		case 0x1460: i = 178;break;//client1_TPOD4_481
		case 0x1461: i = 179;break;
		case 0x1462: i = 180;break;
		case 0x1463: i = 181;break;
		case 0x1464: i = 182;break;
		case 0x1465: i = 183;break;
		case 0x1466: i = 184;break;
		case 0x1467: i = 185;break;
		case 0x1468: i = 186;break;
		case 0x1469: i = 187;break;
		case 0x146A: i = 188;break;
		case 0x146B: i = 189;break;
		case 0x146C: i = 190;break;
		case 0x146D: i = 191;break;
		case 0x146E: i = 192;break;
		case 0x146F: i = 193;break;
		case 0x1470: i = 194;break;
		case 0x1471: i = 195;break;
		case 0x1472: i = 196;break;
		case 0x1473: i = 197;break;
		case 0x1474: i = 198;break;
		case 0x1475: i = 199;break;
		case 0x1476: i = 200;break;
		case 0x1477: i = 201;break;
		case 0x1478: i = 202;break;
		case 0x1479: i = 203;break;
		case 0x147A: i = 204;break;
		case 0x147B: i = 205;break;
		case 0x147C: i = 206;break;
		case 0x147D: i = 207;break;
		case 0x147E: i = 208;break;
		case 0x147F: i = 209;break;//client32__TPOD4_49F        
/*Rcv_PDO_mapping Para*/
		case 0x1600: i = 210;break;//mapp to Slave1_tpd1
		case 0x1601: i = 211;break;
		case 0x1602: i = 212;break;
		case 0x1603: i = 213;break;
		case 0x1604: i = 214;break;
		case 0x1605: i = 215;break;
		case 0x1606: i = 216;break;
		case 0x1607: i = 217;break;
		case 0x1608: i = 218;break;
		case 0x1609: i = 219;break;
		case 0x160A: i = 220;break;
		case 0x160B: i = 221;break;
		case 0x160C: i = 222;break;
		case 0x160D: i = 223;break;
		case 0x160E: i = 224;break;
		case 0x160F: i = 225;break;
		case 0x1610: i = 226;break;
		case 0x1611: i = 227;break;
		case 0x1612: i = 228;break;
		case 0x1613: i = 229;break;
		case 0x1614: i = 230;break;
		case 0x1615: i = 231;break;
		case 0x1616: i = 232;break;
		case 0x1617: i = 233;break;
		case 0x1618: i = 234;break;
		case 0x1619: i = 235;break;
		case 0x161A: i = 236;break;
		case 0x161B: i = 237;break;
		case 0x161C: i = 238;break;
		case 0x161D: i = 239;break;
		case 0x161E: i = 240;break;
		case 0x161F: i = 241;break;//mapp to Slave32_tpd1
		case 0x1620: i = 242;break;//mapp to Slave1_tpd2
		case 0x1621: i = 243;break;
		case 0x1622: i = 244;break;
		case 0x1623: i = 245;break;
		case 0x1624: i = 246;break;
		case 0x1625: i = 247;break;
		case 0x1626: i = 248;break;
		case 0x1627: i = 249;break;
		case 0x1628: i = 250;break;
		case 0x1629: i = 251;break;
		case 0x162A: i = 252;break;
		case 0x162B: i = 253;break;
		case 0x162C: i = 254;break;
		case 0x162D: i = 255;break;
		case 0x162E: i = 256;break;
		case 0x162F: i = 257;break;
		case 0x1630: i = 258;break;
		case 0x1631: i = 259;break;
		case 0x1632: i = 260;break;
		case 0x1633: i = 261;break;
		case 0x1634: i = 262;break;
		case 0x1635: i = 263;break;
		case 0x1636: i = 264;break;
		case 0x1637: i = 265;break;
		case 0x1638: i = 266;break;
		case 0x1639: i = 267;break;
		case 0x163A: i = 268;break;
		case 0x163B: i = 269;break;
		case 0x163C: i = 270;break;
		case 0x163D: i = 271;break;
		case 0x163E: i = 272;break;
		case 0x163F: i = 273;break;//mapp to Slave32_tpd2
		case 0x1640: i = 274;break;//mapp to Slave1_tpd3
		case 0x1641: i = 275;break;
		case 0x1642: i = 276;break;
		case 0x1643: i = 277;break;
		case 0x1644: i = 278;break;
		case 0x1645: i = 279;break;
		case 0x1646: i = 280;break;
		case 0x1647: i = 281;break;
		case 0x1648: i = 282;break;
		case 0x1649: i = 283;break;
		case 0x164A: i = 284;break;
		case 0x164B: i = 285;break;
		case 0x164C: i = 286;break;
		case 0x164D: i = 287;break;
		case 0x164E: i = 288;break;
		case 0x164F: i = 289;break;
		case 0x1650: i = 290;break;
		case 0x1651: i = 291;break;
		case 0x1652: i = 292;break;
		case 0x1653: i = 293;break;
		case 0x1654: i = 294;break;
		case 0x1655: i = 295;break;
		case 0x1656: i = 296;break;
		case 0x1657: i = 297;break;
		case 0x1658: i = 298;break;
		case 0x1659: i = 299;break;
		case 0x165A: i = 300;break;
		case 0x165B: i = 301;break;
		case 0x165C: i = 302;break;
		case 0x165D: i = 303;break;
		case 0x165E: i = 304;break;
		case 0x165F: i = 305;break;//mapp to Slave32_tpd3        
		case 0x1660: i = 306;break;//mapp to Slave1_tpd4
		case 0x1661: i = 307;break;
		case 0x1662: i = 308;break;
		case 0x1663: i = 309;break;
		case 0x1664: i = 310;break;
		case 0x1665: i = 311;break;
		case 0x1666: i = 312;break;
		case 0x1667: i = 313;break;
		case 0x1668: i = 314;break;
		case 0x1669: i = 315;break;
		case 0x166A: i = 316;break;
		case 0x166B: i = 317;break;
		case 0x166C: i = 318;break;
		case 0x166D: i = 319;break;
		case 0x166E: i = 320;break;
		case 0x166F: i = 321;break;
		case 0x1670: i = 322;break;
		case 0x1671: i = 323;break;
		case 0x1672: i = 324;break;
		case 0x1673: i = 325;break;
		case 0x1674: i = 326;break;
		case 0x1675: i = 327;break;
		case 0x1676: i = 328;break;
		case 0x1677: i = 329;break;
		case 0x1678: i = 330;break;
		case 0x1679: i = 331;break;
		case 0x167A: i = 332;break;
		case 0x167B: i = 333;break;
		case 0x167C: i = 334;break;
		case 0x167D: i = 335;break;
		case 0x167E: i = 336;break;
		case 0x167F: i = 337;break;//mapp to Slave32_tpd4                 
/*Trs_PDO_Communication Para*/
		case 0x1800: i = 338;*callbacks = CanObjectDict_Index1800_callbacks; break;//master rpdo1 201
		case 0x1801: i = 339;*callbacks = CanObjectDict_Index1801_callbacks; break;
		case 0x1802: i = 340;*callbacks = CanObjectDict_Index1802_callbacks; break;
		case 0x1803: i = 341;*callbacks = CanObjectDict_Index1803_callbacks; break;
		case 0x1804: i = 342;*callbacks = CanObjectDict_Index1804_callbacks; break;
		case 0x1805: i = 343;*callbacks = CanObjectDict_Index1805_callbacks; break;
		case 0x1806: i = 344;*callbacks = CanObjectDict_Index1806_callbacks; break;
		case 0x1807: i = 345;*callbacks = CanObjectDict_Index1807_callbacks; break;
		case 0x1808: i = 346;*callbacks = CanObjectDict_Index1808_callbacks; break;
		case 0x1809: i = 347;*callbacks = CanObjectDict_Index1809_callbacks; break;
		case 0x180A: i = 348;*callbacks = CanObjectDict_Index180A_callbacks; break;
		case 0x180B: i = 349;*callbacks = CanObjectDict_Index180B_callbacks; break;
		case 0x180C: i = 350;*callbacks = CanObjectDict_Index180C_callbacks; break;
		case 0x180D: i = 351;*callbacks = CanObjectDict_Index180D_callbacks; break;
		case 0x180E: i = 352;*callbacks = CanObjectDict_Index180E_callbacks; break;
		case 0x180F: i = 353;*callbacks = CanObjectDict_Index180F_callbacks; break;
		case 0x1810: i = 354;*callbacks = CanObjectDict_Index1810_callbacks; break;
		case 0x1811: i = 355;*callbacks = CanObjectDict_Index1811_callbacks; break;
		case 0x1812: i = 356;*callbacks = CanObjectDict_Index1812_callbacks; break;
		case 0x1813: i = 357;*callbacks = CanObjectDict_Index1813_callbacks; break;
		case 0x1814: i = 358;*callbacks = CanObjectDict_Index1814_callbacks; break;
		case 0x1815: i = 359;*callbacks = CanObjectDict_Index1815_callbacks; break;
		case 0x1816: i = 360;*callbacks = CanObjectDict_Index1816_callbacks; break;
		case 0x1817: i = 361;*callbacks = CanObjectDict_Index1817_callbacks; break;
		case 0x1818: i = 362;*callbacks = CanObjectDict_Index1818_callbacks; break;
		case 0x1819: i = 363;*callbacks = CanObjectDict_Index1819_callbacks; break;
		case 0x181A: i = 364;*callbacks = CanObjectDict_Index181A_callbacks; break;
		case 0x181B: i = 365;*callbacks = CanObjectDict_Index181B_callbacks; break;
		case 0x181C: i = 366;*callbacks = CanObjectDict_Index181C_callbacks; break;
		case 0x181D: i = 367;*callbacks = CanObjectDict_Index181D_callbacks; break;
		case 0x181E: i = 368;*callbacks = CanObjectDict_Index181E_callbacks; break;
		case 0x181F: i = 369;*callbacks = CanObjectDict_Index181F_callbacks; break;//21F
 		case 0x1820: i = 370;*callbacks = CanObjectDict_Index1820_callbacks; break;//master rpdo2 301
		case 0x1821: i = 371;*callbacks = CanObjectDict_Index1821_callbacks; break;
		case 0x1822: i = 372;*callbacks = CanObjectDict_Index1822_callbacks; break;
		case 0x1823: i = 373;*callbacks = CanObjectDict_Index1823_callbacks; break;
		case 0x1824: i = 374;*callbacks = CanObjectDict_Index1824_callbacks; break;
		case 0x1825: i = 375;*callbacks = CanObjectDict_Index1825_callbacks; break;
		case 0x1826: i = 376;*callbacks = CanObjectDict_Index1826_callbacks; break;
		case 0x1827: i = 377;*callbacks = CanObjectDict_Index1827_callbacks; break;
		case 0x1828: i = 378;*callbacks = CanObjectDict_Index1828_callbacks; break;
		case 0x1829: i = 379;*callbacks = CanObjectDict_Index1829_callbacks; break;
		case 0x182A: i = 380;*callbacks = CanObjectDict_Index182A_callbacks; break;
		case 0x182B: i = 381;*callbacks = CanObjectDict_Index182B_callbacks; break;
		case 0x182C: i = 382;*callbacks = CanObjectDict_Index182C_callbacks; break;
		case 0x182D: i = 383;*callbacks = CanObjectDict_Index182D_callbacks; break;
		case 0x182E: i = 384;*callbacks = CanObjectDict_Index182E_callbacks; break;
		case 0x182F: i = 385;*callbacks = CanObjectDict_Index182F_callbacks; break;  
 		case 0x1830: i = 386;*callbacks = CanObjectDict_Index1830_callbacks; break;
		case 0x1831: i = 387;*callbacks = CanObjectDict_Index1831_callbacks; break;
		case 0x1832: i = 388;*callbacks = CanObjectDict_Index1832_callbacks; break;
		case 0x1833: i = 389;*callbacks = CanObjectDict_Index1833_callbacks; break;
		case 0x1834: i = 390;*callbacks = CanObjectDict_Index1834_callbacks; break;
		case 0x1835: i = 391;*callbacks = CanObjectDict_Index1835_callbacks; break;
		case 0x1836: i = 392;*callbacks = CanObjectDict_Index1836_callbacks; break;
		case 0x1837: i = 393;*callbacks = CanObjectDict_Index1837_callbacks; break;
		case 0x1838: i = 394;*callbacks = CanObjectDict_Index1838_callbacks; break;
		case 0x1839: i = 395;*callbacks = CanObjectDict_Index1839_callbacks; break;
		case 0x183A: i = 396;*callbacks = CanObjectDict_Index183A_callbacks; break;
		case 0x183B: i = 397;*callbacks = CanObjectDict_Index183B_callbacks; break;
		case 0x183C: i = 398;*callbacks = CanObjectDict_Index183C_callbacks; break;
		case 0x183D: i = 399;*callbacks = CanObjectDict_Index183D_callbacks; break;
		case 0x183E: i = 400;*callbacks = CanObjectDict_Index183E_callbacks; break;
		case 0x183F: i = 401;*callbacks = CanObjectDict_Index183F_callbacks; break;//31F     
 		case 0x1840: i = 402;*callbacks = CanObjectDict_Index1840_callbacks; break;//master rpdo3 401
		case 0x1841: i = 403;*callbacks = CanObjectDict_Index1841_callbacks; break;
		case 0x1842: i = 404;*callbacks = CanObjectDict_Index1842_callbacks; break;
		case 0x1843: i = 405;*callbacks = CanObjectDict_Index1843_callbacks; break;
		case 0x1844: i = 406;*callbacks = CanObjectDict_Index1844_callbacks; break;
		case 0x1845: i = 407;*callbacks = CanObjectDict_Index1845_callbacks; break;
		case 0x1846: i = 408;*callbacks = CanObjectDict_Index1846_callbacks; break;
		case 0x1847: i = 409;*callbacks = CanObjectDict_Index1847_callbacks; break;
		case 0x1848: i = 410;*callbacks = CanObjectDict_Index1848_callbacks; break;
		case 0x1849: i = 411;*callbacks = CanObjectDict_Index1849_callbacks; break;
		case 0x184A: i = 412;*callbacks = CanObjectDict_Index184A_callbacks; break;
		case 0x184B: i = 413;*callbacks = CanObjectDict_Index184B_callbacks; break;
		case 0x184C: i = 414;*callbacks = CanObjectDict_Index184C_callbacks; break;
		case 0x184D: i = 415;*callbacks = CanObjectDict_Index184D_callbacks; break;
		case 0x184E: i = 416;*callbacks = CanObjectDict_Index184E_callbacks; break;
		case 0x184F: i = 417;*callbacks = CanObjectDict_Index184F_callbacks; break;  
 		case 0x1850: i = 418;*callbacks = CanObjectDict_Index1850_callbacks; break;
		case 0x1851: i = 419;*callbacks = CanObjectDict_Index1851_callbacks; break;
		case 0x1852: i = 420;*callbacks = CanObjectDict_Index1852_callbacks; break;
		case 0x1853: i = 421;*callbacks = CanObjectDict_Index1853_callbacks; break;
		case 0x1854: i = 422;*callbacks = CanObjectDict_Index1854_callbacks; break;
		case 0x1855: i = 423;*callbacks = CanObjectDict_Index1855_callbacks; break;
		case 0x1856: i = 424;*callbacks = CanObjectDict_Index1856_callbacks; break;
		case 0x1857: i = 425;*callbacks = CanObjectDict_Index1857_callbacks; break;
		case 0x1858: i = 426;*callbacks = CanObjectDict_Index1858_callbacks; break;
		case 0x1859: i = 427;*callbacks = CanObjectDict_Index1859_callbacks; break;
		case 0x185A: i = 428;*callbacks = CanObjectDict_Index185A_callbacks; break;
		case 0x185B: i = 429;*callbacks = CanObjectDict_Index185B_callbacks; break;
		case 0x185C: i = 430;*callbacks = CanObjectDict_Index185C_callbacks; break;
		case 0x185D: i = 431;*callbacks = CanObjectDict_Index185D_callbacks; break;
		case 0x185E: i = 432;*callbacks = CanObjectDict_Index185E_callbacks; break;
		case 0x185F: i = 433;*callbacks = CanObjectDict_Index185F_callbacks; break;//41F    
 		case 0x1860: i = 434;*callbacks = CanObjectDict_Index1860_callbacks; break;//master rpdo4 501
		case 0x1861: i = 435;*callbacks = CanObjectDict_Index1861_callbacks; break;
		case 0x1862: i = 436;*callbacks = CanObjectDict_Index1862_callbacks; break;
		case 0x1863: i = 437;*callbacks = CanObjectDict_Index1863_callbacks; break;
		case 0x1864: i = 438;*callbacks = CanObjectDict_Index1864_callbacks; break;
		case 0x1865: i = 439;*callbacks = CanObjectDict_Index1865_callbacks; break;
		case 0x1866: i = 440;*callbacks = CanObjectDict_Index1866_callbacks; break;
		case 0x1867: i = 441;*callbacks = CanObjectDict_Index1867_callbacks; break;
		case 0x1868: i = 442;*callbacks = CanObjectDict_Index1868_callbacks; break;
		case 0x1869: i = 443;*callbacks = CanObjectDict_Index1869_callbacks; break;
		case 0x186A: i = 444;*callbacks = CanObjectDict_Index186A_callbacks; break;
		case 0x186B: i = 445;*callbacks = CanObjectDict_Index186B_callbacks; break;
		case 0x186C: i = 446;*callbacks = CanObjectDict_Index186C_callbacks; break;
		case 0x186D: i = 447;*callbacks = CanObjectDict_Index186D_callbacks; break;
		case 0x186E: i = 448;*callbacks = CanObjectDict_Index186E_callbacks; break;
		case 0x186F: i = 449;*callbacks = CanObjectDict_Index186F_callbacks; break;  
 		case 0x1870: i = 450;*callbacks = CanObjectDict_Index1870_callbacks; break;
		case 0x1871: i = 451;*callbacks = CanObjectDict_Index1871_callbacks; break;
		case 0x1872: i = 452;*callbacks = CanObjectDict_Index1872_callbacks; break;
		case 0x1873: i = 453;*callbacks = CanObjectDict_Index1873_callbacks; break;
		case 0x1874: i = 454;*callbacks = CanObjectDict_Index1874_callbacks; break;
		case 0x1875: i = 455;*callbacks = CanObjectDict_Index1875_callbacks; break;
		case 0x1876: i = 456;*callbacks = CanObjectDict_Index1876_callbacks; break;
		case 0x1877: i = 457;*callbacks = CanObjectDict_Index1877_callbacks; break;
		case 0x1878: i = 458;*callbacks = CanObjectDict_Index1878_callbacks; break;
		case 0x1879: i = 459;*callbacks = CanObjectDict_Index1879_callbacks; break;
		case 0x187A: i = 460;*callbacks = CanObjectDict_Index187A_callbacks; break;
		case 0x187B: i = 461;*callbacks = CanObjectDict_Index187B_callbacks; break;
		case 0x187C: i = 462;*callbacks = CanObjectDict_Index187C_callbacks; break;
		case 0x187D: i = 463;*callbacks = CanObjectDict_Index187D_callbacks; break;
		case 0x187E: i = 464;*callbacks = CanObjectDict_Index187E_callbacks; break;
		case 0x187F: i = 465;*callbacks = CanObjectDict_Index187F_callbacks; break;//51F          
        
/*Trs_PDO_mapping Para*/
		case 0x1A00: i = 466;break;//tpdo1 map slave1
		case 0x1A01: i = 467;break;
		case 0x1A02: i = 468;break;
		case 0x1A03: i = 469;break;
		case 0x1A04: i = 470;break;
		case 0x1A05: i = 471;break;
		case 0x1A06: i = 472;break;
		case 0x1A07: i = 473;break;
		case 0x1A08: i = 474;break;
		case 0x1A09: i = 475;break;
		case 0x1A0A: i = 476;break;
		case 0x1A0B: i = 477;break;
		case 0x1A0C: i = 478;break;
		case 0x1A0D: i = 479;break;
		case 0x1A0E: i = 480;break;
		case 0x1A0F: i = 481;break;
		case 0x1A10: i = 482;break;
		case 0x1A11: i = 483;break;
		case 0x1A12: i = 484;break;
		case 0x1A13: i = 485;break;
		case 0x1A14: i = 486;break;
		case 0x1A15: i = 487;break;
		case 0x1A16: i = 488;break;
		case 0x1A17: i = 489;break;
		case 0x1A18: i = 490;break;
		case 0x1A19: i = 491;break;
		case 0x1A1A: i = 492;break;
		case 0x1A1B: i = 493;break;
		case 0x1A1C: i = 494;break;
		case 0x1A1D: i = 495;break;
		case 0x1A1E: i = 496;break;
		case 0x1A1F: i = 497;break;//tpdo1 map slave32
		case 0x1A20: i = 498;break;//tpdo2 map slave1
		case 0x1A21: i = 499;break;
		case 0x1A22: i = 500;break;
		case 0x1A23: i = 501;break;
		case 0x1A24: i = 502;break;
		case 0x1A25: i = 503;break;
		case 0x1A26: i = 504;break;
		case 0x1A27: i = 505;break;
		case 0x1A28: i = 506;break;
		case 0x1A29: i = 507;break;
		case 0x1A2A: i = 508;break;
		case 0x1A2B: i = 509;break;
		case 0x1A2C: i = 510;break;
		case 0x1A2D: i = 511;break;
		case 0x1A2E: i = 512;break;
		case 0x1A2F: i = 513;break;
		case 0x1A30: i = 514;break;
		case 0x1A31: i = 515;break;
		case 0x1A32: i = 516;break;
		case 0x1A33: i = 517;break;
		case 0x1A34: i = 518;break;
		case 0x1A35: i = 519;break;
		case 0x1A36: i = 520;break;
		case 0x1A37: i = 521;break;
		case 0x1A38: i = 522;break;
		case 0x1A39: i = 523;break;
		case 0x1A3A: i = 524;break;
		case 0x1A3B: i = 525;break;
		case 0x1A3C: i = 526;break;
		case 0x1A3D: i = 527;break;
		case 0x1A3E: i = 528;break;
		case 0x1A3F: i = 529;break;//tpdo2 map slave32  
		case 0x1A40: i = 530;break;//tpdo3 map slave1
		case 0x1A41: i = 531;break;
		case 0x1A42: i = 532;break;
		case 0x1A43: i = 533;break;
		case 0x1A44: i = 534;break;
		case 0x1A45: i = 535;break;
		case 0x1A46: i = 536;break;
		case 0x1A47: i = 537;break;
		case 0x1A48: i = 538;break;
		case 0x1A49: i = 539;break;
		case 0x1A4A: i = 540;break;
		case 0x1A4B: i = 541;break;
		case 0x1A4C: i = 542;break;
		case 0x1A4D: i = 543;break;
		case 0x1A4E: i = 544;break;
		case 0x1A4F: i = 545;break;
		case 0x1A50: i = 546;break;
		case 0x1A51: i = 547;break;
		case 0x1A52: i = 548;break;
		case 0x1A53: i = 549;break;
		case 0x1A54: i = 550;break;
		case 0x1A55: i = 551;break;
		case 0x1A56: i = 552;break;
		case 0x1A57: i = 553;break;
		case 0x1A58: i = 554;break;
		case 0x1A59: i = 555;break;
		case 0x1A5A: i = 556;break;
		case 0x1A5B: i = 557;break;
		case 0x1A5C: i = 558;break;
		case 0x1A5D: i = 559;break;
		case 0x1A5E: i = 560;break;
		case 0x1A5F: i = 561;break;//tpdo3 map slave32     
 		case 0x1A60: i = 562;break;//tpdo4 map slave1
		case 0x1A61: i = 563;break;
		case 0x1A62: i = 564;break;
		case 0x1A63: i = 565;break;
		case 0x1A64: i = 566;break;
		case 0x1A65: i = 567;break;
		case 0x1A66: i = 568;break;
		case 0x1A67: i = 569;break;
		case 0x1A68: i = 570;break;
		case 0x1A69: i = 571;break;
		case 0x1A6A: i = 572;break;
		case 0x1A6B: i = 573;break;
		case 0x1A6C: i = 574;break;
		case 0x1A6D: i = 575;break;
		case 0x1A6E: i = 576;break;
		case 0x1A6F: i = 577;break;
		case 0x1A70: i = 578;break;
		case 0x1A71: i = 579;break;
		case 0x1A72: i = 580;break;
		case 0x1A73: i = 581;break;
		case 0x1A74: i = 582;break;
		case 0x1A75: i = 583;break;
		case 0x1A76: i = 584;break;
		case 0x1A77: i = 585;break;
		case 0x1A78: i = 586;break;
		case 0x1A79: i = 587;break;
		case 0x1A7A: i = 588;break;
		case 0x1A7B: i = 589;break;
		case 0x1A7C: i = 590;break;
		case 0x1A7D: i = 591;break;
		case 0x1A7E: i = 592;break;
		case 0x1A7F: i = 593;break;//tpdo4 map slave32        
        
/*Manufacturer Specific and Standardised Device Profile Area*/
		case 0x2201: i = 594;*callbacks = CanObjectDict_0x2201_callbacks; break;
		case 0x221C: i = 595;*callbacks = CanObjectDict_0x221C_callbacks; break;
		case 0x2202: i = 596;*callbacks = CanObjectDict_0x2202_callbacks; break;
		case 0x2190: i = 597;*callbacks = CanObjectDict_0x2190_callbacks; break;
		case 0x6040: i = 598;*callbacks = CanObjectDict_0x6040_callbacks; break;
		case 0x60FF: i = 599;*callbacks = CanObjectDict_0x60FF_callbacks; break;
		case 0x6041: i = 600;*callbacks = CanObjectDict_0x6041_callbacks; break;
		case 0x6069: i = 601;*callbacks = CanObjectDict_0x6069_callbacks; break;
		case 0x6061: i = 602;*callbacks = CanObjectDict_0x6061_callbacks; break;
		case 0x2300: i = 603;*callbacks = CanObjectDict_0x2300_callbacks; break;
		case 0x2341: i = 604;*callbacks = CanObjectDict_0x2341_callbacks; break;
		case 0x2191: i = 605;*callbacks = CanObjectDict_0x2191_callbacks; break;
		case 0x2340: i = 606;*callbacks = CanObjectDict_0x2340_callbacks; break;
		case 0x2183: i = 607;*callbacks = CanObjectDict_0x2183_callbacks; break;
		case 0x6063: i = 608;*callbacks = CanObjectDict_0x6063_callbacks; break;
		case 0x6079: i = 609;*callbacks = CanObjectDict_0x6079_callbacks; break;
		case 0x6078: i = 610;*callbacks = CanObjectDict_0x6078_callbacks; break;
		case 0x2601: i = 611;*callbacks = CanObjectDict_0x2601_callbacks; break;
		case 0x606C: i = 612;*callbacks = CanObjectDict_0x606C_callbacks; break;		
		case 0x607A: i = 613;*callbacks = CanObjectDict_0x607A_callbacks; break;	
		case 0x2602: i = 614;*callbacks = CanObjectDict_0x2602_callbacks; break;		
		case 0x2009: i = 615;*callbacks = CanObjectDict_0x2009_callbacks; break;
		case 0x200A: i = 616;*callbacks = CanObjectDict_0x200A_callbacks; break;
		case 0x200B: i = 617;*callbacks = CanObjectDict_0x200B_callbacks; break;		
		case 0x603F: i = 618;*callbacks = CanObjectDict_0x603F_callbacks; break;
		case 0x210C: i = 619;*callbacks = CanObjectDict_0x210C_callbacks; break;		
		case 0x200C: i = 620;*callbacks = CanObjectDict_0x200C_callbacks; break;
		case 0x2003: i = 621;*callbacks = CanObjectDict_0x2003_callbacks; break;	
		case 0x22A2: i = 622;*callbacks = CanObjectDict_0x22A2_callbacks; break;	
		case 0x2000: i = 623;*callbacks = CanObjectDict_0x2000_callbacks; break;		
		case 0x6064: i = 624;*callbacks = CanObjectDict_0x6064_callbacks; break;		
		case 0x6081: i = 625;*callbacks = CanObjectDict_0x6081_callbacks; break;		
		case 0x6083: i = 626;*callbacks = CanObjectDict_0x6083_callbacks; break;	
		case 0x6084: i = 627;*callbacks = CanObjectDict_0x6084_callbacks; break;
		case 0x2004: i = 628;*callbacks = CanObjectDict_0x2004_callbacks; break;		
		case 0x2010: i = 629;*callbacks = CanObjectDict_0x2010_callbacks; break;	
		case 0x2011: i = 630;*callbacks = CanObjectDict_0x2011_callbacks; break;	
		case 0x2012: i = 631;*callbacks = CanObjectDict_0x2012_callbacks; break;	
		case 0x2013: i = 632;*callbacks = CanObjectDict_0x2013_callbacks; break;	
		case 0x2016: i = 633;*callbacks = CanObjectDict_0x2016_callbacks; break;	
		case 0x2020: i = 634;*callbacks = CanObjectDict_0x2020_callbacks; break;	
		case 0x2021: i = 635;*callbacks = CanObjectDict_0x2021_callbacks; break;	
		case 0x2022: i = 636;*callbacks = CanObjectDict_0x2022_callbacks; break;	
		case 0x2026: i = 637;*callbacks = CanObjectDict_0x2026_callbacks; break;	
		case 0x203C: i = 638;*callbacks = CanObjectDict_0x203C_callbacks; break;	
		
		default:
			*errorCode = OD_NO_SUCH_OBJECT;
			return NULL;
	}
	*errorCode = OD_SUCCESSFUL;
	return &CanObjectDict_objdict[i];
}

const quick_index CanObjectDict_firstIndex = {
  0, /* SDO_SVR */
  50, /* SDO_CLT */
  82, /* PDO_RCV */
  210, /* PDO_RCV_MAP */
  338, /* PDO_TRS */
  466 /* PDO_TRS_MAP */
};

const quick_index CanObjectDict_lastIndex = {
  0, /* SDO_SVR */
  81, /* SDO_CLT */
  209, /* PDO_RCV */
  337, /* PDO_RCV_MAP */
  465, /* PDO_TRS */   
  593 /* PDO_TRS_MAP */
};

/* 
 * To count at which received SYNC a PDO must be sent.
 * Even if no pdoTransmit are defined, at least one entry is computed
 * for compilations issues.
 */
s_PDO_status CanObjectDict_PDO_status[128] = {s_PDO_status_Initializer, };//备注:此空间的大小应和"PDO_TRS"的长度 =last - first 对应!!!

UNS32 CanObjectDict_valueRangeTest (UNS8 typeValue, void * value)
{
  switch (typeValue) {
    case valueRange_EMC:
      if (*(UNS8*)value != (UNS8)0) return OD_VALUE_RANGE_EXCEEDED;
      break;
  }
  return 0;
}

const UNS16 CanObjectDict_ObjdictSize = sizeof(CanObjectDict_objdict)/sizeof(CanObjectDict_objdict[0]); 

CO_Data CanObjectDict_Data = CANOPEN_NODE_DATA_INITIALIZER(CanObjectDict);

