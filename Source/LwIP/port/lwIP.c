/*
******************************************************************************
**
** Project     : 
** Description :    file for LWIP Ports
** Author      :    yds                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/

/*
******************************************************************************
**                               Include Files
******************************************************************************
*/

#include "lwIP/memp.h"
#include "lwIP.h"
#include "lwIP/tcp.h"
#include "lwIP/udp.h"
#include "lwIP/tcpip.h"
#include "netif/etharp.h"
#include "lwIP/dhcp.h"
#include "ethernetif.h"
#include "arch/sys_arch.h"
#include "lwIP/inet.h"
#include "lwIP/netdb.h"
#include "lwIP/dns.h"
#include "../App/DhcpServer/dhcp_server.h"
#include "../App/DnsServer/dns_server.h"

#ifdef LWIP_SUPPORT

/*
******************************************************************************
**                               MACROS and VARIABLES
******************************************************************************
*/
NET_IF netif;
extern BOOL_T LinkReady;

void TcpipInitDone(void *arg)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;
    
    // init ip, mask, gw to zero
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
    NST_ZERO_MEM(&netif, sizeof(NET_IF));
    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&netif);
}

/*
******************************************************************************
**                  VOID LwIPInit(VOID)
**
** Description  : initialize the LwIP
** Arguments    : 
                  pParam
                  
** Returns      : нч
** Author       :                                     
** Date         : 
**
******************************************************************************
*/
VOID LwIPInit(VOID)
{
    tcpip_init(NULL, NULL);

    TcpipInitDone(NULL);

    return;
}

static LoadStaticParam()
{
    CFG_PARAM *pParm = &SysParam;
    struct ip_addr addr;
    
    IP4_ADDR(&addr, \
        pParm->IPCfg.Ip[0],\
        pParm->IPCfg.Ip[1],\
        pParm->IPCfg.Ip[2],\
        pParm->IPCfg.Ip[3]);
    netif_set_ipaddr(&netif, &addr);
    
    IP4_ADDR(&addr, \
        pParm->IPCfg.Netmask[0],\
        pParm->IPCfg.Netmask[1],\
        pParm->IPCfg.Netmask[2],\
        pParm->IPCfg.Netmask[3]);
    netif_set_netmask(&netif, &addr);
        
    IP4_ADDR(&addr, \
        pParm->IPCfg.GateWay[0],\
        pParm->IPCfg.GateWay[1],\
        pParm->IPCfg.GateWay[2],\
        pParm->IPCfg.GateWay[3]);
    netif_set_gw(&netif, &addr);
        
    IP4_ADDR(&addr, \
        pParm->IPCfg.Dns[0],\
        pParm->IPCfg.Dns[1],\
        pParm->IPCfg.Dns[2],\
        pParm->IPCfg.Dns[3]);
    dns_setserver(0, &addr);

    netif_set_up(&netif);

    DBGPRINT(DEBUG_TRACE, "USE STATIC IP ADDRESS:%s !!!\n", inet_ntoa(netif.ip_addr.addr));
}

#if LWIP_DHCP
/*
* ifUpWhenDhcp:   netif inf link status is up , then layer3 address should be got
* author: yds
*/
static INT32 ifUpWhenDhcp()
{
    dhcp_start(&netif);

    while(netif.ip_addr.addr == 0)
    {
        OSTimeDly(20);

        /* If no response from a DHCP server for DhcpTryTimes */
        /* stop the dhcp client and notify application layer */
        if ((LinkReady == NST_FALSE) || 
            (SysParam.IPCfg.DhcpTryTimes !=0 && 
            netif.dhcp->tries > SysParam.IPCfg.DhcpTryTimes))
        {
            dhcp_stop(&netif);
            PostTaskMsg(gpSysMngTskMsgQ, SYS_DHCP_TIMEOUT_ID, NULL, 0);            
            return -1;
        }
    }
    netif_set_up(&netif);

    DBGPRINT(DEBUG_TRACE, "[DHCP] MY IP ADDRESS:%s \n", inet_ntoa(netif.ip_addr.addr));
    return 0;
}
#endif

INT32 L2LinkUp(VOID)
{
	INT32 Ret = 0;
	CFG_PARAM *pParm = &SysParam;
	   
#if LWIP_DHCP
	if ((pParm->WiFiCfg.Protocol == PARAM_NET_MODE_STA_BSS) && pParm->IPCfg.bDhcp)
	{
		Ret = ifUpWhenDhcp();
	}
	else
#endif
	{
		LoadStaticParam();
	}

	if (pParm->WiFiCfg.Protocol == PARAM_NET_MODE_SOFTAP)
	{
#ifdef DHCP_SERVER_SUPPORT
		if (pParm->bDhcpServer)
			Ret = DHCPS_Start(&netif);
#endif

#ifdef DNS_SERVER_SUPPORT
		if (pParm->bDnsServer)
			Ret = DNSS_Start(&netif, pParm->DnsName);
#endif
	}

	return (Ret == 0) ? 0 : -1;
}

VOID L2LinkDown(VOID)
{
	CFG_PARAM *pParm = &SysParam;
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;

	// down netif
	netif_set_down(&netif);

	// stop dhcp client
#if LWIP_DHCP
	if ((pParm->WiFiCfg.Protocol == PARAM_NET_MODE_STA_BSS) && pParm->IPCfg.bDhcp)
		dhcp_stop(&netif);
#endif

	// clear ip
	ipaddr.addr = 0x0;
	netmask.addr = htonl(SUBNET_MASK_ADR);
	gw.addr = 0x0;
	netif_set_addr(&netif, &ipaddr , &netmask, &gw);

	if (pParm->WiFiCfg.Protocol == PARAM_NET_MODE_SOFTAP)
	{
#ifdef DHCP_SERVER_SUPPORT
		if (pParm->bDhcpServer)
			DHCPS_Stop();
#endif

#ifdef DNS_SERVER_SUPPORT
		if (pParm->bDnsServer)
			DNSS_Stop();
#endif
	}

	// todo inform the application layer
}

/*
******************************************************************************
**                            End Of File                                   **
******************************************************************************
*/

#endif

