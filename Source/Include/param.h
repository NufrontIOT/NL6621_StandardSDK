/*************************************************
 Copyright (C), 2013, BEIJING NUFRONT CO., LTD
 File name: Param.h     
 Author: jun.chen@nufront.com                  
 Version: V1.0.0     
 Date:  2013-7-12     
 Description: This file contains system parameter manage structures and APIs.
 Others: ...      
 Function List:     
     1. ...
 History: 
     1. Date: ...
       Author: ...
       Modification: ...
     2. ...
*************************************************/
#ifndef __PARAM_H__
#define __PARAM_H__


typedef enum
{
    PARAM_NET_MODE_STA_BSS = 0,
    PARAM_NET_MODE_STA_ADHOC,
    PARAM_NET_MODE_SOFTAP,
    PARAM_NET_MODE_MAX
}PARAM_NET_MODE;

typedef enum
{
    PARAM_PHY_MODE_BGN = 0,
    PARAM_PHY_MODE_BG,
    PARAM_PHY_MODE_B_ONLY,
    PARAM_PHY_MODE_MAX
}PARAM_PHY_MODE;

typedef enum
{
    PARAM_ENC_MODE_OPEN = 0,
    PARAM_ENC_MODE_WEP,
    PARAM_ENC_MODE_TKIP,
    PARAM_ENC_MODE_CCMP,
    PARAM_ENC_MODE_AUTO,	     // auto adapt security mode, only used in sta mode
    PARAM_ENC_MODE_MAX           // Not a real mode, defined as upper bound
}PARAM_ENC_MODE;

typedef enum
{
    PARAM_AUTH_MODE_OPEN = 0,
    PARAM_AUTH_MODE_SHARE,
    PARAM_AUTH_MODE_WPAPSK,
    PARAM_AUTH_MODE_WPA2PSK,
    PARAM_AUTH_MODE_MAX           // Not a real mode, defined as upper bound
}PARAM_AUTH_MODE;

typedef enum
{
    PARAM_WSC_MODE_DISABLE = 0,
    PARAM_WSC_MODE_PIN,
    PARAM_WSC_MODE_PBC,
    PARAM_WSC_MODE_MAX
}PARAM_WSC_MODE;

typedef enum
{
    PARAM_RATE_MODE_CCK = 0,
    PARAM_RATE_MODE_OFDM,
    PARAM_RATE_MODE_HT,
    PARAM_RATE_MODE_MAX,
} PARAM_RATE_MODE;

typedef enum
{
    PARAM_RATE_IDX_AUTO = 0,
    PARAM_RATE_IDX_0,
    PARAM_RATE_IDX_1,
    PARAM_RATE_IDX_2,
    PARAM_RATE_IDX_3,
    PARAM_RATE_IDX_4,
    PARAM_RATE_IDX_5,
    PARAM_RATE_IDX_6,
    PARAM_RATE_IDX_7
} PARAM_RATE_IDX;

typedef enum
{
    PARAM_TX_PWR_LEVEL_LOW = 0,
    PARAM_TX_PWR_LEVEL_NORM,
    PARAM_TX_PWR_LEVEL_HIGH
} PARAM_TX_PWR_LEVEL;

typedef	struct _WIFI_CFG{
	INT8U Ssid[33];			// ssid to create or join, end with '\0'
	INT8U SSIDBrdEnable;	// boardcast ssid in softap mode
	INT8U PhyMode;			// 802.11 phy mode, 0-802.11bgn mixed,  1-802.11b/g mixed, 2-802.11b only
	INT8U Channel;			// default operate channel used in softap and adhoc mode
	INT8U Encry;				// 0-none 1-wep 2-tkip  3-ccmp 4-auto
	INT8U AuthMode;			// 0-open 1-wep 2-wpapsk 3-wpa2psk
	INT8U Protocol;			// 0-sta 1-adhoc 2-softap
	INT8U KeyLength;		// key data length in the PSK field
	INT8U KeyIndex;			// key index, valid in wep encryption mode
	INT8U PSK[64];			// key data
	INT8U WscEn;			// ap wps enable, 0 - disable, 1- enable 
	INT8U ConTryTimes;		// max times for sta try to join an bss, 0 - infinite, other - max try times
	INT8U WmmEn;		              // wmm enable, 0 - disable, 1- enable
}WIFI_CFG;

typedef	struct _IP_CFG{
		INT8U Ip[4];				// ipv4 address
		INT8U Netmask[4];		// subnet mask
		INT8U GateWay[4];		// default gateway ip
		INT8U Dns[4];			// dns server ip
		INT8U bDhcp;			// dhcp client 0-disable, 1-enable
		INT8U DhcpTryTimes;		// dhcp client try times, 0 - infinite, other - max try times
		INT8U Res_2[2];
}IP_CFG;

typedef struct _CFG_PARAM{
	WIFI_CFG WiFiCfg;
	IP_CFG IPCfg;
	INT8U bDhcpServer;			// dhcp server 0-disable, 1-enable
	INT8U bDnsServer;			// dns server 0-disable, 1-enable
	INT8U DnsName[33];			// local dns name, end with '\0'
	INT8U Res_3[1];  /* reserv and mark the end */
}CFG_PARAM;

extern CFG_PARAM SysParam;

// scan info table
typedef struct _SCAN_INFO{
	INT8U Bssid[6];
	INT8U Ssid[32];
	INT8U SsidLen;
	INT8U BssType;			// 1-ADHOC, 2-AP
	INT8U Channel;			// 1~13
	INT8U EncryMode;		// 0-none 1-wep 2-tkip  3-ccmp
	INT8U AuthMode;			// 0-open 1-wep 2-wpapsk 3-wpa2psk
	INT8   Rssi;				//(dbm)
}SCAN_INFO;

typedef struct _SCAN_INFO_TABLE{
	INT8U           InfoCount;	// the number of scan info items in the Info array
	SCAN_INFO   InfoList[1];
}SCAN_INFO_TABLE;

// system event used by application callback
typedef enum
{
    SYS_EVT_LINK_DOWN = 0,                /* WiFi Link Down */
    SYS_EVT_LINK_UP,                 		/* WiFi Link Up */
    SYS_EVT_JOIN_FAIL,				/* Sta Join Fail */
    SYS_EVT_DHCP_FAIL,			       /* Dhcp Client failed */
    SYS_EVT_SCAN_DONE,				/* Sta Scan completed */
    SYS_EVT_DIRECT_CFG_DONE		/* Direct Configure Done */
}SYS_EVT_ID;
typedef VOID (*SysEvtCallBack)(SYS_EVT_ID Event);

// deep sleep wake up mode
#define WAKEUP_MODE_GPIO0          	0x00000000
#define WAKEUP_MODE_32K_TIMER  	0x80000000

typedef enum
{
    PS_MODE_DISABLE = 0,
    PS_MODE_LEGACY,
    PS_MODE_FAST,
    PS_MODE_SOFT
} PWR_SAVE_MODE;

typedef enum
{
    PROTECT_MODE_AUTO = 0,
    PROTECT_MODE_ALWAYS,
    PROTECT_MODE_NONE
} PROTECT_MODE;

typedef VOID (*MonRecvCallBack)(UINT8 *pRecvBuf, UINT32 pRecvLen);

/*================================================================*/
/*                 configuration interface declare                */
/*================================================================*/
VOID InfLoadDefaultParam(VOID);

INT32 InfConfigQuery(CFG_PARAM *pParam);

INT32 InfNetModeSet(PARAM_NET_MODE Mode);

INT32 InfPhyModeSet(PARAM_PHY_MODE Mode);

INT32 InfSsidSet(UINT8 *pSsid, UINT8 SsidLen);

INT32 InfSsidBrdcastSet(UINT8 Enable);

INT32 InfChannelSet(UINT8 Ch);

INT32 InfEncModeSet(PARAM_ENC_MODE EncMode);

INT32 InfAuthModeSet(PARAM_AUTH_MODE AuthMode);

INT32 InfKeySet(UINT8 Index, UINT8 *KeyBuf, UINT8 KeyLen);

INT32 InfConTryTimesSet(UINT8 TryTimes);

INT32 InfWmmEnableSet(BOOLEAN Enable);

#ifdef AP_WSC_INCLUDED
INT32 InfWscEnSet(BOOLEAN WscEn);
#endif //AP_WSC_INCLUDED

INT32 InfIpSet(IP_CFG *IpParam);

INT32 InfDhcpSrvSet(UINT8 Enable);

INT32 InfDnsSrvSet(UINT8 Enable, UINT8 *DnsName, UINT8 DnsNameLen);

INT32 InfSysEvtCBSet(SysEvtCallBack pFuncCb);

INT32 InfMacAddrSet(UINT8 Addr[6]);

/*==================================================================*/
/*                     control interface declare                    */
/*==================================================================*/

INT32 InfWiFiStart(VOID);

INT32 InfWiFiStop(VOID);

INT32 InfScanStart(SCAN_INFO_TABLE * pScanTable, INT8 MinRssiFilter, INT8 ScanType);

INT32 InfTxRateSet(PARAM_RATE_MODE RateMode, PARAM_RATE_IDX RateIdx);

INT32 InfTxPwrLevelSet(PARAM_TX_PWR_LEVEL TxPwrLevel);

INT32 InfPowerSaveSet(UINT8 PowerSaveEn);

INT32 InfDeepSleepSet(UINT32 WakeUpMode);

INT8 InfPeerRssiGet(UINT8 pMacAddr[6] );

UINT8 InfCurChGet(VOID);

INT32  InfEfuseInfoGet(UINT8 Offset, UINT8 Cnt, UINT32* pEfuseInfo);

INT32  InfBeaconPeriodSet(UINT16 BcnPrd, UINT8 NotifyPeerEn);

INT32 InfListenIntervalSet(UINT8 ListenIntv, BOOL_T ListenDtim);

INT32 InfDirectCfgStart(INT8 MinRssiFilter, UINT8 TryPeerNum, UINT8 *pCustomStr);

INT32 InfPeerAgeOutSet(INT32U Time);

INT32 InfPeerKickOut(UINT8 pMacAddr[6]);

INT32 InfProtectModeSet(PROTECT_MODE Mode);

#ifdef WSC_INCLUDED
INT32 InfWPSStart(UINT8 WscMode, UINT32 PinCode);

INT32 InfWPSStop(VOID);

#ifdef STA_WSC_INCLUDED
INT32  InfStaWpsPinGet(UINT8 pWpsPin[9]);
#endif //STA_WSC_INCLUDED
#endif // WSC_INCLUDED

INT32  InfSnifferStart(MonRecvCallBack pFuncCb, UINT8 Bssid[6], UINT8 Channel);

INT32 InfVendorIESet(UINT8 * pIePtr, UINT8 IeLen);

UINT64 InfSysTimeGet(VOID);

#endif//__PARAM_H__

