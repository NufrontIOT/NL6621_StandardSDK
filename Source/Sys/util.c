#include <includes.h>

/*
******************************************************************************
Description: 
    This function is called by your application code to define a timer.
Arguments: 
    cb          Is a pointer to a callback function that will be called when the timer expires. The 
                 callback function must be declared as follows:
                 
                 void MyCallback (OS_TMR *ptmr, void *p_arg);
    cb_arg   Is an argument (a pointer) that is passed to the callback function when it is called.

    repeat   The timer counts down only once or reloads itself.
Returns: 
    A pointer to an OS_TMR data structure.
              This is the 'handle' that your application will use to reference the timer created.
Author: 
    lihongbin
Date: 
    2011/10/21
******************************************************************************
*/
VOID    NST_InitTimer(
    IN  NST_TIMER **    pTimer,
    IN  OS_TMR_CALLBACK     TimerFunc,
    IN  PVOID                   pData,
    IN  BOOL_T          Repeat)
{
    UINT32  dly = 0;
    UINT8   opt;
    UINT8   name[10] = "Timer";
    UINT8   err = 0;
    
    if (*pTimer)
    {
        //DBGPRINT(DEBUG_WARN, "WARN: pTimer already exists !!! \n");
        return;
    }

    if (NST_TRUE == Repeat)
    {
        opt = OS_TMR_OPT_PERIODIC;
    }
    else
    {
        opt = OS_TMR_OPT_ONE_SHOT;
    }
    
    *pTimer = OSTmrCreate(dly, dly, opt, TimerFunc, pData, name, &err);
    if (*pTimer == NULL)
    {
        //DBGPRINT(DEBUG_WARN, "WARN: OSTmrCreate failed !!! \n");   
    }
}

//
//
VOID    NST_DelTimer(
    IN  NST_TIMER **    pTimer,
    OUT BOOL_T      *pCancelled)
{
    UINT8   err = 0;

    if (*pTimer == NULL)
    {
        //DBGPRINT(DEBUG_WARN, "WARN: Delete Timer - pTimer is NULL !!! \n");
        return;
    }
    else
    {
        *pCancelled = OSTmrDel(*pTimer, &err);
        *pTimer = NULL;
    }
}

/*
******************************************************************************
Description: 
    This function is called by your application code to delete a timer.
Arguments: 
    ptmr        Is a pointer to an OS_TMR
    time        period or delay time
Returns: 
    TRUE        If the call was successful
    FALSE       If not
Author: 
    lihongbin
Date: 
    2011/10/21
******************************************************************************
*/
VOID    NST_SetTimer(
    IN  NST_TIMER * pTimer,
    IN  UINT32          Value)      // in ms, at least 10 ms
{
    UINT32  TimeValue = Value*OS_TMR_CFG_TICKS_PER_SEC/1000;    // in tick
    UINT8   err = 0;

    if (pTimer == NULL)
    {
        //DBGPRINT(DEBUG_WARN, "WARN: Set Timer - pTimer is NULL !!! \n");
        return;
    }

    if (TimeValue ==0)
    {
        //DBGPRINT(DEBUG_WARN, "WARN: value of timer should >= 10 ms\n");
        TimeValue = 1;
    }

    if (OS_TMR_OPT_PERIODIC == pTimer->OSTmrOpt)
    {
        pTimer->OSTmrPeriod = TimeValue;
        pTimer->OSTmrDly = 0u;
    }
    else
    {
        pTimer->OSTmrPeriod = 0u;
        pTimer->OSTmrDly = TimeValue;
    }
    
    OSTmrStart(pTimer, &err);
}

/*
******************************************************************************
Description: 
    This function is called by your application code to stop a timer.
Arguments: 
    ptmr        Is a pointer to an OS_TMR
Returns: 
    TRUE        If the call was successful
    FALSE       If not
Author: 
    lihongbin
Date: 
    2011/10/21
******************************************************************************
*/
VOID    NST_CancelTimer(
    IN  NST_TIMER * pTimer,
    OUT BOOL_T      *pCancelled)
{
    UINT8   err = 0;
    UINT8   opt = OS_TMR_OPT_NONE;
    
    if (pTimer == NULL)
    {
        //DBGPRINT(DEBUG_WARN, "WARN: Cancel Timer - pTimer is NULL !!! \n");
        return;
    }
    
    *pCancelled = OSTmrStop(pTimer, opt, NULL, &err);
}

/*
    ========================================================================
    
    Routine Description:
        Init timer objects

    Arguments:
        pTimer                Timer structure
        Value                Timer value in milliseconds

    Return Value:
        None

    Note:
        To use this routine, must call NSTInitTimer before.
        
    ========================================================================
*/
VOID    NST_ModTimer(
    IN    NST_TIMER    *pTimer,
    IN    UINT32                    Value)
{
    BOOL_T Cancelled;
    
    if (pTimer == NULL)
    {
        //DBGPRINT(DEBUG_WARN, "WARN: Modify Timer - pTimer is NULL !!! \n");
        return;
    }

    NST_CancelTimer(pTimer, &Cancelled);
    NST_SetTimer(pTimer, Value);
}

// =============================================================================

VOID NST_ALLOC_LOCK(
    OUT  NST_LOCK **  Lock)
{
    UINT8 Prio = OS_LOWEST_PRIO, Err = 0;
    
    *Lock = OSMutexCreate(Prio, &Err);
}

VOID NST_FREE_LOCK(
    IN  NST_LOCK *  Lock)
{
    UINT8 Opt = OS_DEL_NO_PEND, Err = 0;
    
    OSMutexDel(Lock, Opt, &Err);
}

VOID NST_AQUIRE_LOCK(
    IN  NST_LOCK *  Lock)
{
    UINT32 Timeout = 0;
    UINT8 Err = 0; 
    
    OSMutexPend(Lock, Timeout, &Err);  
}

VOID NST_RELEASE_LOCK(
    IN  NST_LOCK *  Lock)
{
    OSMutexPost(Lock);
}

/*
******************************************************************************
**  VOID NST_MoveMemOvlap(UINT8  *pDst, UINT8  *pSrc, UINT16  Size)
**
** Description  :  
                memory move(if source and destination is ovlapped, call this function not NST_MOVE_MEM) 
** Arguments    : 
                  pDst, pSrc, Size
                  
** Returns      : 
**                
                  
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID NST_MoveMemOvlap(UINT8  *pDst, UINT8  *pSrc, UINT16  Size)
{
    if((UINT32)pDst > (UINT32)pSrc)
    {// start from the last byte
        UINT8* pSrcStart = pSrc + Size - 1;
        UINT8* pDstStart = pDst + Size - 1;
        while (Size > 0u) 
        {
            *pDstStart-- = *pSrcStart--;
            Size--;
        }
    }
    else
    {
        while (Size > 0u) 
        {
            *pDst++ = *pSrc++;
            Size--;
        }
    }
}

#ifdef DMA_MOVE_MEM
/*
******************************************************************************
**  VOID NST_DmaMoveMem(UINT8  *pDst, UINT8  *pSrc, UINT16  Size)
**
** Description  :  
                memory move using DMA
** Arguments    : 
                  pDst, pSrc, Size
                  
** Returns      : 
**                
                  
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
/*
DMA模式： AFE_test_mode   0  Tx_calib_en/Rx_Calib_en  0    Dma_disable   0   DMA_RX  x
1、配DMA RX/TX 模式（传输方向）, SRC->DST 为TX方向，DST->SRC为RX方向
2、配DMA模式
3、配DMA首地址
4、配DMA目的地址
5、配传输WORD数
6、启动DMA   
7、等待DMA完成中断
8、处理DMA中断，清中断
*/
//注意:通过DMA搬数据要求源和目的地址都是4字节对齐的
VOID NST_DmaMoveMem(UINT8  *pDst, UINT8  *pSrc, UINT16  Size)
{
    UINT8 MemAlignModDst = (UINT32)pDst & (MEM_ALIGNMENT - 1);
    UINT8 MemAlignModSrc = (UINT32)pSrc & (MEM_ALIGNMENT - 1);
    UINT8 Err = 0;
    //UINT32 BEGIN = 0, END = 0;

    //ENTER;
    
    //printf("Size = %d \n", Size);
    //BEGIN = *Tmr1Val;
    
    // MemAlignModDst != MemAlignModSrc
    if (MemAlignModDst != MemAlignModSrc)
    {
        //printf("MemAlignModDst != MemAlignModSrc \n");
        NST_MoveBigMem(pDst, pSrc, Size);
        goto DONE;
    }

    // MemAlignModDst == MemAlignModSrc
    if (MemAlignModDst > 0)
    {
        //printf("MemAlignModDst == MemAlignModSrc \n");
        NST_MoveBigMem(pDst, pSrc, MemAlignModDst);
    }
    
    pDmaDst = pDst + MemAlignModDst;
    pDmaSrc = pSrc + MemAlignModSrc;
    DmaSize = Size - MemAlignModDst;
    
    // Short Data
    if ((Size - MemAlignModDst) < DMA_TRANSFER_SIZE_IN_BYTE)
    {
        //printf("Short = %d \n", DmaSize);
        NST_MoveBigMem(pDmaDst, pDmaSrc, DmaSize);
        goto DONE;
    }
        
    pDmaDst = pDst + MemAlignModDst;
    pDmaSrc = pSrc + MemAlignModSrc;
    DmaSize = Size - MemAlignModDst;
    //printf("DMA = %d \n", DmaSize);
    // First Block
    BSP_DmaStart(pDmaDst, pDmaSrc, DMA_CHANNEL_1);

    OSSemPend(pDmaSem, 0, &Err);
    OSSemSet(pDmaSem, 0, &Err);
    
    // Left
    if (DmaSize > 0)
    {
        //printf("Left = %d \n", DmaSize);
        NST_MoveBigMem(pDmaDst, pDmaSrc, DmaSize);
        goto DONE;
    }

DONE:    
    //END = *Tmr1Val;
    //printf("Time = %d %d\n", (BEGIN-END), (BEGIN-END)/40);
    //LEAVE;
    return;
}
#endif // DMA_MOVE_MEM //

#if 0
/*
******************************************************************************
**  VOID NST_MoveBigMem(UINT8  *pDst, UINT8  *pSrc, UINT16  Size)
**
** Description  :  
                memory move considering memory align,but not consider overlap
** Arguments    : 
                  pDst, pSrc, Size
                  
** Returns      : 
**                
                  
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID NST_MoveBigMem(UINT8  *pDst, UINT8  *pSrc, UINT16  Size)
{
    UINT8 MemAlignModDst = (UINT32)pDst & (MEM_ALIGNMENT - 1);
    UINT8 MemAlignModSrc = (UINT32)pSrc & (MEM_ALIGNMENT - 1);
    BOOL_T MemAligned = (MemAlignModDst == MemAlignModSrc) ? NST_TRUE : NST_FALSE; 
    UINT8 i;
    UINT16 SizeRem = Size;// size remainder
    UINT32* pMemAlignDst;
    UINT32* pMemAlignSrc;
    
    // 源地址和目的地址离mem对齐边界等距
    if(MemAligned)
    {
        if(MemAlignModDst > 0)
        {
        // mem align边界前的部分
            i = MemAlignModDst;
            while(SizeRem > 0 && i < MEM_ALIGNMENT)
            {
                *pDst++ = *pSrc++;
                SizeRem--;
                i++;
            }
        }

        pMemAlignDst = (UINT32*)pDst;
        pMemAlignSrc = (UINT32*)pSrc;
        while(SizeRem > MEM_ALIGNMENT)
        {
            *pMemAlignDst++ = *pMemAlignSrc++;
            SizeRem -= MEM_ALIGNMENT;
        }

        pDst = (UINT8*)pMemAlignDst;
        pSrc = (UINT8*)pMemAlignSrc;
    }
    
    while(SizeRem > 0)
    {
        *pDst++ = *pSrc++;
        SizeRem--;
    }
}
#endif   

const INT8U __crc8_tbl[256]=
{
	0x00,0x91,0xe3,0x72,0x07,0x96,0xe4,0x75,
	0x0e,0x9f,0xed,0x7c,0x09,0x98,0xea,0x7b,
	0x1c,0x8d,0xff,0x6e,0x1b,0x8a,0xf8,0x69,
	0x12,0x83,0xf1,0x60,0x15,0x84,0xf6,0x67,
	0x38,0xa9,0xdb,0x4a,0x3f,0xae,0xdc,0x4d,
	0x36,0xa7,0xd5,0x44,0x31,0xa0,0xd2,0x43,
	0x24,0xb5,0xc7,0x56,0x23,0xb2,0xc0,0x51,
	0x2a,0xbb,0xc9,0x58,0x2d,0xbc,0xce,0x5f,
	0x70,0xe1,0x93,0x02,0x77,0xe6,0x94,0x05,
	0x7e,0xef,0x9d,0x0c,0x79,0xe8,0x9a,0x0b,
	0x6c,0xfd,0x8f,0x1e,0x6b,0xfa,0x88,0x19,
	0x62,0xf3,0x81,0x10,0x65,0xf4,0x86,0x17,
	0x48,0xd9,0xab,0x3a,0x4f,0xde,0xac,0x3d,
	0x46,0xd7,0xa5,0x34,0x41,0xd0,0xa2,0x33,
	0x54,0xc5,0xb7,0x26,0x53,0xc2,0xb0,0x21,
	0x5a,0xcb,0xb9,0x28,0x5d,0xcc,0xbe,0x2f,
	0xe0,0x71,0x03,0x92,0xe7,0x76,0x04,0x95,
	0xee,0x7f,0x0d,0x9c,0xe9,0x78,0x0a,0x9b,
	0xfc,0x6d,0x1f,0x8e,0xfb,0x6a,0x18,0x89,
	0xf2,0x63,0x11,0x80,0xf5,0x64,0x16,0x87,
	0xd8,0x49,0x3b,0xaa,0xdf,0x4e,0x3c,0xad,
	0xd6,0x47,0x35,0xa4,0xd1,0x40,0x32,0xa3,
	0xc4,0x55,0x27,0xb6,0xc3,0x52,0x20,0xb1,
	0xca,0x5b,0x29,0xb8,0xcd,0x5c,0x2e,0xbf,
	0x90,0x01,0x73,0xe2,0x97,0x06,0x74,0xe5,
	0x9e,0x0f,0x7d,0xec,0x99,0x08,0x7a,0xeb,
	0x8c,0x1d,0x6f,0xfe,0x8b,0x1a,0x68,0xf9,
	0x82,0x13,0x61,0xf0,0x85,0x14,0x66,0xf7,
	0xa8,0x39,0x4b,0xda,0xaf,0x3e,0x4c,0xdd,
	0xa6,0x37,0x45,0xd4,0xa1,0x30,0x42,0xd3,
	0xb4,0x25,0x57,0xc6,0xb3,0x22,0x50,0xc1,
	0xba,0x2b,0x59,0xc8,0xbd,0x2c,0x5e,0xcf
};

INT8U NST_GetCrc8(INT8U *ptr, INT32U len)
{
	INT8U crc8,data;

	crc8=0;
	while(len--!=0)
	{
		data = *ptr++;
    	crc8=__crc8_tbl[crc8^data];
    }
    return crc8;
}

INT32 _cal_crc32(INT32 crc_result, INT8U data_8)
{
	INT8U crc_out[32];
	INT8U crc_buf[32];
	INT8U in_data_buf[8];
	INT32 i;
	INT32 flag = 0x1;

	for(i = 0; i < 32; i++)
	{
		crc_out[i] = 0;
	}
	

	for(i = 0; i < 8; i++)
	{
		in_data_buf[i] = (data_8 >> i) & flag;
	}

	for(i = 0; i < 32; i++)
	{
		crc_buf[i] = (unsigned char)(crc_result >> i) & flag;
	}

 	crc_out[0]  = in_data_buf[1]^in_data_buf[7]^crc_buf[30]^crc_buf[24];
	crc_out[1]  = in_data_buf[0]^in_data_buf[1]^in_data_buf[6]^in_data_buf[7]^crc_buf[31]^crc_buf[30]^crc_buf[25]^crc_buf[24];
	crc_out[2]  = in_data_buf[0]^in_data_buf[1]^in_data_buf[5]^in_data_buf[6]^in_data_buf[7]^crc_buf[31]^crc_buf[30]^crc_buf[26]^crc_buf[25]^crc_buf[24];
	crc_out[3]  = in_data_buf[0]^in_data_buf[4]^in_data_buf[5]^in_data_buf[6]^crc_buf[31]^crc_buf[27]^crc_buf[26]^crc_buf[25];
	crc_out[4]  = in_data_buf[1]^in_data_buf[3]^in_data_buf[4]^in_data_buf[5]^in_data_buf[7]^crc_buf[30]^crc_buf[28]^crc_buf[27]^crc_buf[26]^crc_buf[24];
	crc_out[5]  = in_data_buf[0]^in_data_buf[1]^in_data_buf[2]^in_data_buf[3]^in_data_buf[4]^in_data_buf[6]^in_data_buf[7]^
                 crc_buf[31]^crc_buf[30]^crc_buf[29]^crc_buf[28]^crc_buf[27]^crc_buf[25]^crc_buf[24];
	crc_out[6]  = in_data_buf[0]^in_data_buf[1]^in_data_buf[2]^in_data_buf[3]^in_data_buf[5]^in_data_buf[6]^
                 crc_buf[31]^crc_buf[30]^crc_buf[29]^crc_buf[28]^crc_buf[26]^crc_buf[25];
	crc_out[7]  = in_data_buf[0]^in_data_buf[2]^in_data_buf[4]^in_data_buf[5]^in_data_buf[7]^crc_buf[31]^crc_buf[29]^crc_buf[27]^crc_buf[26]^crc_buf[24];
	crc_out[8]  = in_data_buf[3]^in_data_buf[4]^in_data_buf[6]^in_data_buf[7]^crc_buf[28]^crc_buf[27]^crc_buf[25]^crc_buf[24]^crc_buf[0];
	crc_out[9]  = in_data_buf[2]^in_data_buf[3]^in_data_buf[5]^in_data_buf[6]^crc_buf[29]^crc_buf[28]^crc_buf[26]^crc_buf[25]^crc_buf[1];
	crc_out[10] = in_data_buf[2]^in_data_buf[4]^in_data_buf[5]^in_data_buf[7]^crc_buf[29]^crc_buf[27]^crc_buf[26]^crc_buf[24]^crc_buf[2];
	crc_out[11] = in_data_buf[3]^in_data_buf[4]^in_data_buf[6]^in_data_buf[7]^crc_buf[28]^crc_buf[27]^crc_buf[25]^crc_buf[24]^crc_buf[3];
  
	crc_out[12] = in_data_buf[1]^in_data_buf[2]^in_data_buf[3]^in_data_buf[5]^in_data_buf[6]^in_data_buf[7]^
                 crc_buf[30]^crc_buf[29]^crc_buf[28]^crc_buf[26]^crc_buf[25]^crc_buf[24]^crc_buf[4];
	crc_out[13] = in_data_buf[0]^in_data_buf[1]^in_data_buf[2]^in_data_buf[4]^in_data_buf[5]^in_data_buf[6]^
                 crc_buf[31]^crc_buf[30]^crc_buf[29]^crc_buf[27]^crc_buf[26]^crc_buf[25]^crc_buf[5];
	crc_out[14] = in_data_buf[0]^in_data_buf[1]^in_data_buf[3]^in_data_buf[4]^in_data_buf[5]^crc_buf[31]^crc_buf[30]^crc_buf[28]^crc_buf[27]^crc_buf[26]^crc_buf[
6];
	crc_out[15] = in_data_buf[0]^in_data_buf[2]^in_data_buf[3]^in_data_buf[4]^crc_buf[31]^crc_buf[29]^crc_buf[28]^crc_buf[27]^crc_buf[7];
	crc_out[16] = in_data_buf[2]^in_data_buf[3]^in_data_buf[7]^crc_buf[29]^crc_buf[28]^crc_buf[24]^crc_buf[8];
	crc_out[17] = in_data_buf[1]^in_data_buf[2]^in_data_buf[6]^crc_buf[30]^crc_buf[29]^crc_buf[25]^crc_buf[9];
	crc_out[18] = in_data_buf[0]^in_data_buf[1]^in_data_buf[5]^crc_buf[31]^crc_buf[30]^crc_buf[26]^crc_buf[10];
	crc_out[19] = in_data_buf[0]^in_data_buf[4]^crc_buf[31]^crc_buf[27]^crc_buf[11];
	crc_out[20] = in_data_buf[3]^crc_buf[28]^crc_buf[12];
	crc_out[21] = in_data_buf[2]^crc_buf[29]^crc_buf[13];
	crc_out[22] = in_data_buf[7]^crc_buf[24]^crc_buf[14];
	crc_out[23] = in_data_buf[1]^in_data_buf[6]^in_data_buf[7]^crc_buf[30]^crc_buf[25]^crc_buf[24]^crc_buf[15];
	crc_out[24] = in_data_buf[0]^in_data_buf[5]^in_data_buf[6]^crc_buf[31]^crc_buf[26]^crc_buf[25]^crc_buf[16];
	crc_out[25] = in_data_buf[4]^in_data_buf[5]^crc_buf[27]^crc_buf[26]^crc_buf[17];
	crc_out[26] = in_data_buf[1]^in_data_buf[3]^in_data_buf[4]^in_data_buf[7]^crc_buf[30]^crc_buf[28]^crc_buf[27]^crc_buf[24]^crc_buf[18];
	crc_out[27] = in_data_buf[0]^in_data_buf[2]^in_data_buf[3]^in_data_buf[6]^crc_buf[31]^crc_buf[29]^crc_buf[28]^crc_buf[25]^crc_buf[19];
	crc_out[28] = in_data_buf[1]^in_data_buf[2]^in_data_buf[5]^crc_buf[30]^crc_buf[29]^crc_buf[26]^crc_buf[20];
	crc_out[29] = in_data_buf[0]^in_data_buf[1]^in_data_buf[4]^crc_buf[31]^crc_buf[30]^crc_buf[27]^crc_buf[21];
	crc_out[30] = in_data_buf[0]^in_data_buf[3]^crc_buf[31]^crc_buf[28]^crc_buf[22];
	crc_out[31] = in_data_buf[2]^crc_buf[23]^crc_buf[29];
 
	crc_result = 0;
	for(i = 0; i < 32; i++)
	{
		if(crc_out[i])
			crc_result |= (1<<i);
	}
	return crc_result;
}

INT32 NST_GetCrc32(UINT8 *pData, INT32 DataSize)
{
	INT32 i,val;
	INT32 crc_result = 0xFFFFFFFF;
	
	for(i = 0; i < DataSize; i++)
	{	
		crc_result = _cal_crc32(crc_result, pData[i]);		
	}

	val = 0;
	for(i = 0; i < 32; i++)
	{
		if((crc_result>>i) & 0x1)
		{
			val |= (1<<(31-i));
		}
	}
	
	return ~val;
}

UINT8 BtoH(char ch)
{
    if (ch >= '0' && ch <= '9') return (ch - '0');        // Handle numerals
    if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  // Handle capitol hex digits
    if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  // Handle small hex digits
    return(255);
}

//
//  FUNCTION: AtoH(char *, UCHAR *, int)
//
//  PURPOSE:  Converts ascii string to network order hex
//
//  PARAMETERS:
//    src    - pointer to input ascii string
//    dest   - pointer to output hex
//    destlen - size of dest
//
//  COMMENTS:
//
//    2 ascii bytes make a hex byte so must put 1st ascii byte of pair
//    into upper nibble and 2nd ascii byte of pair into lower nibble.
//
// IRQL = PASSIVE_LEVEL

void AtoH(PSTRING src, PUINT8 dest, int destlen)
{
    PSTRING srcptr;
    PUINT8 destTemp;

    srcptr = src;   
    destTemp = (PUINT8) dest; 

    while(destlen--)
    {
        *destTemp = BtoH(*srcptr++) << 4;    // Put 1st ascii byte in upper nibble.
        *destTemp += BtoH(*srcptr++);      // Add 2nd ascii byte to above.
        destTemp++;
    }
}

char *strcpy(char *dest, const char *src)
{
	char *dst = dest;

	while ((*dst = *src) != '\0') {
		src++;
		dst++;
	}

	return dest;
}

char *strcat (char *dest, const char *src)
{
  char *s1 = dest;
  const char *s2 = src;
  char c;

  /* Find the end of the string.  */
  do
    c = *s1++;
  while (c != '\0');

  /* Make S1 point before the next character, so we can increment
     it while memory is read (wins on pipelined cpus).  */
  s1 -= 2;

  do
    {
      c = *s2++;
      *++s1 = c;
    }
  while (c != '\0');

  return dest;
}

INT32U strspn (const char *s, const char *accept)
{
  const char *p;
  const char *a;
  size_t count = 0;

  for (p = s; *p != '\0'; ++p)
    {
      for (a = accept; *a != '\0'; ++a)
	if (*p == *a)
	  break;
      if (*a == '\0')
	return count;
      else
	++count;
    }

  return count;
}

char *strncpy (char *s1, const char *s2, INT32U n)
{
  char c;
  char *s = s1;

  --s1;

  if (n >= 4)
    {
      size_t n4 = n >> 2;

      for (;;)
	{
	  c = *s2++;
	  *++s1 = c;
	  if (c == '\0')
	    break;
	  c = *s2++;
	  *++s1 = c;
	  if (c == '\0')
	    break;
	  c = *s2++;
	  *++s1 = c;
	  if (c == '\0')
	    break;
	  c = *s2++;
	  *++s1 = c;
	  if (c == '\0')
	    break;
	  if (--n4 == 0)
	    goto last_chars;
	}
      n = n - (s1 - s) - 1;
      if (n == 0)
	return s;
      goto zero_fill;
    }

 last_chars:
  n &= 3;
  if (n == 0)
    return s;

  do
    {
      c = *s2++;
      *++s1 = c;
      if (--n == 0)
	return s;
    }
  while (c != '\0');

 zero_fill:
  do
    *++s1 = '\0';
  while (--n > 0);

  return s;
}
char *strpbrk(const char *s1, const char *s2)
{
	const char *s;
	const char *p;

	for ( s=s1 ; *s ; s++ ) {
		for ( p=s2 ; *p ; p++ ) {
			if (*p == *s) return (char *) s; /* silence the warning */
		}
	}
	return NULL;
}

VOID NST_DbgTimestamp(VOID)
{
    UINT32 tv;

    tv = OSTimeGet( );
   //printf("(%10d)",tv);
   printf("(%d)",tv);
}

VOID DBGPRINT_MGMT(UINT8 level, UINT8 *fmt, ...)
{
#if DEBUG_ON
    __va_list ap;
    char string[256];
	
    if(level <= DEBUG_LEVEL)  
    {
        NST_DbgTimestamp();
        va_start(ap,fmt);
        vsprintf(string,(const char *)fmt,ap);
        printf(string);
        va_end(ap);
    }
#endif
}

void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen)
{
#if DEBUG_ON
    unsigned char *pt;
    int x;

    pt = pSrcBufVA;
    printf("%s: %p, len = %d\n", str, pSrcBufVA, SrcBufLen);
    for (x = 0; x < SrcBufLen; x++) {
        if (x % 16 == 0)
            printf("0x%04x : ", x);
        printf("%02x ", ((unsigned char)pt[x]));
        if (x % 16 == 15)
            printf("\n");
    }
    printf("\n");
#endif
}
                  
#if 1
__asm VOID __aeabi_memcpy(UINT8 *pDest, UINT8  *pSrc, UINT32  Size)
{    
    cmp  r2, #3
    bls.w        _memcpy_lastbytes
    ands.w       ip, r0, #3
    beq.w        dst_aligned
    ldrb.w       r3, [r1], #1
    cmp.w        ip, #2
    add  r2, ip
    it   ls
    ldrbls.w     ip, [r1], #1
    strb.w       r3, [r0], #1
    it   cc
    ldrbcc.w     r3, [r1], #1
    sub.w        r2, r2, #4
    it   ls
    strbls.w     ip, [r0], #1
    it   cc
    strbcc.w     r3, [r0], #1
    
dst_aligned     

    ands.w       r3, r1, #3
    beq.w        __aeabi_memcpy4
    subs r2, #8
    
copy_8_bytes_loop

    bcc.w        copy_less_8_bytes

    ldr.w        r3, [r1], #4
    subs r2, #8
    ldr.w        ip, [r1], #4
    stmia.w      r0!, {r3, ip}
    b.n  copy_8_bytes_loop

copy_less_8_bytes

    adds r2, r2, #4
    itt  pl
    ldrpl.w      r3, [r1], #4
    strpl.w      r3, [r0], #4
    nop.w

_memcpy_lastbytes

    lsls r2, r2, #31
    itt  cs
    ldrbcs.w     r3, [r1], #1
    ldrbcs.w     ip, [r1], #1
    it   mi
    ldrbmi.w     r2, [r1], #1
    itt  cs
    strbcs.w     r3, [r0], #1
    strbcs.w     ip, [r0], #1
    it   mi
    strbmi.w     r2, [r0], #1
    bx   lr

}

__asm VOID __aeabi_memcpy4(UINT8 *pDest, UINT8  *pSrc, UINT32  Size)
{
    push {r4, lr}
    subs r2, #32
    bcc.w        copy_16_bytes_loop

copy_32_bytes_loop

    ldmia.w      r1!, {r3, r4, ip, lr}
    subs r2, #32
    stmia.w      r0!, {r3, r4, ip, lr}
    ldmia.w      r1!, {r3, r4, ip, lr}
    stmia.w      r0!, {r3, r4, ip, lr}
    bcs.w        copy_32_bytes_loop

copy_16_bytes_loop

    movs.w       ip, r2, lsl #28
    itt  cs
    ldmiacs.w    r1!, {r3, r4, ip, lr}
    stmiacs.w    r0!, {r3, r4, ip, lr}
    itt  mi
    ldmiami      r1!, {r3, r4}
    stmiami      r0!, {r3, r4}
    ldmia.w      sp!, {r4, lr}
    movs.w       ip, r2, lsl #30
    itt  cs
    ldrcs.w      r3, [r1], #4
    strcs.w      r3, [r0], #4
    it   eq
    bxeq lr
    
_memcpy_lastbytes_aligned       

    lsls         r2, r2, #31
    it           cs
    ldrhcs.w     r3, [r1], #2
    it   mi
    ldrbmi.w     r2, [r1], #1
    it   cs
    strhcs.w     r3, [r0], #2
    it   mi
    strbmi.w     r2, [r0], #1
    bx   lr
    
}
#endif
