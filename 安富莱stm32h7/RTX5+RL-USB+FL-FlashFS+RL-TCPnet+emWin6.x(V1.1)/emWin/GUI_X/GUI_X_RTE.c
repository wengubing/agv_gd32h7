/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.46 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011 
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : GUI_X_RTE.c
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "RTE_Components.h"
#ifdef RTE_CMSIS_RTOS2
#include "cmsis_os2.h"
#else
#include "cmsis_os.h"
#endif

#include "GUI.h"

/*********************************************************************
*
*       Global data
*/

#ifndef RTE_CMSIS_RTOS2
static volatile GUI_TIMER_TIME OS_TimeMS;
#endif

/*********************************************************************
*
*       GUI Tick (executed each ms)
*/

#ifndef RTE_CMSIS_RTOS2
static void GUI_Tick (void const *argument) {
  (void)argument;
  OS_TimeMS++;
}
#endif

/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
*/


GUI_TIMER_TIME GUI_X_GetTime(void) {
#ifdef RTE_CMSIS_RTOS2
  return ((GUI_TIMER_TIME)(osKernelGetTickCount())); 
#else
  return OS_TimeMS; 
#endif
}

void GUI_X_Delay(int ms) {
  osDelay((uint32_t)ms);
}

/*********************************************************************
*
*       GUI_X_Init()
*
* Note:
*     GUI_X_Init() is called from GUI_Init is a possibility to init
*     some hardware which needs to be up and running before the GUI.
*     If not required, leave this routine blank.
*/

#ifndef RTE_CMSIS_RTOS2
osTimerDef(GUI_Timer, GUI_Tick);
static osTimerId GUI_TimerId;
#endif

#ifdef RTE_Graphics_Touchscreen
extern void GUI_TOUCH_Initialize(void);
extern void GUI_TOUCH_Exec(void);
#ifdef RTE_CMSIS_RTOS2
static osTimerId_t GUI_TOUCH_TimerId;
static void GUI_TOUCH_Tick(void *argument)       { (void)argument; GUI_TOUCH_Exec(); }
#else
static osTimerId   GUI_TOUCH_TimerId;
static void GUI_TOUCH_Tick(void const *argument) { (void)argument; GUI_TOUCH_Exec(); }
osTimerDef (GUI_TOUCH_Timer, GUI_TOUCH_Tick);
#endif
#endif

#ifdef RTE_Graphics_Joystick
extern void GUI_JOYSTICK_Initialize(void);
extern void GUI_JOYSTICK_Exec(void);
#ifdef RTE_CMSIS_RTOS2
static osTimerId_t GUI_JOYSTICK_TimerId;
static void GUI_JOYSTICK_Tick(void *argument)       { (void)argument; GUI_JOYSTICK_Exec(); }
#else
static osTimerId   GUI_JOYSTICK_TimerId;
static void GUI_JOYSTICK_Tick(void const *argument) { (void)argument; GUI_JOYSTICK_Exec(); }
osTimerDef (GUI_JOYSTICK_Timer, GUI_JOYSTICK_Tick);
#endif
#endif

void GUI_X_Init(void) {

#ifndef RTE_CMSIS_RTOS2
  GUI_TimerId = osTimerCreate(osTimer(GUI_Timer), osTimerPeriodic, NULL);
  osTimerStart(GUI_TimerId, 1);
#endif

#ifdef RTE_Graphics_Touchscreen
  GUI_TOUCH_Initialize();
#ifdef RTE_CMSIS_RTOS2
  GUI_TOUCH_TimerId = osTimerNew(&GUI_TOUCH_Tick, osTimerPeriodic, NULL, NULL);
#else
  GUI_TOUCH_TimerId = osTimerCreate(osTimer(GUI_TOUCH_Timer), osTimerPeriodic, NULL);
#endif
  osTimerStart(GUI_TOUCH_TimerId, 20U);
#endif

#ifdef RTE_Graphics_Joystick
  GUI_JOYSTICK_Initialize();
#ifdef RTE_CMSIS_RTOS2
  GUI_JOYSTICK_TimerId = osTimerNew(&GUI_JOYSTICK_Tick, osTimerPeriodic, NULL, NULL);
#else
  GUI_JOYSTICK_TimerId = osTimerCreate(osTimer(GUI_JOYSTICK_Timer), osTimerPeriodic, NULL);
#endif
  osTimerStart(GUI_JOYSTICK_TimerId, 50U);
#endif

}

/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) {
  osDelay(1U);
}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }

/*********************************************************************
*
*      Multitasking:
*
*                 GUI_X_InitOS()
*                 GUI_X_GetTaskId()
*                 GUI_X_Lock()
*                 GUI_X_Unlock()
*
* Note:
*   The following routines are required only if emWin is used in a
*   true multi task environment, which means you have more than one
*   thread using the emWin API.
*   In this case the
*                       #define GUI_OS 1
*  needs to be in GUIConf.h
*/

#ifdef RTE_CMSIS_RTOS2
static osMutexId_t      GUI_MutexId;
static osEventFlagsId_t GUI_EventId;
//static osSemaphoreId_t GUI_SemId;  
#else
osMutexDef(GUI_Mutex);
static osMutexId        GUI_MutexId;
#endif

void GUI_X_InitOS(void) {
#ifdef RTE_CMSIS_RTOS2
  GUI_MutexId = osMutexNew(NULL);
  GUI_EventId = osEventFlagsNew(NULL);
	//GUI_SemId =  osSemaphoreNew(1, 1, NULL);
#else
  GUI_MutexId = osMutexCreate(osMutex(GUI_Mutex));
#endif
}

void GUI_X_Unlock(void) {
  osMutexRelease(GUI_MutexId);
//	osSemaphoreRelease (GUI_SemId);      
}

void GUI_X_Lock(void) {
#ifdef RTE_CMSIS_RTOS2
	 //osSemaphoreAcquire (GUI_SemId, osWaitForever);
  osMutexAcquire(GUI_MutexId, osWaitForever);
#else
  osMutexWait   (GUI_MutexId, osWaitForever);
#endif
}

U32  GUI_X_GetTaskId(void) {
  return ((U32)(osThreadGetId()));
}

/*********************************************************************
*
*      Event driving (optional with multitasking)
*
*                 GUI_X_WaitEvent()
*                 GUI_X_WaitEventTimed()
*                 GUI_X_SignalEvent()
*/

#ifndef RTE_CMSIS_RTOS2
static osThreadId GUI_ThreadId = NULL;
#endif

void GUI_X_WaitEvent(void) {
#ifdef RTE_CMSIS_RTOS2
  osEventFlagsWait(GUI_EventId, 0x00000001U, osFlagsWaitAny, osWaitForever);
#else
  GUI_ThreadId = osThreadGetId();
  osSignalWait(0x00000001, osWaitForever); 
#endif
}

void GUI_X_WaitEventTimed(int Period) {
#ifdef RTE_CMSIS_RTOS2
  osEventFlagsWait(GUI_EventId, 0x00000001U, osFlagsWaitAny, (uint32_t)Period);
#else
  GUI_ThreadId = osThreadGetId();
  osSignalWait(0x00000001, (uint32_t)Period); 
#endif
}

void GUI_X_SignalEvent(void) {
#ifdef RTE_CMSIS_RTOS2
  osEventFlagsSet (GUI_EventId, 0x00000001U);
#else
  if (GUI_ThreadId) {
    osSignalSet(GUI_ThreadId, 0x00000001);
  }
#endif
}

/*************************** End of file ****************************/
