/*
******************************************************************************
**
** Project     : 
** Description :    sdio
** Author      :                                 
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/
//------------------------------------------------------------------------------------------------------
#include "includes.h"

SDIO_REG    SdioReg;

#ifdef SDIO_TEST
#define TEST_SDIO_BUF_NUM 16
UINT8 TestSdioBuf[512 * TEST_SDIO_BUF_NUM] = {0};
UINT8 CurWrIdx = 0;
UINT8 CurRdIdx = 0;
#endif

/*
******************************************************************************
**      VOID BSP_SDIOInit(VOID)
**
** Description  : Init SDIO
** Arguments    : 
                  
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_SDIOInit(VOID)
{
    UINT16 i, count = 256;
    CSR_CCCR_CTL_REG	CSRCccrCtlReg;

    //卡寄存器配置
/*
（1）写CSR的卡地址寄存器（0x48，写入0x0003_fc00往1.3 节的SDIO卡寄存器写入255个数，起始地址为0），写CSR卡数据寄存器（0x40，写255次，即写入255个32bit
的数，地址递增，写入内容0x0f1f_0f1f）
（2）写CSR的中断状态使能寄存器(0x40，写入0xffff_ffff使能所有中断)
（3）写CSR的中断信号使能寄存器(0x44，写入0xffff_ffff使能所有中断)，这一步testcase的波形上怎么没有？
（4）写CSR的IOREADY寄存器(0x50，写入0x0007)指示function1/function2 ready
（5）写CSR的卡OCR寄存器(0x84，写入0x01ff_8000)设置卡支持的电压范围
（6）写CSR的控制寄存器(0x00，写入0x0004)，指示card init done

*/
    NST_WR_SDIO_CSR_REG(ADDR_CSR_CARD_ADR_REG, count<<10); //255
    for(i = 0; i < count; i++)
        NST_WR_SDIO_CSR_REG(ADDR_CSR_CARD_DATA_REG, 0x0f1f0f1f); 

    CSRCccrCtlReg.word = 0;
    CSRCccrCtlReg.bit.CccrFmtVer = 0x02;
    CSRCccrCtlReg.bit.SdioSpecVer = 0x03;
    CSRCccrCtlReg.bit.SdxFmtVer = 0x02;
    CSRCccrCtlReg.bit.S8B = 0;
    CSRCccrCtlReg.bit.SCSI = 0;
    CSRCccrCtlReg.bit.SDC = 1;
    CSRCccrCtlReg.bit.SMB = 1;
    CSRCccrCtlReg.bit.SRW = 1;
    CSRCccrCtlReg.bit.SBS = 1;	
    CSRCccrCtlReg.bit.S4MI = 0;
    CSRCccrCtlReg.bit.LSC = 0;
    CSRCccrCtlReg.bit.M4BLS = 1;
    CSRCccrCtlReg.bit.SMPC = 1;
    CSRCccrCtlReg.bit.SHS = 1;
    CSRCccrCtlReg.bit.SAI = 1;
    NST_WR_SDIO_CSR_REG(ADDR_CSR_CCCR_CTL_REG, CSRCccrCtlReg.word);
    
    NST_WR_SDIO_CSR_REG(ADDR_CSR_INT_STATUS_REG, HOST_INT_ALL_CLEAR);
    NST_WR_SDIO_CSR_REG(ADDR_CSR_INT_STATUS_ENA_REG, HOST_INT_ALL_EABLE); 
    NST_WR_SDIO_CSR_REG(ADDR_CSR_INT_SIGNAL_ENA_REG, HOST_INT_ALL_EABLE);

    NST_WR_SDIO_CSR_REG(ADDR_CSR_IO_READY_REG, (FUNC1_RDY|FUNC2_RDY));
    NST_WR_SDIO_CSR_REG(ADDR_CSR_CARD_OCR_REG, 0x10300000/*0x01ff8000*/);
    NST_WR_SDIO_CSR_REG(ADDR_CSR_CTL_REG, CARD_INIT_DONE);

    NST_WR_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL) & (~ENA_SPIM_GPIO));

}

/*
******************************************************************************
**                        VOID BSP_HostIntISR(VOID)
**
** Description  : host INT ISR
** Arguments    : 
                  
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_HostIntISR(VOID)
{
    UINT32 IntSource;
    
    OSIntEnter();

    //  关SDIO中断
    NVIC_DisableIRQ(SMID_IRQn);
    
    /*读CSR寄存器*/
    IntSource = NST_RD_SDIO_CSR_REG(ADDR_CSR_INT_STATUS_REG);
  
    /*  清M3 的SMID 中断*/
    NVIC_ClearIRQChannelPendingBit(SMID_IRQn);
    
    /*  中断状态寄存器RW1C，写1清0*/
    NST_WR_SDIO_CSR_REG(ADDR_CSR_INT_STATUS_REG, IntSource);

    if (IntSource & CMD0_52_SOFT_RST_INT) 
    {
        BSP_SDIOInit();
    }
    
    if (IntSource & PRG_START_INT) 
    {
        ///TODO: deal with program start event
    }

    //1  读/写完成中断
    if (IntSource & TRANS_DONE_INT) 
    {   
        if (REG_CSR_CMD53_ARG.bit.RWFlag == SDIO_WRITE) // host write
        {  
        	if(IntSource & (CRC_END_ERR_INT))
        	{
			///TODO: deal with crc err event
        	}

		if(IntSource & (ABORT_INT)) 
		{
			///TODO: deal with abort event
		}

		///TODO: deal with the data from host   
		#ifdef SDIO_TEST
		CurWrIdx++;
		if (CurWrIdx == TEST_SDIO_BUF_NUM)
			CurWrIdx = 0;
		#endif
		
		/*write PrgDone*/
		NST_WR_SDIO_CSR_REG(ADDR_CSR_CTL_REG, 1 << 0);
        }
        else  // host read
        {
		///TODO: deal with host read done event
		#ifdef SDIO_TEST
		CurRdIdx++;
		if (CurRdIdx == TEST_SDIO_BUF_NUM)
			CurRdIdx = 0;
		#endif
        }
    }// trans done

    // host 写操作开始中断
    if (IntSource & (WR_START_INT))
    {
        UINT8*   pTransBuf = NULL;
        //保存cmd寄存器和argument寄存器的值,trans done时需要用到argment
        REG_CSR_CMD.word = NST_RD_SDIO_CSR_REG(ADDR_CSR_CMD_REG);  
        REG_CSR_CMD53_ARG.word = NST_RD_SDIO_CSR_REG(ADDR_CSR_ARG_REG);    

        ///TODO: check cmd53 arg and set transfer buffer

        // 设置待传输的Buffer地址
        #ifdef SDIO_TEST
        pTransBuf = TestSdioBuf + CurWrIdx*512;
        #endif
        
        if (pTransBuf)
        {
            /*配置CSR_DMA_ADR/CTL_REG,数据将往哪写或者从哪读*/
            /* DMA Buf size尽量设大一些，避免地址到达DMA页边界产生DMA中断*/
            NST_WR_SDIO_CSR_REG(ADDR_CSR_DMA1_ADR_REG, pTransBuf);
            NST_WR_SDIO_CSR_REG(ADDR_CSR_DMA1_CTL_REG, ((DMA_BUF_512K<<1)|DMA_ADR_VALID));
        }
    }

    // host 读操作开始中断    
    if (IntSource & (RD_START_INT))
    {
        UINT8*   pTransBuf = NULL;
        //保存cmd寄存器和argument寄存器的值,trans done时需要用到argment
        REG_CSR_CMD.word = NST_RD_SDIO_CSR_REG(ADDR_CSR_CMD_REG);  
        REG_CSR_CMD53_ARG.word = NST_RD_SDIO_CSR_REG(ADDR_CSR_ARG_REG);    

        ///TODO: check cmd53 arg and set transfer buffer

        // 设置待传输的Buffer地址
        #ifdef SDIO_TEST
        pTransBuf = TestSdioBuf + CurRdIdx*512;
        #endif

        if (pTransBuf)
        {
                /*配置CSR_DMA_ADR/CTL_REG,数据将往哪写或者从哪读*/
                /* DMA Buf size尽量设大一些，避免地址到达DMA页边界产生DMA中断*/
            NST_WR_SDIO_CSR_REG(ADDR_CSR_DMA1_ADR_REG, pTransBuf);
            NST_WR_SDIO_CSR_REG(ADDR_CSR_DMA1_CTL_REG, ((DMA_BUF_512K<<1)|DMA_ADR_VALID));
        }
    }
    
   // 开SDIO中断
    NVIC_EnableIRQ(SMID_IRQn);
    
    OSIntExit();

}

/*
******************************************************************************
**                            End Of File                                    **
******************************************************************************
*/
