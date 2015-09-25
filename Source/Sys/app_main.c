/*
******************************************************************************
**
** Project     : 
** Description :    file for application layer task
** Author      :    pchn                             
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
#include "includes.h"

#ifdef TEST_APP_SUPPORT
#ifdef LWIP_SUPPORT
#include "lwip/memp.h"
#include "lwIP.h"
#include "lwIP/tcp.h"
#include "lwIP/udp.h"
#include "lwIP/tcpip.h"
#include "netif/etharp.h"
#include "lwIP/dhcp.h"
#include "arch/sys_arch.h"
#include "lwIP/sockets.h"
#include "lwIP/netdb.h"
#include "lwIP/dns.h"
#include "airkiss.h"

/*
******************************************************************************
**                               MACROS and VARIABLES
******************************************************************************
*/
#define TCP_SERVER_PORT  10000
#define TCP_CLIENT_PORT   10001
#define TCP_SERVER_IP_ADR        0xC0A80002  // 192.168.0.2

#define UDP_SERVER_PORT  10002
#define UDP_CLIENT_PORT   10003
#define UDP_SERVER_IP_ADR        0xC0A80002  // 192.168.0.2
 
#define RECV_BUFFER_SIZE    2048

#define APP_DATA_LEN  1460

UINT8 SendData[APP_DATA_LEN];

UINT32 SendDataCnt = 0;
UINT32 RxDataCnt = 0;

OS_EVENT * pLinkUpSem;

/*
******************************************************************************
**                               FUNCTION DELEARATIONS
******************************************************************************
*/



/*
******************************************************************************
**                               FUNCTION PROTOTYPES
******************************************************************************
*/

static void Delay(UINT8 Cnt)
{
    volatile int i,j;

    for(i=0 ; i < 1000 ; i++)
    {
        for(j = 0; j < Cnt; j++)
            ;
    }
}

#ifdef TEST_TCP_CLIENT
/*
******************************************************************************
**                        void TestTcpClientThread(void)
**
** Description  : An simple example show tcpclient how to connect to tcpserver, 
                          how to start a socket and how to communicate with tcpserver
                          udpclient send udp packet to remote udp server. It can send any data if you want.
** Arguments    : 
                  
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
void TestTcpClient(void)
{
//    char *recv_data;
    int sock;
    //int bytes_received;
    struct sockaddr_in server_addr;
    UINT32 TargetIp = htonl(TCP_SERVER_IP_ADR); 
    UINT8  FailCnt = 0;
    //static UINT32 RxTcpPduCnt = 0;
    int DataLen;  
   
   DBGPRINT_LWIP(DEBUG_TRACE, "\nTCP client on port %d...\n", TCP_CLIENT_PORT);
    
    /* 分配用于存放接收数据的缓冲 */
    //recv_data = mem_malloc(1024);
    //if (recv_data == NULL)
    //{
    //    DBGPRINT_LWIP(DEBUG_ERROR, "TCP Client  memory ERR\n");
    //    return;
    //}

    while(1)
    {
        while(1)
        {
            /* 创建一个socket，类型是SOCKET_STREAM，TCP类型 */
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            {
                /* 创建socket失败 */
                DBGPRINT_LWIP(DEBUG_ERROR, "TCP Client Socket error\n");
                /* 释放接收缓冲 */
                //mem_free(recv_data);
                return;
            }
    
            /* 初始化预连接的服务端地址 */
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(TCP_CLIENT_PORT); 
            server_addr.sin_addr.s_addr= TargetIp;
            memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    		
            /* 连接到服务端 */
            if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
            {
                /* 连接失败 */
                DBGPRINT_LWIP(DEBUG_TRACE, "TCP Client connect error !");
                lwip_close(sock);
                   
                ///*释放接收缓冲 */
                //mem_free(recv_data);
                //return;
                OSTimeDly(2000);
            }
            else
            {
                DBGPRINT_LWIP(DEBUG_TRACE, "TCP Client connect OK!\n");
                break;
            }
        }
        
        while(1)
        {
        #if 0
            /* 从sock连接中接收最大1024字节数据 */
            bytes_received = recv(sock, recv_data, 1024, 0);
            if (bytes_received < 0)
            {
                /* 接收失败，关闭这个连接 */
                lwip_close(sock);
    
                /* 释放接收缓冲 */
                mem_free(recv_data);
                break;
            }
    
            /* 有接收到数据，把末端清零 */
            recv_data[bytes_received] = '\0';
    
            if (strcmp(recv_data , "q") == 0 || strcmp(recv_data , "Q") == 0)
            {
                /* 如果是首字母是q或Q，关闭这个连接 */
                lwip_close(sock);
    
                /* 释放接收缓冲 */
                mem_free(recv_data);
                break;
            }
            else
            {
                /* 在控制终端显示收到的数据 */
                DBGPRINT_LWIP(DEBUG_TRACE, "RX=%s\n", recv_data);
                DBGPRINT_LWIP(DEBUG_TRACE, "RX=%s\n", ++RxTcpPduCnt);
            }
    #endif
            /* 发送数据到sock连接 */
           //DataLen = rand() & 0x7FF;
           //if(DataLen < 10 || DataLen > APP_DATA_LEN)
            DataLen = APP_DATA_LEN;
           
           if(send(sock,SendData, DataLen, 0) < 0)
           {
                OSTimeDly(20);
                FailCnt ++;
           }
           // send(sock,recv_data,1024, 0);
           else
           {
                Delay(2);
                FailCnt = 0;
           }
    
           if(FailCnt >= 5)
           {
                FailCnt = 0;
                if (sock)
                   lwip_close(sock);
                break;
           }
    	 
           SendDataCnt++;           
        }
    }
}
#endif

#ifdef TEST_TCP_SERVER
/*
******************************************************************************
**                         void TestTcpServer(void)
**
** Description  :  An simple example show how to establish a tcpserver, 
                          how to start a socket and how to communicate with tcpclient
                         tcpserver accept tcp connect receive data and output on debug thermal.
** Arguments    : 
                  
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
void TestTcpServer(void)
{
   char *recv_data; /* 用于接收的指针，后面会做一次动态分配以请求可用内存 */
   u32_t sin_size;
   int sock, connected, bytes_received;
   struct sockaddr_in server_addr, client_addr;
   BOOL_T stop = NST_FALSE; /* 停止标志 */

   recv_data = mem_malloc(RECV_BUFFER_SIZE); /* 分配接收用的数据缓冲 */
   if (recv_data == NULL)
   {
       DBGPRINT_LWIP(DEBUG_ERROR, "No memory\n");
       return;
   }

   /* 一个socket在使用前，需要预先创建出来，指定SOCK_STREAM为TCP的socket */
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
       /* 创建失败的错误处理 */
       DBGPRINT_LWIP(DEBUG_ERROR, "Socket error\n");

       /* 释放已分配的接收缓冲 */
       mem_free(recv_data);
       return;
   }

   /* 初始化服务端地址 */
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(TCP_SERVER_PORT); /* 服务端工作的端口 */
   server_addr.sin_addr.s_addr = INADDR_ANY;
   memset(&(server_addr.sin_zero),8, sizeof(server_addr.sin_zero));

   /* 绑定socket到服务端地址 */
   if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
   {
       /* 绑定失败 */
       DBGPRINT_LWIP(DEBUG_ERROR, "Unable to bind\n");

       /* 释放已分配的接收缓冲 */
       mem_free(recv_data);
       return;
   }

   /* 在socket上进行监听 */
   if (listen(sock, 5) == -1)
   {
       DBGPRINT_LWIP(DEBUG_ERROR, "Listen error\n");

       /* release recv buffer */
       mem_free(recv_data);
       return;
   }

   DBGPRINT_LWIP(DEBUG_TRACE, "\nTCPServer Waiting for client on port %d...\n", TCP_SERVER_PORT);
   while(stop != NST_TRUE)
   {
       sin_size = sizeof(struct sockaddr_in);

       /* 接受一个客户端连接socket的请求，这个函数调用是阻塞式的 */
       connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
       /* 返回的是连接成功的socket */

       /* 接受返回的client_addr指向了客户端的地址信息 */
       DBGPRINT(DEBUG_TRACE, "I got a connection from (%s , %d)\n",
                  inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

       /* 客户端连接的处理 */
       while (1)
       {
           /* 发送数据到connected socket */
           //send(connected, send_data, strlen(send_data), 0);

           /* 从connected socket中接收数据，接收buffer是1024大小，但并不一定能够收到1024大小的数据 */
           bytes_received = recv(connected,recv_data, RECV_BUFFER_SIZE, 0);
           if (bytes_received <= 0)
           {
               if(bytes_received == 0)
                   DBGPRINT(DEBUG_TRACE, "client close connection.\n");
               else
                   DBGPRINT(DEBUG_ERROR, "received failed, server close connection.\n");
               
               /* 接收失败，关闭这个connected socket */
               lwip_close(connected);
               break;
           }
           
           RxDataCnt++;
       } // end of while(1)
   } // end of while(stop != TRUE)

   /* 释放接收缓冲 */
   mem_free(recv_data);

   return ;
}
#endif

#ifdef TEST_UDP_CLIENT
/*
******************************************************************************
**                        void TestUdpClient()
**
** Description  :  An simple example show how to establish a udpclient, 
                          how to start a socket and how to communicate with udpserver
                         udpclient send udp packet to remote udp server.It can send any data if you want.
** Arguments    : 
                  
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
void TestUdpClient()
{
    int sock;
    struct sockaddr_in server_addr;
    int DataLen;
    //ip_addr_t addr;
    //UINT8 Err;
    UINT32 TargetIp = htonl(UDP_SERVER_IP_ADR);

    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
       DBGPRINT_LWIP(DEBUG_ERROR,"Socket error\n");
       return;
    }

    DBGPRINT_LWIP(DEBUG_TRACE,"\nUDP client on port %d server addr %x...\n", UDP_CLIENT_PORT,TargetIp);

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_CLIENT_PORT);
    server_addr.sin_addr.s_addr= TargetIp;
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 循环发送数据，用户可自行调整实现方式完成发送数据量控制 */
    while (1)
    {
       /* 发送数据到服务远端 */
       //DataLen = rand() & 0x7FF;
       //if(DataLen < 10 || DataLen > APP_DATA_LEN)
       DataLen = APP_DATA_LEN;
       sendto(sock, SendData, DataLen, 0,
              (struct sockaddr *)&server_addr, sizeof(struct sockaddr));      
       SendDataCnt++;

       /* 线程休眠一段时间 */
       OSTimeDly(10);

    }

    /* 关闭这个socket */
    //lwip_close(sock);
}
#endif

#ifdef TEST_UDP_SERVER
/*
******************************************************************************
**                        void TestUdpServer(void)
**
** Description  : An simple example show how to establish a udpserver, 
                          how to start a socket and how to communicate with udpclient
                         udpserver accept udp connect receive data and send it returnl.
** Arguments    : 
                  
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
void TestUdpServer(void)
{
   int sock;
   int bytes_read;
   UINT8 * gb_recv_data = NULL;
   u32_t addr_len;
   struct sockaddr_in server_addr, client_addr;

   /* 分配接收用的数据缓冲 */
   gb_recv_data = mem_malloc(RECV_BUFFER_SIZE);
   if (gb_recv_data == NULL)
   {
       /* 分配内存失败，返回 */
       DBGPRINT_LWIP(DEBUG_ERROR, "No memory\n");
       return;
   }

   /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
   {
       DBGPRINT_LWIP(DEBUG_ERROR, "Socket error\n");
	   
       /* 释放接收用的数据缓冲 */
       mem_free(gb_recv_data);
       return;
   }

   /* 初始化服务端地址 */
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(UDP_SERVER_PORT);
   server_addr.sin_addr.s_addr = INADDR_ANY;
   memset(&(server_addr.sin_zero),0, sizeof(server_addr.sin_zero));

   /* 绑定socket到服务端地址 */
   if (bind(sock,(struct sockaddr *)&server_addr,
            sizeof(struct sockaddr)) == -1)
   {
       /* 绑定地址失败 */
       DBGPRINT_LWIP(DEBUG_ERROR, "Bind error\n");

       /* 释放接收用的数据缓冲 */
       mem_free(gb_recv_data);
       return;
   }

   addr_len = sizeof(struct sockaddr);
   DBGPRINT_LWIP(DEBUG_TRACE, "\nUDPServer Waiting for client on port %d...\n", UDP_SERVER_PORT);
   while (1)
   {
       /* 从sock中收取最大2048字节数据 */
       /* UDP不同于TCP，它基本不会出现收取的数据失败的情况，除非设置了超时等待 */
       bytes_read = recvfrom(sock, gb_recv_data, RECV_BUFFER_SIZE, 0,
                             (struct sockaddr *)&client_addr, &addr_len);

       gb_recv_data[bytes_read] = '\0'; /* 把末端清零 */
	RxDataCnt++;

       /* 将接收的数据直接发出，可采用TCP&UDP工具形成UDP环路测试UDP丢包 */
	sendto(sock, gb_recv_data, bytes_read, 0,
              (struct sockaddr *)&client_addr, sizeof(struct sockaddr));

       /* 可通过打印信息输出接收的数据或在此处添加用户数据处理函数 */
       //DBGPRINT_LWIP(DEBUG_TRACE,"\n(%s,%d) said : %d",inet_ntoa(client_addr.sin_addr),
       //        ntohs(client_addr.sin_port), bytes_read);
       //DBGPRINT_LWIP(DEBUG_TRACE,"%s", gb_recv_data);
   }
}
#endif

#ifdef TEST_DIRECT_CONFIG
int TestDirectCfgFlag = 0;

/*
******************************************************************************
**                        void RespondUdpBroadcast(void)
**
** Description  : respond with udp broadcast packet for 10 seconds.
** Arguments    : 
                  
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID RespondUdpBroadcast(VOID)
{
    int ret = 0;
    int count = 0;
	int trycount = 0;
	int conncount = 0;
    
    int udp_sock;
    const int opt = 1;    
    struct sockaddr_in addrto;
    int nlen = sizeof(addrto);
    char sendbuf[24] = {0}; 

    /* fill with response data */
    sendbuf[0] = 0x18;
    sendbuf[8] = 0x06;
    sendbuf[10] = 0x01;
 
AGAIN:    
    if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        DBGPRINT(DEBUG_ERROR, "UDP Broadcast Socket error\n");
        return;
    }
    
    if (setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)) == -1) 
    {  
        DBGPRINT(DEBUG_ERROR, "set UDP Broadcast socket error...\n");  
		lwip_close(udp_sock);  
        return;  
    }  
    
    memset(&addrto, 0, sizeof(struct sockaddr_in));  
    addrto.sin_family = AF_INET;  
    addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);  
    addrto.sin_port=htons(60002);  
    
    DBGPRINT(DEBUG_TRACE, "Respond with udp broadcast.");
	count = 0;
    trycount = 0;

    while (1)
    {
        ret = sendto(udp_sock, sendbuf, 24, 0, (struct sockaddr *)&addrto, nlen);
        if (ret < 0) 
        {
			trycount++;
			DBGPRINT(DEBUG_TRACE, "Send UDP Broadcast data error(ret:%d)\n", ret);
			OSTimeDly(30);
			if (trycount > 5) {
				DBGPRINT(DEBUG_TRACE, "Respond UDP Broadcast Failed.\n");
				lwip_close(udp_sock);
				conncount++;
				if (conncount > 3) {
					return;
				}
				goto AGAIN;
			} else {
            	continue;
			}
        } 
        
        /* print debug message to console */
        if (count < 20)  // broadcast 6s
        {
            if (count % 3 == 0) 
                printf("\b..");
    
            if (count % 3 == 0) 
                printf("\b\\");
            else if (count % 3 == 1) 
                printf("\b-");
            else if (count % 3 == 2) 
                printf("\b/");
                
            OSTimeDly(30);
        } 
        else
        {
            printf("\n");
            break;
        }
        count++;
    }

	lwip_close(udp_sock);
    DBGPRINT(DEBUG_TRACE, "Respond finished.\n");
}

VOID TestDirectConfig(VOID)
{
    UINT8 SsidLen = 0;
    UINT32 count = 0;

    /* start direct config */
    DBGPRINT(DEBUG_TRACE, "Direct config start, waiting ");
    InfDirectCfgStart(0, 0, 0);

    /* print steps */
    while (1) 
    {
        if (TestDirectCfgFlag == 1) 
        {
            TestDirectCfgFlag = 0;
            printf("\n");
            break;
        }
        
        if (count % 10 == 0) 
            printf("\b..");		
        
        if (count % 3 == 0) 
            printf("\b\\");
        else if (count % 3 == 1) 
            printf("\b-");
        else if (count % 3 == 2) 
            printf("\b/");
            
        count++;
        OSTimeDly(30);				
    }

    DBGPRINT(DEBUG_TRACE, "Direct config succeeded\n");

    /* stop direct config */
    InfWiFiStop();

    /* print the received config parameters (ssid and key) */
    SsidLen = strlen((const char *)SysParam.WiFiCfg.Ssid);
    if (SsidLen)
    	hex_dump("get ssid", SysParam.WiFiCfg.Ssid, SsidLen);
    if (SysParam.WiFiCfg.KeyLength)
    	hex_dump("get key", SysParam.WiFiCfg.PSK, SysParam.WiFiCfg.KeyLength);

    /* use the received parameters to start wifi connection */
    InfNetModeSet(PARAM_NET_MODE_STA_BSS);
    InfWiFiStart();
    DBGPRINT(DEBUG_TRACE, "Start wifi connection.\n");

    /* wait wifi link up */
    while (TestDirectCfgFlag == 0)
        OSTimeDly(30);
    TestDirectCfgFlag = 0;

    /* send broadcast message to inform direct config peer*/
    RespondUdpBroadcast();  

    while (1) 
        OSTimeDly(1000);
}
#endif

#ifdef TEST_SOFTAP_CONFIG
SYS_EVT_ID link_status;

VOID TestSoftApConfig(VOID)
{
	ip_addr_t ipAddr;
	int i,j,ssid_len,password_len;
	int bytes_read;
	INT32 sockfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	UINT16 socketPort = 60001;
	char ssid[33] = {0};
	char password[65] = {0};

	u32_t addr_len = sizeof(struct sockaddr);
	char *recv_data;
	recv_data = mem_malloc(RECV_BUFFER_SIZE);

	if (recv_data == NULL)
	{
		DBGPRINT(DEBUG_ERROR, "no memory\r\n");
		return;
	}

	InfWiFiStop();
	OSTimeDly(10);

	InfNetModeSet(PARAM_NET_MODE_SOFTAP);
	inet_aton("10.10.10.1",&ipAddr);
	SysParam.IPCfg.Ip[0] = ip4_addr1(&ipAddr);
	SysParam.IPCfg.Ip[1] = ip4_addr2(&ipAddr);
	SysParam.IPCfg.Ip[2] = ip4_addr3(&ipAddr);
	SysParam.IPCfg.Ip[3] = ip4_addr4(&ipAddr);
	SysParam.WiFiCfg.Encry = 3;
	SysParam.WiFiCfg.AuthMode = 3;
	InfSsidSet("NL6621", 6);
	InfKeySet(0, "12345678", 8);

	InfWiFiStart();
	OSTimeDly(100);

	if (link_status != SYS_EVT_LINK_UP)
	{
		DBGPRINT(DEBUG_ERROR, "WiFi Started Failed\r\n");
		return;
	}
	else
	{
		if ((sockfd = socket(AF_INET, SOCK_DGRAM,0)) == -1)
		{
			DBGPRINT_LWIP(DEBUG_ERROR, "socket error\n");
			return;
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(socketPort);
		server_addr.sin_addr.s_addr = INADDR_ANY;
		memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

		if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
		{
			DBGPRINT_LWIP(DEBUG_ERROR, "bind error\n");
			return;
		}
		
		DBGPRINT(DEBUG_TRACE, "Provisioning...\r\n");

		while (1)
		{
			if (link_status != SYS_EVT_LINK_UP)
			{
				DBGPRINT(DEBUG_ERROR, "WiFi link down\r\n");
				return;
			}

			bytes_read = recvfrom(sockfd, recv_data, RECV_BUFFER_SIZE, 0, 
				(struct sockaddr *)&client_addr, &addr_len);
				
			if (bytes_read <= 0 || bytes_read != 200)
				continue;
			if (bytes_read <= 0)
				continue;

			for (i = 0; i < 8; i++)
			{
				if (recv_data[i] != i)
					break;
			}

			if (i != 8)
				continue;

			ssid_len = recv_data[i++];
			if (ssid_len > 32 || ssid_len == 0)
			{
				DBGPRINT(DEBUG_ERROR, "ssid length invalid\r\n");
				return;
			}

			for (j = 0; j < ssid_len; j++)
			{
				ssid[j] = recv_data[i+j];
			}

			password_len = recv_data[i+ssid_len];
			if (password_len > 64)
			{
				DBGPRINT(DEBUG_ERROR, "password length invaild\r\n");
				return;
			}

			for (j = 0; j < password_len; j++)
			{
				password[j] = recv_data[i+ssid_len+1+j];
			}
			DBGPRINT(DEBUG_TRACE, "ssid:%s\r\n",ssid);
			DBGPRINT(DEBUG_TRACE, "password:%s\r\n",password);

			InfWiFiStop();
			OSTimeDly(10);
			InfNetModeSet(PARAM_NET_MODE_STA_BSS);
			InfSsidSet((UINT8 *)ssid, strlen(ssid));    /* set ssid */
			InfKeySet(0, (UINT8 *)password, strlen(password));
			InfConTryTimesSet(5);
			SysParam.WiFiCfg.Encry = 4;
			InfWiFiStart();
			break;
		}
	}	

	while(1)
		OSTimeDly(1000);
}
#endif

#ifdef TEST_SNIFFER
int TestSnifferFlag = 0;

VOID TestSnifferRecvCb(UINT8 *pRecvBuf, UINT32 pRecvLen)
{
    hex_dump("sniffer packet", pRecvBuf, pRecvLen);
}

VOID TestSniffer(VOID)
{
    SCAN_INFO_TABLE * pScanTable = NULL;
    UINT8 i;
    
    DBGPRINT(DEBUG_TRACE, "Test sniffer start\n");   

    // scan all channel to get ap list
    pScanTable = OSMMalloc(sizeof(SCAN_INFO_TABLE) + sizeof(SCAN_INFO) * (32 -1));
    if (!pScanTable)
    {
        DBGPRINT(DEBUG_TRACE, "Test sniffer out of memory\n");
        return;
    }
    pScanTable->InfoCount = 32;
    InfWiFiStart();
    InfScanStart(pScanTable, 0, 0);

    // wait scan done
    while (TestSnifferFlag != 1)
        OSTimeDly(10);
    InfWiFiStop();
    DBGPRINT(DEBUG_TRACE, "Scan done start sniff\n");
    
    // sniff packets of every ap in the scan info list
    for (i = 0; i < pScanTable->InfoCount; i++)
    {
        DBGPRINT(DEBUG_TRACE, "Sniff bssid = %02X-%02X-%02X-%02X-%02X-%02X channel=%d\n",
                       pScanTable->InfoList[i].Bssid[0], pScanTable->InfoList[i].Bssid[1], pScanTable->InfoList[i].Bssid[2],
                       pScanTable->InfoList[i].Bssid[3], pScanTable->InfoList[i].Bssid[4], pScanTable->InfoList[i].Bssid[5],
                       pScanTable->InfoList[i].Channel);
        OSTimeDly(300);
        InfSnifferStart(TestSnifferRecvCb, pScanTable->InfoList[i].Bssid, pScanTable->InfoList[i].Channel);
        OSTimeDly(10);
        InfWiFiStop();
    }

    OSMFree(pScanTable);
    DBGPRINT(DEBUG_TRACE, "Test sniffer done\n");
}
#endif

#ifdef TEST_UART_UPDATE_FW
#define NU_MAX_REG_NUM 1024

typedef struct  {
	 unsigned short length;//表示整个帧的长度，当ONE VALUE时长度20byte
	 unsigned short type;  //现已定为固定值0；
}__attribute__((packed)) UartCommandHead;
 
typedef struct {
	 UartCommandHead  head;
	 unsigned short   command; //自定义COMMAND，从500开始
	 unsigned short   size;  //16+value_len
	 unsigned short   seq_num; //现已定为固定值0；
	 unsigned short   result;  //定success是0，fail是1；
	 unsigned short   action;  //定0是读，1是写；
	 unsigned short   offset;  //数据value的偏移地址；是指从BBP或RF开始
	 unsigned int     value_len; //data 数据中有效数据长度
	 unsigned char    data[NU_MAX_REG_NUM]; //512-20 //这个可以改
}__attribute__((packed)) COMMAND;

typedef struct {
	 UartCommandHead  head;
	 unsigned short   command; //自定义COMMAND，从500开始
	 unsigned short   size;  //16+value_len
	 unsigned short   seq_num; //现已定为固定值0；
	 unsigned short   result;  //定success是0，fail是1；
	 unsigned short   action;  //定0是读，1是写；
	 unsigned short   offset;  //数据value的偏移地址；是指从BBP或RF开始
	 unsigned int     value_len; //data 数据中有效数据长度
}__attribute__((packed)) COMMAND_ACK;

#define MAX_HEADER_LENTH MAX_FW_HDR_SIZE + sizeof(COMMAND_ACK)

/*
******************************************************************************
**     BOOL_T  TestUartUpdateFw (VOID)
**
** Description  :  UART update firmware
** Arguments    : 
                  
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
BOOL_T TestUartUpdateFw (VOID)
{
    FW_HDR* spFwHdr = NULL;
    UINT8* pSramAdr;
    UINT8  ch, get_header_right = 0;
    UINT32 i;
    UINT32 FWLen = 0;
    COMMAND* cmd_pkt;       
    UINT32 WriteAddr = NVM_FW_BAK_OFFSET;
    UINT32 frameLen = NU_MAX_REG_NUM+sizeof(COMMAND_ACK);
    UINT32 PageCnt;
    OS_CPU_SR  cpu_sr;
    
    UINT8 * TmpBuf = NULL;
    
    TmpBuf = OSMMalloc(2048);
    if (TmpBuf == NULL)
        return NST_FALSE;
    memset(TmpBuf, 0, 2048);

    OS_ENTER_CRITICAL();
    
    BSP_UartOpen(DEFAULT_UART_BAUDRATE); 

    BSP_UartPutcPolled('I');
    BSP_UartPutcPolled('n');
    BSP_UartPutcPolled('i');
    BSP_UartPutcPolled('t');

    BSP_QSpiInit(DEFAULT_QSPI_CLK);
    
    /* 擦除备份固件区域*/
    for (i = 0; i < FW_IMG_SIZE_MAX/FLASH_SECTOR_SIZE; i++)
    {   
        QSpiFlashEraseSector(WriteAddr + i*FLASH_SECTOR_SIZE);
        NSTusecDelay(100000); 
        BSP_UartPutcPolled('.');
    }

    for (i = 0; i < 30; i++)
    {   
        NSTusecDelay(100000); 
        BSP_UartPutcPolled('.');
    }

    BSP_UartPutcPolled('\n');
    BSP_UartPutcPolled('\r');
// 发送握手信号给host
    // send flag to host first
    BSP_UartPutcPolled('N');
    BSP_UartPutcPolled('U');
    BSP_UartPutcPolled('L');
    BSP_UartPutcPolled('K');
    BSP_UartPutcPolled(' ');
    BSP_UartPutcPolled('R');
    BSP_UartPutcPolled('E');
    BSP_UartPutcPolled('A');
    BSP_UartPutcPolled('D');
    BSP_UartPutcPolled('Y');
    BSP_UartPutcPolled('!');
    BSP_UartPutcPolled('\n');
    BSP_UartPutcPolled('\r');
    
    /*循环等待接收头正确*/
    while (!get_header_right)
    {
        pSramAdr = (UINT8*)TmpBuf;
        
    // 先获得固件头
        for (i = 0; i < MAX_HEADER_LENTH; i++)
        {
            ch = BSP_UartGetcPolled();
            //BSP_UartPutcPolled(ch);
            *pSramAdr = ch;
            pSramAdr++;
        }

        /*处理报文开始*/
        cmd_pkt = (COMMAND*)TmpBuf;
    
        if ( (cmd_pkt->size != 
            (sizeof(COMMAND_ACK) - sizeof(UartCommandHead) + cmd_pkt->value_len)) 
            || (cmd_pkt->offset != 0))
        {
            cmd_pkt->result = 1;
            
        }
        else
        {
            /*接收正确，将一页数据(256 BYTE)写入SPI flash*/
            cmd_pkt->result = 0;
            
            spFwHdr = (FW_HDR*)(TmpBuf+sizeof(COMMAND_ACK));

            
            pSramAdr = (UINT8*)TmpBuf;
    
            QSpiFlashWriteOnePage(WriteAddr, pSramAdr+sizeof(COMMAND_ACK));
            
            WriteAddr += FLASH_PAGE_SIZE;   
            
            get_header_right = 1;
        }

        /*处理报文结束*/
        
        cmd_pkt->command |= 0x8000; /*响应命令标志*/

        pSramAdr = (UINT8*)TmpBuf;
        /*回复响应报文*/
        for (i=0; i<20; i++)
        {
            BSP_UartPutcPolled(*(pSramAdr+i));
        }
    }

    spFwHdr = (FW_HDR*)(TmpBuf+sizeof(COMMAND_ACK));
// 获得剩下的所有内容
    if(spFwHdr->UartBaudrate != DEFAULT_UART_BAUDRATE)
        BSP_UartOpen(spFwHdr->UartBaudrate);                            
    
    FWLen = spFwHdr->FwSize - MAX_FW_HDR_SIZE; 
    while (FWLen > 0)
    {
        pSramAdr = (UINT8*)TmpBuf;
        for (i = 0; i < frameLen; i++)
        {
            *pSramAdr = BSP_UartGetcPolled();

            pSramAdr++;
        }

        cmd_pkt = (COMMAND*)TmpBuf;
        
        if ( (cmd_pkt->size != 
            (sizeof(COMMAND_ACK) - sizeof(UartCommandHead) + cmd_pkt->value_len)))
        {
            cmd_pkt->result = 1;
            
        }
        else
        {
            cmd_pkt->result = 0;
            pSramAdr = (UINT8*)TmpBuf;
            PageCnt = cmd_pkt->value_len/FLASH_PAGE_SIZE + (cmd_pkt->value_len%FLASH_PAGE_SIZE > 0 ? 1 : 0);
            while (PageCnt > 0)
            {
                QSpiFlashWriteOnePage(WriteAddr, pSramAdr+sizeof(COMMAND_ACK));

                WriteAddr += FLASH_PAGE_SIZE;
                pSramAdr += FLASH_PAGE_SIZE;
                PageCnt--;
            }       
            
            FWLen -= cmd_pkt->value_len;
            if (FWLen < NU_MAX_REG_NUM)
            {
                frameLen = FWLen+sizeof(COMMAND_ACK);
            }               
        }

        cmd_pkt->command |= 0x8000;

        pSramAdr = (UINT8*)TmpBuf;
        for (i=0; i<20; i++)
        {
            BSP_UartPutcPolled(*(pSramAdr+i));
        }   

    }

    NSTusecDelay(1000000);
    // 固件下载成功, 复制到主镜像区
    if (FWLen == 0)
    {
        INT32U TmeLen = 0;
        INT32U ReadAddr = NVM_FW_BAK_OFFSET;
        INT8U * FlashBuf = OSMMalloc(FLASH_PAGE_SIZE);
       // INT8U * ChkBuf = OSMMalloc(FLASH_PAGE_SIZE);
        
        if (FlashBuf == NULL)
        {
            OSMFree(TmpBuf);
            return NST_FALSE;
        }
        
        WriteAddr = NVM_FW_MAIN_OFFSET;
        FWLen = FW_IMG_SIZE_MAX;
        
        /* 擦除主镜像区*/
        for (i = 0; i < FW_IMG_SIZE_MAX/FLASH_SECTOR_SIZE; i++)
        {
            
        #ifdef NULINK2_SOC
            QSpiFlashEraseSector(WriteAddr + i*FLASH_SECTOR_SIZE);
        #else
            SpiFlashEraseSector(WriteAddr + i*FLASH_SECTOR_SIZE);
        #endif
            NSTusecDelay(100000);
        }
        
        while (FWLen > 0)
        {
            memset(FlashBuf, 0, FLASH_PAGE_SIZE);
            TmeLen = (FWLen > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : FWLen;
        #ifdef NULINK2_SOC
            QSpiFlashRead(ReadAddr, FlashBuf, TmeLen);
        #else
            SpiFlashRead(ReadAddr, FlashBuf, TmeLen);
         #endif
            
        #ifdef NULINK2_SOC
            QSpiFlashWriteOnePage(WriteAddr, FlashBuf);
        #else
            SpiFlashWriteOnePage(WriteAddr, FlashBuf);
        #endif
            
            ReadAddr += TmeLen;
            WriteAddr += TmeLen;
            FWLen -= TmeLen;
        }
        
        OSMFree(FlashBuf);
    }

    BSP_UartPutcPolled('D');
    BSP_UartPutcPolled('o');
    BSP_UartPutcPolled('n');
    BSP_UartPutcPolled('e');
    BSP_UartPutcPolled('\n');
    BSP_UartPutcPolled('\r');

    OSMFree(TmpBuf);
    OS_EXIT_CRITICAL();
    while (1);
}
#endif

#ifdef TEST_OTA_UPDATE_FW
#define UDP_CTRL_PORT  7200
#define TCP_DATA_PORT 8200

#define OTA_CTRL_BUF_SIZE    256

#define WIFI_DEV  0x01

#define FW_UPGRADE_CMD 0x20
#define DEV_ACK_CMD  0xA1

#define UPGRADE_START   0x01
#define UPGRADE_END     0x02
#define UPGRADE_DOING   0x04
#define UPGRADE_OKFILE  0x08
#define UPGRADE_DONE    0x10

typedef struct 
{
    INT32 local_fd;
    UINT32 ip;
    UINT16 port;
}SOCKET_INFO;

typedef struct
{
    UINT16 pkt_len;       //报文长度
    UINT8 dev_type;       //设备类型
    UINT8  cmd;            //命令ID  
}__attribute__((packed)) PKT_HEADER;

typedef struct 
{
    PKT_HEADER     header;         //报文头
    UINT8  ack_cmd_id;     //确认命令ID
    UINT8  ack_status;     //确认状态
    UINT8  reserve1;       //备用1
    UINT8  reserve2;       //备用2
}__attribute__((packed)) GEN_ACK_PKT;

typedef struct 
{
    PKT_HEADER      header;         //报文头
    UINT8   flag;
    UINT8   unused1;  
    UINT32    fileSize;
    UINT8   unused2;
    UINT8   unused3;
}__attribute__((packed)) FW_UPGRADE_PKT;

UINT8 OtaCtrlBuf[OTA_CTRL_BUF_SIZE];
UINT8 OtaDataBuf[FLASH_SECTOR_SIZE]; 

UINT32 WritePtr;
UINT32 ReadPtr;

UINT32 upgradeFlag = 0;
UINT32 fileSize = 0;
UINT32 recvFileSize = 0;
UINT32 upgradeSector = 0;
SOCKET_INFO upgrade_socket_info;

#define UPP_SCAN_DATA_LEN_MAX                    (10)
#define UDP_SCAN_BROADCAST_PORT                  (7878)
#define UDP_SCAN_BROADCAST_RESPONSE_PORT         (8787)
#define UDP_SCAN_RESPONSE_TIME                   (5) 
#define UDP_SCAN_STRING                          "scan"                   
#define UDP_UPGRADE_RESPONSE_TIME                (3) 
/*
******************************************************************************
**                        void DevDiscoverResponse(void)
**
** Description  : Device response the control app, which would like to discover
					or reflash device IP.  
** Arguments    : NULL                 
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
void DevDiscoverResponse(const char *ip_buf)
{
    int count = 0;
    int udp_sock;
    struct sockaddr_in addrto;
    int nlen = sizeof(addrto);
    char *sendbuf = "ok";		 
    
    if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        DBGPRINT(DEBUG_ERROR, "UDP Broadcast Socket error\n");
        return;
    }
  
    memset(&addrto, 0, sizeof(struct sockaddr_in));  
    addrto.sin_family = AF_INET;  
    addrto.sin_addr.s_addr = inet_addr(ip_buf); 
    addrto.sin_port = htons(UDP_SCAN_BROADCAST_RESPONSE_PORT); /* port:8787 */  
	
    while (1) 
    {	
        sendto(udp_sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&addrto, nlen);
        if (count < UDP_SCAN_RESPONSE_TIME) 
            OSTimeDly(1);
        else
            break;
        count++;
    }
    
    lwip_close(udp_sock);
    DBGPRINT(DEBUG_TRACE, "Respond APP discover device finish.\n");
}

VOID DevDiscoverThread(VOID * arg)
{
    int udp_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    char buffer[UPP_SCAN_DATA_LEN_MAX];
    unsigned char ip_buf[20]; 
    
    if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        DBGPRINT(DEBUG_ERROR, "UDP Broadcast Socket error\n");
        goto DEL_TASK;
    }
  
    memset(&server_addr, 0, sizeof(struct sockaddr_in));  
    server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); 
    server_addr.sin_port=htons(UDP_SCAN_BROADCAST_PORT); /* port:7878 */ 

    if (bind(udp_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) 
    {
        DBGPRINT_LWIP(DEBUG_ERROR, "bind error\n");
        goto DEL_TASK;
    }

    while (1)
    {
        if (upgradeFlag != 0)
        {
            fcntl(udp_sock, F_SETFL, O_NONBLOCK);
            recvfrom(udp_sock, buffer, UPP_SCAN_DATA_LEN_MAX, 0,
                           (struct sockaddr *)&client_addr, &addr_len);
            OSTimeDly(1);
            continue;
        }
        fcntl(udp_sock, F_SETFL, 0);
        
        DBGPRINT(DEBUG_TRACE, "Waiting for device scan on port %d....\n", UDP_SCAN_BROADCAST_PORT);
        
        memset(buffer, '\0', sizeof(buffer));
        memset(ip_buf, '\0', sizeof(ip_buf));	
        DBGPRINT(DEBUG_WARN, "Poll to read UDP data..........\n");
        
        recvfrom(udp_sock, buffer, UPP_SCAN_DATA_LEN_MAX, 0,
                       (struct sockaddr *)&client_addr, &addr_len);
        
        DBGPRINT(DEBUG_WARN, "Receive APP(IP:%s) discover device broadcast message.......\n", inet_ntoa(client_addr.sin_addr));

        if (strcmp(buffer, UDP_SCAN_STRING) != 0) 
        {
            DBGPRINT(DEBUG_TRACE, "Receive device scan data error.\n");
            continue;
        } 
        else 
        {
            memcpy(ip_buf, (INT8U *)inet_ntoa(client_addr.sin_addr), 
            strlen(inet_ntoa(client_addr.sin_addr)));		
            DevDiscoverResponse((const char *)ip_buf); 
        }
    }

DEL_TASK:	
	OSTaskDel(OS_PRIO_SELF);
}

//UDP发送
int DevSendCtrlMsg(UINT8 * buf, int len, SOCKET_INFO* sock_info)
{
    struct sockaddr_in PeerAddr;    // peer address information
    int snd;
    
    PeerAddr.sin_family = AF_INET;         
    PeerAddr.sin_port = sock_info->port;    
    PeerAddr.sin_addr.s_addr = sock_info->ip; 
    memset(PeerAddr.sin_zero, '\0', sizeof(PeerAddr.sin_zero));
    
    snd = sendto(sock_info->local_fd, buf, len, 0,
                        (struct sockaddr *) &PeerAddr, 
                        sizeof(struct sockaddr));

    if (snd < 0) 
    {
        printf("DevSendCtrlMsg failed\n");
        return -1;
    }
    return 0;
}

static void FwUpgradeFinishAck(void)
{
	int ret;
	int count = 0;
    GEN_ACK_PKT upgrade_resp;

    upgrade_resp.header.cmd = DEV_ACK_CMD; 
    upgrade_resp.header.dev_type = WIFI_DEV;
    upgrade_resp.ack_cmd_id = FW_UPGRADE_CMD;
    upgrade_resp.ack_status = 1;
    upgrade_resp.reserve1 = 0;
    upgrade_resp.reserve2 = 0;
    upgrade_resp.header.pkt_len = htons(sizeof(upgrade_resp) - 2); //2个字节的报文长度字段。 
	
    while (1) 
    {	
	    ret = DevSendCtrlMsg((UINT8 *)&upgrade_resp, sizeof(upgrade_resp), &upgrade_socket_info);
	    if (ret < 0) 
	    {
	        printf("Failed to send upgrade data finish ack\n");
	    }													
        
		if (count < UDP_UPGRADE_RESPONSE_TIME) 
            OSTimeDly(1);
        else
            break;
        count++; 	
    }
}

static void FwUpgradeProcess(UINT8 * cmd_pkt, unsigned short cmd_len, SOCKET_INFO* sock_info)
{
    GEN_ACK_PKT upgrade_resp;
    FW_UPGRADE_PKT *pkt = (FW_UPGRADE_PKT *)cmd_pkt;
    UINT8 ret = 0;
        
    printf("upgrade start\n");
    upgradeFlag |= UPGRADE_START;
    fileSize = htonl(pkt->fileSize);
    
    upgrade_resp.header.cmd = DEV_ACK_CMD; 
    upgrade_resp.header.dev_type = WIFI_DEV;
    upgrade_resp.ack_cmd_id = FW_UPGRADE_CMD;
    upgrade_resp.ack_status = (ret == 0) ? 1 : 0;
    upgrade_resp.reserve1 = 0;
    upgrade_resp.reserve2 = 0;
    upgrade_resp.header.pkt_len = htons(sizeof(upgrade_resp) - 2); //2个字节的报文长度字段。

    if (DevSendCtrlMsg((UINT8 *)&upgrade_resp, sizeof(upgrade_resp), sock_info) != 0)
    {
        upgradeFlag = 0;
        printf("Failed to send upgrade cmd ack\n");
    }

    return;
}

static void OtaPktProcess(UINT8 * pkt, int pkt_len, SOCKET_INFO* sock_info)
{
    FW_UPGRADE_PKT* pkt_data = (FW_UPGRADE_PKT*)pkt;
    
    UINT16 length;

    printf("UDP control info: pkt_len:%d, device type:%u, cmd_id:%u, flag:%u, fileSize:%u; data package:%u.\n", 
            ntohs(pkt_data->header.pkt_len), 
            pkt_data->header.dev_type, 
            pkt_data->header.cmd,
            pkt_data->flag,
            htonl(pkt_data->fileSize),
            pkt_len);
    
    length = ntohs(pkt_data->header.pkt_len);

    if (pkt_len == (length+2))
    {
        switch (pkt_data->header.cmd) 
        {     
            case FW_UPGRADE_CMD:
                FwUpgradeProcess(pkt, length, sock_info);
                break;
            default:
                break;
        }
    }

    return;
}

void OtaCtrlThread(void *arg)
{
    int sock;
    int bytes_read;
 
    u32_t addr_len;
    struct sockaddr_in server_addr, client_addr;
    SOCKET_INFO client_socket_info;
 
    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        DBGPRINT_LWIP(DEBUG_ERROR, "Socket error\n");
        goto DEL_TASK;
    }
 
    /* 初始化服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_CTRL_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero),0, sizeof(server_addr.sin_zero));
 
    /* 绑定socket到服务端地址 */
    if (bind(sock,(struct sockaddr *)&server_addr,
             sizeof(struct sockaddr)) == -1)
    {
        /* 绑定地址失败 */
        DBGPRINT_LWIP(DEBUG_ERROR, "Bind error\n");
        goto DEL_TASK;
    }
 
    addr_len = sizeof(struct sockaddr);
 
    while (1)
    {
        /* 从sock中收取最大600字节数据 */
        bytes_read = recvfrom(sock, OtaCtrlBuf, OTA_CTRL_BUF_SIZE, 0,
                              (struct sockaddr *)&client_addr, &addr_len); 
 
        client_socket_info.local_fd = sock; 
        client_socket_info.ip = client_addr.sin_addr.s_addr;
        client_socket_info.port = client_addr.sin_port;

        upgrade_socket_info = client_socket_info;

        OtaPktProcess(OtaCtrlBuf, bytes_read, &client_socket_info);
    }
 
DEL_TASK:	
	OSTaskDel(OS_PRIO_SELF);
}


/*
******************************************************************************
**                        void OtaCtrlThread(void)
**
** Description  : 处理TCP报文，UDP为Process_Pkts  
** Arguments    : 
                  
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/

void OtaDataThread(void *arg)
{
   u32_t sin_size;
   int sock, connected, bytes_received;
   struct sockaddr_in server_addr, client_addr;
   BOOL_T stop = NST_FALSE; /* 停止标志 */
   int ret = 0;

   /* 一个socket在使用前，需要预先创建出来，指定SOCK_STREAM为TCP的socket */
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
        /* 创建失败的错误处理 */
        DBGPRINT_LWIP(DEBUG_ERROR, "Socket error\n");
        goto DEL_TASK;
   }

   /* 初始化服务端地址 */
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(TCP_DATA_PORT); /* 服务端工作的端口 */
   server_addr.sin_addr.s_addr = INADDR_ANY;
   memset(&(server_addr.sin_zero),8, sizeof(server_addr.sin_zero));

   /* 绑定socket到服务端地址 */
   if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
   {
        /* 绑定失败 */
        DBGPRINT_LWIP(DEBUG_ERROR, "Unable to bind\n");
        goto DEL_TASK;
   }

   /* 在socket上进行监听 */
   if (listen(sock, 5) == -1)
   {
        DBGPRINT_LWIP(DEBUG_ERROR, "Listen error\n");
        goto DEL_TASK;
   }
   
   sin_size = sizeof(struct sockaddr_in);

   DBGPRINT_LWIP(DEBUG_TRACE ,"\nTCPServer Waiting for client on port %d...\n", TCP_DATA_PORT);
   while (stop == NST_FALSE)
   {
        /* 接受一个客户端连接socket的请求，这个函数调用是阻塞式的 */
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        /* 返回的是连接成功的socket */
        if (connected < 0)
        {
            DBGPRINT_LWIP(DEBUG_ERROR, "accept socket error!");
            continue;
        }

       /* 接受返回的client_addr指向了客户端的地址信息 */
        DBGPRINT_LWIP(DEBUG_TRACE, "I got a connection from (%s , %d)\n",
              inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

        while (1)
        {
            /*B:handle fw upgrade*/
            if (upgradeFlag & UPGRADE_START)
            {
                int i = 0;
                
                if (!(upgradeFlag & UPGRADE_DOING))
                {
                    WritePtr = 0;
                    recvFileSize = 0;
                    upgradeFlag |= UPGRADE_DOING;    
                }

                /* 从connected socket中接收数据，接收buffer是1024大小，但并不一定能够收到1024大小的数据 */
                bytes_received = recv(connected, &OtaDataBuf[WritePtr], FLASH_SECTOR_SIZE - WritePtr, 0);
                if (bytes_received <= 0)
                {
                    if (bytes_received == 0)
                        DBGPRINT_LWIP(DEBUG_ERROR, "client close connection.\n");
                    else
                        DBGPRINT_LWIP(DEBUG_ERROR, "received failed, server close connection.\n");
                    
                    /* 接收失败，关闭这个connected socket */
                    lwip_close(connected);
                    upgradeFlag = 0;    
                    break;
                }

                recvFileSize += bytes_received;
                WritePtr += bytes_received;

                if (recvFileSize == fileSize)
                {
                    upgradeFlag |= UPGRADE_OKFILE;
                    /*写最后一个sector*/
                    if (QSpiWriteOneSector(NVM_FW_BAK_OFFSET + upgradeSector * FLASH_SECTOR_SIZE, OtaDataBuf) != 0)
                    {
                        lwip_close(connected);
                        upgradeFlag = 0;
                        break;
                    }
                    
                    upgradeSector++;
                    WritePtr = 0; 

                    printf("Download success, start to replace the current firmware\n");
                    for (i = 0; i < upgradeSector; i++)
                    {
                        QSpiFlashRead(NVM_FW_BAK_OFFSET + i * FLASH_SECTOR_SIZE, OtaDataBuf, FLASH_SECTOR_SIZE);
                        if (QSpiWriteOneSector(NVM_FW_MAIN_OFFSET + i * FLASH_SECTOR_SIZE, OtaDataBuf) != 0)
                        {
                            lwip_close(connected);
                            upgradeFlag = 0;
                            ret = -1;
                            break;
                        }
                    }

                    if (ret <0)
                        break;
			
                    upgradeFlag |= UPGRADE_DONE;
                    upgradeSector = 0;
                    recvFileSize = 0;
                    printf("Firmware update success\n");

                    FwUpgradeFinishAck();
                    OSTimeDly(100); // wait for upgrade finish ack being sent out
                    lwip_close(connected);
                    upgradeFlag = 0;
                    break;
                }
                else if (WritePtr == FLASH_SECTOR_SIZE)
                {				
                    /*写一个sector*/
                    if (QSpiWriteOneSector(NVM_FW_BAK_OFFSET + upgradeSector * FLASH_SECTOR_SIZE, OtaDataBuf) != 0)
                    {
                        lwip_close(connected);
                        upgradeFlag = 0;   
                        break;
                    }
                    
                    upgradeSector++;
                    WritePtr = 0;
                }
                
                printf("use sector:%d, tot:%d, WritePtr:%d, bytes_received:%d\n", 
						upgradeSector, recvFileSize, WritePtr, bytes_received);
            }
            else
            {
                lwip_close(connected);
                break;
            }
        }
    }
    
DEL_TASK:	
	OSTaskDel(OS_PRIO_SELF);
}

BOOL_T TestOTAUpdateFw(VOID)
{
    sys_thread_new("DevDiscoverThread", DevDiscoverThread, NULL, 512, TCPIP_THREAD_PRIO + 2);
    sys_thread_new("OtaCtrlThread", OtaCtrlThread, NULL, 512, TCPIP_THREAD_PRIO + 3);
    sys_thread_new("OtaDataThread", OtaDataThread, NULL, 512, TCPIP_THREAD_PRIO + 4);

    printf("OTA update ready\n");
    while (1) 
        OSTimeDly(1000);
}
#endif

#ifdef TEST_AIRKISS
int TestAirkissFlag = 0;
airkiss_context_t AirkissCtx;
airkiss_config_t AirkissCfg;
airkiss_result_t AirkissRes;
VOID TestAirkissRecvCb(UINT8 *pRecvBuf, UINT32 pRecvLen)
{
    UINT32 Ret = 0;

    Ret = airkiss_recv(&AirkissCtx, pRecvBuf, pRecvLen);
    if (Ret == AIRKISS_STATUS_CHANNEL_LOCKED)
        TestAirkissFlag = 2;
    else if (Ret == AIRKISS_STATUS_COMPLETE)
        TestAirkissFlag = 3;
}

VOID BroadcastRand()
{
    int ret = 0;
    int count = 0;
    
    int udp_sock;
    const int opt = 1;    
    struct sockaddr_in addrto;
    int nlen = sizeof(addrto);
    
    if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        DBGPRINT(DEBUG_ERROR, "UDP Broadcast Socket error\n");
        return;
    }
    
    if (setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)) == -1) 
    {  
        DBGPRINT(DEBUG_ERROR, "set UDP Broadcast socket error...\n");  
		lwip_close(udp_sock);  
        return;  
    }  
    
    memset(&addrto, 0, sizeof(struct sockaddr_in));  
    addrto.sin_family = AF_INET;  
    addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);  
    addrto.sin_port=htons(10000);  
    
    DBGPRINT(DEBUG_TRACE, "Respond with udp broadcast.");
    
    while (1)
    {
        ret = sendto(udp_sock, &AirkissRes.random, 1, 0, (struct sockaddr *)&addrto, nlen);
        if (ret < 0) 
        {
            continue;
        } 
        
        /* print debug message to console */
        if (count < 5 * 10)  // broadcast 5s
        {
            if (count % 200 == 0) 
                printf("\b..");
    
            if (count % 3 == 0) 
                printf("\b\\");
            else if (count % 3 == 1) 
                printf("\b-");
            else if (count % 3 == 2) 
                printf("\b/");
                
            OSTimeDly(10);
        } 
        else
        {
            printf("\n");
            break;
        }
        count++;
    }

    lwip_close(udp_sock);
    DBGPRINT(DEBUG_TRACE, "Respond finished.\n");
}

VOID TestAirkiss(VOID)
{
    SCAN_INFO_TABLE * pScanTable = NULL;
    UINT8 i, j;
    
    DBGPRINT(DEBUG_TRACE, "Test airkiss start\n");   

    // init airkiss
    AirkissCfg.memset = memset;
    AirkissCfg.memcpy = memcpy;
    AirkissCfg.memcmp = memcmp;
    //AirkissCfg.printf = printf; // not necessary
    airkiss_init(&AirkissCtx, &AirkissCfg);

    // scan all channel to get ap list
    pScanTable = OSMMalloc(sizeof(SCAN_INFO_TABLE) + sizeof(SCAN_INFO) * (32 -1));
    if (!pScanTable)
    {
        DBGPRINT(DEBUG_TRACE, "Test airkiss out of memory\n");
        return;
    }

SCAN_LOOP:
    TestAirkissFlag = 0;
    InfSsidSet(0, 0);
    InfNetModeSet(PARAM_NET_MODE_STA_BSS);
    InfWiFiStart();
    pScanTable->InfoCount = 32;   
    InfScanStart(pScanTable, 0, 0);

    // wait scan done
    while (TestAirkissFlag != 1)
        OSTimeDly(10);
    InfWiFiStop();
    
    // sniff packets of every ap in the scan info list
    for (i = 0; i < pScanTable->InfoCount; i++)
    {
        InfSnifferStart(TestAirkissRecvCb, pScanTable->InfoList[i].Bssid, pScanTable->InfoList[i].Channel);
        for (j = 0; j < 50; j++)
        {
            // wait channel lock
            if (TestAirkissFlag >= 2)
            {
                break;
            }
            OSTimeDly(1);
        }

        if (TestAirkissFlag >= 2)
            break;
        InfWiFiStop();
    }

    if (TestAirkissFlag >= 2)
    {
        for (j = 0; j < 6000; j++)// wait 60s
        {
            if (TestAirkissFlag >= 3)
            {
                break;
            }
            OSTimeDly(1);
        }
    }
    else
        goto SCAN_LOOP;

    InfWiFiStop();
    if (TestAirkissFlag >= 3)
    {
        DBGPRINT(DEBUG_TRACE, "airkiss ok\n");
        TestAirkissFlag = 0;
        airkiss_get_result(&AirkissCtx, &AirkissRes);
        InfSsidSet(AirkissRes.ssid, AirkissRes.ssid_length);
        InfKeySet(0, AirkissRes.pwd, AirkissRes.pwd_length);
        InfEncModeSet(PARAM_ENC_MODE_AUTO);
        InfWiFiStart();

        /* wait wifi link up */
        while (TestAirkissFlag == 0)
            OSTimeDly(30);
        
        BroadcastRand();
    }
    else
        DBGPRINT(DEBUG_TRACE, "airkiss timeout\n");

    OSMFree(pScanTable);
    DBGPRINT(DEBUG_TRACE, "Test airkiss done\n");
    while (1) 
        OSTimeDly(1000);
}
#endif

/*
******************************************************************************
**                        VOID AppEvtCallBack(UINT32 event)
**
** Description  : deal with system event, such as linkup/linkdown
** Arguments    : 
                  event: SYS_EVT_ID
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID AppEvtCallBack(SYS_EVT_ID event)
{
	switch (event)
	{
		case SYS_EVT_LINK_UP:
			DBGPRINT(DEBUG_TRACE, "SYS_EVT_LINK_UP\n");
			OSSemPost(pLinkUpSem);
		#ifdef TEST_DIRECT_CONFIG
       		TestDirectCfgFlag = 1;
       	#endif
		#ifdef TEST_SOFTAP_CONFIG
       		link_status = SYS_EVT_LINK_UP;
       	#endif
		#ifdef TEST_AIRKISS
			TestAirkissFlag = 1;
		#endif
			break;
		
		case SYS_EVT_LINK_DOWN:
			DBGPRINT(DEBUG_TRACE, "SYS_EVT_LINK_DOWN\n");
		#ifdef TEST_SOFTAP_CONFIG
       		link_status = SYS_EVT_LINK_DOWN;
       	#endif
			break;

		case SYS_EVT_JOIN_FAIL:
			DBGPRINT(DEBUG_TRACE, "SYS_EVT_JOIN_FAIL\n");
			break;

		case SYS_EVT_DHCP_FAIL:
			DBGPRINT(DEBUG_TRACE, "SYS_EVT_DHCP_FAIL\n");
			break;

		case SYS_EVT_SCAN_DONE:
			DBGPRINT(DEBUG_TRACE, "SYS_EVT_SCAN_DONE\n");
		#ifdef TEST_SNIFFER
			TestSnifferFlag = 1;
		#endif
		#ifdef TEST_AIRKISS
			TestAirkissFlag = 1;
		#endif
			break;

		case SYS_EVT_DIRECT_CFG_DONE:
		#ifdef TEST_DIRECT_CONFIG
			TestDirectCfgFlag = 1;
		#else
			DBGPRINT(DEBUG_TRACE, "SYS_EVT_DIRECT_CFG_DONE\n");
		#endif
			break;

		default:
			break;
	}
}

/*
******************************************************************************
**                        static void TestAppThread(void *arg)
**
** Description  : supply applicantion test example
                         user can set TEST_TCP_CLIENT/TEST_TCP_SERVER/TEST_UDP_SERVER/TEST_UDP_CLIENT
                         at app_cfg.h, but only one of them each time
** Arguments    : 
                  arg:void *
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static void TestAppThread(void *arg)
{
	
#if defined TEST_TCP_CLIENT
    TestTcpClient(); 
#elif defined TEST_UDP_SERVER
    TestUdpServer();
#elif defined TEST_UDP_CLIENT
    TestUdpClient();
#endif

    while(1)
    {
        OSTimeDly(200);
    }
}

/*
******************************************************************************
**                        VOID TestAppMain(VOID * pParam)
**
** Description  : application test task
** Arguments    : 
                  pParam
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID TestAppMain(VOID * pParam)
{
    UINT8 Err; 
    
    int i = 0;

#ifdef LWIP_SUPPORT
    pLinkUpSem = OSSemCreate(0);
#endif
    InfSysEvtCBSet(AppEvtCallBack);
    InfLoadDefaultParam();

#ifdef TEST_DIRECT_CONFIG
    TestDirectConfig();
#endif

#ifdef TEST_SOFTAP_CONFIG
    TestSoftApConfig();
#endif

#ifdef TEST_SNIFFER
    TestSniffer();
#endif

#ifdef TEST_AIRKISS
    TestAirkiss();
#endif

#ifdef TEST_UART_UPDATE_FW
    TestUartUpdateFw();
#endif
    
#ifdef HW_I2S_SUPPORT
    //I2S interface start, only for I2S interface test, should be delete after test
    BSP_I2SStart();
#endif //HW_I2S_SUPPORT

#ifdef SPI_SDIO_CMD_TEST
    BSP_SpiSdioCmdTest();
#endif

    InfWiFiStart();
  
    //Waiting for connect success
    OSSemPend(pLinkUpSem, 0, &Err);

#ifdef TEST_OTA_UPDATE_FW
    TestOTAUpdateFw();
#endif

    //generat test senddata
    for(i = 0; i < APP_DATA_LEN; i++)
        SendData[i] = i & 0xFF;

    sys_thread_new("TestAppThread", TestAppThread, NULL, NST_TEST_APP_TASK_STK_SIZE, TCPIP_THREAD_PRIO + 2);

#ifdef TEST_TCP_SERVER
    TestTcpServer();
#endif   

    while (1) 
    {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDly(20);
        DBGPRINT_LWIP(DEBUG_INFO, "TestApp RxDataCnt=%d, SendDataCnt=%d.\n", RxDataCnt, SendDataCnt);
    }

}
#endif // LWIP_SUPPORT
#endif // TEST_APP_SUPPORT

