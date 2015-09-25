/*
******************************************************************************
**
** Project     : 
** Description :    SPI driver
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/
#include "includes.h"
#include "qspi.h"

#ifdef NULINK2_SOC

/*
******************************************************************************
**            VOID BSP_QSpiInit(VOID)
**
** Description  : QSPI init
** Arguments    : 
                        
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/

VOID BSP_QSpiInit(UINT16 SpiClkDiv)
{
// SPIpins can't used as GPIO
    NST_WR_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL) & (~ENA_QSPIM_GPIO));

    NST_QSPI->SSIENR= 0x00;
    NST_QSPI->IMR= 0x00;
    //NST_QSPI->DMACR= 0x00;
    //NST_QSPI->DMATDLR= 0x00;
    NST_QSPI->BAUDR= SpiClkDiv; 
    NST_QSPI->CTRLR0=  (QSPI_TMOD_TR << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
    NST_QSPI->CTRLR1= 0x00;
    //NST_QSPI->SER= 0x01;
    NST_QSPI->SSIENR= 0x01;
}
/*
******************************************************************************
**            VOID BSP_QSpiWait(VOID)
**
** Description  : 用于标志一个读写过程完成
** Arguments    : 
                        
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_QSpiWait(VOID)
{
    UINT8 retry=0;
    
    while(!(NST_QSPI->SR & QSPI_SR_TF_EMPT))//等待发送区空  
    {
        retry++;    
    }
    
    retry=0;
    while(NST_QSPI->SR & QSPI_SR_BUSY) //等待接收完一个byte  
    {
        retry++;
    }

}

/*
******************************************************************************
**            VOID  BSP_QSpiWriteByte(UINT8 Byte)
**
** Description  : qspi write one byte
** Arguments    : 
                        
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID  BSP_QSpiWriteByte(UINT8 Byte)   
{
    
    while(!(NST_QSPI->SR & QSPI_SR_TF_NOT_FULL));
    
    NST_QSPI->DR = Byte;    
}

/*
******************************************************************************
**            BSP_QSpiRead(UINT32 RdCnt, UINT8* pBuf) 
**
** Description  : qspi continous read
** Arguments    : 
                        RdCnt
                        pBuf
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_QSpiRead(UINT32 RdCnt, UINT8* pBuf)   
{
    UINT8 RxByte;
    
    while(RdCnt > 0)
    {
        if(NST_QSPI->SR & QSPI_SR_RF_NOT_EMPT)
        {
            *pBuf++ =  NST_QSPI->DR;
            RdCnt--;
        }
    }

// throw the redundant bytes
    if(NST_QSPI->SR & QSPI_SR_RF_NOT_EMPT)
    {
        RxByte =  NST_QSPI->DR;
        RxByte =RxByte; // remove warning
    }
}
/*
******************************************************************************
**            VOID BSP_QSpiDirectReadInit(VOID)
**
** Description  : QSPI direct read init
** Arguments    : 
                        
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_QSpiDirectReadInit(UINT16 SpiClkDiv)
{
// SPIpins can't used as GPIO
    NST_WR_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL) & (~ENA_QSPIM_GPIO));

    NST_QSPI->SSIENR= 0x00;
    NST_QSPI->IMR= 0x00;
    //NST_QSPI->DMACR= 0x00;
    //NST_QSPI->DMATDLR= 0x00;
    NST_QSPI->BAUDR= SpiClkDiv; 
    NST_QSPI->CTRLR0=  (QSPI_TMOD_TR << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
    NST_QSPI->READ_CMD = FLASH_CMD_SEQ_READ|(QSPI_STD_FAST_RD_MODE << QSPI_RD_MODE_OFFSET);
    NST_QSPI->CTRLR1= 0x00;
    //NST_QSPI->SER= 0x01;
    NST_QSPI->SSIENR= 0x01;
}

#if 0
/*
******************************************************************************
**            VOID BSP_QSpiDirectWriteInit(VOID)
**
** Description  : QSPI direct write init
** Arguments    : 
                        
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/

VOID BSP_QSpiDirectWriteInit(UINT16 SpiClkDiv)
{
// SPIpins can't used as GPIO
    NST_WR_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL) & (~ENA_SPIM_GPIO));

    NST_QSPI->SSIENR= 0x00;
    NST_QSPI->IMR= 0x00;
    //NST_QSPI->DMACR= 0x00;
    //NST_QSPI->DMATDLR= 0x00;
    NST_QSPI->BAUDR= SpiClkDiv; 
    NST_QSPI->CTRLR0=  (QSPI_TMOD_TR << QSPI_TMOD_OFFSET) | (QSPI_FRF_QSPI << QSPI_FRF_OFFSET) |(QSPI_FRM_SIZE - 1) |( QSPI_MODE_3 << 6);
    NST_QSPI->PGM_CMD = FLASH_CMD_PAGE_WRITE;
    NST_QSPI->CTRLR1= 0x00;
    //NST_QSPI->SER= 0x01;
    NST_QSPI->SSIENR= 0x01;
}

#endif

#endif // NULINK2_SOC

