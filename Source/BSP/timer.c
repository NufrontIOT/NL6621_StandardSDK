/*
** Copyright (C) ARM Limited, 2006. All rights reserved.
*/

/*
** This file contains the SysTick Initialisation function and
** SysTick exception handler.
*/
#include "includes.h"
#include "timer.h"
#include "NVIC.h"

//#include "cortexm3.h"
#define MAGIC_VALUE 299999

#include <stdio.h>
/*
******************************************************************************
**                        VOID  BSP_TickInit (VOID)
**
** Description  : Init system clock step(tick)
** Arguments    : 
                  
                  
** Returns      : ÎÞ
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
#ifdef USE_CORTEX_M3

void BSP_TickInit(void)
{
#ifndef ISSM  /* Configure SysTick for real Hardware */
    
    int         ClkFreq; //Hz
    int         cnts;

    ClkFreq = CpuClkFreq;
    cnts = ClkFreq/OS_TICKS_PER_SEC;
    *SysTickLoad = cnts;
    
#else

    *SysTickLoad = MAGIC_VALUE; /* Found to be more realistic on ISSM */

#endif /* !defined ISSM */

    /* Start, with interrupts enabled */
    *SysTickCtrl = SysTickEnable | SysTickInterrupt | SysTickClkSource;

    *(volatile UINT8*)(0xE000ED23) = (SYS_TICK_PRIO << 5); // b7..b5ÓÐÐ§
    
  //  *SysTickCtrl = SysTickEnable; /* Start, without interrupts enabled */
}

#else

VOID  BSP_TickInit (VOID)
{
    // Install IRQ Handler
     *(volatile unsigned *)0x18 = (unsigned)&OSTickISR - 0x20 >> 2 |0xEA000000;
    // Set up Timer


    *(volatile unsigned *)0x0a800000 = 0x4000;

    *(volatile unsigned *)0x0a800008 = 0xc0;

    // Enable Interrupt Controller

    *(volatile unsigned *)0x0a000008 = 0x10;  
  //  SysTickPeriodSet((INT32U)(SysCtlClockGet() / OS_TICKS_PER_SEC) -1 );
  //  SysTickEnable();
  //  SysTickIntEnable();
  
  
}

#endif


VOID BSP_Timer0Init(UINT32 usTime)
{
    UINT32 RegEoi;

// clear timer interrupt
    RegEoi = *Tmr0Eoi;
    RegEoi = *TmrsEoi;
    RegEoi = RegEoi;
    
    *Tmr0Load = ApbClkFreq/1000000*usTime;
    
    *Tmr0Ctl = (~TMR_INT_MASK) |TMR_ENA | TMR_USER_DEFINE_MODE;
}

VOID BSP_Timer1Init(UINT32 usTime)
{
    UINT32 RegEoi;

// clear timer interrupt
    RegEoi = *Tmr1Eoi;
    RegEoi = *TmrsEoi;
    RegEoi = RegEoi;
    
    *Tmr1Load = ApbClkFreq/1000000*usTime;
    
    *Tmr1Ctl = (~TMR_INT_MASK) |TMR_ENA | TMR_USER_DEFINE_MODE;
}

/*
******************************************************************************
**                            End Of File                                   **
******************************************************************************
*/

