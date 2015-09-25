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
                  
                  
** Returns      : ��
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_SDIOInit(VOID)
{
    UINT16 i, count = 256;
    CSR_CCCR_CTL_REG	CSRCccrCtlReg;

    //���Ĵ�������
/*
��1��дCSR�Ŀ���ַ�Ĵ�����0x48��д��0x0003_fc00��1.3 �ڵ�SDIO���Ĵ���д��255��������ʼ��ַΪ0����дCSR�����ݼĴ�����0x40��д255�Σ���д��255��32bit
��������ַ������д������0x0f1f_0f1f��
��2��дCSR���ж�״̬ʹ�ܼĴ���(0x40��д��0xffff_ffffʹ�������ж�)
��3��дCSR���ж��ź�ʹ�ܼĴ���(0x44��д��0xffff_ffffʹ�������ж�)����һ��testcase�Ĳ�������ôû�У�
��4��дCSR��IOREADY�Ĵ���(0x50��д��0x0007)ָʾfunction1/function2 ready
��5��дCSR�Ŀ�OCR�Ĵ���(0x84��д��0x01ff_8000)���ÿ�֧�ֵĵ�ѹ��Χ
��6��дCSR�Ŀ��ƼĴ���(0x00��д��0x0004)��ָʾcard init done

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
                  
                  
** Returns      : ��
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID BSP_HostIntISR(VOID)
{
    UINT32 IntSource;
    
    OSIntEnter();

    //  ��SDIO�ж�
    NVIC_DisableIRQ(SMID_IRQn);
    
    /*��CSR�Ĵ���*/
    IntSource = NST_RD_SDIO_CSR_REG(ADDR_CSR_INT_STATUS_REG);
  
    /*  ��M3 ��SMID �ж�*/
    NVIC_ClearIRQChannelPendingBit(SMID_IRQn);
    
    /*  �ж�״̬�Ĵ���RW1C��д1��0*/
    NST_WR_SDIO_CSR_REG(ADDR_CSR_INT_STATUS_REG, IntSource);

    if (IntSource & CMD0_52_SOFT_RST_INT) 
    {
        BSP_SDIOInit();
    }
    
    if (IntSource & PRG_START_INT) 
    {
        ///TODO: deal with program start event
    }

    //1  ��/д����ж�
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

    // host д������ʼ�ж�
    if (IntSource & (WR_START_INT))
    {
        UINT8*   pTransBuf = NULL;
        //����cmd�Ĵ�����argument�Ĵ�����ֵ,trans doneʱ��Ҫ�õ�argment
        REG_CSR_CMD.word = NST_RD_SDIO_CSR_REG(ADDR_CSR_CMD_REG);  
        REG_CSR_CMD53_ARG.word = NST_RD_SDIO_CSR_REG(ADDR_CSR_ARG_REG);    

        ///TODO: check cmd53 arg and set transfer buffer

        // ���ô������Buffer��ַ
        #ifdef SDIO_TEST
        pTransBuf = TestSdioBuf + CurWrIdx*512;
        #endif
        
        if (pTransBuf)
        {
            /*����CSR_DMA_ADR/CTL_REG,���ݽ�����д���ߴ��Ķ�*/
            /* DMA Buf size�������һЩ�������ַ����DMAҳ�߽����DMA�ж�*/
            NST_WR_SDIO_CSR_REG(ADDR_CSR_DMA1_ADR_REG, pTransBuf);
            NST_WR_SDIO_CSR_REG(ADDR_CSR_DMA1_CTL_REG, ((DMA_BUF_512K<<1)|DMA_ADR_VALID));
        }
    }

    // host ��������ʼ�ж�    
    if (IntSource & (RD_START_INT))
    {
        UINT8*   pTransBuf = NULL;
        //����cmd�Ĵ�����argument�Ĵ�����ֵ,trans doneʱ��Ҫ�õ�argment
        REG_CSR_CMD.word = NST_RD_SDIO_CSR_REG(ADDR_CSR_CMD_REG);  
        REG_CSR_CMD53_ARG.word = NST_RD_SDIO_CSR_REG(ADDR_CSR_ARG_REG);    

        ///TODO: check cmd53 arg and set transfer buffer

        // ���ô������Buffer��ַ
        #ifdef SDIO_TEST
        pTransBuf = TestSdioBuf + CurRdIdx*512;
        #endif

        if (pTransBuf)
        {
                /*����CSR_DMA_ADR/CTL_REG,���ݽ�����д���ߴ��Ķ�*/
                /* DMA Buf size�������һЩ�������ַ����DMAҳ�߽����DMA�ж�*/
            NST_WR_SDIO_CSR_REG(ADDR_CSR_DMA1_ADR_REG, pTransBuf);
            NST_WR_SDIO_CSR_REG(ADDR_CSR_DMA1_CTL_REG, ((DMA_BUF_512K<<1)|DMA_ADR_VALID));
        }
    }
    
   // ��SDIO�ж�
    NVIC_EnableIRQ(SMID_IRQn);
    
    OSIntExit();

}

/*
******************************************************************************
**                            End Of File                                    **
******************************************************************************
*/
