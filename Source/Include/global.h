/*
******************************************************************************
**
** Project     : 
** Description :    Global header file
** Author      :    NST                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/

#ifndef _GLOBAL_H
#define _GLOBAL_H


#include "includes.h"

extern OS_EVENT  * gpMacMngTskMsgQ;
extern OS_EVENT*  gpSysMngTskMsgQ;

typedef enum
{
    // Mlme Task Message Id
    MLME_SCAN_REQUEST_ID = 0x100,
    MLME_DIRECT_CFG_ID,    
    MLME_START_ID,
    MLME_STOP_ID,
    MLME_PS_MODE_SET_ID, 
    MLME_DEEP_SLEEP_SET_ID,
    MLME_TX_RATE_SET_ID,
    MLME_TX_PWR_LEVEL_SET_ID,
    MLME_AUTO_WAKEUP_ID,
    MLME_BCN_PRD_SET_ID,
    MLME_LISTEN_INTVL_SET_ID,
    MLME_PEER_AGE_OUT_SET_ID,
    MLME_PEER_KICK_OUT_ID,
    MLME_PROTECT_MODE_SET_ID,
    MLME_WPS_START_SET_ID,
    MLME_WPS_STOP_SET_ID,
    MLME_SNIFFER_START_ID,
    MLME_VENDOR_IE_SET_ID,
    
    // System Mgmt Task Message Id
    SYS_LINK_UP_ID= 0x200,
    SYS_LINK_DOWN_ID,
    SYS_DHCP_TIMEOUT_ID,
    SYS_SCAN_DONE_ID,
    SYS_STA_JOIN_FAIL_ID,
    SYS_DIRECT_CFG_DONE_ID
}SDK_TSK_MSG_ID;

// task����Ϣ���ݣ��ɷ��ͷ����䣬���շ��������
typedef struct _NST_TSK_MSG 
{
    UINT16  msgId;     /*  ��Ϣ����  */ 
    UINT16  rsvd; // for 4-byte align
    UINT8   msgBody [32]; /*��Ϣ���ݣ����������һƬ�ڴ���ȡ��һ��Ԫ�ػ���ڴ��׵�ַ������ǽṹ�����ֵ��ǿ��ת���ɶ�Ӧ������*/
} NST_TskMsg;

NST_STATUS PostTaskMsg(
    IN  OS_EVENT    *TskMsgQueue, 
    IN  UINT16  TskMsgId,
    IN  PUINT8  pMsgBody,
    IN  UINT8   MsgBodyLen);

#endif


