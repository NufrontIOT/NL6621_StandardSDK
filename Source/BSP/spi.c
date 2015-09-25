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
#include "spi.h"

/*
******************************************************************************
**            VOID BSP_SpiInit(VOID)
**
** Description  : SPI init
** Arguments    : 
                        
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/

VOID BSP_SpiInit(VOID)
{
// SPIpins can't used as GPIO
#ifdef NULINK2_SOC 
    NST_WR_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL) & (~ENA_SPIM_GPIO));
    NST_WR_PWM_REG(ADDR_SPI_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_SPI_PIN_MUX_CTRL) & (~SPI_MUX_QSPI_PINS));
#else
    NST_WR_PWM_REG(ADDR_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_PIN_MUX_CTRL) & (~ENA_SPIM_GPIO));

    // b5置1使能串口，置0使能SPI            
    NST_WR_PWM_REG(ADDR_UART_SPI_MUX, NST_RD_PWM_REG(ADDR_UART_SPI_MUX) & (~ (1 << SEL_UART_SPI)));
#endif

    NST_SPI->SSIENR= 0x00;
    NST_SPI->IMR= 0x00;
    NST_SPI->DMACR= 0x00;
    NST_SPI->DMATDLR= 0x00;
    NST_SPI->BAUDR= SPI_CLK_DIV; 
    NST_SPI->CTRLR0=  (SPI_TMOD_TR << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
    NST_SPI->CTRLR1= 0x00;
    NST_SPI->SER= 0x01;
    NST_SPI->SSIENR= 0x01;
}
/*
******************************************************************************
**            VOID BSP_SpiWait(VOID)
**
** Description  : 用于标志一个读写过程完成
** Arguments    : 
                        
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_SpiWait(VOID)
{
    UINT8 retry=0;
    
    while(!(NST_SPI->SR & SR_TF_EMPT))//等待发送区空    
    {
        retry++;    
    }

    retry=0;
    while(NST_SPI->SR & SR_BUSY) //等待接收完一个byte  
    {
        retry++;
    }

}

/*
******************************************************************************
**            VOID  BSP_SpiWriteByte(UINT8 Byte)
**
** Description  : spi write one byte
** Arguments    : 
                        
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID  BSP_SpiWriteByte(UINT8 Byte)   
{
    
    while(!(NST_SPI->SR & SR_TF_NOT_FULL));
    
    NST_SPI->DR = Byte;    

}

/*
******************************************************************************
**            BSP_SpiRead(UINT32 RdCnt, UINT8* pBuf) 
**
** Description  : spi continous read
** Arguments    : 
                        RdCnt
                        pBuf
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_SpiRead(UINT32 RdCnt, UINT8* pBuf)   
{
    UINT8 RxByte;
    
    while(RdCnt > 0)
    {
        if(NST_SPI->SR & SR_RF_NOT_EMPT)
        {
            *pBuf++ =  NST_SPI->DR;
            RdCnt--;
        }
    }

// throw the redundant bytes
    if(NST_SPI->SR & SR_RF_NOT_EMPT)
    {
        RxByte =  NST_SPI->DR;
        RxByte =RxByte; // remove warning
    }
}

#ifdef SPI_SDIO_CMD_TEST
/*SPI Master Tx Cmd for EUMU SPI slave*/
UINT8 EUM_CMD[13][6] ={
/*CMD0    */     0x40,	0x00,	0x00,	0x00,	0x00,	0x95,
/*CMD5_1  */   0x45,	0x00,	0x00,	0x00,	0x00,	0x5B,
/*CMD5_2  */   0x45,	0x00,	0x30,	0x00,	0x00,	0x87,
/*CMD52_1 */  0x74,	0x88,	0x00,	0x26,	0x03,	0x71,
/*CMD52_2 */  0x74,	0x88,	0x00,	0x26,	0x03,	0x71,
/*CMD52_3 */  0x74,	0x88,	0x00,	0x08,	0x07,	0x19,
/*CMD52_4 */  0x74,	0x88,	0x00,	0x04,	0x07,	0xf1,
/*CMD52_5 */  0x74,	0x88,	0x00,	0x06,	0x07,	0xdd,
/*CMD52_6 */  0x74,	0x88,	0x00,	0x10,	0xff,	0x57,
/*CMD52_7 */  0x74,	0x88,	0x00,	0x08,	0x00,	0x67,
/*CMD52_8 */  0x74,	0x88,	0x00,	0x0e,	0x20,	0x77,
/*CMD52_9 */  0x74,	0x88,	0x00,	0x08,	0x07,	0x19,
/*CMD52_10*/ 0x74,	0x88,	0x00,	0x2c,	0x03,	0xed	
	};

UINT8 SPIEUM[13] = {
    8,
	12,
	12,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9
	};

UINT8 CMD53R[5] = {
	0x75,
	0x10,
	0x00,
	0x00,
	0x00
	};	

UINT8 CMD53W[5] = {
	0x75,
	0x90,
	0x00,
	0x00,
	0x00
	};

UINT8 TestSdioWriteBuf[4096] = {0};
UINT8 TestSdioReadBuf[4096] = {0};

VOID BSP_SpiEumCmdWrite(UINT8 index)
{
    UINT32 i;
    
    NST_WR_GPIO_REG(SWPORTA_DR, 0x00); 
    
    for (i=0; i<SPIEUM[index]; i++)
    {	    
        if (i<6) 
            BSP_SpiWriteByte(EUM_CMD[index][i]); // write cmd bytes
        else    
            BSP_SpiWriteByte(0xFF); // send clk for response token
    }
    
    BSP_SpiWait();
    
    NST_WR_GPIO_REG(SWPORTA_DR, 0xFF);
}

VOID  BSP_SpiEumSdioSlave(VOID)   
{
    UINT8 i;
    
    // use spi tx only mode
    NST_SPI->SSIENR = 0x00;
    NST_SPI->CTRLR0 = (SPI_TMOD_TO << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
    NST_SPI->SSIENR = 0x01;
    
    // sync clk domain
    NST_WR_GPIO_REG(SWPORTA_DR, 0xFF);
    BSP_SpiWriteByte(0xFF);
    BSP_SpiWait();

    // send command for sdio slave enumeration
    for(i=0; i< 13;i++)
    { 
        BSP_SpiEumCmdWrite(i);
    }
}

VOID BSP_SpiGenCmd53Crc(UINT16 bytecnt, UINT8 FlagRW)
{
    UINT32 i;
    UINT32 j;
    UINT8  crc7=0;
    UINT8  cmd;
    UINT8 * pCmd53 = FlagRW ? CMD53W : CMD53R;
    
    NST_WR_GPIO_REG(SWPORTA_DR, 0x00); 
            
    for (i=0;i<5;i++)
    {
        if (i==3) 
            cmd=((bytecnt>>8)&0x1) | (pCmd53[i]&0xFE);
        else if (i==4) 
            cmd = bytecnt & 0xFF;
        else 
            cmd=pCmd53[i];
        
        BSP_SpiWriteByte(cmd);
        
        for(j=0;j<8;j++)
        { 	//crc <= {crc[5:3], crc[2]^crc_in^crc[6], crc[1:0], crc_in^crc[6]};	
            crc7=( ((crc7<<1)&0x76)  | (((crc7<<1) ^ (crc7>>3) ^ (cmd>>4))&0x8) | (((cmd>>7) ^ (crc7>>6))&0x1));
            cmd = cmd<<1;					
        }	
    }
            
    crc7=(crc7<<1) | 0x1; 
            
    BSP_SpiWriteByte(crc7);	
}

VOID BSP_SpiReadSingleBlock(UINT32 RdCnt, UINT8* pBuf)   
{      
    // start clk
    while(!(NST_SPI->SR & SR_TF_NOT_FULL));    
    NST_SPI->DR = 0xFF;  

    // wait for start token(1 byte)    
    while (1)
    {
        if (NST_SPI->SR & SR_RF_NOT_EMPT)
        {
             if (NST_SPI->DR == 0xFE)
                 break;
        }
    }
    
    // read data
    while(RdCnt)
    {
        if(NST_SPI->SR & SR_RF_NOT_EMPT)
        {
            *pBuf++ =  NST_SPI->DR;
            RdCnt--;
        }
    }

    // ignore data crc16

    ///TODO: wait for slave to complete TRANS_DONE_INT, no need to send sclk
}

VOID BSP_SpiCmd53Read(UINT16 bytecnt,UINT8* pBuf)
{
    UINT32 i;
    UINT16 bytecnt_i;
    OS_CPU_SR  cpu_sr;
 
    while (bytecnt > 0)
    {
        if(bytecnt>511) 
            bytecnt_i=512;
        else 
            bytecnt_i = bytecnt;

        // use spi tx only mode to send cmd53
        NST_SPI->SSIENR = 0x00;
        NST_SPI->CTRLR0 = (SPI_TMOD_TO << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
        NST_SPI->SSIENR = 0x01;
        
        BSP_SpiGenCmd53Crc(bytecnt_i, 0);  //sens cmd53 and crc (6 bytes total)
        
        // wait cmd53 response  (1 byte for start token, 2 bytes for response)
        for (i = 0; i < 3; i++)
            BSP_SpiWriteByte(0xFF);

        BSP_SpiWait();

        ///TODO: wait for slave to complete RD_START_INT, need to send sclk

        NST_WR_GPIO_REG(SWPORTA_DR, 0xFF);
        // switch to spi read only mode to read data response
        NST_SPI->SSIENR = 0x00;
        NST_SPI->CTRLR0 = (SPI_TMOD_RO << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
        NST_SPI->CTRLR1= bytecnt_i+10; //  delay byte(1~7) + 1start byte + bytecnt + 2byte crc + 1end byte - 1
        NST_SPI->SSIENR = 0x01;

        OS_ENTER_CRITICAL();
        
        NST_WR_GPIO_REG(SWPORTA_DR, 0x00);
        
        BSP_SpiReadSingleBlock(bytecnt_i, pBuf) ; 

        BSP_SpiWait();
       
        NST_WR_GPIO_REG(SWPORTA_DR, 0xFF);
        
        OS_EXIT_CRITICAL();
	
        bytecnt -= bytecnt_i;
        pBuf += bytecnt_i;
    }
}

VOID BSP_SpiCmd53Write(UINT16 bytecnt,UINT8* pBuf)
{
    UINT32 i;// j;
    UINT16 bytecnt_i;
    UINT16 crc16;
   // UINT16 Data;

    // use spi tx only mode to send cmd53 and write data
    NST_SPI->SSIENR = 0x00;
    NST_SPI->CTRLR0 = (SPI_TMOD_TO << SPI_TMOD_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET) |(SPI_FRM_SIZE - 1) |( SPI_MODE_3 << 6);
    NST_SPI->SSIENR = 0x01;

    while (bytecnt > 0)
    {
        if(bytecnt>511) 
            bytecnt_i=512;
        else 
            bytecnt_i = bytecnt;
    
        BSP_SpiGenCmd53Crc(bytecnt_i, 1);  //send cmd53 and crc (6 bytes total)
        
        // wait cmd53 response  (1 byte for start token, 2 bytes for response)
        for (i = 0; i < 3; i++)
            BSP_SpiWriteByte(0xFF);

        ///TODO: wait for slave firmware to complete WR_START_INT, need to send sclk
    
        // send start token
        BSP_SpiWriteByte(0xFE);
    
        // send tx data
        for (i = 0; i < bytecnt_i; i++)
        {
            //Data = (UINT16)pBuf[i];
            BSP_SpiWriteByte(pBuf[i]);
    
            #if 0 // slave not care data crc, so set to zero
    		for(j=8;j>0;j--)
    		{ 					
    			// crc = {crc[14:12],crc[15]^crc[11]^bit[j-1],crc[10:5],crc[15]^crc[4]^bit[j-1],crc[3:0],crc[15]^bit[j-1]
                crc16 = ((crc16<<1)&0xE000) | 
                        (((crc16>>3)^(crc16<<1)^ (Data<<(13-j)))&0x1000)| 
                        ((crc16<<1)&0x0FC0) | 
                        (((crc16>>10)^(crc16<<1)^ ((j>6)?(Data>>(j-6)):(Data<<(6-j))))&0x0020)| 
                        ((crc16<<1)&0x001E) | 
                        (((crc16>>15) ^ (Data>>(j-1))) & 0x0001);
    		}
    		#endif
        }

        // send data crc
        crc16 = 0;
        BSP_SpiWriteByte((UINT8)((crc16 & 0xFF00)>>8));
        BSP_SpiWriteByte((UINT8)(crc16 & 0xFF));

        ///TODO: wait for slave firmware to complete TRANS_DONE_INT, no need to send sclk
    
        // wait data response token and busy token
        for (i = 0; i < 8; i++)
            BSP_SpiWriteByte(0xFF);    

        BSP_SpiWait();
    
        NST_WR_GPIO_REG(SWPORTA_DR, 0xFF);
        bytecnt -= bytecnt_i;
        pBuf += bytecnt_i;
    }
}

VOID BSP_SpiSdioCmdTest(VOID)
{
    UINT32 i;
    UINT32 WriteStartTime = 0;
    UINT32 WriteDoneTime = 0;
    UINT32 ReadDoneTime = 0;
    
    for (i = 0; i < 4096; i++)
    {
        TestSdioWriteBuf[i] = 255 - (UINT8)i;
        TestSdioReadBuf[i] = 0xFF;
    }

    BSP_SpiInit();

    BSP_SpiEumSdioSlave();

    WriteStartTime = *Tmr1Val;
    BSP_SpiCmd53Write(4096, TestSdioWriteBuf);
    WriteDoneTime = *Tmr1Val;

    BSP_SpiCmd53Read(4096, TestSdioReadBuf);
    ReadDoneTime = *Tmr1Val;

    if (memcmp(TestSdioWriteBuf, TestSdioReadBuf, 4096) == 0)
    {
        DBGPRINT(DEBUG_TRACE, "test sdio succeed, WriteStartTime = %d, WriteDoneTime = %d, ReadDoneTime = %d\n",
        						WriteStartTime, WriteDoneTime, ReadDoneTime);
    }
    else
    {
        DBGPRINT(DEBUG_TRACE, "test sdio failed\n");
        hex_dump("TestSdioReadBuf", TestSdioReadBuf, 4096);
    }
}
#endif //SPI_SDIO_CMD_TEST

