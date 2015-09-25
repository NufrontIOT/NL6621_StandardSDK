/*************************************************
 Copyright (C), 2013, BEIJING NUFRONT CO., LTD
 File name: dhcp_server.c     
 Author: jun.chen@nufront.com                  
 Version: V1.0.0     
 Date:  2013-7-12     
 Description: This file contains DHCP server implementation.
 Others: ...      
 Function List:     
     1. ...
 History: 
     1. Date: ...
       Author: ...
       Modification: ...
     2. ...
*************************************************/
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/timers.h"
#include "netif/etharp.h"
#include "err.h"
#include "includes.h"
#include "dhcp_server.h"

extern UINT8 APGetStationStatus(IN UINT8 * MacAddr);


#ifdef DHCP_SERVER_SUPPORT
static DHCP_SERVER DhcpServer;
static DHCP_MSG DhcpMsg;


#define DHCP_SET_OPTION_SUBNET_MASK(buffer, mask, len)	\
	{	\
		INT32U tmp;	\
		*buffer++ = DHCP_OPTION_ID_SUBNET_MASK;	\
		*buffer++ = 4;	\
		tmp = mask;	\
		NST_MOVE_MEM(buffer, &tmp, 4);	\
		buffer += 4;	\
		len += 6;	\
	}

#define DHCP_SET_OPTION_GW(buffer, gw, len)	\
	{	\
		INT32U tmp;	\
		*buffer++ = DHCP_OPTION_ID_DEF_GW;	\
		*buffer++ = 4;	\
		tmp = gw;	\
		NST_MOVE_MEM(buffer, &tmp, 4);	\
		buffer += 4;	\
		len += 6;	\
	}

#define DHCP_SET_OPTION_DNS(buffer, dns, len)	\
	{	\
		INT32U tmp;	\
		*buffer++ = DHCP_OPTION_ID_DNS_SERVER;	\
		*buffer++ = 4;	\
		tmp = dns;	\
		NST_MOVE_MEM(buffer, &tmp, 4);	\
		buffer += 4;	\
		len += 6;	\
	}

#define DHCP_SET_OPTION_SERVER_ID(buffer, ip, len)	\
	{	\
		INT32U tmp;	\
		*buffer++ = DHCP_OPTION_ID_SERVER_ID;	\
		*buffer++ = 4;	\
		tmp = ip;	\
		NST_MOVE_MEM(buffer, &tmp, 4);	\
		buffer += 4;	\
		len += 6;	\
	}

#define DHCP_SET_OPTION_MSG_TYPE(buffer, type, len)	\
	{	\
		*buffer++ = DHCP_OPTION_ID_MSG_TYPE;	\
		*buffer++ = 1;	\
		*buffer++ = type;	\
		len += 3;	\
	}

#define DHCP_SET_OPTION_LEASE_TIME(buffer, time, len)	\
	{	\
		INT32U tmp;	\
		*buffer++ = DHCP_OPTION_ID_LEASE_TIME;	\
		*buffer++ = 4;	\
		tmp = htonl(time);	\
		NST_MOVE_MEM(buffer, &tmp, 4);	\
		buffer += 4;	\
		len += 6;	\
	}

#define DHCP_SET_OPTION_END(buffer, len)	\
	{	\
		*buffer++ = DHCP_OPTION_ID_END;	\
		len ++;	\
	}

static VOID _DhcpTickHandle(VOID * Arg)
{
	INT8U i;
	PDHCP_CLIENT pClient;

	if (DhcpServer.Enable == 0)
	{
		return;
	}
	
	for (i = 0; i < DHCPS_HISTORY_CLIENT_NUM; i++)
	{
		pClient = &DhcpServer.Clients[i];
		if (pClient->State == DHCP_CLIENT_STATE_REQUEST)
		{
			if (pClient->Timeout && (--pClient->Timeout == 0))
			{
				/* Timeout for the client's request frame. */
				memset(pClient->MacAddr, 0, 6);
				pClient->State = DHCP_CLIENT_STATE_IDLE;
			}
		}
		else if (pClient->State == DHCP_CLIENT_STATE_BIND)
		{
			if(pClient->Lease && (--pClient->Lease == 0))
			{
				/* The lease time over. */
				pClient->State = DHCP_CLIENT_STATE_IDLE;
			}
			#ifdef CONFIG_AP_SUPPORT
			else if (APGetStationStatus(pClient->MacAddr) == 0)
			{
				/* The client leave the wireless network. */
				pClient->State = DHCP_CLIENT_STATE_IDLE;
			}
			#endif
		}
	}
	
	if (DhcpServer.Enable) 
		sys_timeout(DHCP_TICK_TIME, _DhcpTickHandle, NULL);
}

static INT8U _ParseDhcpOptions(PDHCP_MSG pMsg, INT8U * pMsgType, INT32U * pReqIpAddr, INT32U * pServerId)
{
	INT8U * pDhcpOptions, * pEnd;
	INT32U Len;
	INT8U Ret;

	pDhcpOptions = pMsg->Options;
	pEnd = (INT8U *)(pMsg) + sizeof(DHCP_MSG);
	Len = 0;
	Ret = 0;
	
	while((*pDhcpOptions != DHCP_OPTION_ID_END) && (pDhcpOptions < pEnd))
	{
		if (*pDhcpOptions == DHCP_OPTION_ID_PAD)
		{
			/* Skip the pad. */
			pDhcpOptions += 1;
		}
		else
		{
			Len = (*(pDhcpOptions + 1));
			if(*pDhcpOptions == DHCP_OPTION_ID_MSG_TYPE)
			{
				/* Get the message type. */
				pDhcpOptions += 2;
				*pMsgType = *pDhcpOptions;
				Ret |= 0x01;
			}
			else if(*pDhcpOptions == DHCP_OPTION_ID_REQ_IP_ADDR)
			{
				/* Get the client's requested ip address. */
				pDhcpOptions += 2;
				NST_MOVE_MEM((INT8U*)pReqIpAddr, pDhcpOptions,4 );
				Ret |= 0x02;
			}
			else if(*pDhcpOptions == DHCP_OPTION_ID_SERVER_ID)
			{
				/* Get the server's ip address. */
				pDhcpOptions += 2;
				NST_MOVE_MEM((INT8U*)pServerId, pDhcpOptions,4 );
				Ret |= 0x04;
			}
			else
			{
				/* Dropped the other options. */
				pDhcpOptions += 2;
			}

			pDhcpOptions += Len;
		}
	}

	return Ret;
}

static PDHCP_CLIENT _ClientTableLookup(INT8U * MacAddr, INT8U MsgType, INT32U ReqIpAddr, INT32U ServerId)
{
	INT8U i;
	INT8U IpUnavailable;
	PDHCP_CLIENT pClient;
	PDHCP_CLIENT pFreeClient;
	PDHCP_CLIENT pReqClient;
	PDHCP_CLIENT pMyHistoryClient;
	PDHCP_CLIENT pHistoryClient;
	PDHCP_CLIENT pMyClient;
	PDHCP_CLIENT pReturnClient;

	pFreeClient = NULL;
	pReqClient = NULL;
	pHistoryClient= NULL;
	pMyHistoryClient = NULL;
	pMyClient = NULL;
	pReturnClient = NULL;
	IpUnavailable = 0;
	
	for(i = 0; i < DHCPS_HISTORY_CLIENT_NUM; i++)
	{
		pClient = &DhcpServer.Clients[i];
		if(pClient->State == DHCP_CLIENT_STATE_IDLE)
		{	
			if((pMyHistoryClient == NULL) && (memcmp(pClient->MacAddr, MacAddr, 6) == 0))
			{
				/* Get my history entry. */
				pMyHistoryClient = pClient;
			}

			if((pReqClient == NULL) && ip_addr_cmp(&pClient->IpAddr, (struct ip_addr *)&ReqIpAddr) )
			{
				/* Get the idle entry that hold my requested ip address. */
				pReqClient = pClient;
			}

			if((pFreeClient == NULL) && (memcmp(pClient->MacAddr, "\x00\x00\x00\x00\x00\x00", 6) == 0) )
			{
				/* Get the first free entry. */
				pFreeClient = pClient;
			}

			if((pHistoryClient == NULL) && (memcmp(pClient->MacAddr, "\x00\x00\x00\x00\x00\x00", 6) != 0))
			{
				/* Get the first histoy entry that not belong to me. */
				pHistoryClient = pClient;
			}
		}
		else
		{
			if(memcmp(pClient->MacAddr, MacAddr, 6) == 0)
			{
				/* Is negotiating ip address or has negotiated now. */
				pMyClient = pClient;
			}
			else if(ip_addr_cmp(&pClient->IpAddr, (struct ip_addr *)&ReqIpAddr))
			{
				/* The requested ip address is allocated. */
				IpUnavailable = 1;
			}
		}
	}

	switch(MsgType)
	{
		case DHCP_MSG_DISCOVER:
			if(pMyClient)
			{
				/* Amazing!!The client restart the negotiation. */
				pMyClient->State = DHCP_CLIENT_STATE_IDLE;
				
				if(pReqClient)
				{
					/* The client request another ip address and that address is not allocated now. */
					if(pMyClient->State != DHCP_CLIENT_STATE_BIND)
					{
						memset(pMyClient->MacAddr, 0, 6);
					}
					pReturnClient = pReqClient;
				}
				else
				{
					/* The client request the same ip address. */
					pReturnClient = pMyClient;
				}
			}
			else
			{
				/* A new negotiation! */

				/*
					The IP address allocation priority(high to low):
					1. the client's request ip address.
					2. the client's history ip address.
					3. a totally free ip address.
					4. the other client's history ip address.
				*/
				if(pReqClient)
				{
					/* The client's request ip address. */
					pReturnClient = pReqClient;
				}
				else if(pMyHistoryClient)
				{
					/* The client's history ip address. */
					pReturnClient = pMyHistoryClient;
				}
				else if(pFreeClient)
				{
					/* A totally free ip address. */
					pReturnClient = pFreeClient;
				}
				else if(pHistoryClient)
				{
					/* The other client's history ip address. */
					pReturnClient = pHistoryClient;
				}
				else
				{
					/* The IP pool is full, so return null. */
					pReturnClient = NULL;
				}
			}
			break;

		case DHCP_MSG_REQUEST:
			if(pMyClient)
			{
				if(IpUnavailable == 1)
				{
					/* The client request a new address that was allocated, so return null. */
					pReturnClient = NULL;
					
					if(pMyClient->State != DHCP_CLIENT_STATE_BIND)
					{
						memset(pMyClient->MacAddr, 0, 6);
					}
					pMyClient->State = DHCP_CLIENT_STATE_IDLE;
				}
				else
				{
					if(pReqClient)
					{
						/* The client request a new address that is not allocated. */
						if(pMyClient->State != DHCP_CLIENT_STATE_BIND)
						{
							memset(pMyClient->MacAddr, 0, 6);
						}
						pMyClient->State = DHCP_CLIENT_STATE_IDLE;

						if((ServerId == 0) || (DhcpServer.ServerIpAddr.addr == ServerId))
						{
							/* The client request the new address and that is free, allocate it. */
							pReturnClient = pReqClient;
						}
						else
						{
							/* The client refuses my offer and request another ip address. */
							pReturnClient = NULL;
						}
					}
					else if((ServerId == 0) || (DhcpServer.ServerIpAddr.addr == ServerId))
					{
						/* The client request this address that has been allocated to it(a little abnorm) or the client renew the lease time. */
						pReturnClient = pMyClient;
					}
					else
					{
						/* The client refuses my offer. */
						pReturnClient = NULL;
					}
				}
			}
			else
			{
				if(IpUnavailable == 1)
				{
					/* The requested ip address was allocated, so return null. */
					pReturnClient = NULL;
				}
				else if((ServerId == 0) || (DhcpServer.ServerIpAddr.addr == ServerId))
				{
					/* The client request my free address, so allocate it. */
					pReturnClient = pReqClient;
				}
				else
				{
					/* The client request the address from another server, but send it to me, so return null. */
					pReturnClient = NULL;
				}
			}
			break;

		case DHCP_MSG_RELEASE:
			/* The client release the ip address. */
			if(pMyClient)
			{
				if(pMyClient->State != DHCP_CLIENT_STATE_BIND)
				{
					memset(pMyClient->MacAddr, 0, 6);
				}
				pMyClient->State = DHCP_CLIENT_STATE_IDLE;
			}
			pReturnClient = NULL;
			break;

		case DHCP_MSG_DECLINE:
			/* The client refuses my offer directly. */
			if(pMyClient)
			{
				if(pMyClient->State != DHCP_CLIENT_STATE_BIND)
				{
					memset(pMyClient->MacAddr, 0, 6);
				}
				pMyClient->State = DHCP_CLIENT_STATE_IDLE;
			}
			pReturnClient = NULL;
			break;

		default:
			/* Dropped the other frames. */
			pReturnClient = NULL;
			break;
	}

	if(pReturnClient)
	{
		/* Updata the client's MAC address. */
		NST_MOVE_MEM(pReturnClient->MacAddr, MacAddr, 6);
	}

	return pReturnClient;
}

static VOID _DHCPNakGenAndSend(INT8U * pClientMacAddr, INT32U Xid)
{
	INT32U Len;
	INT8U * Body;
	PDHCP_MSG pDhcpMsg;
	struct pbuf * pDhcpBuf;

	pDhcpBuf = pbuf_alloc(PBUF_TRANSPORT, sizeof(DHCP_MSG), PBUF_RAM);
	if(pDhcpBuf == NULL)
	{
		return;
	}
	
	pDhcpMsg = &DhcpMsg;
	memset(pDhcpMsg, 0, sizeof(*pDhcpMsg));

	/* Initialize the DHCP message header. */
	pDhcpMsg->Op = DHCP_OP_REPLY;
	pDhcpMsg->HType = DHCP_HWTYPE_ETHERNET;
	pDhcpMsg->HLen = 6;
	pDhcpMsg->Xid = htonl(Xid);
	pDhcpMsg->Siaddr = DhcpServer.ServerIpAddr.addr;
	NST_MOVE_MEM(pDhcpMsg->Chaddr, pClientMacAddr, 6);
	pDhcpMsg->Magic = htonl(DHCP_MAGIC);

	Len = 240;
	Body = &pDhcpMsg->Options[0];

	/* Set the message type. */
	DHCP_SET_OPTION_MSG_TYPE(Body, DHCP_MSG_NAK, Len);

	DHCP_SET_OPTION_END(Body, Len);

	pbuf_take(pDhcpBuf, (const VOID *)pDhcpMsg, Len);
	pbuf_realloc(pDhcpBuf, Len);

	/* Send broadcast to the DHCP client. */
	udp_sendto(DhcpServer.Socket, pDhcpBuf, IP_ADDR_BROADCAST, DHCP_CLIENT_UDP_PORT);
	pbuf_free(pDhcpBuf);
}

static VOID _DHCPAckGenAndSend(PDHCP_CLIENT pClient, INT8U * pClientMacAddr, INT32U Xid)
{
	INT32U Len;
	INT8U * Body;
	PDHCP_MSG pDhcpMsg;
	struct pbuf * pDhcpBuf;

	pDhcpBuf = pbuf_alloc(PBUF_TRANSPORT, sizeof(DHCP_MSG), PBUF_RAM);
	if(pDhcpBuf == NULL)
	{
		return;
	}
	
	pDhcpMsg = &DhcpMsg;
	memset(pDhcpMsg, 0, sizeof(*pDhcpMsg));

	/* Initialize the DHCP message header. */
	pDhcpMsg->Op = DHCP_OP_REPLY;
	pDhcpMsg->HType = DHCP_HWTYPE_ETHERNET;
	pDhcpMsg->HLen = 6;
	pDhcpMsg->Xid = htonl(Xid);
	pDhcpMsg->Yiaddr = pClient->IpAddr.addr;
	pDhcpMsg->Siaddr = 0;
	NST_MOVE_MEM(pDhcpMsg->Chaddr, pClientMacAddr, 6);
	pDhcpMsg->Magic = htonl(DHCP_MAGIC);

	Len = 240;
	Body = &pDhcpMsg->Options[0];
	
	/* Set the message type. */
	DHCP_SET_OPTION_MSG_TYPE(Body, DHCP_MSG_ACK, Len);

	/* Set the lease time. */
	DHCP_SET_OPTION_LEASE_TIME(Body, DHCP_DEFAULT_LEASE_TIME, Len);

	/* Set the server's ip address */
	DHCP_SET_OPTION_SERVER_ID(Body, DhcpServer.ServerIpAddr.addr, Len);

	/* Set the subnet mask. */
	DHCP_SET_OPTION_SUBNET_MASK(Body, DhcpServer.SubnetMask.addr, Len);

	/* Set the default gatway's ip address. */
	DHCP_SET_OPTION_GW(Body, DhcpServer.GateWay.addr, Len);

	/* Set the dns server's ip address. */
	DHCP_SET_OPTION_DNS(Body, DhcpServer.Dns1.addr, Len);

	DHCP_SET_OPTION_END(Body, Len);

	pbuf_take(pDhcpBuf, (const VOID *)pDhcpMsg, Len);
	pbuf_realloc(pDhcpBuf, Len);

	/* Send broadcast to the DHCP client. */
	udp_sendto(DhcpServer.Socket, pDhcpBuf, IP_ADDR_BROADCAST, DHCP_CLIENT_UDP_PORT);
	pbuf_free(pDhcpBuf);
}

static VOID _DHCPOfferGenAndSend(PDHCP_CLIENT pClient, INT8U * pClientMacAddr, INT32U Xid)
{
	INT32U Len;
	INT8U * Body;
	PDHCP_MSG pDhcpMsg;
	struct pbuf * pDhcpBuf;

	pDhcpBuf = pbuf_alloc(PBUF_TRANSPORT, sizeof(DHCP_MSG), PBUF_RAM);
	if(pDhcpBuf == NULL)
	{
		return;
	}
	
	pDhcpMsg = &DhcpMsg;
	memset(pDhcpMsg, 0, sizeof(*pDhcpMsg));

	/* Initialize the DHCP message header. */
	pDhcpMsg->Op = DHCP_OP_REPLY;
	pDhcpMsg->HType = DHCP_HWTYPE_ETHERNET;
	pDhcpMsg->HLen = 6;
	pDhcpMsg->Xid = htonl(Xid);
	pDhcpMsg->Yiaddr = pClient->IpAddr.addr;
	pDhcpMsg->Siaddr = 0;
	NST_MOVE_MEM(pDhcpMsg->Chaddr, pClientMacAddr, 6);
	pDhcpMsg->Magic = htonl(DHCP_MAGIC);

	Len = 240;
	Body = &pDhcpMsg->Options[0];

	/* Set the message type. */
	DHCP_SET_OPTION_MSG_TYPE(Body, DHCP_MSG_OFFER, Len);

	/* Set the lease time. */
	DHCP_SET_OPTION_LEASE_TIME(Body, DHCP_DEFAULT_LEASE_TIME, Len);

	/* Set the server's ip address */
	DHCP_SET_OPTION_SERVER_ID(Body, DhcpServer.ServerIpAddr.addr, Len);

	/* Set the subnet mask. */
	DHCP_SET_OPTION_SUBNET_MASK(Body, DhcpServer.SubnetMask.addr, Len);

	/* Set the default gatway's ip address. */
	DHCP_SET_OPTION_GW(Body, DhcpServer.GateWay.addr, Len);

	/* Set the dns server's ip address. */
	DHCP_SET_OPTION_DNS(Body, DhcpServer.Dns1.addr, Len);

	DHCP_SET_OPTION_END(Body, Len);

	pbuf_take(pDhcpBuf, (const VOID *)pDhcpMsg, Len);
	pbuf_realloc(pDhcpBuf, Len);

	/* Send broadcast to the DHCP client. */
	udp_sendto(DhcpServer.Socket, pDhcpBuf, IP_ADDR_BROADCAST, DHCP_CLIENT_UDP_PORT);
	pbuf_free(pDhcpBuf);
}

static VOID _CleanClientHistory(INT8U * pClientMacAddr)
{
	INT8U i;
	PDHCP_CLIENT pClient;
	
	for(i = 0; i < DHCPS_HISTORY_CLIENT_NUM; i++)
	{
		pClient = &DhcpServer.Clients[i];
		if((pClient->State == DHCP_CLIENT_STATE_IDLE) && (memcmp(pClient->MacAddr, pClientMacAddr, 6) == 0))
		{
			/* Clean the history client's Mac address. */
			memset(pClient->MacAddr, 0, 6);
		}
	}
}

static VOID _DhcpClientSMEHandle(PDHCP_CLIENT pClient, INT8U MsgType, INT32U Xid, INT8U * MacAddr)
{
	switch(pClient->State)
	{
		case DHCP_CLIENT_STATE_IDLE:
			if(MsgType == DHCP_MSG_DISCOVER)
			{
				/* Receive the "DISCOVER" frame, switch the state to "SELECT". */
				pClient->State = DHCP_CLIENT_STATE_SELECT;
			}
			else if(MsgType == DHCP_MSG_REQUEST)
			{
				/* If the requested ip is not allocated, allocate it. */
				_DHCPAckGenAndSend(pClient, pClient->MacAddr, Xid);
				pClient->Lease = DHCP_DEFAULT_LEASE_TIME;
				pClient->State = DHCP_CLIENT_STATE_BIND;
				_CleanClientHistory(pClient->MacAddr);
				break;
			}

		case DHCP_CLIENT_STATE_SELECT:
			/* Receive the "DISCOVER" frame, send "OFFER" to the client. */
			_DHCPOfferGenAndSend(pClient, pClient->MacAddr, Xid);
			pClient->Timeout = DHCP_DEFFAULT_TIMEOUT;
			pClient->State = DHCP_CLIENT_STATE_REQUEST;
			break;

		case DHCP_CLIENT_STATE_REQUEST:
		case DHCP_CLIENT_STATE_BIND:
			/* Send ACK to the client, if receive the "REQUEST" frame to select the offer or renew the DHCP lease. */
			_DHCPAckGenAndSend(pClient, pClient->MacAddr, Xid);
			pClient->Lease = DHCP_DEFAULT_LEASE_TIME;
			pClient->State = DHCP_CLIENT_STATE_BIND;
			_CleanClientHistory(pClient->MacAddr);
			break;

		default:
			break;
	}
}

/*   DHCPS_RecvCb   */
/*-------------------------------------------------------------------------
	Description:	
		When an incoming DHCP message is to me, this function process it and trigger the state machine.
		
	Arguments:
		Arg: Pointer to the user supplied argument.
		Pcb: Pointer to the udp_pcb which received data.
		P: Pointer to the packet buffer that was received.
		Addr: The remote IP address from which the packet was received.
		Port: The remote port from which the packet was received	.
		
	Return Value:
		None.
		
	Note:	
		
-------------------------------------------------------------------------*/
VOID DHCPS_RecvCb(VOID *Arg, struct udp_pcb *Pcb, struct pbuf *P, struct ip_addr *Addr, INT16U Port)
{
	INT32U Xid;
	INT8U MsgType;
	PDHCP_MSG pDhcpMsg;
	INT32U MsgLen;
	INT32U ReqIpAddr;
	INT32U ServerId;
	INT8U ClientMacAddr[6];
	PDHCP_CLIENT pClient;

	do
	{
		pDhcpMsg = &DhcpMsg;

		/* Copy the DHCP message. */
		MsgLen = pbuf_copy_partial(P, (VOID *)pDhcpMsg, sizeof(DHCP_MSG), 0);

		/* Filter out the frame that is not request frame or has wrong magic number or has wrong hardware address type. */
		if((MsgLen == 0) || (pDhcpMsg->Op != DHCP_OP_REQUEST) || 
			(ntohl(pDhcpMsg->Magic) != DHCP_MAGIC) || (pDhcpMsg->HType != DHCP_HWTYPE_ETHERNET))
		{
			break;
		}

		/* Parse the packet to get message type, ip address requested by client and server ID. */
		MsgType = 0xff;
		ReqIpAddr = 0;
		ServerId = 0;
		if((_ParseDhcpOptions(pDhcpMsg, &MsgType, &ReqIpAddr, &ServerId) & 0x01) == 0)
		{
			break;
		}

		/* Get the Xid and client's MAC address. */
		Xid = ntohl(pDhcpMsg->Xid);
		NST_MOVE_MEM(ClientMacAddr, pDhcpMsg->Chaddr, 6);

		/* Get the client entry that is free or negotiating. */
		pClient = _ClientTableLookup(ClientMacAddr, MsgType, ReqIpAddr, ServerId);
		if(pClient == NULL)
		{
			if((MsgType != DHCP_MSG_RELEASE) && (MsgType != DHCP_MSG_DECLINE))
			{
				/* Ip is already allocated, so send nack. */
				_DHCPNakGenAndSend(ClientMacAddr, Xid);
			}
			break;
		}

		/* Push to client state machine. */
		_DhcpClientSMEHandle(pClient, MsgType, Xid, ClientMacAddr);
	}while(0);

	pbuf_free(P);
}

/*   DHCPS_ClientDelete   */
/*-------------------------------------------------------------------------
	Description:	
		This function is used to delete the dhcp client entry on the server.
	Arguments:
		MacAddr: Specify the MAC Address of the dhcp client entry to be delete.
		
	Return Value:
		The DHCP Server error code:
			DHCPS_ERR_SUCCESS - No error
			DHCPS_ERR_INACTIVE - The Server is inactive
			DHCPS_ERR_PARAM - The input parameter error
			DHCPS_ERR_NOT_FOUND - Not fount this client
			
	Note:	
		
-------------------------------------------------------------------------*/
INT8S DHCPS_ClientDelete(INT8U * MacAddr)
{
	INT8U i;
	PDHCP_CLIENT pClient;

	/* Check the server is active now. */
	if(DhcpServer.Enable == 0)
	{
		return DHCPS_ERR_INACTIVE;
	}

	if(MacAddr == NULL)
	{
		return DHCPS_ERR_PARAM;
	}
	
	for(i = 0; i < DHCPS_HISTORY_CLIENT_NUM; i++)
	{
		pClient = &DhcpServer.Clients[i];
		if((pClient->State != DHCP_CLIENT_STATE_IDLE) && (memcmp(pClient->MacAddr, MacAddr, 6) == 0))
		{
			if(pClient->State != DHCP_CLIENT_STATE_BIND)
			{
				/* For negotiating client, return. */
				return DHCPS_ERR_NOT_BIND;
			}
			else
			{
				/* For bind client, delete it directly. */
				pClient->State = DHCP_CLIENT_STATE_IDLE;
				return DHCPS_ERR_SUCCESS;
			}
		}
	}

	return DHCPS_ERR_NOT_FOUND;
}

/*   DHCPS_Start   */
/*-------------------------------------------------------------------------
	Description:	
		This function is used to start DHCP Server for a network interface.
	Arguments:
		Netif: Pointer to the Lwip network interface.
		
	Return Value:
		The DHCP Server error code:
			DHCPS_ERR_SUCCESS - No error
			DHCPS_ERR_MEM - Out of memory
			DHCPS_ERR_LINKDOWN - The NI is inactive
	Note:	
		The dhcp server must be started after the network interface was actived.
-------------------------------------------------------------------------*/
INT32 DHCPS_Start(struct netif *Netif)
{
	INT32U Val, Mask, tmp, i;
	PDHCP_CLIENT pClient;

	/* Check the network interface is active now. */
	if(netif_is_up(Netif) == 0)
	{
		return DHCPS_ERR_LINKDOWN;
	}
	
	memset(&DhcpServer, 0, sizeof(DhcpServer));

	/* Calculate the start ip address of the server's ip pool. */
	Val = ntohl(Netif->ip_addr.addr);
	Mask = ntohl(Netif->netmask.addr);
	tmp = (Val & (~Mask));
	tmp = ((tmp + 1) % (~Mask)) ? ((tmp + 1) % (~Mask)) : 1;
	Val = htonl((Val & Mask) | tmp);

	/* Configure the DHCP Server. */
	ip_addr_set(&DhcpServer.ServerIpAddr, &Netif->ip_addr);
	ip_addr_set(&DhcpServer.StartIpAddr, (struct ip_addr *)&Val);
	ip_addr_set(&DhcpServer.SubnetMask, &Netif->netmask);
	ip_addr_set(&DhcpServer.GateWay, &Netif->ip_addr);
	ip_addr_set(&DhcpServer.Dns1, &Netif->ip_addr);
	ip_addr_set(&DhcpServer.Dns1, &Netif->ip_addr);

	/* Set the default lease time - 2 hours. */
	DhcpServer.LeaseTime = DHCP_DEFAULT_LEASE_TIME;

	/* Initialize the free DHCP clients. */
	for(i = 0; i < DHCPS_HISTORY_CLIENT_NUM; i++)
	{
		pClient = &DhcpServer.Clients[i];

		/* Set the initial client state is "IDLE". */
		pClient->State = DHCP_CLIENT_STATE_IDLE;

		/* Set the ip address to the client. */
		Val = ntohl(DhcpServer.StartIpAddr.addr);
		tmp = (Val & (~Mask));
		tmp = ((tmp + i) % (~Mask)) ? ((tmp + i) % (~Mask)) : 1;
		Val = htonl((Val & Mask) | tmp);
		ip_addr_set(&pClient->IpAddr, (struct ip_addr *)&Val);

		/* Set the default lease time. */
		pClient->Lease = DHCP_DEFAULT_LEASE_TIME;
	}

	/* Allocate a UDP PCB. */
	DhcpServer.Socket = udp_new();
	if(DhcpServer.Socket == NULL)
	{
		return DHCPS_ERR_MEM;
	}

	/* Set up local and remote port for the pcb. */
	udp_bind(DhcpServer.Socket, IP_ADDR_ANY, DHCP_SERVER_UDP_PORT);
	
	/* Set up the recv callback and argument. */
	udp_recv(DhcpServer.Socket, DHCPS_RecvCb, Netif);

	/* Start the DHCP Server tick timer. */
	sys_timeout(DHCP_TICK_TIME, _DhcpTickHandle, NULL);

	/* Enable the DHCP Server. */
	DhcpServer.Enable = 1;

	return DHCPS_ERR_SUCCESS;
}

/*   DHCPS_Stop   */
/*-------------------------------------------------------------------------
	Description:	
		This function is used to disable dhcp server.
	Arguments:
		None.		
		
	Return Value:
		None.
		
	Note:	
		
-------------------------------------------------------------------------*/
void DHCPS_Stop(void)
{
	/* Disable the dhcp server's service. */
	DhcpServer.Enable = 0;

	/* Stop the tick timer. */
	sys_untimeout(_DhcpTickHandle, NULL);

	/* Release the socket. */
	if(DhcpServer.Socket) udp_remove(DhcpServer.Socket);
	
	memset(&DhcpServer, 0, sizeof(DhcpServer));
}

#endif

