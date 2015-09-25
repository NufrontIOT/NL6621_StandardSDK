/**
* @file lwIP.h
* @brief A brief file description.
* @details
*     A more elaborated file description.
* @author Wang Mengyin
* @date  2010Jul19 15:54:55
* @note
*               Copyright 2010 Wang Mengyin.ALL RIGHTS RESERVED.
*                            http://tigerwang202.blogbus.com
*    This software is provided under license and contains proprietary and
* confidential material which is the property of Company Name tech.
*/


#ifndef __LW_IP_H
#define __LW_IP_H

#include "includes.h"

typedef struct netif  NET_IF;

#define MAX_DHCP_TRIES        4

#define DHCP_STATE_IDLE	0
#define DHCP_STATE_RUN 	1
#define DHCP_STATE_STOP	2

#define SUBNET_MASK_ADR                 0xFFFFFF00


VOID LwIPInit(VOID);
INT32 L2LinkUp(VOID);
VOID L2LinkDown(VOID);
extern NET_IF netif;

#endif /* #ifndef __LW_IP_H */
/*-- File end --*/

