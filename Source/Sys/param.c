/*************************************************
 Copyright (C), 2013, BEIJING NUFRONT CO., LTD
 File name: Param.c     
 Author: jun.chen@nufront.com                  
 Version: V1.0.0     
 Date:  2013-7-17     
 Description: This file contains system parameter manage implementation.
 Others: ...      
 Function List:     
     1. ...
 History: 
     1. Date: ...
       Author: ...
       Modification: ...
     2. ...
*************************************************/
#include "includes.h"

/* global variable define */
CFG_PARAM SysParam;

/* decalare variables and functions */
extern UINT8   PermanentAddress[MAC_ADDR_LEN];
extern SysEvtCallBack SysEvtCB;
extern INT8 MlmeGetRssi(PUINT8 pMacAddr);
extern VOID MlmeGetWpsPin(PUINT8 pPinCode);
extern UINT8 MlmeGetCurCh(VOID);

#define xxxxxxxxxxxxxxxxxxxxxxxx

/*==================================================================*/
/*               configuration interface definition                 */
/*==================================================================*/

/*   InfLoadDefaultParam   */
/*-------------------------------------------------------------------------
	Description:	load default config parameters
	Arguments:
		
	Return Value:
		None.
		
	Note:	
		
-------------------------------------------------------------------------*/
VOID InfLoadDefaultParam(VOID)
{
	CFG_PARAM *cfg = &SysParam;

	memset((VOID *)cfg, 0, sizeof(CFG_PARAM));

	// wifi config
	sprintf((char *)cfg->WiFiCfg.Ssid, "nuap%02x%02x%02x", PermanentAddress[3], PermanentAddress[4], PermanentAddress[5]); 
	cfg->WiFiCfg.SSIDBrdEnable = 1;	// not hide ssid in softap mode
	cfg->WiFiCfg.Channel = 11;			// default operate channel used in softap and adhoc mode
	cfg->WiFiCfg.Encry = 0;			//0-none 1--wep 2--tkip  3--ccmp 4--auto
	cfg->WiFiCfg.AuthMode = 0;			//0-open 1--wep 2--wpa1  3--wpa2
	cfg->WiFiCfg.Protocol = 2;			//0-sta 1-adhoc 2-softap
	cfg->WiFiCfg.KeyIndex = 0;
	sprintf((char *)cfg->WiFiCfg.PSK, "1234567890");
	cfg->WiFiCfg.KeyLength = strlen("1234567890");
	cfg->WiFiCfg.WmmEn = 0;			// enable wmm
		
	// ip config
	cfg->IPCfg.bDhcp = 1;
	cfg->IPCfg.Ip[0] = 192;
	cfg->IPCfg.Ip[1] = 168;
	cfg->IPCfg.Ip[2] = 0;
	cfg->IPCfg.Ip[3] = 1;
	cfg->IPCfg.Netmask[0] = 255;
	cfg->IPCfg.Netmask[1] = 255;
	cfg->IPCfg.Netmask[2] = 255;
	cfg->IPCfg.Netmask[3] = 0;
	cfg->IPCfg.GateWay[0] = 192;
	cfg->IPCfg.GateWay[1] = 168;
	cfg->IPCfg.GateWay[2] = 0;
	cfg->IPCfg.GateWay[3] = 1;
	cfg->IPCfg.Dns[0] = 192;
	cfg->IPCfg.Dns[1] = 168;
	cfg->IPCfg.Dns[2] = 0;
	cfg->IPCfg.Dns[3] = 1;

	// other
	cfg->bDhcpServer = 1;
	cfg->bDnsServer = 1;
	strcpy((char *)cfg->DnsName, "local.nufront");
}

/*   InfConfigQuery   */
/*-------------------------------------------------------------------------
	Description:	
		query current system config param
	Arguments:
		pParam - param buffer to store the querying result
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32  InfConfigQuery(CFG_PARAM *pParam)
{
	if (pParam == NULL)
		return -1;

	NST_MOVE_MEM(pParam, &SysParam, sizeof(CFG_PARAM));
	return 0;
}

/*   InfNetModeSet   */
/*-------------------------------------------------------------------------
	Description:	
		set network mode, sta/adhoc/softap
	Arguments:
		Mode - network mode, check PARAM_NET_MODE for detail
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfNetModeSet(PARAM_NET_MODE Mode)
{
	if ((UINT8)Mode >= PARAM_NET_MODE_MAX)
		return -1;

	SysParam.WiFiCfg.Protocol = (UINT8) Mode;
	return 0;
}

/*   InfPhyModeSet   */
/*-------------------------------------------------------------------------
	Description:	
		set 802.11 phy mode
	Arguments:
		Mode - network mode, check PARAM_PHY_MODE for detail
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfPhyModeSet(PARAM_PHY_MODE Mode)
{
	if ((UINT8)Mode >= PARAM_PHY_MODE_MAX)
		return -1;

	SysParam.WiFiCfg.PhyMode = (UINT8) Mode;
	return 0;
}

/*   InfSsidSet   */
/*-------------------------------------------------------------------------
	Description:	
		set ssid
	Arguments:
		pSsid - ssid string
		SsidLen - ssid length (1 ~ 32)
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfSsidSet(UINT8 *pSsid, UINT8 SsidLen)
{
	if (pSsid == NULL || SsidLen > 32 || SsidLen == 0)
	{
		SysParam.WiFiCfg.Ssid[0] = 0; 
		return -1;
	}

	NST_MOVE_MEM(SysParam.WiFiCfg.Ssid, pSsid, SsidLen);
	SysParam.WiFiCfg.Ssid[SsidLen] = 0;
	return 0;
}

/*   InfSsidBrdcastSet   */
/*-------------------------------------------------------------------------
	Description:	
		set ssid broadcast enable
	Arguments:
		Enable - 0 - hide ssid, 1 - broadcast ssid
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfSsidBrdcastSet(UINT8 Enable)
{
	SysParam.WiFiCfg.SSIDBrdEnable = Enable ? 1 : 0;
	return 0;
}

/*   InfChannelSet   */
/*-------------------------------------------------------------------------
	Description:	
		set work channel, only used in soft ap and adhoc mode
	Arguments:
		Ch - 1 ~ 13
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfChannelSet(UINT8 Ch)
{
	if (Ch < 1 || Ch > 13)
		return -1;

	SysParam.WiFiCfg.Channel = Ch;
	return 0;
}

/*   InfEncModeSet   */
/*-------------------------------------------------------------------------
	Description:	
		set encryption mode
	Arguments:
		EncMode - encryption mode, check PARAM_ENC_MODE for detail
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfEncModeSet(PARAM_ENC_MODE EncMode)
{
	if ((UINT8)EncMode >= PARAM_ENC_MODE_MAX)
		return -1;

	SysParam.WiFiCfg.Encry = (UINT8) EncMode;
	return 0;
}


/*   InfAuthModeSet   */
/*-------------------------------------------------------------------------
	Description:	
		set authentication mode
	Arguments:
		AuthMode - authentication mode, check PARAM_AUTH_MODE for detail
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfAuthModeSet(PARAM_AUTH_MODE AuthMode)
{
	if ((UINT8)AuthMode >= PARAM_AUTH_MODE_MAX)
		return -1;

	SysParam.WiFiCfg.AuthMode = (UINT8) AuthMode;
	return 0;
}

/*   InfKeySet   */
/*-------------------------------------------------------------------------
	Description:	
		set key data
	Arguments:
		Index - key index, only useful in wep encryption mode (0 ~ 3)
		KeyBuf - key data buffer
		KeyLen - key length (restricted by encryption mode)
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfKeySet(UINT8 Index, UINT8 *KeyBuf, UINT8 KeyLen)
{
	if (Index > 3 || KeyBuf == NULL || KeyLen > 64)
		return -1;

	SysParam.WiFiCfg.KeyIndex = Index;
	SysParam.WiFiCfg.KeyLength = KeyLen;
	NST_MOVE_MEM(SysParam.WiFiCfg.PSK, KeyBuf, KeyLen);

	if (KeyLen < 64)
		SysParam.WiFiCfg.PSK[KeyLen] = 0;
	
	return 0;
}

/*   InfConTryTimesSet   */
/*-------------------------------------------------------------------------
	Description:	
		set current param connect try times, only useful in sta mode
	Arguments:
		TryTimes - Try times, 0 indicatie try infinitely
		
	Return Value:
		0 - success
		other - failure
	Note:	
		When sta connect fail times beyond the limit(TryTimes), 
		the connect process will be stopped, and the event SYS_EVT_JOIN_FAIL will be generated
-------------------------------------------------------------------------*/
INT32 InfConTryTimesSet(UINT8 TryTimes)
{
	SysParam.WiFiCfg.ConTryTimes = TryTimes;
	return 0;
}

/*   InfWmmEnableSet   */
/*-------------------------------------------------------------------------
	Description:	
		set WMM function enable
	Arguments:
		Enable - 0 disable wmm, 1 enable wmm
		
	Return Value:
		0 - success
		other - failure
	Note:	
-------------------------------------------------------------------------*/
INT32 InfWmmEnableSet(BOOLEAN Enable)
{
      SysParam.WiFiCfg.WmmEn = (INT8U) Enable;
      return 0 ;
}

#ifdef AP_WSC_INCLUDED
/*   InfWscSet   */
/*-------------------------------------------------------------------------
	Description:	
		set wps enable , only useful in AP mode
	Arguments:
		WscEn - 1 : WPS enable    0:WPS disable
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfWscEnSet(BOOLEAN WscEn)
{
	SysParam.WiFiCfg.WscEn = (UINT8) WscEn;
	return 0;
}
#endif //AP_WSC_INCLUDED

/*   InfIpSet   */
/*-------------------------------------------------------------------------
	Description:	
		set Ip parameters
	Arguments:
		IpParam - IP parameters, check IP_CFG for detail
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfIpSet(IP_CFG *IpParam)
{
	if (IpParam == NULL)
		return -1;

	NST_MOVE_MEM(&SysParam.IPCfg, IpParam, sizeof(IP_CFG));
	return 0;
}

/*   InfDhcpSrvSet   */
/*-------------------------------------------------------------------------
	Description:	
		set dhcp server enable, only useful in softap mode
	Arguments:
		Enable - 0 - disable, other - enable
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfDhcpSrvSet(UINT8 Enable)
{
	SysParam.bDhcpServer = Enable ? 1 : 0;
	return 0;
}

/*   InfDnsSrvSet   */
/*-------------------------------------------------------------------------
	Description:	
		set dns server enable, only useful in softap mode
	Arguments:
		Enable - 0 - disable, other - enable
		DnsName - local dns name
		DnsNameLen - local dns name length

	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfDnsSrvSet(UINT8 Enable, UINT8 *DnsName, UINT8 DnsNameLen)
{
	if (Enable)
	{
		if (DnsName == NULL || DnsNameLen > 32)
			return -1;
		SysParam.bDnsServer = 1;
		NST_MOVE_MEM(SysParam.DnsName, DnsName, DnsNameLen);
	}
	else
		SysParam.bDnsServer = 0;

	return 0;
}

/*   InfMacAddrSet  */
/*-------------------------------------------------------------------------
	Description:	
		set adapter mac address
	Arguments:
		Addr - mac address(48 bits)
		
	Return Value:
		0 - success
		other - failure
	Note:	
		
-------------------------------------------------------------------------*/
INT32 InfMacAddrSet(UINT8 Addr[6])
{
	if (Addr == NULL)
		return -1;

	NST_MOVE_MEM(PermanentAddress, Addr, 6);
	return 0;
}

/*   InfSysEvtCBSet   */
/*-------------------------------------------------------------------------
	Description:	
		set system event callback function
		
	Arguments:
		pFuncCb - fuction pointer set to system, check SysEvtCallBack
				for detail
		
	Return Value:
		0 - success
		other - failure
		
	Note: 

-------------------------------------------------------------------------*/
INT32 InfSysEvtCBSet(SysEvtCallBack pFuncCb)
{
	if (pFuncCb == NULL)
		return -1;

	SysEvtCB = pFuncCb;
	return 0;
}

#define xxxxxxxxxxxxxxxxxxxxxxxx

/*==================================================================*/
/*                    control interface definition                  */
/*==================================================================*/

/*   InfWiFiStart   */
/*-------------------------------------------------------------------------
	Description:	
		interface to start wifi
		
	Arguments:
		
	Return Value:
		0 - success
		other - failure
		
	Note: 
		make sure call this interface after all wifi parameters 
		has been set.
-------------------------------------------------------------------------*/
INT32 InfWiFiStart(VOID)
{
	PostTaskMsg(gpMacMngTskMsgQ, MLME_START_ID, NULL, 0);
	return 0;
}

/*   InfWiFiStop   */
/*-------------------------------------------------------------------------
	Description:	
		interface to stop wifi
		
	Arguments:
		
	Return Value:
		0 - success
		other - failure
		
	Note: 
              make sure call this interface before modify wifi parameters 
-------------------------------------------------------------------------*/
INT32 InfWiFiStop(VOID)
{
	PostTaskMsg(gpMacMngTskMsgQ, MLME_STOP_ID, NULL, 0);
	return 0;
}

/*   InfScanStart   */
/*-------------------------------------------------------------------------
	Description:	
		start an scan operation
	Arguments:
	
       pScanTable -  buffer to store the scan result, InfoCount field indicate the max buffer size
       			when scan result update, InfoCount field indicate the actual scan info number (1~16)

	MinRssiFilter - specify the least rssi of scan record
				-1 ~ -127: the rssi Threshold, scan records below which will be filterred
				other: not filter by rssi

	ScanType -   0 active scan, other  passive scan

	Return Value:
		0 - success
		other - failure
		
	Note:	
	scan result will update to the ScanTable when event SYS_EVT_SCAN_DONE generate
-------------------------------------------------------------------------*/
INT32 InfScanStart(SCAN_INFO_TABLE * pScanTable, INT8 MinRssiFilter, INT8 ScanType)
{
	INT8 MsgBody[6] = {0};

	if (!pScanTable || pScanTable->InfoCount == 0)
		return -1;

	if (MinRssiFilter > 0)
		MinRssiFilter = 0;

	NST_MOVE_MEM((INT8U *)MsgBody, &pScanTable, 4);
	MsgBody[4] = MinRssiFilter;
	MsgBody[5] = ScanType;
	
	PostTaskMsg(gpMacMngTskMsgQ, MLME_SCAN_REQUEST_ID, (PUINT8)MsgBody, 6);
	return 0;
}

/*   InfTxRateSet   */
/*-------------------------------------------------------------------------
    Description:    
        set phy tx rate after wifi link up
        
    Arguments:
	RateMode - modulation mode of tx rate, correspond to certain rate map
	RateIdx -  tx rate index of certain rate map
        
    Return Value:
        0 - success
        other - failure
        
    Note: 
       1) This function should be called after event SYS_EVT_LINK_UP
       2) Setting rules:
	Mode_CCK:   RateIdx IDX_0~IDX_3 => 1, 2, 5.5, 11 (Mbps)
	Mode_OFDM: RateIdx IDX_0~IDX_7 => 6, 9, 12, 18, 24, 36, 48, 54 (Mbps)
	Mode_HT:      RateIdx IDX_0~IDX_7 => Mcs0~Mcs7
-------------------------------------------------------------------------*/
INT32 InfTxRateSet(PARAM_RATE_MODE RateMode, PARAM_RATE_IDX RateIdx) 
{
	INT8U MsgBody[2] = {0};

	switch (RateMode)
	{
		case PARAM_RATE_MODE_CCK:
			if (RateIdx > PARAM_RATE_IDX_3)
				return -1;
			break;

		case PARAM_RATE_MODE_OFDM:
		case PARAM_RATE_MODE_HT:
			if (RateIdx > PARAM_RATE_IDX_7)
				return -1;
			break;

		default:
			return -1;
	}

	MsgBody[0] = (INT8U)RateMode;
	MsgBody[1] = (INT8U)RateIdx;

	PostTaskMsg(gpMacMngTskMsgQ, MLME_TX_RATE_SET_ID, MsgBody, 2);
	return 0;
}

/*   InfTxPwrLevelSet   */
/*-------------------------------------------------------------------------
    Description:    
        set tx power level: low, normal, high
        
    Arguments:
        TxPwrLevel - tx power level, check PARAM_TX_PWR_LEVEL for detail
    Return Value:
        0 - success
        other - failure
        
    Note: 

-------------------------------------------------------------------------*/
INT32 InfTxPwrLevelSet(PARAM_TX_PWR_LEVEL TxPwrLevel) 
{
	if (TxPwrLevel > PARAM_TX_PWR_LEVEL_10)
		return -1;

	PostTaskMsg(gpMacMngTskMsgQ, MLME_TX_PWR_LEVEL_SET_ID, (PUINT8)&TxPwrLevel, 1);
	return 0;
}

/*   InfPowerSaveSet   */
/*-------------------------------------------------------------------------
	Description:	
		interface to enable 802.11 power save mode
		
	Arguments:
		PowerSaveEn - power save enable, 
		0 - disable
		1 - legacy ps mode, send ps-poll frame to ap for buffered data
		2 - fast ps mode, switch to active mode dynamically when there is data to send or receive
		3 - soft ps mode, mcu keep running, open/close radio power dynamically.
		
	Return Value:
		0 - success
		other - failure
		
	Note: 

-------------------------------------------------------------------------*/
INT32 InfPowerSaveSet(UINT8 PowerSaveMode)
{	
	if (PowerSaveMode > PS_MODE_SOFT)
	    return -1;

	PostTaskMsg(gpMacMngTskMsgQ, MLME_PS_MODE_SET_ID, (PUINT8)&PowerSaveMode, 1);
	return 0;
}

/*   InfDeepSleepSet   */
/*-------------------------------------------------------------------------
	Description:	
		interface to enter deep sleep mode
		
	Arguments:
		WakeUpMode - wake up mode for deep sleep mode
		0x00000000 - wake up by Gpio0
		0x8xxxxxxx  - wake up by 32K Timer, bit0 ~ 25 indicate wakeup interval (ms), bit26 ~30 reserved
		
	Return Value:
		0 - success
		other - failure
		
	Note: 
		All registers and data in sram will be clear, cpu will be reset after wake up
-------------------------------------------------------------------------*/
INT32 InfDeepSleepSet(UINT32 WakeUpMode)
{
	if ((WakeUpMode > WAKEUP_MODE_GPIO0) && !(WakeUpMode & WAKEUP_MODE_32K_TIMER))
		return -1;
	
	PostTaskMsg(gpMacMngTskMsgQ, MLME_DEEP_SLEEP_SET_ID, (PUINT8)&WakeUpMode, sizeof(UINT32));
	return 0;
}

/*   InfPeerRssiGet   */
/*-------------------------------------------------------------------------
	Description:	
		get rssi of peer terminal
		
	Arguments:
		pMacAddr - Mac address, if set null, the rssi of associated ap will be returned in sta mode
		
	Return Value:
		0 - failure
		< 0 - rssi value (dbm)

-------------------------------------------------------------------------*/
INT8 InfPeerRssiGet(UINT8 pMacAddr[6] )
{	
    return MlmeGetRssi(pMacAddr);	  
}

/*   InfCurChGet   */
/*-------------------------------------------------------------------------
	Description:	
		query current work channel
		
	Arguments:
		
	Return Value:
		channel number

-------------------------------------------------------------------------*/
UINT8 InfCurChGet(VOID)
{	
    return MlmeGetCurCh();	  
}

/*   InfEfuseInfoGet   */
/*-------------------------------------------------------------------------
	Description:	
		read chip efuse info
	Arguments:
		Offset - efuse info offset (0~7 by word )
		Cnt - word count to read
		pEfuseInfo - buffer to get efuse info
		
	Return Value:
		0 - success
		other - failure
	Note:	
		nl6621 efuse has 8 words (256 bits)
-------------------------------------------------------------------------*/
INT32  InfEfuseInfoGet(UINT8 Offset, UINT8 Cnt, UINT32* pEfuseInfo)
{
	if (!pEfuseInfo || Offset > 0x07)
		return -1;
	
	BSP_EfuseRead(Offset, Cnt, pEfuseInfo);
	return 0;
}

/*   InfBeaconPeriodSet   */
/*-------------------------------------------------------------------------
	Description:	
		set beacon period for softap or adhoc mode
	Arguments:
		BcnPrd - beacon period (ms), typical value is equal or larger than 100ms
		NotifyPeerEn - notify all peers to reconnect local softap, 0 - not notify, other - notify
		                     invalid in adhoc mode
		
	Return Value:
		0 - success
		other - failure
	Note:	
-------------------------------------------------------------------------*/
INT32  InfBeaconPeriodSet(UINT16 BcnPrd, UINT8 NotifyPeerEn)
{
	INT8U MsgBody[3] = {0};

	NST_MOVE_MEM((INT8U *)MsgBody, (INT8U *)&BcnPrd, 2);
	MsgBody[2] = NotifyPeerEn;
	
	PostTaskMsg(gpMacMngTskMsgQ, MLME_BCN_PRD_SET_ID, (PUINT8)MsgBody, sizeof(MsgBody));
	return 0;
}

/*   InfListenIntervalSet   */
/*-------------------------------------------------------------------------
    Description:    
        interface to set STA Listen Interval
        
    Arguments:
        ListenInterval - Tbtt count for sta to keep sleeping, 1~ 255 (typical value is 1~3)
        ListenDtim - True - listen dtim, False - not listen dtim
        
    Return Value:
        0 - success
        other - failure
        
    Note: 
-------------------------------------------------------------------------*/
INT32 InfListenIntervalSet(UINT8 ListenIntv, BOOL_T ListenDtim)
{
    INT8U MsgBody[2] = {0};

    if (ListenIntv == 0)
        return -1;

    MsgBody[0] = ListenIntv;
    MsgBody[1] = (INT8U)ListenDtim;
    PostTaskMsg(gpMacMngTskMsgQ, MLME_LISTEN_INTVL_SET_ID, (PUINT8)MsgBody, sizeof(MsgBody));
    
    return 0;
}

/*   InfDirectCfgStart   */
/*-------------------------------------------------------------------------
	Description:	
		interface to start sta direct config process
		
	Arguments:
        	MinRssiFilter - specify the least rssi of peer ap to listen to
        				-1 ~ -127: the rssi Threshold, ap with rssi below which will be ignored
        				other: not filter by rssi
        	TryPeerNum - max number of peer ap, we try to listen to
        				0 - no restrictiion
        				other - the max number of ap

        	pCustomStr - custom string buffer(maximum 10 ascii characters), could be NULL if no custom data to receive
        	
	Return Value:
		0 - success
		other - failure
		
	Note: 
		Direct config is used for sta to get settings of ap for connection at first time,
		this function must cooperate with the matched application run in pc, smart phone, pad, etc..
		
		This interface will start wifi internally, so don`t call InfWiFiStart any more.
		
		When the process succeeds, the ssid and key data will be filled in SysParam.WiFiCfg
-------------------------------------------------------------------------*/
INT32 InfDirectCfgStart(INT8 MinRssiFilter, UINT8 TryPeerNum, UINT8 *pCustomStr)
{
	INT8U MsgBody[6] = {0};

	MsgBody[0] = MinRssiFilter;
	MsgBody[1] = (INT8U)TryPeerNum;
	if (pCustomStr)
		NST_MOVE_MEM((INT8U *)(&MsgBody[2]), &pCustomStr, 4);

	PostTaskMsg(gpMacMngTskMsgQ, MLME_DIRECT_CFG_ID, (PUINT8)MsgBody, 6);
	return 0;
}

/*   InfSnifferStart   */
/*-------------------------------------------------------------------------
	Description:	
		start sniffer mode
	Arguments:
	       pFuncCb : data receiving callback
	       Bssid : the bssid of ap to bind
		Channel : the sniffer channel
		
	Return Value:
		0 - success
		other - failure
	Note:	
	Sniffer only support receiving packets from/to the BindBssid matched ap at a time
	If need to change sniffer settings, InfWiFiStart should be called before calling this function again
-------------------------------------------------------------------------*/
INT32  InfSnifferStart(MonRecvCallBack pFuncCb, UINT8 BindBssid[6], UINT8 Channel)
{
	INT8U MsgBody[11] = {0};

	memcpy(&MsgBody[0], (VOID *)(&pFuncCb), 4);
	memcpy(&MsgBody[4], BindBssid, 6);
	MsgBody[10] = Channel;
	PostTaskMsg(gpMacMngTskMsgQ, MLME_SNIFFER_START_ID, (PUINT8)MsgBody, 11);
	return 0;
}

/*   InfPeerAgeOutSet   */
/*-------------------------------------------------------------------------
	Description:	
		interface to set idle sta age out time
		
	Arguments:
        	Time - specify the time to wait before age out idle sta
        		   0 - not age out idle peer
        		   other - age out time in second
        	
	Return Value:
		0 - success
		other - failure
		
	Note: 
		peer age out function only be valid in softap mode
-------------------------------------------------------------------------*/
INT32 InfPeerAgeOutSet(INT32U Time)
{
	PostTaskMsg(gpMacMngTskMsgQ, MLME_PEER_AGE_OUT_SET_ID, (PUINT8)&Time, sizeof(INT32U));
	return 0;
}

/*   InfPeerKickOut   */
/*-------------------------------------------------------------------------
	Description:	
		interface to kick out specified peer sta
		
	Arguments:
        	pMacAddr: peer sta mac address
        	
	Return Value:
		0 - success
		other - failure
		
	Note: 
		this function only be valid in softap mode
-------------------------------------------------------------------------*/
INT32 InfPeerKickOut(UINT8 pMacAddr[6])
{
	if (pMacAddr == NULL)
		return -1;
	PostTaskMsg(gpMacMngTskMsgQ, MLME_PEER_KICK_OUT_ID, (PUINT8)pMacAddr, 6);
	return 0;
}

/*   InfProtectModeSet   */
/*-------------------------------------------------------------------------
	Description:	
		interface to set b/g protection mode
		
	Arguments:
        	Mode: protection mode
        		0 - auto use protection
        		1 - always use protection
        		2 - not use protection
        	
	Return Value:
		0 - success
		other - failure
		
	Note: 
-------------------------------------------------------------------------*/
INT32 InfProtectModeSet(PROTECT_MODE Mode)
{
	if (Mode > PROTECT_MODE_NONE)
		return -1;

	PostTaskMsg(gpMacMngTskMsgQ, MLME_PROTECT_MODE_SET_ID, (PUINT8)(&Mode), 1);
	return 0;
}

#ifdef WSC_INCLUDED
/*   InfWPSStart   */
/*-------------------------------------------------------------------------
	Description:	
		interface to start wps
		
	Arguments:
		WscMode - wps work mode, check PARAM_WSC_MODE for detail
		PinCode - wps pin code ( set peer netcard pin code when start AP WPS PIN mode, otherwise set 0)
		
	Return Value:
		0 - success
		other - failure
		
	Note: 

-------------------------------------------------------------------------*/
INT32 InfWPSStart(UINT8 WscMode, UINT32 PinCode)
{
       INT8U MsgBody[5] = {0};

	DBGPRINT_WSC(DEBUG_TRACE,"-----------InfWPSStart ---------\n");
	if (WscMode > PARAM_WSC_MODE_PBC)
		return -1;
	   	
#ifdef AP_WSC_INCLUDED
      if (WscMode == PARAM_WSC_MODE_PIN)
	    NST_MOVE_MEM((INT8U *)MsgBody, (INT8U *)&PinCode, 4);
#endif

	MsgBody[4] = (UINT8)WscMode;
	PostTaskMsg(gpMacMngTskMsgQ, MLME_WPS_START_SET_ID, (PUINT8)MsgBody, 5);
	return 0;
}

/*   InfWPSStop   */
/*-------------------------------------------------------------------------
	Description:	
		interface to stop WPS
		
	Arguments:
		
	Return Value:
		0 - success
		other - failure
		
	Note: 
-------------------------------------------------------------------------*/
INT32 InfWPSStop(VOID)
{
       DBGPRINT_WSC(DEBUG_TRACE,"-----------InfWPSStop ---------\n");
	PostTaskMsg(gpMacMngTskMsgQ, MLME_WPS_STOP_SET_ID, NULL, 0);
	return 0;
}

#ifdef STA_WSC_INCLUDED
/*   InfStaWpsPinGet   */
/*-------------------------------------------------------------------------
	Description:	
		read sta wps pincode info
	Arguments:
	       pWpsPin[9] : store Pin code as string, 8 byte character and 1byte '\0'
		
	Return Value:
		0 - success
		other - failure
	Note:	
-------------------------------------------------------------------------*/
INT32  InfStaWpsPinGet(UINT8 pWpsPin[9])
{
	if (pWpsPin == NULL)
		return -1;
	
	MlmeGetWpsPin(pWpsPin);
	printf("Get WPS PIN   %s \n", pWpsPin);
	return 0;
}
#endif //STA_WSC_INCLUDED

#endif //WSC_INCLUDED

/*   InfVendorIESet   */
/*-------------------------------------------------------------------------
	Description:	
		interface to set vendor ie in beacon and probe response frame of softap mode
		
	Arguments:
        	pIePtr: ie buffer pointer
              IeLen: ie length
        	
	Return Value:
		0 - success
		other - failure
		
	Note: 
	The memory of ie buffer should not be free until call this function again with pIePtr equal NULL
-------------------------------------------------------------------------*/
INT32 InfVendorIESet(UINT8 * pIePtr, UINT8 IeLen)
{
	INT8U MsgBody[5] = {0};

	NST_MOVE_MEM((INT8U *)MsgBody, (INT8U *)&pIePtr, 4);
	MsgBody[4] = IeLen;

	PostTaskMsg(gpMacMngTskMsgQ, MLME_VENDOR_IE_SET_ID, (PUINT8)(MsgBody), 5);
	return 0;
}

/*   InfSysTimeGet   */
/*-------------------------------------------------------------------------
	Description:	
		interface to get system launch time
		
	Arguments:
        	
	Return Value:
		UINT64	system launch time in millisecond
		
	Note: 
	The memory of ie buffer should not be free until call this function again with pIePtr equal NULL
-------------------------------------------------------------------------*/
UINT64 InfSysTimeGet(VOID)
{
	LARGE_INTEGER Ticks = BSP_GetLaunchTicks();
	return ((Ticks.QuadPart)/1024*125) + ((Ticks.QuadPart)%1024*125/1024);
}

