/*
******************************************************************************
**
** Project     : 
** Description :    I2C driver
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/
#include "includes.h"
#include "i2c.h"

/* System Clock Frequency */
#define CONFIG_XILINX_CLOCK_FREQ    100000000

/*
******************************************************************************
**            VOID BSP_I2CInit(UINT8 TargetAdr)
**
** Description  : I2C init
** Arguments    : 
                        target(device) address
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
/* 
    // ClkFreq is the clock speed (in MHz) that is being supplied to the
    // DW_apb_i2c device.  The correct clock count values are determined
    // by using this inconjunction with the minimum high and low signal
    // hold times as per the I2C bus specification.
#define SS_MIN_SCL_HIGH         4000
#define SS_MIN_SCL_LOW          4700
#define FS_MIN_SCL_HIGH         600
#define FS_MIN_SCL_LOW          1300
#define HS_MIN_SCL_HIGH_100PF   60
#define HS_MIN_SCL_LOW_100PF    120
    SS_SclHigh = ((UINT16) (((SS_MIN_SCL_HIGH * ClkFreq) / 100) + 1));
    SS_SclLow = ((UINT16) (((SS_MIN_SCL_LOW * ClkFreq) / 100) + 1));
    FS_SclHigh = ((UINT16) (((FS_MIN_SCL_HIGH * ClkFreq) / 100) + 1));
    FS_SclLow = ((UINT16) (((FS_MIN_SCL_LOW * ClkFreq) / 100) + 1));
    HS_SclHigh = ((UINT16) (((HS_MIN_SCL_HIGH_100PF * ClkFreq) / 100)+ 1));
    HS_SclLow = ((UINT16) (((HS_MIN_SCL_LOW_100PF * ClkFreq) / 100) + 1));
*/

VOID BSP_I2CInit(UINT8 TargetAdr)
{
// I2C pins can't used as GPIO
#ifdef NULINK2_SOC 
    NST_WR_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL) & (~ENA_I2C_GPIO));
#else
    NST_WR_PWM_REG(ADDR_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_PIN_MUX_CTRL) & (~ENA_I2C_GPIO));
#endif

    NST_WR_I2C_REG(0x0, IC_ENABLE);
    NST_WR_I2C_REG(FAST_MODE | SLV_DIS | RESTART | MASTER_EN, IC_CON); // TODO standard mode
    //NST_WR_I2C_REG(M_STOP_DET, IC_INTR_MASK);

    // SM BUS < 100Kbps?, read 
    NST_WR_I2C_REG(66, IC_FS_SCL_LCNT);
    NST_WR_I2C_REG(31, IC_FS_SCL_HCNT);
  //  NST_WR_I2C_REG(STANDARD_MODE | SLV_DIS | RESTART | MASTER_EN, IC_CON);
  //  NST_WR_I2C_REG(189, IC_SS_SCL_LCNT);
  //  NST_WR_I2C_REG(161, IC_SS_SCL_HCNT);
    // I2C 初始化后配一次IC_TAR即可
    NST_WR_I2C_REG(TargetAdr, IC_TAR);
    NST_WR_I2C_REG(0x82, IC_SDA_SETUP);
    NST_WR_I2C_REG(0x01, IC_ENABLE);
    NST_RD_I2C_REG(IC_CLR_INTR);
}

/*
******************************************************************************
**             VOID I2C_WaitForBus(VOID)
**
** Description  : wait for bus busy
** Arguments    : 
                 
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID I2C_WaitForBus(VOID)
{
    while(NST_RD_I2C_REG(IC_STATUS) & STAT_MST_ACTIVITY 
        ||!(NST_RD_I2C_REG(IC_STATUS) & STAT_TFE)); 
}


/*
******************************************************************************
**             VOID I2C_XferFinish(VOID)
**
** Description  : wait for transfer finish
** Arguments    : 
                  
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID I2C_XferFinish(VOID)
{
    UINT8 DataByte;
    
    while(1)
    {
        if(((NST_RD_I2C_REG(IC_RAW_INTR_STAT) & STOP_DET) == STOP_DET))
        {
            NST_RD_I2C_REG(IC_CLR_STOP_DET);
            break;
        }
    }
    
    I2C_WaitForBus();
    
    DataByte = DataByte;
    while(NST_RD_I2C_REG(IC_STATUS) & STAT_RFNE) // rx fifo not empty
    {
        DataByte = NST_RD_I2C_REG(IC_DATA_CMD);
    }

}


/*
******************************************************************************
**             BOOL_T BSP_I2CRead(UINT8 *pBuf, UINT32 Len)
**
** Description  : I2C read
** Arguments    : 
                  pBuf: read buffer
                  Len: read len
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
BOOL_T BSP_I2CRead(UINT8* pBuf, UINT32 Len)
{
    UINT32 i = 0;
    BOOL_T Ret = NST_TRUE;

    while(i < Len)
    {
        NST_WR_I2C_REG(IC_RD, IC_DATA_CMD); // send read command
        if((NST_RD_I2C_REG(IC_STATUS) & STAT_RFNE) == STAT_RFNE) // rx fifo not empty, then read it
        {
            pBuf[i++] = NST_RD_I2C_REG(IC_DATA_CMD);
        }
        
        if(NST_RD_I2C_REG(IC_RAW_INTR_STAT) & TX_ABRT)
        {
            Ret = NST_FALSE;
            break;
        }
    }
    
    NST_WR_I2C_REG(1 << 9, IC_DATA_CMD); // stop
    
    I2C_XferFinish();    

    return Ret;
}
/*
******************************************************************************
**             INT32 BSP_I2CWrite(UINT8 *pBuf, UINT32 Len)
**
** Description  : I2C write
** Arguments    : 
                  pBuf: write buffer
                  Len: write len
** Returns      : 
                   write len 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
UINT32 BSP_I2CWrite(UINT8* pBuf, UINT32 Len)
{
    UINT32 i = 0;

    ASSERT(Len <= I2C_PAGESIZE);

    while(i < Len)
    {
        if(NST_RD_I2C_REG(IC_STATUS) & STAT_TFNF)  // TX fifo not full, then write it
        {
            NST_WR_I2C_REG(pBuf[i], IC_DATA_CMD);
            i++;
        }
    }

  //  I2C_XferFinish();  
    
    /* Wait for read/write operation to complete on actual memory */
    NSTusecDelay(20000); // 10ms

    return i;
}

/*
******************************************************************************
**      VOID BSP_I2CSetAdr(UINT32 Adr)
**
** Description  : I2C set address
** Arguments    : 
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_I2CSetAdr(UINT32 Adr)
{
#ifndef AT24C08
    /*发送地址*/
    // High byte address
    NST_WR_I2C_REG((Adr >> 8) & 0xFF, IC_DATA_CMD);
#endif
    // Low byte address
    NST_WR_I2C_REG((Adr & 0xFF), IC_DATA_CMD);
}

/*
******************************************************************************
**             BOOL_T BSP_I2CSeqRead(UINT32 Adr, UINT8* pByte)
**
** Description  : I2C sequential read
** Arguments    : 
                  Adr: 
                  pByte: read buffer
                  Len
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
BOOL_T BSP_I2CSeqRead(UINT32 Adr, UINT8 *pBuf, UINT32 Len)
{
    // I2C IP send start/device adr  automatically
    
    BSP_I2CSetAdr(Adr);

    return BSP_I2CRead(pBuf, Len);
}

/*
******************************************************************************
**             VOID BSP_I2CPageWrite(UINT32 Adr, UINT8 *pBuf)
**
** Description  : I2C page write
** Arguments    : 
                  Adr: 
                  pBuf: write buffer
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_I2CPageWrite(UINT32 Adr, UINT8 *pBuf)
{
    ASSERT((Adr & (I2C_PAGESIZE - 1)) == 0); // on page edge
    
    ASSERT(Adr <= E2PROM_SIZE - I2C_PAGESIZE);

    BSP_I2CSetAdr(Adr);
    
    BSP_I2CWrite(pBuf, I2C_PAGESIZE);
}

/*
******************************************************************************
**             VOID BSP_I2CByteWrite(UINT32 Adr, UINT8 *pByte)
**
** Description  : I2C byte write
** Arguments    : 
                  Adr: 
                  pByte: write buffer
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_I2CByteWrite(UINT32 Adr, UINT8* pByte)
{
    BSP_I2CSetAdr(Adr);

    BSP_I2CWrite(pByte, 1);
}

/*
******************************************************************************
**             VOID BSP_I2CPageRead(UINT32 Adr, UINT8 *pBuf)
**
** Description  : I2C page read
** Arguments    : 
                  Adr: 
                  pBuf: read buffer
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_I2CPageRead(UINT32 Adr, UINT8 *pBuf)
{
    ASSERT((Adr & (I2C_PAGESIZE - 1)) == 0); // on page edge

    // I2C IP send start/device adr  automatically

    BSP_I2CSetAdr(Adr);

    BSP_I2CRead(pBuf, I2C_PAGESIZE);
}

/*
******************************************************************************
**             VOID BSP_I2CByteRead(UINT32 Adr, UINT8* pByte)
**
** Description  : I2C byte read
** Arguments    : 
                  Adr: 
                  pByte: read buffer
** Returns      : 
                    
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_I2CByteRead(UINT32 Adr, UINT8* pByte)
{
    // I2C IP send start/device adr  automatically

    BSP_I2CSetAdr(Adr);

    BSP_I2CRead(pByte, 1);
}

/*
******************************************************************************
**                            End Of File                                    **
******************************************************************************
*/



