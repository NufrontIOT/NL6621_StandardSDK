#include "includes.h"
#include "uart.h"


//#define PRINTF_TIME_OUT   2000

/*
******************************************************************************
**                          void BSP_UartPutcPolled(uint8_t Data)
**
** Description    : Put char
** Arguments    : UINT8
** Returns        : 
** Author        :                                     
** Date         : 
**
******************************************************************************
*/
VOID BSP_UartPutcPolled(UINT8 Data)
{
    //INT32 Timeout = PRINTF_TIME_OUT;
    
    /*Data should only be written to the THR when the THR Empty (THRE) bit (LSR[5]) is set.*/
    /* Loop until the end of transmission */
    while((NST_RD_UART_REG(LSR_OFFSET) & LSR_THRE_MASK) == 0/* && (Timeout-- > 0)*/)
    {}

    NST_WR_UART_REG(Data, THR_OFFSET);

}
/*
******************************************************************************
**                       UINT8 BSP_UartGetcPolled(VOID)
**
** Description    : Get char
** Arguments    : 
** Returns      : UINT8
** Author        :                                     
** Date         : 
**
******************************************************************************
*/
UINT8 BSP_UartGetcPolled (VOID)
{

    while((NST_RD_UART_REG(LSR_OFFSET) & LSR_DR_MASK) == 0)
    {
    }
    
    return NST_RD_UART_REG(RBR_OFFSET);
   
}

/*
******************************************************************************
**                       VOID BSP_UartOpen(UINT32 Baudrate)
**
** Description    : Open USART
** Arguments    : Baudrate 
** Returns        : 
** Author        :                                     
** Date         : 
**
******************************************************************************
*/
VOID BSP_UartOpen(UINT32 Baudrate)
{
    UINT16 Divisor = ApbClkFreq/(Baudrate*16);
    UINT32 Tmp = 0;

    Tmp = ApbClkFreq%(Baudrate*16);
    if (Tmp > ((Baudrate*16)>>1))
        Divisor++;

    // UART_BAUD_RATE  8bit  1bit stop no parity
/*DLAB is used to enable reading and writing of the Divisor Latch register
(DLL and DLH) to set the baud rate of the UART. This bit must be cleared after initial
baud rate setup in order to access other registers.
*/
// UART pins can't used as GPIO
#ifdef NULINK2_SOC 
    NST_WR_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL) & (~ENA_UART_GPIO));
#else
    NST_WR_PWM_REG(ADDR_PIN_MUX_CTRL, NST_RD_PWM_REG(ADDR_PIN_MUX_CTRL) & (~ENA_UART_GPIO));
// b5置1使能串口，置0使能SPI
    NST_WR_PWM_REG(ADDR_UART_SPI_MUX, NST_RD_PWM_REG(ADDR_UART_SPI_MUX) | (1 << SEL_UART_SPI));
#endif
    
    NST_WR_UART_REG(LCR_DLAB_MASK|LCR_STOP_MASK|UART_8BIT_MODE, LCR_OFFSET);    //0x83
    NSTusecDelay(5);
    //Delay();
    
    NST_WR_UART_REG((Divisor >> 8), DLH_OFFSET);  
    NSTusecDelay(5);
    NST_WR_UART_REG((Divisor & 0xFF), DLL_OFFSET);  
    NSTusecDelay(5);
    //Delay();
    
    NST_WR_UART_REG(UART_8BIT_MODE, LCR_OFFSET);    //0x3
    NSTusecDelay(5);
    //Delay();
    
    NST_WR_UART_REG(FCR_FIFO_EN_MASK, FCR_OFFSET);  //0x01
    NSTusecDelay(5);

#ifdef HW_UART_IRQ_SUPPORT
    //enable uart interrupt
    NST_WR_UART_REG(0x5, IER_OFFSET);  //0x05 --receiver line status interrupt & Received data available interrupt
    NSTusecDelay(5);
#endif
    //Delay();    
}

/*
******************************************************************************
**                       VOID BSP_UartISR(VOID)
**
** Description    : Uart interrupt, show interrupt set & reset as follow

     interrupt ID  priority level       interrupt type                            interrupt reset
        0110             high           receiver line status                      read the line status reg
        0100            second       received data available                 read the receiver buffer reg
        1100            second      character timeout indicate              read the receiver buffer reg
        0010            third          transmit holding reg empty            read the IIR reg
        0000            fourth            modem status                          read the modem status reg
        0111            fifth             busy detect indicate                    read the UART status reg
                           
** Arguments    : 
** Returns        : 
** Author        :                                     
** Date         : 
**
******************************************************************************
*/
VOID BSP_UartISR(VOID)
{
    INT8U     IIRValue;
    INT8U     LSRValue;
    INT8U     Dummy = 0;
    BOOLEAN   bFlag = TRUE;

    OSIntEnter();
    
    // Disable UART interrupt 
    NVIC_DisableIRQ(UART_IRQn);

    while (bFlag)
    {
        IIRValue = NST_RD_UART_REG(IIR_OFFSET);         //read interrupt register  
        IIRValue = IIR_IID_GET(IIRValue);                         //get  interrupt id 
        switch (IIRValue)
        {
            case  UART_IIR_RLS :                            // --receiver line status
                LSRValue = NST_RD_UART_REG(LSR_OFFSET);      // read line status register & clear interrupt 

                // check overrun error,parity error, framing error,receiver fifo error bits
                if ( LSR_OE_GET(LSRValue) &&  LSR_PE_GET(LSRValue) &&  LSR_FE_GET(LSRValue) && LSR_FERR_GET(LSRValue))                       // -- 线状态 接收有关中断 只丢弃错误的成对接收RSR中字符
                {                            
                    break;                                 
                }

                if ( LSR_DR_GET(LSRValue) )               // -- indicate receiver data ready
                {          
                    Dummy = NST_RD_UART_REG(RBR_OFFSET);
                }
                break;
                
            case UART_IIR_RDA :                             // -- received data avaliable
                Dummy = NST_RD_UART_REG(RBR_OFFSET);    // read receive buffer register & clear interrupt
                //TODO:
                //for UART interrupt test, print data, should be delete after test
                BSP_UartPutcPolled(Dummy);

                break;
                
            case UART_IIR_CTI :                           // -- character timeout. we must read receiver buffer register to clear this inerrupt 
                Dummy = NST_RD_UART_REG(RBR_OFFSET);    // clear interrupt
                break;
				
            case UART_IIR_THRE :                            //--THR empty
                break;
				
            case UART_IIR_MDS :                             //--modem status 
                Dummy = NST_RD_UART_REG(MSR_OFFSET);    //clear interrupt
                break;

            case UART_IIR_BUSY :                             //--modem status 
                Dummy = NST_RD_UART_REG(USR_OFFSET);    //clear interrupt
                break;
                           
            default:
                bFlag = FALSE;                              
                break;
        }
    }

    // Enable UART interrupt
    NVIC_EnableIRQ(UART_IRQn);

    OSIntExit();		   
}
