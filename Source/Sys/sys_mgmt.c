/*************************************************
 Copyright (C), 2013, BEIJING NUFRONT CO., LTD
 File name: dhcp_server.c     
 Author: jun.chen@nufront.com                  
 Version: V1.0.0     
 Date:  2013-7-22     
 Description: This file contains system management implementation.
 Others: ...      
 Function List:     
     1. ...
 History: 
     1. Date: ...
       Author: ...
       Modification: ...
     2. ...
*************************************************/
/*
******************************************************************************
**                               Include Files
******************************************************************************
*/
#include "includes.h"
#include "lwIP.h"
#include "uart.h"

/*
******************************************************************************
**                               MACROS and VARIABLES
******************************************************************************
*/
const INT8U FwCreatedDate[] = __DATE__;
const INT8U FwCreatedTime[] = __TIME__;
const INT8U FwType[] = "SDK";
const INT8U FwVerNum[3] = {
	0x01,  /* Main version */ 
	0x13, /* Sub version */
	0x00  /* Internal version */
};

SysEvtCallBack SysEvtCB = NULL;

extern VOID    NST_CoreTasksInit(VOID);

VOID DisplayVersion(VOID)
{
	DBGPRINT(DEBUG_TRACE, "\n\r");
	DBGPRINT(DEBUG_TRACE, "=========================================================\n\r");
	DBGPRINT(DEBUG_TRACE, " ********************** NL6621 SDK ********************* \n\r");
	DBGPRINT(DEBUG_TRACE, " *                                                     * \n\r");
	DBGPRINT(DEBUG_TRACE, " *    Version: %s%x.%02x.%02x(%s %s) %s    *\n\r", \
		FwType, FwVerNum[0], FwVerNum[1], FwVerNum[2],
		FwCreatedTime, FwCreatedDate,
		#if DEBUG_ON
		"dbg"
		#else
		"rel"
		#endif
	);
	DBGPRINT(DEBUG_TRACE, " *                                                     * \n\r");
	DBGPRINT(DEBUG_TRACE, " ******************************************************* \n\r");
	DBGPRINT(DEBUG_TRACE, "=========================================================\n\r\n\r");
}

VOID SystemInit(VOID)
{
    BSP_ClkInit();
    
    InitRomHookFuncTbl();

    OSInit();
    
    NST_CoreTasksInit();

    BSP_Init();
}

void DefaultProc(NST_TskMsg *msg)
{
	if (msg == NULL)
		return;

	switch (msg->msgId)
	{
		case SYS_LINK_UP_ID:    /*WiFi Connected*/
		{	
		#ifdef LWIP_SUPPORT
			if (L2LinkUp() != 0)
				break;
		#endif

			if (SysEvtCB)
				(*SysEvtCB)(SYS_EVT_LINK_UP);
			break;
		}

		case SYS_LINK_DOWN_ID:    /*WiFi Disconnected*/
		{
		#ifdef LWIP_SUPPORT
			L2LinkDown();
		#endif

			if (SysEvtCB)
				(*SysEvtCB)(SYS_EVT_LINK_DOWN);
			break;
		}

		case SYS_STA_JOIN_FAIL_ID:	/* sta failed to join */
		{
			if (SysEvtCB)
				(*SysEvtCB)(SYS_EVT_JOIN_FAIL);
			break;
		}

		case SYS_DHCP_TIMEOUT_ID:    /*dhcp client timeout */
		{
			if (SysEvtCB)
				(*SysEvtCB)(SYS_EVT_DHCP_FAIL);
			break;
		}

		case SYS_SCAN_DONE_ID:
			if (SysEvtCB)
				(*SysEvtCB)(SYS_EVT_SCAN_DONE);
			break;

		case SYS_DIRECT_CFG_DONE_ID:
			if (SysEvtCB)
				(*SysEvtCB)(SYS_EVT_DIRECT_CFG_DONE);
			break;
		
		default:
			break;
	}
}

VOID SysMgmtMain(VOID * pParam)
{
	NST_TskMsg* pTaskMsg;
    
#ifdef CPU_USAGE_STAT
    OSTimeDly(100);
#endif // CPU_USAGE_STAT //

	DisplayVersion();

	while (1)
	{
		if (NST_GetMsg(gpSysMngTskMsgQ, &pTaskMsg ) != OS_ERR_NONE)
			continue;

		if (pTaskMsg != NULL)
		{
			DefaultProc(pTaskMsg);

			NST_FreeTskMsg(pTaskMsg);
		}
	}
}

