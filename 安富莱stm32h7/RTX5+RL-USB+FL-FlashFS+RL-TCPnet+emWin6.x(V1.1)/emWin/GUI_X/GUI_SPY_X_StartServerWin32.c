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
File        : GUI_SPY_X_StartServer.c
Purpose     : Start of the emWinSPY server in WIN32 simulation.
---------------------------END-OF-HEADER------------------------------
*/

#include <windows.h>

#include "GUI.h"
#include "GUI_Debug.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define PORT 2468 // emWinSPY-Port

static HINSTANCE _hLib;

static void _LoadWSock32(void) {
  if (_hLib == 0) {
    _hLib = LoadLibrary("WSock32.dll");
  }
}

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _ListenAtTcpAddr
*
* Starts listening at the given TCP port.
*/
static int _ListenAtTcpAddr(unsigned short port) {
  #define _TRY(x) if ((x) < 0) { closesocket(sock); return -1; }
  
  int sock;
  struct sockaddr_in addr;
  int one = 1;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  _TRY(sock = socket(AF_INET, SOCK_STREAM, 0));
  _TRY(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &one, sizeof(one)));
  _TRY(bind(sock, (struct sockaddr *)&addr, sizeof(addr)));
  _TRY(listen(sock, 5));
  return sock;
}

/*********************************************************************
*
*       _Send / _Recv
*
*  Function description:
*    Functions for sending and receiving data used by emWinSPY
*
*  Return value:
*    In case of success both functions should return the number 
*    of successfully transmitted bytes, otherwise -1.
*/
static int _Send(const U8 * buf, int len, void * pConnectInfo) {
  int r;

  if (len <= 0) {
    return -1;
  }
  r = send((SOCKET)pConnectInfo, buf, len, 0);
  if (r == SOCKET_ERROR) {
    return -1;
  }
  return r;
}

static int _Recv(U8 * buf, int len, void * pConnectInfo) {
  int r;

  if (len <= 0) {
    return -1;
  }
  r = recv((SOCKET)pConnectInfo, buf, len, 0);
  if (r == SOCKET_ERROR) {
    return -1;
  }
  return r;
}

/*********************************************************************
*
*       _ServerThread
*
*  Function description:
*    The server thread waits for an incoming connection. If a connection
*    has been established it calls GUI_SPY_Process() which should return
*    if the connection is closed.
*/
static UINT __stdcall _ServerThread(void * p) {
  WORD wVersionRequested;
  WSADATA wsaData;
  int r, s, sock, addrlen;
  struct sockaddr addr;
  
  GUI_USE_PARA(p);
  //
  // Initialize WinSock
  //
  wVersionRequested = MAKEWORD(2, 0);
  if (WSAStartup(wVersionRequested, &wsaData) != 0) {
    return 1;
  }
  s = _ListenAtTcpAddr(PORT);
  if (s < 0) {
    return 1; // Error
  }
  while (1) {
    //
    // Wait for an incoming connection
    //
    addrlen = sizeof(addr);
    if ((sock = accept(s, &addr, &addrlen)) < 0) {
      return 1; // Error
    }
    //
    // Execute emWinSPY
    //
    r = GUI_SPY_Process(_Send, _Recv, (void *)sock);
    //
    // Close connection
    //
    closesocket(sock);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SPY_X_StartServer
*/
int GUI_SPY_X_StartServer(void) {
  int ThreadId;
  HANDLE hThread;
  
  hThread = CreateThread(NULL, 0, _ServerThread, NULL, 0, &ThreadId);
  return hThread ? 0 : 1;
}

/*************************** End of file ****************************/
