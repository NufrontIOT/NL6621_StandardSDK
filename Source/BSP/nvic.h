#ifndef _NUF_NVIC_H
#define _NUF_NVIC_H

#include "types_def.h"

/* System Control Space memory map */
#define SCS_BASE              ((UINT32)0xE000E000)

#define SysTick_BASE          (SCS_BASE + 0x0010)
#define NVIC_BASE             (SCS_BASE + 0x0100)
#define SCB_BASE              (SCS_BASE + 0x0D00)

#define DWT_CYCLE_CNT   (VUINT32*)(0xE0001004)

/*------------------------ Nested Vectored Interrupt Controller --------------*/
typedef struct
{
  VUINT32 ISER[2];
  UINT32  RESERVED0[30];
  VUINT32 ICER[2];
  UINT32  RSERVED1[30];
  VUINT32 ISPR[2];
  UINT32  RESERVED2[30];
  VUINT32 ICPR[2];
  UINT32  RESERVED3[30];
  VUINT32 IABR[2];
  UINT32  RESERVED4[62];
  VUINT32 IPR[15];
} NVIC_TypeDef;


typedef struct
{
    VUC32   CPUID; // 0xE000_ED00
    VUINT32 ICSR;  // 0xE000_ED04
    VUINT32 VTOR; // 0xE000_ED08
    VUINT32 AIRCR; // 0xE000_ED0C
    VUINT32 SCR;    // 0xE000_ED10
    VUINT32 CCR;    // 0xE000_ED14
    VUINT32 SHPR[3];  // 0xE000_ED18--ED23
    VUINT32 SHCSR;  // 0xE000_ED24
    VUINT32 CFSR;    // 0xE000_ED28  MFSR(8bit)/BFSR(8bit)/UFSR(16bit)
    VUINT32 HFSR;    // 0xE000_ED2C
    VUINT32 DFSR;    // 0xE000_ED30
    VUINT32 MMFAR; // 0xE000_ED34
    VUINT32 BFAR;    // 0xE000_ED38
    VUINT32 AFSR;    // 0xE000_ED3C
    VUINT32 RESERVED[44];    // 0xE000_ED40-EDEF
    VUINT32 DHCSR;  // 0xE000_EDF0
    VUINT32 DCRSR;  // 0xE000_EDF4
    VUINT32 DCRDR;  // 0xE000_EDF8
    VUINT32 DEMCR;  // 0xE000_EDFC
} SCB_TypeDef;



#ifndef EXT
  #define EXT extern
#endif /* EXT */

/*------------------------ Non Debug Mode ------------------------------------*/
#ifndef  DEBUG
#define NST_NVIC            ((NVIC_TypeDef *) NVIC_BASE)
#define NST_SCB                 ((SCB_TypeDef *) SCB_BASE)  
#else
/*------------------------ Debug Mode ----------------------------------------*/
EXT NVIC_TypeDef  *NST_NVIC;
EXT SCB_TypeDef  *NST_SCB;
#endif

/******************  Bit definition for SCB_CPUID register  *******************/
#define  SCB_CPUID_REVISION                  ((UINT32)0x0000000F)        /*!< Implementation defined revision number */
#define  SCB_CPUID_PARTNO                    ((UINT32)0x0000FFF0)        /*!< Number of processor within family */
#define  SCB_CPUID_Constant                  ((UINT32)0x000F0000)        /*!< Reads as 0x0F */
#define  SCB_CPUID_VARIANT                   ((UINT32)0x00F00000)        /*!< Implementation defined variant number */
#define  SCB_CPUID_IMPLEMENTER               ((UINT32)0xFF000000)        /*!< Implementer code. ARM is 0x41 */

/*******************  Bit definition for SCB_ICSR register  *******************/
#define  SCB_ICSR_VECTACTIVE                 ((UINT32)0x000001FF)        /*!< Active ISR number field */
#define  SCB_ICSR_RETTOBASE                  ((UINT32)0x00000800)        /*!< All active exceptions minus the IPSR_current_exception yields the empty set */
#define  SCB_ICSR_VECTPENDING                ((UINT32)0x003FF000)        /*!< Pending ISR number field */
#define  SCB_ICSR_ISRPENDING                 ((UINT32)0x00400000)        /*!< Interrupt pending flag */
#define  SCB_ICSR_ISRPREEMPT                 ((UINT32)0x00800000)        /*!< It indicates that a pending interrupt becomes active in the next running cycle */
#define  SCB_ICSR_PENDSTCLR                  ((UINT32)0x02000000)        /*!< Clear pending SysTick bit */
#define  SCB_ICSR_PENDSTSET                  ((UINT32)0x04000000)        /*!< Set pending SysTick bit */
#define  SCB_ICSR_PENDSVCLR                  ((UINT32)0x08000000)        /*!< Clear pending pendSV bit */
#define  SCB_ICSR_PENDSVSET                  ((UINT32)0x10000000)        /*!< Set pending pendSV bit */
#define  SCB_ICSR_NMIPENDSET                 ((UINT32)0x80000000)        /*!< Set pending NMI bit */

/*******************  Bit definition for SCB_VTOR register  *******************/
#define  SCB_VTOR_TBLOFF                     ((UINT32)0x1FFFFF80)        /*!< Vector table base offset field */
#define  SCB_VTOR_TBLBASE                    ((UINT32)0x20000000)        /*!< Table base in code(0) or RAM(1) */

/*!<*****************  Bit definition for SCB_AIRCR register  *******************/
#define  SCB_AIRCR_VECTRESET                 ((UINT32)0x00000001)        /*!< System Reset bit */
#define  SCB_AIRCR_VECTCLRACTIVE             ((UINT32)0x00000002)        /*!< Clear active vector bit */
#define  SCB_AIRCR_SYSRESETREQ               ((UINT32)0x00000004)        /*!< Requests chip control logic to generate a reset */

#define  SCB_AIRCR_PRIGROUP                  ((UINT32)0x00000700)        /*!< PRIGROUP[2:0] bits (Priority group) */
#define  SCB_AIRCR_PRIGROUP_0                ((UINT32)0x00000100)        /*!< Bit 0 */
#define  SCB_AIRCR_PRIGROUP_1                ((UINT32)0x00000200)        /*!< Bit 1 */
#define  SCB_AIRCR_PRIGROUP_2                ((UINT32)0x00000400)        /*!< Bit 2  */

/* prority group configuration */
#define  SCB_AIRCR_PRIGROUP0                 ((UINT32)0x00000000)        /*!< Priority group=0 (7 bits of pre-emption priority, 1 bit of subpriority) */
#define  SCB_AIRCR_PRIGROUP1                 ((UINT32)0x00000100)        /*!< Priority group=1 (6 bits of pre-emption priority, 2 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP2                 ((UINT32)0x00000200)        /*!< Priority group=2 (5 bits of pre-emption priority, 3 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP3                 ((UINT32)0x00000300)        /*!< Priority group=3 (4 bits of pre-emption priority, 4 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP4                 ((UINT32)0x00000400)        /*!< Priority group=4 (3 bits of pre-emption priority, 5 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP5                 ((UINT32)0x00000500)        /*!< Priority group=5 (2 bits of pre-emption priority, 6 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP6                 ((UINT32)0x00000600)        /*!< Priority group=6 (1 bit of pre-emption priority, 7 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP7                 ((UINT32)0x00000700)        /*!< Priority group=7 (no pre-emption priority, 8 bits of subpriority) */

#define  SCB_AIRCR_ENDIANESS                 ((UINT32)0x00008000)        /*!< Data endianness bit */
#define  SCB_AIRCR_VECTKEY                   ((UINT32)0xFFFF0000)        /*!< Register key (VECTKEY) - Reads as 0xFA05 (VECTKEYSTAT) */

/*******************  Bit definition for SCB_SCR register  ********************/
#define  SCB_SCR_SLEEPONEXIT                 ((UINT8)0x02)               /*!< Sleep on exit bit */
#define  SCB_SCR_SLEEPDEEP                   ((UINT8)0x04)               /*!< Sleep deep bit */
#define  SCB_SCR_SEVONPEND                   ((UINT8)0x10)               /*!< Wake up from WFE */

/********************  Bit definition for SCB_CCR register  *******************/
#define  SCB_CCR_NONBASETHRDENA              ((UINT16)0x0001)            /*!< Thread mode can be entered from any level in Handler mode by controlled return value */
#define  SCB_CCR_USERSETMPEND                ((UINT16)0x0002)            /*!< Enables user code to write the Software Trigger Interrupt register to trigger (pend) a Main exception */
#define  SCB_CCR_UNALIGN_TRP                 ((UINT16)0x0008)            /*!< Trap for unaligned access */
#define  SCB_CCR_DIV_0_TRP                   ((UINT16)0x0010)            /*!< Trap on Divide by 0 */
#define  SCB_CCR_BFHFNMIGN                   ((UINT16)0x0100)            /*!< Handlers running at priority -1 and -2 */
#define  SCB_CCR_STKALIGN                    ((UINT16)0x0200)            /*!< On exception entry, the SP used prior to the exception is adjusted to be 8-byte aligned */

/*******************  Bit definition for SCB_SHPR register ********************/
#define  SCB_SHPR_PRI_N                      ((UINT32)0x000000FF)        /*!< Priority of system handler 4,8, and 12. Mem Manage, reserved and Debug Monitor */
#define  SCB_SHPR_PRI_N1                     ((UINT32)0x0000FF00)        /*!< Priority of system handler 5,9, and 13. Bus Fault, reserved and reserved */
#define  SCB_SHPR_PRI_N2                     ((UINT32)0x00FF0000)        /*!< Priority of system handler 6,10, and 14. Usage Fault, reserved and PendSV */
#define  SCB_SHPR_PRI_N3                     ((UINT32)0xFF000000)        /*!< Priority of system handler 7,11, and 15. Reserved, SVCall and SysTick */

/******************  Bit definition for SCB_SHCSR register  *******************/
#define  SCB_SHCSR_MEMFAULTACT               ((UINT32)0x00000001)        /*!< MemManage is active */
#define  SCB_SHCSR_BUSFAULTACT               ((UINT32)0x00000002)        /*!< BusFault is active */
#define  SCB_SHCSR_USGFAULTACT               ((UINT32)0x00000008)        /*!< UsageFault is active */
#define  SCB_SHCSR_SVCALLACT                 ((UINT32)0x00000080)        /*!< SVCall is active */
#define  SCB_SHCSR_MONITORACT                ((UINT32)0x00000100)        /*!< Monitor is active */
#define  SCB_SHCSR_PENDSVACT                 ((UINT32)0x00000400)        /*!< PendSV is active */
#define  SCB_SHCSR_SYSTICKACT                ((UINT32)0x00000800)        /*!< SysTick is active */
#define  SCB_SHCSR_USGFAULTPENDED            ((UINT32)0x00001000)        /*!< Usage Fault is pended */
#define  SCB_SHCSR_MEMFAULTPENDED            ((UINT32)0x00002000)        /*!< MemManage is pended */
#define  SCB_SHCSR_BUSFAULTPENDED            ((UINT32)0x00004000)        /*!< Bus Fault is pended */
#define  SCB_SHCSR_SVCALLPENDED              ((UINT32)0x00008000)        /*!< SVCall is pended */
#define  SCB_SHCSR_MEMFAULTENA               ((UINT32)0x00010000)        /*!< MemManage enable */
#define  SCB_SHCSR_BUSFAULTENA               ((UINT32)0x00020000)        /*!< Bus Fault enable */
#define  SCB_SHCSR_USGFAULTENA               ((UINT32)0x00040000)        /*!< UsageFault enable */

/*******************  Bit definition for SCB_CFSR register  *******************/
/*!< MFSR */
#define  SCB_CFSR_IACCVIOL                   ((UINT32)0x00000001)        /*!< Instruction access violation */
#define  SCB_CFSR_DACCVIOL                   ((UINT32)0x00000002)        /*!< Data access violation */
#define  SCB_CFSR_MUNSTKERR                  ((UINT32)0x00000008)        /*!< Unstacking error */
#define  SCB_CFSR_MSTKERR                    ((UINT32)0x00000010)        /*!< Stacking error */
#define  SCB_CFSR_MMARVALID                  ((UINT32)0x00000080)        /*!< Memory Manage Address Register address valid flag */
/*!< BFSR */
#define  SCB_CFSR_IBUSERR                    ((UINT32)0x00000100)        /*!< Instruction bus error flag */
#define  SCB_CFSR_PRECISERR                  ((UINT32)0x00000200)        /*!< Precise data bus error */
#define  SCB_CFSR_IMPRECISERR                ((UINT32)0x00000400)        /*!< Imprecise data bus error */
#define  SCB_CFSR_UNSTKERR                   ((UINT32)0x00000800)        /*!< Unstacking error */
#define  SCB_CFSR_STKERR                     ((UINT32)0x00001000)        /*!< Stacking error */
#define  SCB_CFSR_BFARVALID                  ((UINT32)0x00008000)        /*!< Bus Fault Address Register address valid flag */
/*!< UFSR */
#define  SCB_CFSR_UNDEFINSTR                 ((UINT32)0x00010000)        /*!< The processor attempt to excecute an undefined instruction */
#define  SCB_CFSR_INVSTATE                   ((UINT32)0x00020000)        /*!< Invalid combination of EPSR and instruction */
#define  SCB_CFSR_INVPC                      ((UINT32)0x00040000)        /*!< Attempt to load EXC_RETURN into pc illegally */
#define  SCB_CFSR_NOCP                       ((UINT32)0x00080000)        /*!< Attempt to use a coprocessor instruction */
#define  SCB_CFSR_UNALIGNED                  ((UINT32)0x01000000)        /*!< Fault occurs when there is an attempt to make an unaligned memory access */
#define  SCB_CFSR_DIVBYZERO                  ((UINT32)0x02000000)        /*!< Fault occurs when SDIV or DIV instruction is used with a divisor of 0 */

/*******************  Bit definition for SCB_HFSR register  *******************/
#define  SCB_HFSR_VECTTBL                    ((UINT32)0x00000002)        /*!< Fault occures because of vector table read on exception processing */
#define  SCB_HFSR_FORCED                     ((UINT32)0x40000000)        /*!< Hard Fault activated when a configurable Fault was received and cannot activate */
#define  SCB_HFSR_DEBUGEVT                   ((UINT32)0x80000000)        /*!< Fault related to debug */

/*******************  Bit definition for SCB_DFSR register  *******************/
#define  SCB_DFSR_HALTED                     ((UINT8)0x01)               /*!< Halt request flag */
#define  SCB_DFSR_BKPT                       ((UINT8)0x02)               /*!< BKPT flag */
#define  SCB_DFSR_DWTTRAP                    ((UINT8)0x04)               /*!< Data Watchpoint and Trace (DWT) flag */
#define  SCB_DFSR_VCATCH                     ((UINT8)0x08)               /*!< Vector catch flag */
#define  SCB_DFSR_EXTERNAL                   ((UINT8)0x10)               /*!< External debug request flag */

/*******************  Bit definition for SCB_MMFAR register  ******************/
#define  SCB_MMFAR_ADDRESS                   ((UINT32)0xFFFFFFFF)        /*!< Mem Manage fault address field */

/*******************  Bit definition for SCB_BFAR register  *******************/
#define  SCB_BFAR_ADDRESS                    ((UINT32)0xFFFFFFFF)        /*!< Bus fault address field */

/*******************  Bit definition for SCB_afsr register  *******************/
#define  SCB_AFSR_IMPDEF                     ((UINT32)0xFFFFFFFF)        /*!< Implementation defined */

#define  SCB_DEMCR_HARDERR                     ((UINT32)1 << 10)       
#define  SCB_DEMCR_INTERR                     ((UINT32)1 << 9)       
#define  SCB_DEMCR_BUSERR                     ((UINT32)1 << 8)       
#define  SCB_DEMCR_STATERR                     ((UINT32)1 << 7)     
#define  SCB_DEMCR_CHKERR                     ((UINT32)1 << 6)     
#define  SCB_DEMCR_NOCPERR                     ((UINT32)1 << 5)     
#define  SCB_DEMCR_MMERR                     ((UINT32)1 << 4)     


typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {IRQ_DISABLE = 0, IRQ_ENABLE = !IRQ_DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == IRQ_DISABLE) || ((STATE) == IRQ_ENABLE))

/* NVIC Init Structure definition */
typedef struct
{
  UINT8 NVIC_IRQChannel;
  UINT8 NVIC_IRQChannelPreemptionPriority;
  UINT8 NVIC_IRQChannelSubPriority;
  FunctionalState NVIC_IRQChannelCmd;
} NVIC_InitTypeDef;

typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                             */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt              */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                      */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                    */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                   */

/******  STM32 specific Interrupt Numbers *********************************************************/
  WAKEUP_CPU_IRQn            = 0,      /*!< Wakeup CPU Interrupt                            */
  EXTI7_IRQn                 = 1,      /*!< Extern line 7 Interrupt            */
  EXTI6_IRQn                 = 2,      /*!< Extern line 6 Interrupt                                     */
  EXTI5_IRQn                 = 3,      /*!< Extern line 5 Interrupt                                 */
  EXTI4_IRQn                 = 4,      /*!< Extern line 4 Interrupt                               */
  EXTI3_IRQn                 = 5,      /*!< Extern line 3 Interrupt                                 */
  EXTI2_IRQn                 = 6,      /*!<Extern line 2 Interrupt                                 */
  EXTI1_IRQn                 = 7,      /*!< Extern Line1 Interrupt                                 */
  EXTI0_IRQn                 = 8,      /*!< Extern Line0 Interrupt                                 */
  TMR1_IRQn                  = 9,      /*!< timer1 Interrupt                                 */
  TMR0_IRQn                  = 10,     /*!< timer0 Interrupt                                 */
  UART_IRQn                  = 11,     /*!< uart Interrupt                      */
  WWDG0_IRQn              = 12,     /*!< watch dog0 Interrupt                      */
  SMID_IRQn                  = 13,     /*!< smid Interrupt                      */
  LOW_MAC_IRQn           = 14,      /*!< low mac Interrupt                      */
  I2C_IRQn                     = 15,
  SPI_IRQn                     = 16,
  IQ_DMA_IRQn                    = 17,
  EXTI15_IRQn                 = 18,      /*!< Extern line 15 Interrupt            */
  EXTI14_IRQn                 = 19,      /*!< Extern line 14 Interrupt                                     */
  EXTI13_IRQn                 = 20,      /*!< Extern line 13 Interrupt                                 */
  EXTI12_IRQn                 = 21,      /*!< Extern line 12 Interrupt                               */
  EXTI11_IRQn                 = 22,      /*!< Extern line 11 Interrupt                                 */
  EXTI10_IRQn                 = 23,      /*!<Extern line 10 Interrupt                                 */
  EXTI9_IRQn                 = 24,      /*!< Extern Line 9 Interrupt                                 */
  EXTI8_IRQn                 = 25,      /*!< Extern Line 8 Interrupt */           
  QSPI_IRQn                   =26,
  DMA_IRQn                    = 27,
  I2S_IRQn                    =28,
  EXTI16_31_IRQn                =29
} IRQn_Type;


typedef enum _INT_PRIO
{
     WAKEUP_CPU_IRQn_PRIO = 0,
     SMID_IRQn_PRIO,
     DMA_IRQn_PRIO,
     SYS_TICK_PRIO,
     LOW_MAC_IRQn_PRIO,
     WWDG0_IRQn_PRIO,
     TMR0_IRQn_PRIO,
     TMR1_IRQn_PRIO,
     UART_IRQn_PRIO,
     GPIO_IRQn_PRIO,
     I2S_IRQn_PRIO
} INT_PRIO;

/*
#define WAKEUP_CPU_IRQn_PRIO     0
#define SMID_IRQn_PRIO                  1 
#define LOW_MAC_IRQn_PRIO           2
#define WWDG0_IRQn_PRIO              3
#define TMR0_IRQn_PRIO                  4
#define TMR1_IRQn_PRIO                  5
*/

#define AIRCR_VECT_KEY        0x05FA0000
#define AIRCR_PRI_GROUP     4
#define VECRESET            1


VOID NVIC_StructInit(NVIC_InitTypeDef* NVIC_InitStruct);
VOID NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);
VOID NVIC_DeInit(VOID);
ITStatus NVIC_GetIRQChannelPendingBitStatus(UINT8 NVIC_IRQChannel);
VOID NVIC_SetIRQChannelPendingBit(UINT8 NVIC_IRQChannel);
VOID NVIC_ClearIRQChannelPendingBit(UINT8 NVIC_IRQChannel);
VOID NVIC_EnableIRQ(UINT8 NVIC_IRQChannel);
VOID NVIC_DisableIRQ(UINT8 NVIC_IRQChannel);
void NVIC_SetVectorTable(UINT32 NVIC_VectTab, UINT32 Offset);  

#endif //_NUF_NVIC_H


