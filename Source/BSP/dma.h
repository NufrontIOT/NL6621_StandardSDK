/*
******************************************************************************
**
** Project     : 
** Description :    dma.h
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/

#ifndef _DMA_H
#define _DMA_H

typedef OS_EVENT    NST_LOCK;

#ifdef DMA_MOVE_MEM
extern OS_EVENT *pDmaSem;
extern PUINT8 pDmaSrc;
extern PUINT8 pDmaDst;
extern UINT16 DmaSize;
#endif // DMA_MOVE_MEM //

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

//*******************************************************************************************************
VOID BSP_DmaInit (UINT8 channel_num);
#ifdef DMA_MOVE_MEM
VOID BSP_DmaMoveMemInit (UINT8 channel_num);
#endif // DMA_MOVE_MEM //
VOID BSP_DmaStart(UINT32* pSrc, UINT32* pDest, UINT8 channel_num);
VOID BSP_DmaHalt(UINT8 channel_num);
VOID BSP_DmaDeinit(VOID);
VOID BSP_DmaIntISR(VOID);

#endif //_DMA_H
