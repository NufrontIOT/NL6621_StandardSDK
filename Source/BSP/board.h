#ifndef _BOARD_H
#define _BOARD_H

//AHB bus space mapping
#define _ROM_BASE_ADR               0x00000000
#define ROM_SIZE                    0x10000 /*64KB*/
    
#define _CODE_SRAM_BASE_ADR        0x00010000
#define CODE_SRAM_SIZE               0x30000/*160KB+32KB(for customer)*/

// 把中断向量表放256字节边界且确保其之前的4个字节可以读写
#define NVIC_VectTab_ADR             (_CODE_SRAM_BASE_ADR + 0x100) // 0x00010100

/* ROM/Code/DATA SRAM image:
 ROM part:
    0x00000000-0x00001FFF     Bootloader
    0x00002000-0x0000FFFF     FW ROM RO-CODE/DATA
 CODE_SRAM part:
    0x00010000-0x000100FF     unused 
    0x00010100-0x000101FF     FW interrupt vector table 
    0x00010200-0x0003FFFF     FW CODE SRAM RO-CODE/DATA(including 32KB for customer)
 DATA_SRAM:
    0x00080000-0x000807FF     FW ROM hook function table
    0x00080800-0x00081FFF     FW ROM_CODE RW/ZI data
    0x00082000-0x00098000     FW SRAM_CODE RW/ZI data
*/

#define _DATA_SRAM_BASE_ADR    0x00080000
#define DATA_SRAM_SIZE               0x18000/*96KB*/

#define _QSPI_MEM_BASE_ADR               0x1000000
#define MAX_QSPI_FLASH_SIZE         0x1000000  

#define _BUF_SRAM0_BASE_ADR         0x20000000
#define BUF_SRAM0_SIZE              0x18000/*96KB*/ //0x40000/*256KB*/
    
#define _BUF_SRAM1_BASE_ADR         0x20040000
#define BUF_SRAM1_SIZE              0x10000/*64KB*/ //0x40000/*256KB*/

#define _MAC_AHB_SLAVE_BASE_ADR1     0x40080000 // for MAC regs
#define MAC_AHB_SLAVE_SIZE          0xF8//0x40000

#define _MAC_AHB_SLAVE_BASE_ADR2     0x400A0000  // for KEY mem
#define KEY_MEM_BASE_ADR     _MAC_AHB_SLAVE_BASE_ADR2  // for KEY mem
#define KEY_MEM_SIZE              0x2C4
    
#define _SDIO_AHB_SLAVE_BASE_ADR    0x400C0000
#define SDIO_AHB_SLAVE_SIZE         0x100//0x40000

#define _PWM_AHB_SLAVE_BASE_ADR    0x40100000
#define PWM_AHB_SLAVE_SIZE         0x28

#define _APB_BRG0_BASE_ADR          0x40000000
#define APB_BRG0_SIZE               0x40000
        
#define _APB_BRG1_BASE_ADR          0x40040000
#define APB_BRG1_SIZE               0x40000

//APB bus space mapping
// APB bridge 0
#define _SPI_BASE_ADR               0x40000000
#define SPI_SIZE                    0x400

#define _QSPI_BASE_ADR               0x40140000
#define QSPI_SIZE                    0x400

#define _GDMAC_BASE_ADR               0x40180000

#define _TMR_BASE_ADR               0x40001000
#define TMR_SIZE                    0x400

#define _PHY_APB_BASE_ADR           0x40002000
#define PHY_APB_SIZE                0x2000

#define _WDT0_BASE_ADR              0x40004000
#define WDT0_SIZE                   0x400

#define _I2C_BASE_ADR              0x40005000
#define I2C_SIZE                   0x400

#define _I2S_BASE_ADR              0x40043000
#define I2S_SIZE                   0x400

// APB bridge 1
#define _GPIO_BASE_ADR              0x40040000
#define GPIO_SIZE                   0x400

#define _UART_BASE_ADR              0x40041000
#define UART_SIZE                   0x400
#define _UART_STATUS_ADR            (_UART_BASE_ADR + 0x14)


// TODO
#define _IQ_CAL_BASE_ADR         0x40006000    // IQ calibration
#define IQ_CAL_SIZE                    0x400

#define _RF_SPI_BASE_ADR         0x40007000
#define RF_SPI_SIZE                    0x12

#define _EFUSE_BASE_ADR           0x40042000
#define EFUSE_SIZE                      0x20    // 256 bits

#define _RF_APB_BASE_ADR            0x40041000
#define RF_APB_SIZE                 0x1000

#define _AFE_BASE_ADR               0x40042000
#define AFE_SIZE                    0x1000



#define SRAM_BITBAND_ADR                0x20000000
#define SRAM_BITBAND_ALIAS_ADR     0x22000000

#define PERIPHA_BITBAND_ADR                0x40000000
#define PERIPHA_BITBAND_ALIAS_ADR     0x42000000

#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)  *((volatile unsigned long *)(addr))

//Systick为cpu的时钟,其他的为40MHz，总线时钟
#define CPU_CLK_FREQ_160M      160000000 
#define CPU_CLK_FREQ_120M        120000000 
#define CPU_CLK_FREQ_80M        80000000 
#define CPU_CLK_FREQ_40M        40000000 

#define APB_CLK_FREQ_40M      40000000 
#define APB_CLK_FREQ_20M      20000000 
#define APB_CLK_FREQ_10M      10000000 

/*!< Peripheral memory map */
#define TMR0_LOAD             (_TMR_BASE_ADR + 0x00) // Value to be loaded into Timer0
#define TMR0_VAL              (_TMR_BASE_ADR + 0x04) // Current Value of Timer0
#define TMR0_CTL              (_TMR_BASE_ADR + 0x08) // Control Register for Timer0
#define TMR0_EOI              (_TMR_BASE_ADR + 0x0C) // Clears the interrupt from Timer0
#define TMR0_INT_STATUS       (_TMR_BASE_ADR + 0x10) // Contains the interrupt status for Timer0


#define TMR1_LOAD             (_TMR_BASE_ADR + 0x14) // Value to be loaded into Timer1
#define TMR1_VAL              (_TMR_BASE_ADR + 0x18) // Current Value of Timer1
#define TMR1_CTL              (_TMR_BASE_ADR + 0x1C) // Control Register for Timer1
#define TMR1_EOI              (_TMR_BASE_ADR + 0x20) // Clears the interrupt from Timer1
#define TMR1_INT_STATUS       (_TMR_BASE_ADR + 0x24) // Contains the interrupt status for Timer1

#define TMRS_INT_STATUS        (_TMR_BASE_ADR + 0xA0) // Contains the interrupt status of all timers in the component
#define TMRS_EOI               (_TMR_BASE_ADR + 0xA4) // Returns all zeroes (0) and clears all active interrupts
#define TMRS_RAW_INT_STATUS    (_TMR_BASE_ADR + 0xA8) // Contains the unmasked interrupt status of all timers in the component.
#define TMRS_COMP_VER          (_TMR_BASE_ADR + 0xAC) // Current revision number of the DW_apb_timers component




/*[4:2]                Reset pulse length Writes have no effect
[1]                Response mode.Writes have no effect,Generate a system reset.
[0]                WDT enable.
This bit is used to enable and disable the DW_apb_wdt. When disabled, the counter does not decrement. Thus, no interrupts or system resets are generated. Once this bit has been enabled, it can be cleared only by a system reset.
0 = WDT disabled.
1 = WDT enabled.
*/
#define WDT0_CR                 (_WDT0_BASE_ADR + 0x00) 

/*
[7:4]                Timeout period for initialization. Used to select the timeout period that the watchdog counter restarts from for the first counter restart (kick). This register should be written after reset and before the WDT is enabled. A change of the TOP_INIT is seen only once the WDT has been enabled, and any change after the first kick is not seen as subsequent kicks use the period specified by the TOP bits. The range of values is limited by the WDT_CNT_WIDTH. If TOP_INIT is programmed to select a range that is greater than the counter width, the timeout period is truncated to fit to the counter width. This affects only the non-user specified values as users are limited to these boundaries during configuration.
The range of values available for a 32-bit watchdog counter are: 
Where i = TOP_INIT and
t = timeout period
For i = 0 to 15
if WDT_USE_FIX_TOP==1
t = 2(16 + i)
else
t = WDT_USER_TOP_INIT_(i)
[3:0]                Timeout period.
This field is used to select the timeout period from which the watchdog counter restarts. A change of the timeout period takes effect only after the next counter restart (kick). The range of values is limited by the WDT_CNT_WIDTH. If TOP is programmed to select a range that is greater than the counter width, the timeout period is truncated to fit to the counter width. This affects only the non-user specified values as users are limited to these boundaries during configuration.
The range of values available for a 32-bit watchdog counter are: 
Where i = TOP and
t = timeout period
For i = 0 to 15
if WDT_USE_FIX_TOP==1
t = 2(16 + i)
else
t = WDT_USER_TOP_(i)
*/
#define WDT0_TORR                       (_WDT0_BASE_ADR + 0x04) 

/*
This register, when read, is the current value of the internal counter. This value is read coherently when ever it is read, which is relevant when the APB_DATA_WIDTH is less than the counter
width*/
#define WDT0_CCVR                        (_WDT0_BASE_ADR + 0x08) 

/*This register is used to restart the WDT counter. As a safety feature to prevent accidental restarts, the value 0x76 must be written. A restart also clears the WDT interrupt. Reading this register returns zero
*/
#define WDT0_CRR                         (_WDT0_BASE_ADR + 0x0C) 

/*This register shows the interrupt status of the WDT.
1 = Interrupt is active regardless of polarity.
0 = Interrupt is inactive.
*/
#define WDT0_STAT                        (_WDT0_BASE_ADR + 0x10) 

/*Clears the watchdog interrupt. This can be used to clear the interrupt without restarting the watchdog counter.*/
#define WDT0_EOI                         (_WDT0_BASE_ADR + 0x14) 

/* Upper limit of Timeout Period parameters*/
#define WDT0_COMP_PARAMS_5               (_WDT0_BASE_ADR + 0xE4) 

/* Upper limit of Initial Timeout Period parameters*/
#define WDT0_COMP_PARAMS_4               (_WDT0_BASE_ADR + 0xE8) 

#define WDT0_COMP_PARAMS_3               (_WDT0_BASE_ADR + 0xEC) 
#define WDT0_COMP_PARAMS_2               (_WDT0_BASE_ADR + 0xF0) 
#define WDT0_COMP_PARAMS_1               (_WDT0_BASE_ADR + 0xF4) 

/* ASCII value for each number in the version, followed by *. For example
32_30_31_2A represents the version 2.01* */
#define WDT0_COMP_VERSION                (_WDT0_BASE_ADR + 0xF8) 
/*Designware Component Type number = 0x44_57_01_20. This assigned unique hex value is constant, and is derived from the two ASCII letters "DW" followed by a 16-bit unsigned number.
*/
#define WDT0_COMP_TYPE                   (_WDT0_BASE_ADR + 0xFC) 

/*Values written to this register are output on the I/O signals for GPIO Port if the corresponding data direction bits for Port A are set to Output mode and the corresponding control bit for Port A is set to Software mode. The value read back is equal to the last value written to this register.
*/
#define GPIO_SWPORTA_DR                (_GPIO_BASE_ADR + 0x00)
#define SWPORTA_DR                           (0x0)

/*  Values written to this register independently control the direction of the corresponding data bit in Port A. The default direction can be configured as input or output after system reset through the GPIO_DFLT_DIR_A parameter.
0 - Input (default)
1 - Output
*/
#define GPIO_SWPORTA_DDR               (_GPIO_BASE_ADR + 0x04) 
#define SWPORTA_DDR               (0x04) 

/*
Allows each bit of Port to be configured for interrupts. By default the generation of interrupts is disabled. Whenever a 1 is written to a bit of this register, it configures the corresponding bit on Port to become an interrupt; otherwise,Port operates as a normal GPIO signal. Interrupts are disabled on the corresponding bits of Port if the corresponding data direction register is set to Output or if Port A mode is set to Hardware.
0 - Configure Port A bit as normal GPIO signal (default)
1 - Configure Port A bit as interrupt
*/
#define GPIO_INT_ENA                   (_GPIO_BASE_ADR + 0x30) 
#define G_INT_ENA               (0x30) 

/* Controls whether an interrupt on Port can create an interrupt for the interrupt controller by not masking it. By default, all
interrupts bits are unmasked. Whenever a 1 is written to a bit in this register, it masks the interrupt generation capability for this signal; otherwise interrupts are allowed through. The unmasked status can be read as well as the resultant status after masking.
0 - Interrupt bits are unmasked (default)
1 - Mask interrupt
*/
#define GPIO_INT_MASK                   (_GPIO_BASE_ADR + 0x34) 
#define G_INT_MASK              (0x34) 

/*
Controls the type of interrupt that can occur on Port Whenever a 0 is written to a bit of this register, it configures the interrupt type to be level-sensitive; otherwise, it is edge-sensitive.
0 - Level-sensitive (default)
1 - Edge-sensitive
*/
#define GPIO_INT_TYPE_LEVEL             (_GPIO_BASE_ADR + 0x38)
#define G_INT_TYPE_LEVEL              (0x38) 

/*Controls the polarity of edge or level sensitivity that can occur on input of Port A. Whenever a 0 is written to a bit of this register, it configures the interrupt type to falling-edge or active-low sensitive; otherwise, it is rising-edge or active-high sensitive.
0 - Active-low (default)
1 - Active-high
*/
#define GPIO_INT_POLARITY              (_GPIO_BASE_ADR + 0x3C) 
#define G_INT_POLARITY             (0x3C) 

/* Interrupt status of GPIO port*/
#define GPIO_INT_STATUS                (_GPIO_BASE_ADR + 0x40) 
#define G_INT_STATUS             (0x40) 

/* Raw interrupt of status of GPIO port (premasking bits)*/
#define GPIO_RAW_INT_STATUS             (_GPIO_BASE_ADR + 0x44) 
#define RAW_INT_STATUS             (0x44) 

/* 
Controls the clearing of edge type interrupts from Port .When a 1 is written into a corresponding bit of this register, the interrupt is cleared. All interrupts are cleared when Port A is not configured for interrupts.
0 - No interrupt clear (default)
1 - Clear interrupt
*/
#define GPIO_PORTA_EOI             (_GPIO_BASE_ADR + 0x4C) 
#define PORTA_EOI             (0x4C) 

/* 
When Port  is configured as Input, then reading this location reads the values on the signal. When the data direction of Port  is set as Output, reading this location reads the data register for Port A.
Reset Value: 0x0 
*/
#define GPIO_EXT_PORTA             (_GPIO_BASE_ADR + 0x50) 
#define EXT_PORTA             (0x50) 

/* 
Writing a 1 to this register results in all level-sensitive interrupts being synchronized to pclk_intr.
0 - No synchronization to pclk_intr (default)
1 - Synchronize to pclk_intr
*/
#define GPIO_LS_SYNC             (_GPIO_BASE_ADR + 0x60) 

/* This is a user-specified code that a system can read. It can be used for chip identification, and so on. */
#define GPIO_ID_CODE             (_GPIO_BASE_ADR + 0x64) 

/*ASCII value for each number in the version, followed by *. For example 32_30_31_2A represents the version 2.01*  */
#define GPIO_VER_ID_CODE         (_GPIO_BASE_ADR + 0x6C) 


/* Registers */
#define Tmr0Load            (volatile int*)TMR0_LOAD
#define Tmr0Val             (volatile int*)TMR0_VAL
#define Tmr0Ctl             (volatile int*)TMR0_CTL
#define Tmr0Eoi             (volatile int*)TMR0_EOI
#define Tmr0IntStatus       (volatile int*)TMR0_INT_STATUS


#define Tmr1Load            (volatile int*)TMR1_LOAD
#define Tmr1Val             (volatile int*)TMR1_VAL
#define Tmr1Ctl             (volatile int*)TMR1_CTL
#define Tmr1Eoi             (volatile int*)TMR1_EOI
#define Tmr1IntStatus       (volatile int*)TMR1_INT_STATUS

#define TmrsIntStatus       (volatile int*)TMRS_INT_STATUS
#define TmrsEoi             (volatile int*)TMRS_EOI
#define TmrsRawIntStatus    (volatile int*)TMRS_RAW_INT_STATUS
#define TmrsCompVer         (volatile int*)TMRS_COMP_VER

#define Wdt0Cr                (volatile int*)WDT0_CR // 使能看门狗
#define Wdt0Torr              (volatile int*)WDT0_TORR  //  timeout period (TOP)for initialization
#define Wdt0Ccvr              (volatile int*)WDT0_CCVR  // RO, current value
#define Wdt0Crr               (volatile int*)WDT0_CRR  // 喂狗，往里写0x76
#define Wdt0Stat              (volatile int*)WDT0_STAT  // int status
#define Wdt0Eoi               (volatile int*)WDT0_EOI    // clear int
#define Wdt0CompParams5       (volatile int*)WDT0_COMP_PARAMS_5 // upper limit of TOP
#define Wdt0CompParams4       (volatile int*)WDT0_COMP_PARAMS_4 // upper limit of Initial TOP
#define Wdt0CompParams3       (volatile int*)WDT0_COMP_PARAMS_3
#define Wdt0CompParams2       (volatile int*)WDT0_COMP_PARAMS_2
#define Wdt0CompParams1       (volatile int*)WDT0_COMP_PARAMS_1
#define Wdt0CompVersion       (volatile int*)WDT0_COMP_VERSION
#define Wdt0CompType          (volatile int*)WDT0_COMP_TYPE

#endif //_BOARD_H
