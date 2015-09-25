
;***************************************************************************************
; Amount of memory (in bytes) allocated for Stack and Heap
; Tailor those values to your application needs          
;***************************************************************************************
Stack_Size   EQU     0x400
Heap_Size    EQU     0  ;0x200

;*******************************************************************************
; Allocate space for the Stack
;*******************************************************************************
  AREA    STACK, NOINIT, READWRITE, ALIGN=3

Stack
    SPACE   Stack_Size

;*******************************************************************************
; Allocate space for the Heap
;*******************************************************************************
  AREA    HEAP, NOINIT, READWRITE, ALIGN=3

Heap
    SPACE   Heap_Size

;********************************************************************************
;*  Declarations for the interrupt handlers that are used by the application.                                                                     
;********************************************************************************
          IMPORT  __main

            IMPORT  OSPendSV
            IMPORT  SysTickHandler
            IMPORT  NMIException       
            IMPORT  HardFaultException       
            IMPORT  MemManageException     
            IMPORT  BusFaultException   
            IMPORT  UsageFaultException       
            IMPORT  WakeupCpu_IRQHandler       ; cpu wakeup interrupt    IRQ0
            IMPORT  EXTI7_IRQHandler          ; ex_i_gpio_Intr7		IRQ1
            IMPORT  EXTI6_IRQHandler          ; ex_i_gpio_Intr6		IRQ2
            IMPORT  EXTI5_IRQHandler          ; ex_i_gpio_Intr5		IRQ3
            IMPORT  EXTI4_IRQHandler          ; ex_i_gpio_Intr4		IRQ4
            IMPORT  EXTI3_IRQHandler          ; ex_i_gpio_Intr3		IRQ5
            IMPORT  EXTI2_IRQHandler          ; ex_i_gpio_Intr2		IRQ6
            IMPORT  EXTI1_IRQHandler          ; ex_i_gpio_Intr1		IRQ7
            IMPORT  EXTI0_IRQHandler          ; ex_i_gpio_Intr0		IRQ8
            IMPORT  TMR1_IRQHandler           ; ex_i_timers_Intr1_irq		IRQ9
            IMPORT  TMR0_IRQHandler           ; ex_i_timers_Intr0_irq		IRQ10
            IMPORT  UART_IRQHandler           ; ex_i_uart_Intr_irq		IRQ11
            IMPORT  WWDG0_IRQHandler          ; ex_i_wdt_o_Intr_irq		IRQ12
            IMPORT  SMID_IRQHandler           ; smid_interrupt 			IRQ13
            IMPORT  LowMac_IRQHandler         ; low mac interrupt		IRQ14
            IMPORT     I2C_IRQHandler              ; I2C interrupt		IRQ15
            IMPORT     SPI_IRQHandler              ; SPI interrupt		IRQ16
            IMPORT     IQCalibration_IRQHandler ; IQ interrupt			IRQ17
            IMPORT     EXTI15_IRQHandler         ; ex_i_gpio_Intr15		IRQ18
            IMPORT     EXTI14_IRQHandler         ; ex_i_gpio_Intr14		IRQ19
            IMPORT     EXTI13_IRQHandler         ; ex_i_gpio_Intr13		IRQ20
            IMPORT     EXTI12_IRQHandler         ; ex_i_gpio_Intr12		IRQ21
            IMPORT     EXTI11_IRQHandler         ; ex_i_gpio_Intr11		IRQ22
            IMPORT     EXTI10_IRQHandler         ; ex_i_gpio_Intr10		IRQ23
            IMPORT     EXTI9_IRQHandler           ; ex_i_gpio_Intr9		IRQ24
            IMPORT     EXTI8_IRQHandler           ; ex_i_gpio_Intr8		IRQ25
            IMPORT     DUMMY0_IRQHandler       ; Reserved			IRQ26
            IMPORT     DMA_IRQHandler          ; DMA interrupt			IRQ27
            IMPORT     I2S_IRQHandler          ; I2S interrupt			IRQ28
            IMPORT     EXTI16_31_IRQnHandler    ; ex_i_gpio_Intr16-31	    IRQ29
            IMPORT     DUMMY1_IRQHandler       ; Reserved
            IMPORT     DUMMY2_IRQHandler       ; Reserved
            IMPORT     DUMMY3_IRQHandler       ; Reserved
            IMPORT     DUMMY4_IRQHandler       ; Reserved
            IMPORT     DUMMY5_IRQHandler       ; Reserved
            IMPORT     DUMMY6_IRQHandler       ; Reserved
            IMPORT     DUMMY7_IRQHandler       ; Reserved
            IMPORT     DUMMY8_IRQHandler       ; Reserved
            IMPORT     DUMMY9_IRQHandler       ; Reserved
            IMPORT     DUMMY10_IRQHandler     ; Reserved
            IMPORT     DUMMY11_IRQHandler     ; Reserved
            IMPORT     DUMMY12_IRQHandler     ; Reserved
            IMPORT     DUMMY13_IRQHandler     ; Reserved
            IMPORT     DUMMY14_IRQHandler     ; Reserved
            IMPORT     DUMMY15_IRQHandler     ; Reserved
  
          
        PRESERVE8

;**********************************************************************************
;*  Reset code section.                                                                                                           
;**********************************************************************************
        AREA    RESET, CODE, READONLY
        THUMB

;*******************************************************************************
; Fill-up the Vector Table entries with the exceptions ISR address
;*******************************************************************************
        EXPORT  __Vectors
__Vectors                      
        DCD  Stack + Stack_Size            ; Top of Stack
        DCD  Reset_Handler
        DCD  NMIException
        DCD  HardFaultException
        DCD  MemManageException
        DCD  BusFaultException
        DCD  UsageFaultException
        DCD  0                 ; Reserved
        DCD  0                 ; Reserved
        DCD  0                 ; Reserved
        DCD  0                 ; Reserved
        DCD  SVCHandler
        DCD  DebugMonitor
        DCD  0                 ; Reserved
        DCD  OSPendSV
        DCD  SysTickHandler
        ; External Interrupts
        DCD     WakeupCpu_IRQHandler       ; cpu wakeup interrupt			        IRQ0
        DCD     EXTI7_IRQHandler          ; ex_i_gpio_Intr7					IRQ1
        DCD     EXTI6_IRQHandler          ; ex_i_gpio_Intr6					IRQ2
        DCD     EXTI5_IRQHandler          ; ex_i_gpio_Intr5					IRQ3
        DCD     EXTI4_IRQHandler          ; ex_i_gpio_Intr4					IRQ4
        DCD     EXTI3_IRQHandler          ; ex_i_gpio_Intr3					IRQ5
        DCD     EXTI2_IRQHandler          ; ex_i_gpio_Intr2					IRQ6
        DCD     EXTI1_IRQHandler          ; ex_i_gpio_Intr1					IRQ7
        DCD     EXTI0_IRQHandler          ; ex_i_gpio_Intr0					IRQ8
        DCD     TMR1_IRQHandler           ; ex_i_timers_Intr1_irq				IRQ9
        DCD     TMR0_IRQHandler           ; ex_i_timers_Intr0_irq				IRQ10
        DCD     UART_IRQHandler           ; ex_i_uart_Intr_irq				        IRQ11
        DCD     WWDG0_IRQHandler       ; ex_i_wdt_o_Intr_irq				        IRQ12
        DCD     SMID_IRQHandler           ; smid_interrupt 					IRQ13
        DCD     LowMac_IRQHandler       ; low mac interrupt					IRQ14
        DCD     I2C_IRQHandler              ; I2C interrupt					IRQ15
        DCD     SPI_IRQHandler              ; SPI interrupt					IRQ16
        DCD     IQCalibration_IRQHandler ; IQ interrupt					        IRQ17
        DCD     EXTI15_IRQHandler         ; ex_i_gpio_Intr15					IRQ18
        DCD     EXTI14_IRQHandler         ; ex_i_gpio_Intr14					IRQ19
        DCD     EXTI13_IRQHandler         ; ex_i_gpio_Intr13					IRQ20
        DCD     EXTI12_IRQHandler         ; ex_i_gpio_Intr12					IRQ21
        DCD     EXTI11_IRQHandler         ; ex_i_gpio_Intr11					IRQ22
        DCD     EXTI10_IRQHandler         ; ex_i_gpio_Intr10					IRQ23
        DCD     EXTI9_IRQHandler          ; ex_i_gpio_Intr9					IRQ24
        DCD     EXTI8_IRQHandler          ; ex_i_gpio_Intr8					IRQ25
        DCD     DUMMY0_IRQHandler         ; Reserved						IRQ26
        DCD     DMA_IRQHandler            ; DMA interrupt					IRQ27
        DCD     I2S_IRQHandler            ; I2S interrupt					IRQ28
        DCD     EXTI16_31_IRQnHandler     ; ex_i_gpio_Intr 16-31			        IRQ29
        DCD     DUMMY1_IRQHandler       ; Reserved
        DCD     DUMMY2_IRQHandler       ; Reserved
        DCD     DUMMY3_IRQHandler       ; Reserved
        DCD     DUMMY4_IRQHandler       ; Reserved
        DCD     DUMMY5_IRQHandler       ; Reserved
        DCD     DUMMY6_IRQHandler       ; Reserved
        DCD     DUMMY7_IRQHandler       ; Reserved
        DCD     DUMMY8_IRQHandler       ; Reserved
        DCD     DUMMY9_IRQHandler       ; Reserved
        DCD     DUMMY10_IRQHandler     ; Reserved
        DCD     DUMMY11_IRQHandler     ; Reserved
        DCD     DUMMY12_IRQHandler     ; Reserved
        DCD     DUMMY13_IRQHandler     ; Reserved
        DCD     DUMMY14_IRQHandler     ; Reserved
        DCD     DUMMY15_IRQHandler     ; Reserved


;******************************************************************************************
;*  Reset entry
;******************************************************************************************
        EXPORT  Reset_Handler
Reset_Handler
        IMPORT  __main
        LDR     R0, =__main
        BX      R0


;******************************************************************************************
;*  NMI exception handler. 
;*  It simply enters an infinite loop.
;******************************************************************************************
;NMIException
;        B       NMIException


;******************************************************************************************
;*  Fault interrupt handler. 
;*  It simply enters an infinite loop.
;******************************************************************************************
;HardFaultException
;        B       HardFaultException

;******************************************************************************************
;*  MemManage interrupt handler. 
;*  It simply enters an infinite loop.
;******************************************************************************************
;MemManageException
;        B       MemManageException

;******************************************************************************************
;*  Bus Fault interrupt handler. 
;*  It simply enters an infinite loop.
;******************************************************************************************
;BusFaultException
;        B       BusFaultException

;******************************************************************************************
;*  UsageFault interrupt handler. 
;*  It simply enters an infinite loop.
;******************************************************************************************
;UsageFaultException
;        B       UsageFaultException

;******************************************************************************************
;*  DebugMonitor interrupt handler. 
;*  It simply enters an infinite loop.
;******************************************************************************************
DebugMonitor
        B       DebugMonitor

;******************************************************************************************
;*  SVCall interrupt handler. 
;*  It simply enters an infinite loop.
;******************************************************************************************
SVCHandler
        B       SVCHandler



;*******************************************************************************************
;*  Make sure the end of this section is aligned.
;*******************************************************************************************
        ALIGN


;********************************************************************************************
;*  Code section for initializing the heap and stack                                                                                                          
;********************************************************************************************
        AREA    |.text|, CODE, READONLY


;********************************************************************************************
;*  The function expected of the C library startup 
;*  code for defining the stack and heap memory locations. 
;********************************************************************************************
        IMPORT  __use_two_region_memory
        EXPORT  __user_initial_stackheap 
__user_initial_stackheap
        LDR     R0, =Heap
        LDR     R1, =(Stack + Stack_Size)
        LDR     R2, =(Heap + Heap_Size)
        LDR     R3, =Stack
        BX      LR

;******************************************************************************************
;*  Make sure the end of this section is aligned.
;******************************************************************************************
        ALIGN


;*******************************************************************************************
;*  End Of File                                                     
;*******************************************************************************************
        END
