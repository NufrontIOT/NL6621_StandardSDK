#include "includes.h"
#include "spi.h" 
#include "flash.h"
#include "qspi.h" 
  
/*
AT45DB041D 芯片
page size: 256/264 Bytes 可配
block size: 8 * page size (2048/2112 Bytes)
sector size: sector1-7 size = 32 * block size(65536/67584 Bytes); sector0a size = 1*block(2048/2112 Bytes); sector0b size = 7*block(63488/65472 Bytes);
flash size: 8 * sector size(512KBytes/528KBytes)
*/

//#ifdef BURN_FLASH
/*
******************************************************************************
**            UINT8 SpiFlashReadSR(VOID)
**
** Description  : read flash status register
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
UINT8 SpiFlashReadSR(VOID)   
{  
    UINT8 byte = 0;   

    // send cmd
    NST_SPI->SSIENR= 0x00;
    NST_SPI->CTRLR0=  (SPI_TMOD_TR << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
    NST_SPI->SSIENR= 0x01;
    NST_SPI->DR = FLASH_CMD_RD_SR;

    // send dummy byte enable read sck
    NST_SPI->DR = DUMMY_BYTE;

    BSP_SpiWait();

    byte = NST_SPI->DR;         //丢弃发操作码所读的数据
    
    byte = NST_SPI->DR;  // SR
    
    return byte;
}
/*
******************************************************************************
**        VOID SpiFlashWaitBusy(VOID)   
**
** Description  : wait flash operation done
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID SpiFlashWaitBusy(VOID)   
{   
    UINT8 SR = 0;
    
    while (1)
    {
        SR = SpiFlashReadSR();
#if(defined AT45DB041D)
        if (SR & FLASH_SR_READY)
            break;
#else
        if (!(SR & FLASH_SR_BUSY))
            break;
#endif
    }
}  

/*
******************************************************************************
**       VOID  SpiFlashRead(UINT32 ReadStartPos, UINT8* pBuf, UINT32 Len)   
**
** Description  : continous read flash into pBuf
** Arguments    : 
** Returns      :  
                        
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID  SpiFlashRead(UINT32 ReadStartPos, UINT8* pBuf, UINT32 Len)  
{
    UINT32 ReadAddr = ReadStartPos;
    UINT8* pRxData = pBuf;
    UINT32 DnldByteCnt = 0;
    UINT32 FlashAdr;
    
    //SpiFlashWaitBusy();
    
    while(Len > DnldByteCnt)
    {
        UINT32 RdCnt = (Len - DnldByteCnt) > MAX_FLASH_RD_SIZE ? MAX_FLASH_RD_SIZE : (Len - DnldByteCnt);
#if(defined AT45DB041D)
// AT45DB041D default page size 264B, address is page addrss+byte offset
        FlashAdr = (((ReadAddr/FLASH_PAGE_SIZE) << 9) | (ReadAddr % FLASH_PAGE_SIZE));
#else
        FlashAdr = ReadAddr;
#endif
        //SpiFlashWaitBusy();
        
        NST_SPI->SSIENR= 0x00;
        NST_SPI->CTRLR0=  (SPI_TMOD_EPROMREAD << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
        NST_SPI->CTRLR1= RdCnt - 1; //连续读写数据个数
        NST_SPI->SSIENR= 0x01;
        
        // send cmd
        BSP_SpiWriteByte(FLASH_CMD_SEQ_READ);
        BSP_SpiWriteByte((FlashAdr>>16) & 0xFF);
        BSP_SpiWriteByte((FlashAdr>>8) & 0xFF);
        BSP_SpiWriteByte(FlashAdr & 0xFF);

        BSP_SpiRead(RdCnt, pRxData);
        BSP_SpiWait();
        DnldByteCnt += RdCnt;
        ReadAddr += RdCnt;
        pRxData += RdCnt;

   //     DBGPRINT(DEBUG_INFO, "READ FLASH pBuf=0x%x\n", pBuf);
        //    NSTusecDelay(1000);
    }

}

/*
******************************************************************************
**       VOID SPI_FlashWriteEnable(VOID)   
**
** Description  : enable flash write
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
#if(!defined AT45DB041D) 
// AT45DB041D needn't enable write before change flash's content
VOID SPI_FlashWriteEnable(VOID) 
{
    NST_SPI->SSIENR= 0x00;
    NST_SPI->CTRLR0=  (SPI_TMOD_TO << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
    NST_SPI->SSIENR= 0x01;
    
    NST_SPI->DR = FLASH_CMD_WREN;

    BSP_SpiWait(); 
}
#endif

/*
******************************************************************************
**        VOID SpiFlashEraseChip(VOID)   
**
** Description  : erase whole chip
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID SpiFlashEraseChip(VOID)   
{  
#if(!defined AT45DB041D) 
// AT45DB041D needn't enable write before change flash's content
    SPI_FlashWriteEnable();
#endif

    NST_SPI->SSIENR= 0x00;
    NST_SPI->CTRLR0=  (SPI_TMOD_TO << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
    NST_SPI->SSIENR= 0x01;
    
#if(defined AT45DB041D)
    NST_SPI->DR = FLASH_ERASE_CMD0;
    NST_SPI->DR = FLASH_ERASE_CMD1;
    NST_SPI->DR = FLASH_ERASE_CMD2;
    NST_SPI->DR = FLASH_ERASE_CMD3;
#else
    NST_SPI->DR = FLASH_CMD_CHIP_ERASE;
#endif

    BSP_SpiWait();  // 等待SPI操作结束，确保前一个cmd已经结束(CS掉下来)

    SpiFlashWaitBusy();                       //等待芯片擦除结束
}   


/*
******************************************************************************
**       VOID SpiFlashWriteOnePage(UINT32 Adr, UINT8* pBuf)  
**
** Description  : program one page
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID SpiFlashWriteOnePage(UINT32 Adr, UINT8* pBuf) 
{
    UINT32 i;
    UINT32 FlashAdr;
    OS_CPU_SR  cpu_sr;

#if(defined AT45DB041D)
    FlashAdr = (((Adr/FLASH_PAGE_SIZE) << 9) | (Adr % FLASH_PAGE_SIZE));
#else
    FlashAdr = Adr;
#endif

#ifdef  BURN_FIXED_CONTENT
    NST_ZERO_MEM(pBuf, FLASH_PAGE_SIZE);
#endif

#if(!defined AT45DB041D) 
// AT45DB041D needn't enable write before change flash's content
    SPI_FlashWriteEnable();
#endif

    NST_SPI->SSIENR= 0x00;
    NST_SPI->CTRLR0=  (SPI_TMOD_TO << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
    NST_SPI->SSIENR= 0x01;

    OS_ENTER_CRITICAL();

    // send cmd
    BSP_SpiWriteByte(FLASH_CMD_PAGE_WRITE);
    BSP_SpiWriteByte((FlashAdr>>16) & 0xFF);
    BSP_SpiWriteByte((FlashAdr>>8) & 0xFF);
    BSP_SpiWriteByte(FlashAdr & 0xFF);

    // send data
    for (i = 0; i <FLASH_PAGE_SIZE; i++)
    {
        while(!(NST_SPI->SR & SR_TF_NOT_FULL));
        
        NST_SPI->DR = pBuf[i];
    }

    OS_EXIT_CRITICAL();

    BSP_SpiWait(); 

    SpiFlashWaitBusy(); 
        
}

#ifndef AT45DB041D
VOID SpiFlashEraseSector(UINT32 SectorAddr) 
{
    UINT32 FlashAdr = SectorAddr;

    SPI_FlashWriteEnable();

    NST_SPI->SSIENR= 0x00;
    NST_SPI->CTRLR0=  (SPI_TMOD_TO << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
    NST_SPI->SSIENR= 0x01;

    // send cmd
    BSP_SpiWriteByte(FLASH_CMD_SECTOR_ERASE);
    BSP_SpiWriteByte((FlashAdr>>16) & 0xFF);
    BSP_SpiWriteByte((FlashAdr>>8) & 0xFF);
    BSP_SpiWriteByte(FlashAdr & 0xFF);

    BSP_SpiWait(); 

    SpiFlashWaitBusy(); 
}
#endif
//#endif

#ifdef NULINK2_SOC
/*
******************************************************************************
**       VOID  QSpiFlashRead(UINT32 ReadStartPos, UINT8* pBuf, UINT32 Len)   
**
** Description  : continous read flash into pBuf using register addressing mode
** Arguments    : 
** Returns      :  
                        
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID  QSpiFlashRead(UINT32 ReadStartPos, UINT8* pBuf, UINT32 Len)  
{
    UINT32 ReadAddr = ReadStartPos;
    UINT8* pRxData = pBuf;
    UINT32 DnldByteCnt = 0;
    UINT32 FlashAdr;
    OS_CPU_SR  cpu_sr;

    //SpiFlashWaitBusy();
    
    while(Len > DnldByteCnt)
    {
        UINT32 RdCnt = (Len - DnldByteCnt) > MAX_FLASH_RD_SIZE ? MAX_FLASH_RD_SIZE : (Len - DnldByteCnt);
#if(defined AT45DB041D)
// AT45DB041D default page size 264B, address is page addrss+byte offset
        FlashAdr = (((ReadAddr/FLASH_PAGE_SIZE) << 9) | (ReadAddr % FLASH_PAGE_SIZE));
#else
        FlashAdr = ReadAddr;
#endif
        //SpiFlashWaitBusy();
        
        NST_QSPI->SSIENR= 0x00;
        NST_QSPI->CTRLR0=  (QSPI_TMOD_EPROMREAD << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
        NST_QSPI->CTRLR1= RdCnt - 1; //连续读写数据个数
        NST_QSPI->SSIENR= 0x01;

        OS_ENTER_CRITICAL();
        
        // send cmd
        BSP_QSpiWriteByte(FLASH_CMD_SEQ_READ);
        BSP_QSpiWriteByte((FlashAdr>>16) & 0xFF);
        BSP_QSpiWriteByte((FlashAdr>>8) & 0xFF);
        BSP_QSpiWriteByte(FlashAdr & 0xFF);

        BSP_QSpiRead(RdCnt, pRxData);

        OS_EXIT_CRITICAL();
        
        BSP_QSpiWait();
        DnldByteCnt += RdCnt;
        ReadAddr += RdCnt;
        pRxData += RdCnt;

   //     DBGPRINT(DEBUG_INFO, "READ FLASH pBuf=0x%x\n", pBuf);
        //	NSTusecDelay(1000);
    }

}

/*
******************************************************************************
**       VOID  QSpiFlashDirectRead(UINT32 ReadStartPos, UINT8* pBuf, UINT32 Len)   
**
** Description  : continous read flash into pBuf using direct addressing mode
** Arguments    : 
** Returns      :  
                        
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID  QSpiFlashDirectRead(UINT32 ReadStartPos, UINT8* pBuf, UINT32 Len)  
{
    UINT32 DnldByteCnt = 0;
    UINT8*   FlashMemAdr = (UINT8*)(_QSPI_MEM_BASE_ADR + ReadStartPos);
    
   // BSP_QSpiDirectReadInit(40);
    
    while(Len > DnldByteCnt)
    {
        *pBuf++ = *FlashMemAdr++;
        DnldByteCnt++;
    }
}

/*
******************************************************************************
**            UINT8 QSpiFlashReadSR(VOID)
**
** Description  : read flash status register
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
UINT8 QSpiFlashReadSR(VOID)   
{  
    UINT8 byte = 0;   

    // send cmd
    NST_QSPI->SSIENR= 0x00;
    NST_QSPI->CTRLR0=  (QSPI_TMOD_TR << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
    NST_QSPI->SSIENR= 0x01;
    NST_QSPI->DR = FLASH_CMD_RD_SR;

    // send dummy byte enable read sck
    NST_QSPI->DR = DUMMY_BYTE;

    BSP_QSpiWait();

    byte = NST_QSPI->DR; 		//丢弃发操作码所读的数据
    
    byte = NST_QSPI->DR;  // SR
    
    return byte;
}
/*
******************************************************************************
**        VOID QSpiFlashWaitBusy(VOID)   
**
** Description  : wait flash operation done
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID QSpiFlashWaitBusy(VOID)   
{   
    UINT8 SR = 0;
    
    while (1)
    {
        SR = QSpiFlashReadSR();
#if(defined AT45DB041D)
        if (SR & FLASH_SR_READY)
            break;
#else
        if (!(SR & FLASH_SR_BUSY))
            break;
#endif
    }
}  

/*
******************************************************************************
**       VOID QSPI_FlashWriteEnable(VOID)   
**
** Description  : enable flash write
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
#if(!defined AT45DB041D) 
// AT45DB041D needn't enable write before change flash's content
VOID QSPI_FlashWriteEnable(VOID) 
{
    NST_QSPI->SSIENR= 0x00;
    NST_QSPI->CTRLR0=  (QSPI_TMOD_TO << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
    NST_QSPI->SSIENR= 0x01;
    
    NST_QSPI->DR = FLASH_CMD_WREN;

    BSP_QSpiWait(); 
}
#endif

/*
******************************************************************************
**        VOID QSpiFlashEraseChip(VOID)   
**
** Description  : erase whole chip
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID QSpiFlashEraseChip(VOID)   
{  
#if(!defined AT45DB041D) 
// AT45DB041D needn't enable write before change flash's content
    QSPI_FlashWriteEnable();
#endif

    NST_QSPI->SSIENR= 0x00;
    NST_QSPI->CTRLR0=  (QSPI_TMOD_TO << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
    NST_QSPI->SSIENR= 0x01;
    
#if(defined AT45DB041D)
    NST_QSPI->DR = FLASH_ERASE_CMD0;
    NST_QSPI->DR = FLASH_ERASE_CMD1;
    NST_QSPI->DR = FLASH_ERASE_CMD2;
    NST_QSPI->DR = FLASH_ERASE_CMD3;
#else
    NST_QSPI->DR = FLASH_CMD_CHIP_ERASE;
#endif

	//printf("BSP_QSpiWait\n");
    BSP_QSpiWait();  // 等待SPI操作结束，确保前一个cmd已经结束(CS掉下来)

	//printf("Enter QSpiFlashWaitBusy \n");

    QSpiFlashWaitBusy();    				   //等待芯片擦除结束
  //  printf("QSpiFlashWaitBusy finish!\n");
}   


/*
******************************************************************************
**       VOID SpiFlashWriteOnePage(UINT32 Adr, UINT8* pBuf)  
**
** Description  : QSPI program one page using register addressing mode
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID QSpiFlashWriteOnePage(UINT32 Adr, UINT8* pBuf) 
{
    UINT32 i;
    UINT32 FlashAdr;

#if(defined AT45DB041D)
    FlashAdr = (((Adr/FLASH_PAGE_SIZE) << 9) | (Adr % FLASH_PAGE_SIZE));
#else
    FlashAdr = Adr;
#endif

#ifdef  BURN_FIXED_CONTENT
    NST_ZERO_MEM(pBuf, FLASH_PAGE_SIZE);
#endif

#if(!defined AT45DB041D) 
// AT45DB041D needn't enable write before change flash's content
    QSPI_FlashWriteEnable();
#endif

    NST_QSPI->SSIENR= 0x00;
    NST_QSPI->CTRLR0=  (QSPI_TMOD_TO << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
    NST_QSPI->SSIENR= 0x01;

    // send cmd
    BSP_QSpiWriteByte(FLASH_CMD_PAGE_WRITE);
    BSP_QSpiWriteByte((FlashAdr>>16) & 0xFF);
    BSP_QSpiWriteByte((FlashAdr>>8) & 0xFF);
    BSP_QSpiWriteByte(FlashAdr & 0xFF);

    // send data
    for (i = 0; i <FLASH_PAGE_SIZE; i++)
    {
        while(!(NST_QSPI->SR & QSPI_SR_TF_NOT_FULL));
        
        NST_QSPI->DR = pBuf[i];
    }

    BSP_QSpiWait(); 

    QSpiFlashWaitBusy(); 
}

#ifndef AT45DB041D
VOID QSpiFlashEraseSector(UINT32 SectorAddr) 
{
    UINT32 FlashAdr = SectorAddr;

    QSPI_FlashWriteEnable();

    NST_QSPI->SSIENR= 0x00;
    NST_QSPI->CTRLR0=  (QSPI_TMOD_TO << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
    NST_QSPI->SSIENR= 0x01;

    // send cmd
    BSP_QSpiWriteByte(FLASH_CMD_SECTOR_ERASE);
    BSP_QSpiWriteByte((FlashAdr>>16) & 0xFF);
    BSP_QSpiWriteByte((FlashAdr>>8) & 0xFF);
    BSP_QSpiWriteByte(FlashAdr & 0xFF);

    BSP_QSpiWait(); 

    QSpiFlashWaitBusy(); 
}
#endif

UINT8 PageBuf[FLASH_PAGE_SIZE] = {0};

INT32 QSpiWriteOneSector(UINT32 SectorAddr, UINT8* pBuf)
{
    UINT32 i, j;
    UINT32 WriteAddr = SectorAddr;
    UINT32 PageCnt;
    
    PageCnt = FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE;
    
    QSpiFlashEraseSector(WriteAddr);

    NSTusecDelay(100000);

    for (i = 0; i < PageCnt; i++)
    {
        j = 10;

        while (j--)
        {
            QSpiFlashWriteOnePage(WriteAddr, pBuf);
            NSTusecDelay(1000);
            QSpiFlashRead(WriteAddr, PageBuf, FLASH_PAGE_SIZE);
            if (memcmp(PageBuf, pBuf, FLASH_PAGE_SIZE) == 0)
                break;
        }

        if (j == 0)
            return -1;
        
        pBuf += FLASH_PAGE_SIZE;
        WriteAddr += FLASH_PAGE_SIZE;
    }

    return 0;
}

#endif // NULINK2_SOC

