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

// task间消息传递，由发送方分配，接收方负责回收
typedef struct _NST_TSK_MSG 
{
    UINT16  msgId;     /*  消息类型  */ 
    UINT16  rsvd; // for 4-byte align
    UINT8   msgBody [32]; /*消息内容，如果传的是一片内存则取第一个元素获得内存首地址，如果是结构体或数值则强制转换成对应的类型*/
} NST_TskMsg;

NST_STATUS PostTaskMsg(
    IN  OS_EVENT    *TskMsgQueue, 
    IN  UINT16  TskMsgId,
    IN  PUINT8  pMsgBody,
    IN  UINT8   MsgBodyLen);

#endif


