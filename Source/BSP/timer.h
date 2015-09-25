/*
** Copyright (C) ARM Limited, 2006. All rights reserved.
*/

/*
** SysTick Header file.
**
** Defines SysTick Registers and Masks
*/

#ifndef TIMER_H_
#define TIMER_H_

void SysTick_init(void);
__irq void SysTickHandler(void);
VOID  BSP_TickInit (VOID);
VOID BSP_Timer0Init(UINT32 usTime);
VOID BSP_Timer1Init(UINT32 usTime);

/* SysTick Registers */
#define SysTickCtrl  (volatile int*)0xE000E010
#define SysTickLoad  (volatile int*)0xE000E014
#define SysTickValue (volatile int*)0xE000E018
#define SysTickCalib (volatile int*)0xE000E01c

/* SysTick Masks */
#define SysTickCountFlag (1<<16)
#define SysTickClkSource (1<<2)  // ÄÚºËÊ±ÖÓÔ´
#define SysTickInterrupt (1<<1)
#define SysTickEnable    (1<<0)



#define TMR_INT_MASK        (1<<2)
/*0 - free-running mode
1 - user-defined count mode*/
#define TMR_USER_DEFINE_MODE            (1<<1) 
#define TMR_ENA             (1<<0)



#endif /*TIMER_H_*/
