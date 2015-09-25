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
* File      : OS_CPU.H 
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
 
#ifndef  OS_CPU_H 
#define  OS_CPU_H 
 
 
#ifdef   OS_CPU_GLOBALS 
#define  OS_CPU_EXT 
#else 
#define  OS_CPU_EXT  extern 
#endif 
 
#ifndef  OS_CPU_FPU_EN 
#define  OS_CPU_FPU_EN    0                      /* HW floating point support disabled by default      */ 
#endif 


// NST_ADD
#ifndef  OS_CPU_ARM_DCC_EN 
#define  OS_CPU_ARM_DCC_EN      0   
#endif 

/* 
********************************************************************************************************* 
*                                   INTERRUPT DISABLE TIME MEASUREMENT 
********************************************************************************************************* 
*/ 
 
#define  OS_CPU_INT_DIS_MEAS_EN    0 
 
 
/* 
********************************************************************************************************* 
*                                           EXCEPTION DEFINES 
********************************************************************************************************* 
*/ 
 
                                                                /* ARM exception IDs                                    */ 
#define  OS_CPU_ARM_EXCEPT_RESET                                                                    0x00 
#define  OS_CPU_ARM_EXCEPT_UNDEF_INSTR                                                              0x01 
#define  OS_CPU_ARM_EXCEPT_SWI                                                                      0x02 
#define  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT                                                           0x03 
#define  OS_CPU_ARM_EXCEPT_DATA_ABORT                                                               0x04 
#define  OS_CPU_ARM_EXCEPT_ADDR_ABORT                                                               0x05 
#define  OS_CPU_ARM_EXCEPT_IRQ                                                                      0x06 
#define  OS_CPU_ARM_EXCEPT_FIQ                                                                      0x07 
#define  OS_CPU_ARM_EXCEPT_MAX                                                                      0x08 
 
                                                                /* ARM exception vectors addresses                      */ 
#define  OS_CPU_ARM_EXCEPT_RESET_VECT_ADDR              (OS_CPU_ARM_EXCEPT_RESET          * 0x04 + 0x00) 
#define  OS_CPU_ARM_EXCEPT_UNDEF_INSTR_VECT_ADDR        (OS_CPU_ARM_EXCEPT_UNDEF_INSTR    * 0x04 + 0x00) 
#define  OS_CPU_ARM_EXCEPT_SWI_VECT_ADDR                (OS_CPU_ARM_EXCEPT_SWI            * 0x04 + 0x00) 
#define  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT_VECT_ADDR     (OS_CPU_ARM_EXCEPT_PREFETCH_ABORT * 0x04 + 0x00) 
#define  OS_CPU_ARM_EXCEPT_DATA_ABORT_VECT_ADDR         (OS_CPU_ARM_EXCEPT_DATA_ABORT     * 0x04 + 0x00) 
#define  OS_CPU_ARM_EXCEPT_ADDR_ABORT_VECT_ADDR         (OS_CPU_ARM_EXCEPT_ADDR_ABORT     * 0x04 + 0x00) 
#define  OS_CPU_ARM_EXCEPT_IRQ_VECT_ADDR                (OS_CPU_ARM_EXCEPT_IRQ            * 0x04 + 0x00) 
#define  OS_CPU_ARM_EXCEPT_FIQ_VECT_ADDR                (OS_CPU_ARM_EXCEPT_FIQ            * 0x04 + 0x00) 
 
                                                                /* ARM exception handlers addresses                     */ 
#define  OS_CPU_ARM_EXCEPT_RESET_HANDLER_ADDR           (OS_CPU_ARM_EXCEPT_RESET          * 0x04 + 0x20) 
#define  OS_CPU_ARM_EXCEPT_UNDEF_INSTR_HANDLER_ADDR     (OS_CPU_ARM_EXCEPT_UNDEF_INSTR    * 0x04 + 0x20) 
#define  OS_CPU_ARM_EXCEPT_SWI_HANDLER_ADDR             (OS_CPU_ARM_EXCEPT_SWI            * 0x04 + 0x20) 
#define  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT_HANDLER_ADDR  (OS_CPU_ARM_EXCEPT_PREFETCH_ABORT * 0x04 + 0x20) 
#define  OS_CPU_ARM_EXCEPT_DATA_ABORT_HANDLER_ADDR      (OS_CPU_ARM_EXCEPT_DATA_ABORT     * 0x04 + 0x20) 
#define  OS_CPU_ARM_EXCEPT_ADDR_ABORT_HANDLER_ADDR      (OS_CPU_ARM_EXCEPT_ADDR_ABORT     * 0x04 + 0x20) 
#define  OS_CPU_ARM_EXCEPT_IRQ_HANDLER_ADDR             (OS_CPU_ARM_EXCEPT_IRQ            * 0x04 + 0x20) 
#define  OS_CPU_ARM_EXCEPT_FIQ_HANDLER_ADDR             (OS_CPU_ARM_EXCEPT_FIQ            * 0x04 + 0x20) 
 
                                                                /* ARM "Jump To Self" assembled instruction             */ 
#define  OS_CPU_ARM_INSTR_JUMP_TO_SELF                   0xEAFFFFFE 
                                                                /* ARM "Jump To Exception Handler" assembled instruction*/ 
#define  OS_CPU_ARM_INSTR_JUMP_TO_HANDLER                0xE59FF018 
 
 
/* 
********************************************************************************************************* 
*                                              DATA TYPES 
*                                         (Compiler Specific) 
********************************************************************************************************* 
*/ 
 
typedef unsigned char  BOOLEAN; 
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */ 
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */ 
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */ 
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */ 
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */ 
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */ 
typedef float          FP32;                     /* Single precision floating point                    */ 
typedef double         FP64;                     /* Double precision floating point                    */ 
 
typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */ 
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */ 
 
/* 
********************************************************************************************************* 
*                                                ARM 
* 
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts 
*             will be enabled even if they were disabled before entering the critical section. 
*             NOT IMPLEMENTED 
* 
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if 
*             interrupts were disabled before entering the critical section, they will be disabled when 
*             leaving the critical section. 
*             NOT IMPLEMENTED 
* 
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you 
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then 
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to 
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr' 
*             into the CPU's status register. 
********************************************************************************************************* 
*/ 
 
#define  OS_CRITICAL_METHOD    3 
 
 
#if      OS_CRITICAL_METHOD == 3 
 
#if      OS_CPU_INT_DIS_MEAS_EN > 0 
 
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();  \
                               OS_CPU_IntDisMeasStart();} 
#define  OS_EXIT_CRITICAL()   {OS_CPU_IntDisMeasStop();   \
                               OS_CPU_SR_Restore(cpu_sr);} 
 
#else 
 
#define  OS_ENTER_CRITICAL()  {cpu_sr = gpRomHookFuncTbl->OS_CPU_SR_Save();} 
#define  OS_EXIT_CRITICAL()   {gpRomHookFuncTbl->OS_CPU_SR_Restore(cpu_sr);} 
 
#endif 
 
#endif 
 
/* 
********************************************************************************************************* 
*                                         ARM Miscellaneous 
********************************************************************************************************* 
*/ 
 
#define  OS_STK_GROWTH        1                   /* Stack grows from HIGH to LOW memory on ARM        */ 
 
#define  OS_TASK_SW()         gpRomHookFuncTbl->OSCtxSw() 
 
/* 
********************************************************************************************************* 
*                                            GLOBAL VARIABLES 
********************************************************************************************************* 
*/ 
 
                                                  /* Variables used to measure interrupt disable time  */ 
#if OS_CPU_INT_DIS_MEAS_EN > 0 
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasNestingCtr; 
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsEnter; 
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsExit; 
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsMax; 
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsDelta; 
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsOvrhd; 
#endif 
 
/* 
********************************************************************************************************* 
*                                              PROTOTYPES 
********************************************************************************************************* 
*/ 
 
#if OS_CRITICAL_METHOD == 3 
       OS_CPU_SR  OS_CPU_SR_Save(void);           /* See OS_CPU_A.ASM                                  */ 
       void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr); 
#endif 
 
       void       OS_CPU_SR_INT_Dis(void); 
       void       OS_CPU_SR_INT_En(void); 
       void       OS_CPU_SR_FIQ_Dis(void); 
       void       OS_CPU_SR_FIQ_En(void); 
       void       OS_CPU_SR_IRQ_Dis(void); 
       void       OS_CPU_SR_IRQ_En(void); 
 
#if OS_CPU_FPU_EN > 0 
       void       OS_CPU_FP_Init(void);           /* See OS_CPU_C.C                                    */ 
       void       OS_CPU_FP_Restore(void *pblk); 
       void       OS_CPU_FP_Save(void *pblk); 
#endif 
 
       void       OSCtxSw(void); 
       void       OSIntCtxSw(void); 
       void       OSStartHighRdy(void); 
 
       void       OS_CPU_InitExceptVect(void); 
 
       void       OS_CPU_ARM_ExceptResetHndlr(void); 
       void       OS_CPU_ARM_ExceptUndefInstrHndlr(void); 
       void       OS_CPU_ARM_ExceptSwiHndlr(void); 
       void       OS_CPU_ARM_ExceptPrefetchAbortHndlr(void); 
       void       OS_CPU_ARM_ExceptDataAbortHndlr(void); 
       void       OS_CPU_ARM_ExceptAddrAbortHndlr(void); 
       void       OS_CPU_ARM_ExceptIrqHndlr(void); 
       void       OS_CPU_ARM_ExceptFiqHndlr(void); 
 
      void       OS_CPU_ExceptHndlr(INT32U  except_type); 
 
#if OS_CPU_INT_DIS_MEAS_EN > 0 
       void       OS_CPU_IntDisMeasInit(void); 
       void       OS_CPU_IntDisMeasStart(void); 
       void       OS_CPU_IntDisMeasStop(void); 
       INT16U     OS_CPU_IntDisMeasTmrRd(void); 
#endif 
 
#if OS_CPU_ARM_DCC_EN > 0 
       void       OSDCC_Handler(void); 
#endif 

      void CPU_IntDis(void);    // NST_ADD
      void CPU_IntEn(void);    // NST_ADD
      void CPU_EnterLowPower(void);    // NST_ADD
      void WDResetWDTimer( void ); // NST_ADD
      void PrintCpuUsage(void); // NST_ADD
#endif 


