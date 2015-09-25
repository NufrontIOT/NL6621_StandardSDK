
/*
******************************************************************************
**
** Project     : 
** Description :    NVIC
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/
#include <nvic.h>

/*******************************************************************************
* Function Name  : NVIC_StructInit
* Description    : Fills each NVIC_InitStruct member with its default value.
* Input          : - NVIC_InitStruct: pointer to a NVIC_InitTypeDef structure which
*                    will be initialized.
* Output         : None
* Return         : None
*******************************************************************************/
VOID NVIC_StructInit(NVIC_InitTypeDef* NVIC_InitStruct)
{
    /* NVIC_InitStruct members default value */
    NVIC_InitStruct->NVIC_IRQChannel = 0x00;
    NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct->NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct->NVIC_IRQChannelCmd = IRQ_DISABLE;
}

/*******************************************************************************
* Function Name  : NVIC_Init
* Description    : Initializes the NVIC peripheral according to the specified
*                  parameters in the NVIC_InitStruct.
* Input          : - NVIC_InitStruct: pointer to a NVIC_InitTypeDef structure
*                    that contains the configuration information for the
*                    specified NVIC peripheral.
* Output         : None
* Return         : None
*******************************************************************************/
VOID NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)
{
//    UINT32 tmppriority = 0x00, tmpreg = 0x00, tmpmask = 0x00;
//    UINT32 tmppre = 0, tmpsub = 0x0F;
    UINT8   PrioReg[8] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
    UINT8   PrioBitCnt = 0; // 用来表示优先级的bit个数
    UINT8   i = 0;
    UINT8   PrePrioBitCnt; // 强占优先级占的bit数
    UINT8   *pIPRn = ((UINT8 *)NST_NVIC->IPR) + NVIC_InitStruct->NVIC_IRQChannel;
    UINT8   IPRn;
    
// priority group [10:8]   
    PrePrioBitCnt = (0x700 - (NST_SCB->AIRCR & (UINT32)0x700)) >> 0x08;
    
    if (NVIC_InitStruct->NVIC_IRQChannelCmd != IRQ_DISABLE)
    {
#if 0
        /* Compute the Corresponding IRQ Priority --------------------------------*/    
        tmppriority = (0x700 - (SCB->AIRCR & (UINT32)0x700))>> 0x08;
        tmppre = (0x4 - tmppriority);
        tmpsub = tmpsub >> tmppriority;

        tmppriority = (UINT32)NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << tmppre;
        tmppriority |=  NVIC_InitStruct->NVIC_IRQChannelSubPriority & tmpsub;

        tmppriority = tmppriority << 0x04;
        tmppriority = ((UINT32)tmppriority) << ((NVIC_InitStruct->NVIC_IRQChannel & (UINT8)0x03) * 0x08);

        tmpreg = NST_NVIC ->IPR[(NVIC_InitStruct->NVIC_IRQChannel >> 0x02)];
        tmpmask = (UINT32)0xFF << ((NVIC_InitStruct->NVIC_IRQChannel & (UINT8)0x03) * 0x08);
        tmpreg &= ~tmpmask;
        tmppriority &= tmpmask;  
        tmpreg |= tmppriority;

        NST_NVIC ->IPR[(NVIC_InitStruct->NVIC_IRQChannel >> 0x02)] = tmpreg;
#endif

        *pIPRn = 0xFF;
        IPRn = (*pIPRn) & 0xFF;
        
        if(PrePrioBitCnt == PrioBitCnt) // all bits are preemption priority
            NVIC_InitStruct->NVIC_IRQChannelSubPriority = 0;
        
        for(i = 0; i < 8; i++)
        {
            if(IPRn == PrioReg[i])
            {
                PrioBitCnt = i + 1;
                *pIPRn = (NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << (8 - PrePrioBitCnt)) 
                           | (NVIC_InitStruct->NVIC_IRQChannelSubPriority << (8 - PrioBitCnt));
                break;
            }
        }

        /* Enable the Selected IRQ Channels --------------------------------------*/
        NST_NVIC ->ISER[(NVIC_InitStruct->NVIC_IRQChannel >> 0x05)] =
            (UINT32)0x01 << (NVIC_InitStruct->NVIC_IRQChannel & (UINT8)0x1F);
    }
    else
    {
        /* Disable the Selected IRQ Channels -------------------------------------*/
        NST_NVIC ->ICER[(NVIC_InitStruct->NVIC_IRQChannel >> 0x05)] =
            (UINT32)0x01 << (NVIC_InitStruct->NVIC_IRQChannel & (UINT8)0x1F);
    }
}


/*******************************************************************************
* Function Name  : NVIC_DeInit
* Description    : Deinitializes the NVIC peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
VOID NVIC_DeInit(VOID)
{
    UINT32 index = 0;

    NST_NVIC ->ICER[0] = 0xFFFFFFFF;
    NST_NVIC ->ICER[1] = 0x0FFFFFFF;
    NST_NVIC ->ICPR[0] = 0xFFFFFFFF;
    NST_NVIC ->ICPR[1] = 0x0FFFFFFF;

    for(index = 0; index < 0x0F; index++)
    {
        NST_NVIC ->IPR[index] = 0x00000000;
    } 
}

/*******************************************************************************
* Function Name  : NVIC_GetIRQChannelPendingBitStatus
* Description    : Checks whether the specified IRQ Channel pending bit is set
*                  or not.
* Input          : - NVIC_IRQChannel: specifies the interrupt pending bit to check.
* Output         : None
* Return         : The new state of IRQ Channel pending bit(SET or RESET).
*******************************************************************************/
ITStatus NVIC_GetIRQChannelPendingBitStatus(UINT8 NVIC_IRQChannel)
{
    ITStatus pendingirqstatus = RESET;
    UINT32 tmp = 0x00;

    tmp = ((UINT32)0x01 << (NVIC_IRQChannel & (UINT32)0x1F));

    if (((NST_NVIC ->ISPR[(NVIC_IRQChannel >> 0x05)]) & tmp) == tmp)
    {
        pendingirqstatus = SET;
    }
    else
    {
        pendingirqstatus = RESET;
    }
    return pendingirqstatus;
}

/*******************************************************************************
* Function Name  : NVIC_SetIRQChannelPendingBit
* Description    : Sets the NVIC抯 interrupt pending bit.
* Input          : - NVIC_IRQChannel: specifies the interrupt pending bit to Set.
* Output         : None
* Return         : None
*******************************************************************************/
VOID NVIC_SetIRQChannelPendingBit(UINT8 NVIC_IRQChannel)
{

    *(VUINT32*) 0xE000EF00 = (UINT32)NVIC_IRQChannel;
}

/*******************************************************************************
* Function Name  : NVIC_ClearIRQChannelPendingBit
* Description    : Clears the NVIC抯 interrupt pending bit.
* Input          : - NVIC_IRQChannel: specifies the interrupt pending bit to clear.
* Output         : None
* Return         : None
*******************************************************************************/
VOID NVIC_ClearIRQChannelPendingBit(UINT8 NVIC_IRQChannel)
{
    NST_NVIC ->ICPR[(NVIC_IRQChannel >> 0x05)] = (UINT32)0x01 << (NVIC_IRQChannel & (UINT32)0x1F);
}


/*******************************************************************************
* Function Name  : NVIC_EnableIRQ
* Description    : enable IRQ.
* Input          : - NVIC_IRQChannel: specifies the interrupt pending bit to clear.
* Output         : None
* Return         : None
*******************************************************************************/
VOID NVIC_EnableIRQ(UINT8 NVIC_IRQChannel)
{
    /* Enable the Selected IRQ Channels --------------------------------------*/
    NST_NVIC ->ISER[(NVIC_IRQChannel >> 0x05)] =
            (UINT32)0x01 << (NVIC_IRQChannel & (UINT8)0x1F);
}

/*******************************************************************************
* Function Name  : NVIC_DisableIRQ
* Description    : disable IRQ.
* Input          : - NVIC_IRQChannel: specifies the interrupt pending bit to clear.
* Output         : None
* Return         : None
*******************************************************************************/
VOID NVIC_DisableIRQ(UINT8 NVIC_IRQChannel)
{
    /* Disable the Selected IRQ Channels -------------------------------------*/
    NST_NVIC ->ICER[(NVIC_IRQChannel >> 0x05)] =
        (UINT32)0x01 << (NVIC_IRQChannel & (UINT8)0x1F);
}

/*******************************************************************************
* Function Name  : NVIC_SetVectorTable
* Description    : 
* Input          : - 
* Output         : None
* Return         : None
*******************************************************************************/
 
void NVIC_SetVectorTable(UINT32 NVIC_VectTab, UINT32 Offset)  
{ 
// *((volatile UINT32*)0xE000ED08) = NVIC_VectTab|(Offset & (UINT32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
   NST_SCB->VTOR = NVIC_VectTab|(Offset & (UINT32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
}

/*******************************************************************************
* Function Name  : NVIC_CpuReset
* Description    : 
* Input          : - 
* Output         : None
* Return         : None
*******************************************************************************/
VOID NVIC_CpuReset(VOID)
{
    // 写AIRCR需要有key，只有高16bit为0x05FA时才能写进去
    NST_SCB->AIRCR = (NST_SCB->AIRCR & 0x0000FFFF) | AIRCR_VECT_KEY | VECRESET; 
}

VOID NST_ClearCycleCnt(VOID)
{
    *DWT_CYCLE_CNT = 0;
}

UINT32 NST_GetCycleCnt(VOID)
{
    return *DWT_CYCLE_CNT;
}

/*
******************************************************************************
**                            End Of File                                    **
******************************************************************************
*/

