/*
******************************************************************************
**
** Project     : 
** Description :    gpio
** Author      :    zhangjing
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/
//------------------------------------------------------------------------------------------------------
#include "includes.h"
#include "gpio.h"

#define TEST_GPIO_IDX  5

/*
******************************************************************************
**                        VOID  BSP_GPIOInit (VOID)
**
** Description  : GPIO initialize
** Arguments    : 
                  
                  
** Returns      : нч
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID  BSP_GPIOInit (VOID)
{

//The following code is an example for GPIO test,
//should be delete after test

#ifdef GPIO_TEST_INPUT
     BOOLEAN gpio_val;
     //
     //GPIO output example
     //
     //set pin multiplex gpio
     BSP_GPIOPinMux(TEST_GPIO_IDX);
    
     //set direction for GPIO port, if set all port , portNum = 0xFF; if set individal port ,portnum = port number(0-31)
     BSP_GPIOSetDir(TEST_GPIO_IDX, GPIO_DIRECTION_INPUT); 

     //set gpio value
     gpio_val = BSP_GPIOGetValue(TEST_GPIO_IDX);
#endif

#ifdef GPIO_TEST_OUTPUT
     //
     //GPIO input example
     //
     //set pin multiplex gpio
     BSP_GPIOPinMux(TEST_GPIO_IDX);
    
     //set direction for GPIO port, if set all port , portNum = 0xFF; if set individal port ,portnum = port number(0-31)
     BSP_GPIOSetDir(TEST_GPIO_IDX, GPIO_DIRECTION_OUTPUT); 

     //set gpio value
     BSP_GPIOSetValue(TEST_GPIO_IDX, GPIO_HIGH_LEVEL);
#endif

#ifdef GPIO_TEST_INTERRUPT
     //
     //GPIO Interrupt example
     //
     //set pin multiplex gpio
     BSP_GPIOPinMux(TEST_GPIO_IDX);
    
     //set direction for GPIO port, if set all port , portNum = 0xFF; if set individal port ,portnum = port number(0-31)
     BSP_GPIOSetDir(TEST_GPIO_IDX, GPIO_DIRECTION_INPUT); 
 
     //set gpio interrupt
     BSP_GPIOIntEn (TEST_GPIO_IDX, GPIO_INT_EDGE_SENSITIVE, GPIO_INT_ACTIVE_HIGH);

     BSP_GPIOIntMask(TEST_GPIO_IDX, GPIO_INT_UNMASK);

     //you must enable GPIO interrupt ,we suggest that you copy the following code to bsp.c  BSP_IntCtrlInit to enable GPIO IRQ
     //NVIC_IRQChannel must be matching with GPIO number
     /*   
        NVIC_InitStructure.NVIC_IRQChannel = EXTI5_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = GPIO_IRQn_PRIO;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = IRQ_ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    */


#endif          	 
}

/*
******************************************************************************
*                        VOID  BSP_GPIOPinMux (UINT8 portNum)
*
* Description  : Set GPIO pin multplex
* Arguments    :       
*         portNum:   Port Number value, should be in range from 2 to 31
*                          GPIO0 &GPIO1 are single pin,only GPIO2-31 are multplex
* Returns      : 
* Author       :                                   
* Date         : 
*
******************************************************************************
*/
VOID  BSP_GPIOPinMux (int portNum)
{
    int reg_val = 0;

    if((portNum > 31)&&(portNum <2))
        return;
	
    if(portNum == 2)
    {
        reg_val = NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL);
        reg_val &= ~(1<<2); 
    }
    else if(portNum >2)
    {
        reg_val = NST_RD_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL);
        reg_val |= (1<<portNum); 
    }
    NST_WR_PWM_REG(ADDR_GPIO_PIN_MUX_CTRL, reg_val);
    
}

/*
******************************************************************************
*                        VOID  BSP_GPIOSetDir (VOID)
*
* Description  : Set Direction for GPIO port.
* Arguments    :
* 	portNum		Port Number value, should be in range from 0 to 4
* 	dir			Direction value, should be:
* 							- 0: Input.
* 							- 1: Output.                 
* Returns      : None
* Author       :                                   
* Date         : 
*
******************************************************************************
*/
VOID  BSP_GPIOSetDir ( int portNum, BOOLEAN dir)
{
    int reg_val;

    if(portNum > 31)
        return;
	
    if(dir)
    {
        reg_val = NST_RD_GPIO_REG(SWPORTA_DDR);
        reg_val |= 1 << portNum;
    }
    else
    {
        reg_val = NST_RD_GPIO_REG(SWPORTA_DDR);
        reg_val &= ~(1<<portNum); 
    }
    NST_WR_GPIO_REG(SWPORTA_DDR, reg_val); 
}

/*
********************************************************************
**                        VOID  BSP_GPIOSetValue (int portNum, BOOLEAN bitValue)
**
** Description:		Set value for GPIO output port.
** Arguments    : 
** 	portNum		Port Number value, should be in range from 0 to 31
** 	bitValue	       reg value, should be 0 / 1

** Returns		None
**
**********************************************************************
 */
VOID  BSP_GPIOSetValue (int portNum, BOOLEAN bitValue)
{
     int reg_val;

    if(portNum > 31)
        return;
	
    if(bitValue)
    {
        reg_val = NST_RD_GPIO_REG(SWPORTA_DR);
        reg_val |= 1 << portNum;
    }
    else
    {
        reg_val = NST_RD_GPIO_REG(SWPORTA_DR);
        reg_val &= ~(1<<portNum); 
    }
    NST_WR_GPIO_REG(SWPORTA_DR, reg_val); 
}

/*
********************************************************************
 *                        VOID  BSP_GPIOReadValue (int portNum)
 *
 * Description:		Get value from GPIO port.
 * Arguments    : 
 * 	portNum		Port Number value, should be in range from 0 to 31

 * Returns		bitValue
 *
 *********************************************************************
 */
int  BSP_GPIOGetValue (int portNum)
{
     int reg_val;
     
     if(portNum > 31)
        return NULL;
	
     reg_val = NST_RD_GPIO_REG(EXT_PORTA); 
     reg_val = reg_val>>portNum;
     reg_val &= 1;

    return reg_val;
}

/*
*******************************************************************
 *              VOID  BSP_GPIOIntEn (UINT8 portNum, BOOLEAN typeLev, BOOLEAN polarity)
 *
 * Description:	Enable GPIO interrupt (just used for GPIO 0-31)
 * Arguments    :
 * 	portNum		Port number 
 *    trigMode	       Value that configures the interrupt type to be level-sensitive or edge-sensitive
 * 							- 0: level-sensitive 
 * 							- 1: edge-sensitive
 *    polarity	      polarity of edge, should be:
 *                         when level sensitve: 
 *                                               - 0: active low
 * 			                             - 1: active high
 *                         whed edge sensitive:   
 *                                               - 0: falling-edge
 * 			                             - 1: raiseing-edge
 * return		None
 *********************************************************************
 */
 VOID  BSP_GPIOIntEn (int portNum, BOOLEAN trigMode, BOOLEAN polarity)
{
     int reg_val;
	 
     if(portNum > 31)
        return;
	
     //set int type level
    if(trigMode)
    {
        reg_val = NST_RD_GPIO_REG(G_INT_TYPE_LEVEL);
        reg_val |= 1 << portNum;
    }
    else
    {
        reg_val = NST_RD_GPIO_REG(G_INT_TYPE_LEVEL);
        reg_val &= ~(1<<portNum); 
    }
    NST_WR_GPIO_REG(G_INT_TYPE_LEVEL, reg_val); 
	 
     //set int polarity
    if(polarity)
    {
        reg_val = NST_RD_GPIO_REG(G_INT_POLARITY);
        reg_val |= 1 << portNum;
    }
    else
    {
	reg_val = NST_RD_GPIO_REG(G_INT_POLARITY);
	reg_val &= ~(1<<portNum); 
    }
    NST_WR_GPIO_REG(G_INT_POLARITY, reg_val); 
	 
     //enable interrupt
     reg_val = NST_RD_GPIO_REG(G_INT_ENA);
     reg_val |= 1 << portNum;
     NST_WR_GPIO_REG(G_INT_ENA, reg_val); 
     
}

/*
******************************************************************************
**                        VOID  BSP_GPIOIntDisable (UINT8 portNum)
**
** Description  : Disable GPIO interrupt
** Arguments    :
** 	portNum		Port Number value, should be in range from 0 to 31

** Returns      : None
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
 VOID  BSP_GPIOIntDisable (int portNum)
{
     int reg_val;
	 
     if(portNum > 31)
        return;
		 
     //disable interrupt
     reg_val = NST_RD_GPIO_REG(G_INT_ENA);
     reg_val &= ~(1 << portNum);
     NST_WR_GPIO_REG(G_INT_ENA, reg_val);      
}

/*
******************************************************************************
**                        VOID  BSP_GPIOIntMask (UINT8 portNum)
**
** Description  : Mask GPIO interrupt
** Arguments    :
** 	portNum		Port Number value, should be in range from 0 to 31
**    mask             1  GPIO interrupt mask  
                            0  GPIO interrupt unmask
** Returns      : None
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
 VOID  BSP_GPIOIntMask (int portNum, BOOLEAN mask)
{
    int reg_val;
    
    if(portNum > 31)
        return;
		 
     //Mask or unmask interrupt
    if(mask)
    {
        reg_val = NST_RD_GPIO_REG(G_INT_MASK);
        reg_val |= 1 << portNum;
    }
    else
    {
        reg_val = NST_RD_GPIO_REG(G_INT_MASK);
        reg_val &= ~(1<<portNum); 
    }
    NST_WR_GPIO_REG(G_INT_MASK, reg_val);  
}

/*
******************************************************************************
**                        VOID  BSP_GPIOIntISR (UINT8 portNum)
**
** Description  : GPIO ISR
** Arguments    :
** 	portNum		Port Number value, should be in range from 0 to 31

** Returns      : None
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID  BSP_GPIOIntISR (int portNum)
{
     int reg_val;
     int irq_num;

     //NL6621 gpio irq index
     //GPIO: 0 1 2 3 4 5 6 7  8  9 10 11 12 13 14 15 16~31
     //IRQ:  8 7 6 5 4 3 2 1 25 24 23 22 21 20 19 18  29

    if(portNum >31)
        return;
	 
    if((portNum >=0)&&(portNum < 8))
        irq_num = 8 -portNum;
    else if((portNum >=8)&&(portNum < 16))
        irq_num = 33 -portNum;
    else
        irq_num = 29;
    
    //disable interrupt
    NVIC_DisableIRQ(irq_num);

    //Read GPIO interrupt status
    reg_val = NST_RD_GPIO_REG(G_INT_STATUS);
	
    //clear interrupt
    reg_val = NST_RD_GPIO_REG(PORTA_EOI);
    reg_val |= 1 << portNum;
    NST_WR_GPIO_REG(PORTA_EOI, reg_val); 
    
    //TODO: Add what you want to do
	
    //enable interrupt
    NVIC_EnableIRQ(irq_num);
}

