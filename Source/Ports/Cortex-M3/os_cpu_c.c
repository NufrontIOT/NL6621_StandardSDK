/*  
*********************************************************************************************************  
*                                               uC/OS-II  
*                                         The Real-Time Kernel  
*  
*  
*                             (c) Copyright 1992-2007, Micrium, Weston, FL  
*                                          All Rights Reserved  
*  
*                                           Generic ARM Port  
*  
* File      : OS_CPU_C.C  
* Version   : V1.82  
* By        : Jean J. Labrosse  
*             Jean-Denis Hatier  
*  
* For       : ARMv7M Cortex-M3
* Mode      : Thumb2
* Toolchain : RealView Development Suite
*             RealView Microcontroller Development Kit (MDK)
*             ARM Developer Suite (ADS)
*             Keil uVision
*********************************************************************************************************  
*/   
   
//#define  OS_CPU_GLOBALS    
#include <ucos_ii.h>    
   
#if      OS_VIEW_MODULE > 0    
#include <OS_VIEWc.H>    
#include <OS_VIEW.H>    
#endif    
#include "Os_dmem.h"

/*$PAGE*/   
/*  
*********************************************************************************************************  
*                                             LOCAL CONSTANTS  
*  
* Note(s) : 1) ARM_MODE_ARM is the CPSR bit mask for ARM Mode  
*           2) ARM_MODE_THUMB is the CPSR bit mask for THUMB Mode  
*           3) ARM_SVC_MODE_THUMB is the CPSR bit mask for SVC MODE + THUMB Mode  
*           4) ARM_SVC_MODE_ARM is the CPSR bit mask for SVC MODE + ARM Mode  
            5) OS_NTASKS_FP  establishes the number of tasks capable of supporting floating-point.  One  
*              task is removed for the idle task because it doesn't do floating-point at all.  
*           6) OS_FP_STORAGE_SIZE  currently allocates 1024 bytes of storage in order to accomodate  
*              thirty-two, single precision 32 bit, or sixteen double precision 64 bit VFP registers.  
*********************************************************************************************************  
*/   
   
#define  ARM_MODE_ARM           0x00000000    
#define  ARM_MODE_THUMB         0x00000020    
   
#define  ARM_SVC_MODE_THUMB    (0x00000013L + ARM_MODE_THUMB)    
#define  ARM_SVC_MODE_ARM      (0x00000013L + ARM_MODE_ARM)    
   
#define  OS_NTASKS_FP          (OS_MAX_TASKS + OS_N_SYS_TASKS - 1)    
#define  OS_FP_STORAGE_SIZE            128L    
   
/*  
*********************************************************************************************************  
*                                          LOCAL VARIABLES  
*********************************************************************************************************  
*/   
   
#if OS_TMR_EN > 0    
static  INT16U  OSTmrCtr;   
#endif    
   
#if OS_CPU_FPU_EN > 0    
static  OS_MEM  *OSFPPartPtr;                                           /* Pointer to memory partition for storing FPU registers    */   
static  INT32U   OSFPPart[OS_NTASKS_FP][OS_FP_STORAGE_SIZE / sizeof(INT32U)];   
#endif    


#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
#define OS_EVENT_PATCH_NUM	10             /*  */
OS_EVENT         *OSEventFreeListPatch;          /* Pointer to list of free EVENT control blocks    */
OS_EVENT          OSEventTblPatch[OS_EVENT_PATCH_NUM];/* Table of EVENT control blocks                   */
#endif

void OSEventPatch(void)
{
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
#if (OS_MAX_EVENTS > 1u)
    INT16U     ix;
    INT16U     ix_next;
    OS_EVENT  *pevent1;
    OS_EVENT  *pevent2;


    OS_MemClr((INT8U *)&OSEventTblPatch[0], sizeof(OSEventTblPatch)); /* Clear the event table                   */
    for (ix = 0u; ix < (10u - 1u); ix++) {        /* Init. list of free EVENT control blocks */
        ix_next = ix + 1u;
        pevent1 = &OSEventTblPatch[ix];
        pevent2 = &OSEventTblPatch[ix_next];
        pevent1->OSEventType    = OS_EVENT_TYPE_UNUSED;
        pevent1->OSEventPtr     = pevent2;
#if OS_EVENT_NAME_EN > 0u
        pevent1->OSEventName    = (INT8U *)(void *)"?";     /* Unknown name                            */
#endif
    }
    pevent1                         = &OSEventTblPatch[ix];
    pevent1->OSEventType            = OS_EVENT_TYPE_UNUSED;
    pevent1->OSEventPtr             = (OS_EVENT *)0;
#if OS_EVENT_NAME_EN > 0u
    pevent1->OSEventName            = (INT8U *)(void *)"?"; /* Unknown name                            */
#endif
    OSEventFreeListPatch                 = &OSEventTblPatch[0];
#else
    OSEventFreeListPatch                 = &OSEventTblPatch[0];       /* Only have ONE event control block       */
    OSEventFreeListPatch->OSEventType    = OS_EVENT_TYPE_UNUSED;
    OSEventFreeListPatch->OSEventPtr     = (OS_EVENT *)0;
#if OS_EVENT_NAME_EN > 0u
    OSEventFreeListPatch->OSEventName    = (INT8U *)"?";         /* Unknown name                            */
#endif
#endif
    OSEventTbl[OS_MAX_EVENTS -1].OSEventPtr =  OSEventFreeListPatch;
#endif
}

   
/*$PAGE*/   
/*  
*********************************************************************************************************  
*                                        INITIALIZE FP SUPPORT  
*  
* Description: This function initializes the memory partition used to save FPU registers  
*              during a context switch.  This function MUST be called AFTER calling  
*              OSInit(). OS_CPU_FPU_EN must be defined > 0 in order to compile FPU support into the  
*              build.  
*  
* Arguments  : none  
*  
* Returns    : none  
*  
* Note(s)    : 1) Tasks that are to use FP support MUST be created with OSTaskCreateExt().  
*              2) For the ARM VFP, 1024 bytes are required to save the VFP context.  
*                 The INT32U data type is used to ensure that storage is aligned on a 32-bit boundary.  
*              3) If you need to perform floating point operations from within the OSStatTaskHook(),  
*                 then you must change the 'Options' attribute for OSTaskCreatExt() when creating  
*                 the statistics task. This only applies if OS_TaskStat() was created with OSTaskCreateExt().  
*********************************************************************************************************  
*/   
   
#if OS_CPU_FPU_EN > 0    
void  OS_CPU_FP_Init (void)   
{   
    INT8U    err;   
#if OS_TASK_STAT_EN && OS_TASK_CREATE_EXT_EN    
    OS_TCB  *ptcb;   
    void    *pblk;   
#endif    
   
   
    OSFPPartPtr = OSMemCreate(&OSFPPart[0][0], OS_NTASKS_FP, OS_FP_STORAGE_SIZE, &err);   
   
#if OS_TASK_STAT_EN && OS_TASK_CREATE_EXT_EN                            /* CHANGE 'OPTIONS' for OS_TaskStat()                       */    
    ptcb            = OSTCBPrioTbl[OS_TASK_STAT_PRIO];   
    ptcb->OSTCBOpt |= OS_TASK_OPT_SAVE_FP;                              /* Allow floating-point support for Statistic task          */   
    pblk            = OSMemGet(OSFPPartPtr, &err);                      /* Get storage for VFP registers                            */   
    if (pblk != (void *)0) {                                            /* Did we get a memory block?                               */   
        ptcb->OSTCBExtPtr = pblk;                                       /* Yes, Link to task's TCB                                  */   
        OS_CPU_FP_Save(pblk);                                           /*      Save the VFP registers in block                     */   
    }   
#endif    
}   
#endif    
   
/*  
*********************************************************************************************************  
*                                       OS INITIALIZATION HOOK  
*                                            (BEGINNING)  
*  
* Description: This function is called by OSInit() at the beginning of OSInit().  
*  
* Arguments  : none  
*  
* Note(s)    : 1) Interrupts should be disabled during this call.  
*********************************************************************************************************  
*/   
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203    
void  OSInitHookBegin (void)   
{   
#if OS_TMR_EN > 0    
    OSTmrCtr = 0;   
#endif    

    OS_DMemInit();

}   
#endif    
   
/*  
*********************************************************************************************************  
*                                       OS INITIALIZATION HOOK  
*                                               (END)  
*  
* Description: This function is called by OSInit() at the end of OSInit().  
*  
* Arguments  : none  
*  
* Note(s)    : 1) Interrupts should be disabled during this call.  
*********************************************************************************************************  
*/   
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203    
void  OSInitHookEnd (void)   
{   
#if OS_CPU_INT_DIS_MEAS_EN > 0    
    OS_CPU_IntDisMeasInit();   
#endif    
   
#if OS_CPU_FPU_EN > 0    
    OS_CPU_FP_Init();                                                   /* Initialize support for VFP register save / restore       */   
#endif    

#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
    OSEventPatch();
#endif

}   
#endif    
   
/*  
*********************************************************************************************************  
*                                          TASK CREATION HOOK  
*  
* Description: This function is called when a task is created.  
*  
* Arguments  : ptcb   is a pointer to the task control block of the task being created.  
*  
* Note(s)    : 1) Interrupts are disabled during this call.  
*********************************************************************************************************  
*/   
#if OS_CPU_HOOKS_EN > 0    
void  OSTaskCreateHook (OS_TCB *ptcb)   
{   
#if OS_CPU_FPU_EN > 0    
    INT8U  err;   
    void  *pblk;   
#endif    
   
   
#if OS_CPU_FPU_EN > 0    
    if (ptcb->OSTCBOpt & OS_TASK_OPT_SAVE_FP) {                         /* See if task needs FP support                             */   
        pblk = OSMemGet(OSFPPartPtr, &err);                             /* Yes, Get storage for VFP registers                       */   
        if (pblk != (void *)0) {                                        /*      Did we get a memory block?                          */   
            ptcb->OSTCBExtPtr = pblk;                                   /*      Yes, Link to task's TCB                             */   
            OS_CPU_FP_Save(pblk);                                       /*           Save the VFP registers in block                */   
        }   
    }   
#endif    
   
#if OS_VIEW_MODULE > 0    
    OSView_TaskCreateHook(ptcb);   
#else    
    (void)ptcb;                                                         /* Prevent compiler warning                                 */   
#endif    
}   
#endif    
   
   
/*  
*********************************************************************************************************  
*                                           TASK DELETION HOOK  
*  
* Description: This function is called when a task is deleted.  
*  
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.  
*  
* Note(s)    : 1) Interrupts are disabled during this call.  
*********************************************************************************************************  
*/   
#if OS_CPU_HOOKS_EN > 0    
void  OSTaskDelHook (OS_TCB *ptcb)   
{   
#if OS_CPU_FPU_EN > 0    
    if (ptcb->OSTCBOpt & OS_TASK_OPT_SAVE_FP) {                         /* See if task had FP support                               */   
        if (ptcb->OSTCBExtPtr != (void *)0) {                           /* Yes, OSTCBExtPtr must not be NULL                        */   
            OSMemPut(OSFPPartPtr, ptcb->OSTCBExtPtr);                   /*      Return memory block to free pool                    */   
        }   
    }   
#endif    
   
    (void)ptcb;                                                         /* Prevent compiler warning                                 */   
}   
#endif    
   
/*  
*********************************************************************************************************  
*                                             IDLE TASK HOOK  
*  
* Description: This function is called by the idle task.  This hook has been added to allow you to do  
*              such things as STOP the CPU to conserve power.  
*  
* Arguments  : none  
*  
* Note(s)    : 1) Interrupts are enabled during this call.  
*********************************************************************************************************  
*/   
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION >= 251    
void  OSTaskIdleHook (void)   
{   
#if OS_CPU_ARM_DCC_EN > 0    
    OSDCC_Handler();   
#endif    

#ifndef CPU_USAGE_STAT
#ifdef USE_CORTEX_M3
    CPU_EnterLowPower();
#endif
#endif

}   
#endif    


/*  
*********************************************************************************************************  
*                                             TASK RETURN HOOK  
*  
* Description: 
*              
*  
* Arguments  : none  
*  
* Note(s)    :
*********************************************************************************************************  
*/   
#if OS_CPU_HOOKS_EN > 0     
void          OSTaskReturnHook        (OS_TCB          *ptcb)
{   
}   
#endif    


/*  
*********************************************************************************************************  
*                                           STATISTIC TASK HOOK  
*  
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your  
*              application to add functionality to the statistics task.  
*  
* Arguments  : none  
*********************************************************************************************************  
*/   
   
#if OS_CPU_HOOKS_EN > 0    
void  OSTaskStatHook (void)   
{   
#ifdef CPU_USAGE_STAT   // NST_ADD
    PrintCpuUsage();
#endif
}   
#endif    
   
/*  
*********************************************************************************************************  
*                                        INITIALIZE A TASK'S STACK  
*  
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the  
*              stack frame of the task being created.  This function is highly processor specific.  
*  
* Arguments  : task          is a pointer to the task code  
*  
*              p_arg         is a pointer to a user supplied data area that will be passed to the task  
*                            when the task first executes.  
*  
*              ptos          is a pointer to the top of stack.  It is assumed that 'ptos' points to  
*                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then  
*                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if  
*                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address  
*                            of the stack.  
*  
*              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().  
*                            (see uCOS_II.H for OS_TASK_OPT_xxx).  
*  
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have  
*              been placed on the stack in the proper order.  
*  
* Note(s)    : 1) Interrupts are enabled when your task starts executing.  
*              2) All tasks run in SVC mode.  
*********************************************************************************************************  
*/   
   
OS_STK *OSTaskStkInit (void (*task)(void *p_arg), void *p_arg, OS_STK *ptos, INT16U opt)   
{   
#ifdef USE_CORTEX_M3
    OS_STK *stk;


    (void)opt;                                 /* 'opt' is not used, prevent warning                     */
    stk       = ptos;                          /* Load stack pointer                                     */

                                               /* Registers stacked as if auto-saved on exception        */
    *(stk)    = (INT32U)0x01000000L;           /* xPSR                                                   */
    *(--stk)  = (INT32U)task;                  /* Entry Point                                            */
    *(--stk)  = (INT32U)0xFFFFFFFEL;           /* R14 (LR) (init value will cause fault if ever used)    */
    *(--stk)  = (INT32U)0x12121212L;           /* R12                                                    */
    *(--stk)  = (INT32U)0x03030303L;           /* R3                                                     */
    *(--stk)  = (INT32U)0x02020202L;           /* R2                                                     */
    *(--stk)  = (INT32U)0x01010101L;           /* R1                                                     */
    *(--stk)  = (INT32U)p_arg;                 /* R0 : argument                                          */

                                               /* Remaining registers saved on process stack             */
    *(--stk)  = (INT32U)0x11111111L;           /* R11                                                    */
    *(--stk)  = (INT32U)0x10101010L;           /* R10                                                    */
    *(--stk)  = (INT32U)0x09090909L;           /* R9                                                     */
    *(--stk)  = (INT32U)0x08080808L;           /* R8                                                     */
    *(--stk)  = (INT32U)0x07070707L;           /* R7                                                     */
    *(--stk)  = (INT32U)0x06060606L;           /* R6                                                     */
    *(--stk)  = (INT32U)0x05050505L;           /* R5                                                     */
    *(--stk)  = (INT32U)0x04040404L;           /* R4                                                     */

    return (stk);

#else     
 
    OS_STK *stk;   
    INT32U  task_addr;   
   
   
    opt       = opt;                                                    /* 'opt' is not used, prevent warning                       */   
    stk       = ptos;                                                   /* Load stack pointer                                       */   
    task_addr = (INT32U)task & ~1;                                      /* Mask off lower bit in case task is thumb mode            */   
    *(stk)    = (INT32U)task_addr;                                      /* Entry Point                                              */   
    *(--stk)  = (INT32U)0x14141414L;                                    /* R14 (LR)                                                 */   
    *(--stk)  = (INT32U)0x12121212L;                                    /* R12                                                      */   
    *(--stk)  = (INT32U)0x11111111L;                                    /* R11                                                      */   
    *(--stk)  = (INT32U)0x10101010L;                                    /* R10                                                      */   
    *(--stk)  = (INT32U)0x09090909L;                                    /* R9                                                       */   
    *(--stk)  = (INT32U)0x08080808L;                                    /* R8                                                       */   
    *(--stk)  = (INT32U)0x07070707L;                                    /* R7                                                       */   
    *(--stk)  = (INT32U)0x06060606L;                                    /* R6                                                       */   
    *(--stk)  = (INT32U)0x05050505L;                                    /* R5                                                       */   
    *(--stk)  = (INT32U)0x04040404L;                                    /* R4                                                       */   
    *(--stk)  = (INT32U)0x03030303L;                                    /* R3                                                       */   
    *(--stk)  = (INT32U)0x02020202L;                                    /* R2                                                       */   
    *(--stk)  = (INT32U)0x01010101L;                                    /* R1                                                       */   
    *(--stk)  = (INT32U)p_arg;                                          /* R0 : argument                                            */   
    if ((INT32U)task & 0x01) {                                          /* See if task runs in Thumb or ARM mode                    */   
        *(--stk) = (INT32U)ARM_SVC_MODE_THUMB;                          /* CPSR  (Enable both IRQ and FIQ interrupts, THUMB-mode)   */   
    } else {   
        *(--stk) = (INT32U)ARM_SVC_MODE_ARM;                            /* CPSR  (Enable both IRQ and FIQ interrupts, ARM-mode)     */   
    }   
   
    return (stk);   
   
#endif

}   
   
/*  
*********************************************************************************************************  
*                                           TASK SWITCH HOOK  
*  
* Description: This function is called when a task switch is performed.  This allows you to perform other  
*              operations during a context switch.  
*  
* Arguments  : none  
*  
* Note(s)    : 1) Interrupts are disabled during this call.  
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that  
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the  
*                 task being switched out (i.e. the preempted task).  
*********************************************************************************************************  
*/   
#if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)    
void  OSTaskSwHook (void)   
{   
#if OS_CPU_FPU_EN > 0    
    void  *pblk;   
#endif    
   
#if OS_CPU_FPU_EN > 0                                      /* Save VFP context of preempted task       */    
    if (OSRunning == OS_TRUE) {                            /* Don't save on OSStart()!                 */   
        if (OSTCBCur->OSTCBOpt & OS_TASK_OPT_SAVE_FP) {    /* See if task used FP                      */   
            pblk = OSTCBCur->OSTCBExtPtr;                  /* Yes, Get pointer to FP storage area      */   
            if (pblk != (void *)0) {                       /*      Make sure we have storage           */   
                OS_CPU_FP_Save(pblk);                      /*      Save the VFP registers in block     */   
            }   
        }   
    }   
                                                           /* Restore VFP context of new task          */   
    if (OSTCBHighRdy->OSTCBOpt & OS_TASK_OPT_SAVE_FP) {    /* See if new task uses FP                  */   
        pblk = OSTCBHighRdy->OSTCBExtPtr;                  /* Yes, Get pointer to FP storage area      */   
        if (pblk != (void *)0) {                           /*      Make sure we have storage           */   
            OS_CPU_FP_Restore(pblk);                       /*      Get contents of VFP registers       */   
        }   
    }   
#endif    
   
#if OS_VIEW_MODULE > 0    
    OSView_TaskSwHook();   
#endif    
}   
#endif    
   
/*  
*********************************************************************************************************  
*                                           OS_TCBInit() HOOK  
*  
* Description: This function is called by OS_TCBInit() after setting up most of the TCB.  
*  
* Arguments  : ptcb    is a pointer to the TCB of the task being created.  
*  
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.  
*********************************************************************************************************  
*/   
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203    
void  OSTCBInitHook (OS_TCB *ptcb)   
{   
    (void)ptcb;                                                         /* Prevent Compiler warning                                 */   
}   
#endif    
   
   
/*  
*********************************************************************************************************  
*                                               TICK HOOK  
*  
* Description: This function is called every tick.  
*  
* Arguments  : none  
*  
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.  
*********************************************************************************************************  
*/   
#if (OS_CPU_HOOKS_EN > 0) && (OS_TIME_TICK_HOOK_EN > 0)    
void  OSTimeTickHook (void)   
{   
#if OS_VIEW_MODULE > 0    
    OSView_TickHook();   
#endif    
   
#if OS_TMR_EN > 0    
    OSTmrCtr++;   
    if (OSTmrCtr >= (OS_TICKS_PER_SEC / OS_TMR_CFG_TICKS_PER_SEC)) {   
        OSTmrCtr = 0;   
        OSTmrSignal();   
    }   
#endif    
   
#if OS_CPU_ARM_DCC_EN > 0    
    OSDCC_Handler();   
#endif  

  //  WDResetWDTimer();   // NST_ADD

}   
#endif    
   
   
/*  
*********************************************************************************************************  
*                             INTERRUPT DISABLE TIME MEASUREMENT, START  
*********************************************************************************************************  
*/   
   
#if OS_CPU_INT_DIS_MEAS_EN > 0    
void  OS_CPU_IntDisMeasInit (void)   
{   
    OS_CPU_IntDisMeasNestingCtr = 0;   
    OS_CPU_IntDisMeasCntsEnter  = 0;   
    OS_CPU_IntDisMeasCntsExit   = 0;   
    OS_CPU_IntDisMeasCntsMax    = 0;   
    OS_CPU_IntDisMeasCntsDelta  = 0;   
    OS_CPU_IntDisMeasCntsOvrhd  = 0;   
    OS_CPU_IntDisMeasStart();                                           /* Measure the overhead of the functions                    */   
    OS_CPU_IntDisMeasStop();   
    OS_CPU_IntDisMeasCntsOvrhd  = OS_CPU_IntDisMeasCntsDelta;   
}   
   
   
void  OS_CPU_IntDisMeasStart (void)   
{   
    OS_CPU_IntDisMeasNestingCtr++;   
    if (OS_CPU_IntDisMeasNestingCtr == 1) {                             /* Only measure at the first nested level                   */   
        OS_CPU_IntDisMeasCntsEnter = OS_CPU_IntDisMeasTmrRd();   
    }   
}   
   
   
void  OS_CPU_IntDisMeasStop (void)   
{   
    OS_CPU_IntDisMeasNestingCtr--;                                      /* Decrement nesting ctr                                    */   
    if (OS_CPU_IntDisMeasNestingCtr == 0) {   
        OS_CPU_IntDisMeasCntsExit  = OS_CPU_IntDisMeasTmrRd();   
        OS_CPU_IntDisMeasCntsDelta = OS_CPU_IntDisMeasCntsExit - OS_CPU_IntDisMeasCntsEnter;   
        if (OS_CPU_IntDisMeasCntsDelta > OS_CPU_IntDisMeasCntsOvrhd) {  /* Ensure overhead  delta                                  */   
            OS_CPU_IntDisMeasCntsDelta -= OS_CPU_IntDisMeasCntsOvrhd;   
        } else {   
            OS_CPU_IntDisMeasCntsDelta  = OS_CPU_IntDisMeasCntsOvrhd;   
        }   
        if (OS_CPU_IntDisMeasCntsDelta > OS_CPU_IntDisMeasCntsMax) {    /* Track MAXIMUM                                            */   
            OS_CPU_IntDisMeasCntsMax = OS_CPU_IntDisMeasCntsDelta;   
        }   
    }   
}   
#endif    
   
   
/*  
*********************************************************************************************************  
*                                     INITIALIZE EXCEPTION VECTORS  
*  
* Description : This function initialize exception vectors to the default handlers.  
*  
* Arguments   : None.  
*********************************************************************************************************  
*/   
   
void  OS_CPU_InitExceptVect (void)   
{   
/*  
    (*(INT32U *)OS_CPU_ARM_EXCEPT_RESET_VECT_ADDR)             =         OS_CPU_ARM_INSTR_JUMP_TO_HANDLER;  
    (*(INT32U *)OS_CPU_ARM_EXCEPT_RESET_HANDLER_ADDR)          = (INT32U)OS_CPU_ARM_ExceptResetHndlr;  
*/   
   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_UNDEF_INSTR_VECT_ADDR)       =         OS_CPU_ARM_INSTR_JUMP_TO_HANDLER;   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_UNDEF_INSTR_HANDLER_ADDR)    = (INT32U)OS_CPU_ARM_ExceptUndefInstrHndlr;   
   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_SWI_VECT_ADDR)               =         OS_CPU_ARM_INSTR_JUMP_TO_HANDLER;   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_SWI_HANDLER_ADDR)            = (INT32U)OS_CPU_ARM_ExceptSwiHndlr;   
   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_PREFETCH_ABORT_VECT_ADDR)    =         OS_CPU_ARM_INSTR_JUMP_TO_HANDLER;   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_PREFETCH_ABORT_HANDLER_ADDR) = (INT32U)OS_CPU_ARM_ExceptPrefetchAbortHndlr;   
   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_DATA_ABORT_VECT_ADDR)        =         OS_CPU_ARM_INSTR_JUMP_TO_HANDLER;   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_DATA_ABORT_HANDLER_ADDR)     = (INT32U)OS_CPU_ARM_ExceptDataAbortHndlr;   
   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_ADDR_ABORT_VECT_ADDR)        =         OS_CPU_ARM_INSTR_JUMP_TO_HANDLER;   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_ADDR_ABORT_HANDLER_ADDR)     = (INT32U)OS_CPU_ARM_ExceptAddrAbortHndlr;   
   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_IRQ_VECT_ADDR)               =         OS_CPU_ARM_INSTR_JUMP_TO_HANDLER;   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_IRQ_HANDLER_ADDR)            = (INT32U)OS_CPU_ARM_ExceptIrqHndlr;   
   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_FIQ_VECT_ADDR)               =         OS_CPU_ARM_INSTR_JUMP_TO_HANDLER;   
    (*(INT32U *)OS_CPU_ARM_EXCEPT_FIQ_HANDLER_ADDR)            = (INT32U)OS_CPU_ARM_ExceptFiqHndlr;   
}   

/*  
*********************************************************************************************************  
*                                     EXCEPTION HANDLE  
*  
* Description : .  
*  
* Arguments   : None.  
*********************************************************************************************************  
*/   

void       OS_CPU_ExceptHndlr(INT32U  except_type)
{

}

#ifdef USE_CORTEX_M3

void       OS_CPU_ARM_ExceptResetHndlr(void)
{}
void       OS_CPU_ARM_ExceptUndefInstrHndlr(void)
{}
void       OS_CPU_ARM_ExceptSwiHndlr(void)
{}
void       OS_CPU_ARM_ExceptPrefetchAbortHndlr(void)
{}
void       OS_CPU_ARM_ExceptDataAbortHndlr(void)
{}
void       OS_CPU_ARM_ExceptAddrAbortHndlr(void)
{}
void       OS_CPU_ARM_ExceptIrqHndlr(void)
{}
void       OS_CPU_ARM_ExceptFiqHndlr(void)
{}

#endif
