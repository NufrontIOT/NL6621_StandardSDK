/*************************************************
 Copyright (C), 2000-2010, BEIJING NUFRONT CO., LTD
 File name: ...      
 Author: ...                   
 Version: ...      
 Date:  ...      
 Description: ...    
 Others: ...      
 Function List:     
     1. ...
 History: 
     1. Date: ...
       Author: ...
       Modification: ...
     2. ...
*************************************************/

#ifndef _REG_H_
#define _REG_H_

#define  _PWM_REG_BASE     (_PWM_AHB_SLAVE_BASE_ADR)
#define  _IQ_DMA_REG_BASE     (_IQ_CAL_BASE_ADR)
#define  _DMA_REG_BASE     (_GDMAC_BASE_ADR)

#define NST_WR_PWM_REG(Offset, Value)  /*0x40100000*/\
    {\
        (*(volatile UINT32 *)(_PWM_REG_BASE + Offset)) = (UINT32)Value;\
    }
#define NST_RD_PWM_REG(Offset)   (*(volatile UINT32 *)(_PWM_REG_BASE + Offset))


#define NST_WR_DMA_REG(Offset, Value)  /*0x40180000*/\
    {\
        (*(volatile UINT32 *)(_DMA_REG_BASE + Offset)) = (UINT32)Value;\
    }
#define NST_RD_DMA_REG(Offset)   (*(volatile UINT32 *)(_DMA_REG_BASE + Offset))


#define NST_WR_IQ_DMA_REG(Offset, Value)  /*0x40006000*/\
    {\
        (*(volatile UINT32 *)(_IQ_DMA_REG_BASE + Offset)) = (UINT32)Value;\
    }
#define NST_RD_IQ_DMA_REG(Offset)   (*(volatile UINT32 *)(_IQ_DMA_REG_BASE + Offset))

#define     ADDR_CHIP_MODE          0x1c    // WO
#define     ADDR_CLK_CTRL           0x20    // WR
#define     ADDR_LSB_TIME           0x24    // RO
#define     ADDR_MSB_TIME           0x28    // RO

#ifdef NULINK2_SOC
#define     ADDR_STRAPPING_PIN        0x2C  // RO
#define      STRIP_BOOT_MODE     (1)
/*
[3:1] strap_sel (gpio 5:3)
`B000, sdio/spi加载固件
`B001, i2c_eeprom加载固件
`B010, spi_flash加载固件
`B011，UART加载固件
`B100, JTAG debug模式下的remap
`B101, 从SPI_flash直接执行的remap。
`B110, EFUSE program mode（CHIP_MODE）
`B111, 保留
[0], no 32KHz Crystal Mode
为1，no 32KHz Crystal Mode
为0，32KHz Crystal Exists
*/

#define     ADDR_SPI_PIN_MUX_CTRL        0x30  // WR  default=0
#define     SPI_MUX_QSPI_PINS                  1
/*
Bit 0, 为0，QSPI管脚（QSPI_SCK，QSPI_CS，QSPI_DI, QSPI_DO）就是QSPI模块使用(即SPI模块和QSPI模块使用各自独立的引脚)；
为1，QSPI管脚（QSPI_SCK，QSPI_CS，QSPI_DI, QSPI_DO）就是SPI（sysnopsy）模块使用
即SPI模块使用QSPI模块的引脚，此时SPI模块的引脚不封装出来
*/

#define     ADDR_GPIO_PIN_MUX_CTRL         0x34  // WR default=32'h0004
/*
ENA_GPIO[31:0]
32个GPIO的管脚复用的GPIO使能信号
考虑到GPIO0-1(WAKEUP_DEV/WAKEUP_HOST)是没有复用的GPIO，所以本寄存器的0-1 bit没有任何实际控制效果；
其余bit为1控制相应的管脚为GPIO。
*/
#define     ADDR_WAKEUP_CPU_MASK    0x38    // WR default=2'b11
/*
[0], WAKEUP_CPU_MASK
为1，屏蔽掉wakeup_cpu_irq中断；
为0，打开 wakeup_cpu_irq中断
[1], SEELP_MASK
为1，送给PWM模块的SLEEPING/SLEEPDEEP信号被屏蔽为0；
为0，将M3送出的SLEEPING/SLEEPDEEP信号直接送给PWM模块
*/

#define     ADDR_REDUN_REG       0x3C    // WR
/*
[0], inverse_spi_clk_en
[1], inverse_adc_clk_en
[2], inverse_dac_clk_en
[3], wlan_soft_rst
写1，为复位wlan：MAC & BBP。复位完成后，自动清零。
[4]，当bit3写1后，本bit自动变为1，一直持续直到wlan完成复位后自动清零
*/

/*
GPIO与外设引脚复用情况(GPIO2外，其他引脚默认作为外设引脚，不作为GPIO功能):
GPIO编号    复用功能
0            WAKEUP_DEV特定应用，不复用
1            WAKEUP_HOST特定应用，不复用
2            PA_ON，默认为GPIO引脚
3            SD_CMD
4            SD_CLK
5            SD_DATA0
6            SD_DATA1
7            SD_DATA2
8            SD_DATA3
9            I2C_SCL
10         I2C_SDA
11         UART_RXD
12         UART_TXD
13         CO_BT_ACTIVE
14         CO_BT_FREQ_OVLAP
15         CO_BT_PRIORITY
16         CO_WLAN_ACTIVE
17         I2S_SCLK
18         I2S_SDATA
19         I2S_LCLK
20         I2S_MCLK
21         SPI_PIN_MUX_CTRL ? QSPI_CK : SPI_CK
22         SPI_PIN_MUX_CTRL ? QSPI_DO : SPI_DO
23         SPI_PIN_MUX_CTRL ? QSPI_DI : SPI_DI
24         SPI_PIN_MUX_CTRL ? QSPI_CS : SPI_CS
25         QSPI_WP
26         QSPI_HOLD
27         SPI_CK
28         SPI_DI
29         SPI_CS
30         SPI_DO
31         PWM0
*/
#define   SD_CMD_GPIO                         3
#define   SD_CLK_GPIO                         4
#define   SD_DATA0_GPIO                     5
#define   SD_DATA1_GPIO                     6
#define   SD_DATA2_GPIO                     7
#define   SD_DATA3_GPIO                     8

#define   ENA_SDIO_GPIO                  ((1 << SD_CMD_GPIO) | (1 << SD_CLK_GPIO)| (1 << SD_DATA0_GPIO)\
                                                        | (1 << SD_DATA1_GPIO)| (1 << SD_DATA2_GPIO)| (1 << SD_DATA3_GPIO))     


#define   I2C_SCL_GPIO                         9
#define   I2C_SDA_GPIO                         10
#define   ENA_I2C_GPIO                         ((1 << I2C_SCL_GPIO) | (1 << I2C_SDA_GPIO))     

#define   UART_RXD_GPIO                     11
#define   UART_TXD_GPIO                     12
#define   ENA_UART_GPIO                      ((1 << UART_RXD_GPIO) | (1 << UART_TXD_GPIO))     

#define   CO_BT_ACTIVE_GPIO              13
#define   CO_BT_FREQ_OVLAP_GPIO     14
#define   CO_BT_PRIORITY_GPIO          15
#define   CO_WLAN_ACTIVE_GPIO         16

#define   I2S_SCLK_GPIO                      17
#define   I2S_SDATA_GPIO                   18
#define   I2S_LCLK_GPIO                      19
#define   I2S_MCLK_GPIO                     20
#define   ENA_I2S_GPIO                      ((1 << I2S_SCLK_GPIO) | (1 << I2S_SDATA_GPIO)\
                                                            | (1 << I2S_LCLK_GPIO)| (1 << I2S_MCLK_GPIO))     


#define   QSPI_CK_GPIO                      21
#define   QSPI_DO_GPIO                     22
#define   QSPI_DI_GPIO                      23
#define   QSPI_CS_GPIO                     24
#define   QSPI_WP_GPIO                    25
#define   QSPI_HOLD_GPIO                 26
#define   ENA_QSPIM_GPIO                  ((1 << QSPI_CK_GPIO) | (1 << QSPI_DO_GPIO)| (1 << QSPI_DI_GPIO)\
                                                        | (1 << QSPI_CS_GPIO)| (1 << QSPI_WP_GPIO)| (1 << QSPI_HOLD_GPIO))     
#define   SPI_CK_GPIO                        27
#define   SPI_DI_GPIO                        28
#define   SPI_CS_GPIO                        29
#define   SPI_DO_GPIO                       30
#define   ENA_SPIM_GPIO                  ((1 << SPI_CK_GPIO) | (1 << SPI_DI_GPIO)| (1 << SPI_CS_GPIO)| (1 << SPI_DO_GPIO))     

#define   PWM0_GPIO                         31

#else // NULINK1
#define     ADDR_STRAPPING_PIN        0x2C  // RO
#define     ADDR_PWM_INT_MASK        0x30  // RO
#define     ADDR_PIN_MUX_CTRL         0x34  // WR
#define     ADDR_WAKEUP_CPU_MASK    0x38    // WR
#define     ADDR_UART_SPI_MUX       0x3C    // WR

#define      STRIP_BOOT_MODE     (2)

#define     SEL_UART_SPI                5

#define      ENA_UART_GPIO          (1 << 0)
#define      ENA_I2C_GPIO             (1 << 1)
#define      ENA_SPIM_GPIO           (1 << 2)

#endif


// chip mode reg
// [1:0] BandWidth
#define     CHIP_BW_MODE_20M    0x00   // 20M
#define     CHIP_BW_MODE_10M    0x01   // 10M
#define     CHIP_BW_MODE_5M     0x02   // 5M
#define     CHIP_BW_MODE_RSVD   0x03   // Reserved (5M)
// [2] BandWidth enable
#define     CHIP_BW_ENA                (0x1U << 2)
// [4:3] CPU_SEL
#define     CPU_CLK_SEL                  3
#define     CPU_CLK_MODE_160M   0x00    // 160M 
#define     CPU_CLK_MODE_120M   0x01    // 120M
#define     CPU_CLK_MODE_80M    0x02    // 80M
#define     CPU_CLK_MODE_40M    0x03    // 40M
// [5] CPU sel enable
#define     CPU_SEL_ENA                  (0x1U << 5)
// [6] SMID system reset done
 #define     CHIP_SMID_RST_DONE  (0x1U << 6)
// [31:7] Reserved

// clk control reg
// [1:0] APB_SEL
#define     APB_CLK_MODE_40M      0x00 
#define     APB_CLK_MODE_20M      0x01
#define     APB_CLK_MODE_10M      0x02
#define     APB_CLK_MODE_RSVD    0x03  // Reserved (10M)
// [2] WLAN_GATE_E
#define     WLAN_GATE_ENA            (0x1U << 2)
// [3] APB1_GATE_E
#define     APB1_GATE_ENA             (0x1U << 3)
// [4] APB2_GATE_E
#define     APB2_GATE_ENA             (0x1U << 4)
// [31:5] Reserved

/*----------------------------------------------------------------------------------------*/

//
// IQDMA regs
//
#define     ADDR_TX_CAL_CTL          0x00  // b2  RX cal ena, b1 TX cal ena
#define     ADDR_IQ_DMA_START           0x24    
#define     ADDR_IQ_DMA_CTL                0x28 
/*
[5:4] RW Rx wait time to capture RX data for IQ calibration
[1]   RW  Dma disable   1
[0]   RW  DMA_RX  1 for RX dma, 0 for TX dma
SRC->DST  TX mode
DST->SRC  RX mode
*/

#define     ADDR_IQ_DMA_CNT               0x2C 
#define     ADDR_IQ_DMA_SRC               0x30 
#define     ADDR_IQ_DMA_MODE            0x34 
#define     ADDR_IQ_DMA_INT_STAT      0x38 
/*
[4]  R   fft done
[3]  R   DMA err
[2]  R   rx_vrf, RX fifo overflow
[1]  R   tx empty
[0]  R   rf dma done*/
#define     ADDR_IQ_DMA_INT_ENA      0x3C 
#define     ADDR_IQ_DMA_INT_CLEAR      0x40 
#define     ADDR_IQ_DMA_DST      0x48 

#define  DMA_TX_MODE      0
#define  DMA_RX_MODE      1

/*----------------------------------------------------------------------------------------*/

//
// DMA regs
//

/*------------- Direct Memory Access (DMA) -----------------*/
/**Direct Memory Access (DMA) register structure definition */
#define DMAC_CH0_START_ADDR              ( 0x000)
#define DMAC_CH1_START_ADDR              ( 0x058)
#define DMAC_CH2_START_ADDR              ( 0x0b0)
#define DMAC_CH3_START_ADDR              ( 0x108)

#define DMAC_COMMON_START_ADDR           ( 0x2c0)

#define DMAC_SAR_OFFSET          0x000
#define DMAC_DAR_OFFSET          0x008
#define DMAC_LLP_OFFSET          0x010
#define DMAC_CTL_OFFSET          0x018
#define DMAC_SSTAT_OFFSET        0x020
#define DMAC_DSTAT_OFFSET        0x028
#define DMAC_SSTATAR_OFFSET      0x030
#define DMAC_DSTATAR_OFFSET      0x038
#define DMAC_CFG_OFFSET          0x040
#define DMAC_SGR_OFFSET          0x048
#define DMAC_DSR_OFFSET          0x050

#define DMAC_RAWTFR            (DMAC_COMMON_START_ADDR + 0x000)
#define DMAC_RAWBLOCK          (DMAC_COMMON_START_ADDR + 0x008)
#define DMAC_RAWSRCTRAN        (DMAC_COMMON_START_ADDR + 0x010)
#define DMAC_RAWDSTTRAN        (DMAC_COMMON_START_ADDR + 0x018)
#define DMAC_RAWERR            (DMAC_COMMON_START_ADDR + 0x020)
#define DMAC_STATUSTFR         (DMAC_COMMON_START_ADDR + 0x028)
#define DMAC_STATUSBLOCK       (DMAC_COMMON_START_ADDR + 0x030)
#define DMAC_STATUSSRCTRAN     (DMAC_COMMON_START_ADDR + 0x038)
#define DMAC_STATUSDSTTRAN     (DMAC_COMMON_START_ADDR + 0x040)
#define DMAC_STATUSERR         (DMAC_COMMON_START_ADDR + 0x048)
#define DMAC_MASKTFR           (DMAC_COMMON_START_ADDR + 0x050)
#define DMAC_MASKBLOCK         (DMAC_COMMON_START_ADDR + 0x058)
#define DMAC_MASKSRCTRAN       (DMAC_COMMON_START_ADDR + 0x060)
#define DMAC_MASKDSTTRAN       (DMAC_COMMON_START_ADDR + 0x068)
#define DMAC_MASKERR           (DMAC_COMMON_START_ADDR + 0x070)
#define DMAC_CLEARTFR          (DMAC_COMMON_START_ADDR + 0x078)
#define DMAC_CLEARBLOCK        (DMAC_COMMON_START_ADDR + 0x080)
#define DMAC_CLEARSRCTRAN      (DMAC_COMMON_START_ADDR + 0x088)
#define DMAC_CLEARDSTTRAN      (DMAC_COMMON_START_ADDR + 0x090)
#define DMAC_CLEARERR          (DMAC_COMMON_START_ADDR + 0x098)
#define DMAC_STATUSINT         (DMAC_COMMON_START_ADDR + 0x0a0)
#define DMAC_REQSRCREG         (DMAC_COMMON_START_ADDR + 0x0a8)
#define DMAC_REQDSTREG         (DMAC_COMMON_START_ADDR + 0x0b0)
#define DMAC_SGLRQSRCREG       (DMAC_COMMON_START_ADDR + 0x0b8)
#define DMAC_SGLRQDSTREG       (DMAC_COMMON_START_ADDR + 0x0c0)
#define DMAC_LSTSRCREG         (DMAC_COMMON_START_ADDR + 0x0c8)
#define DMAC_LSTDSTREG         (DMAC_COMMON_START_ADDR + 0x0d0)
#define DMAC_DMACFGREG         (DMAC_COMMON_START_ADDR + 0x0d8)
#define DMAC_CHENREG           (DMAC_COMMON_START_ADDR + 0x0e0)
#define DMAC_DMAIDREG          (DMAC_COMMON_START_ADDR + 0x0e8)
#define DMAC_DMATESTREG        (DMAC_COMMON_START_ADDR + 0x0f0)
#define DMAC_DMA_COMP_PARAM_6  (DMAC_COMMON_START_ADDR + 0x108)
#define DMAC_DMA_COMP_PARAM_5  (DMAC_COMMON_START_ADDR + 0x110)
#define DMAC_DMA_COMP_PARAM_4  (DMAC_COMMON_START_ADDR + 0x118)
#define DMAC_DMA_COMP_PARAM_3  (DMAC_COMMON_START_ADDR + 0x120)
#define DMAC_DMA_COMP_PARAM_2  (DMAC_COMMON_START_ADDR + 0x128)
#define DMAC_DMA_COMP_PARAM_1  (DMAC_COMMON_START_ADDR + 0x130)
#define DMAC_DMA_COMP_ID       (DMAC_COMMON_START_ADDR + 0x138)

/*DMA channel
   NL6621 supply four DMA channel, DMA channel 0-3 is available
*/
#define             DMA_CHANNEL_0                  (0)
#define             DMA_CHANNEL_1                  (1)
#define             DMA_CHANNEL_2                  (2)
#define             DMA_CHANNEL_3                  (3)

/*DMAC_DMACFGREG*/
#define             DMA_EN                  (1)

/*DMAC_CTLX*/
#define DMA_INTR_EN             (1<<0)
#define DMA_INTR_DIS            (0<<0)
/*
CTLx.SRC_TR_WIDTH / CTLx.DST_TR_WIDTH Size (bits)
000 8
001 16
010 32
011 64
100 128
101 256
11x 256
*/
#define DMA_DST_WIDTH_8             (0<<1)  
#define DMA_DST_WIDTH_16            (1<<1)  
#define DMA_DST_WIDTH_32            (2<<1)  
#define DMA_DST_WIDTH_64            (3<<1)  
#define DMA_DST_WIDTH_128           (4<<1)  
#define DMA_DST_WIDTH_256           (5<<1) 

#define DMA_SRC_WIDTH_8             (0<<4)  
#define DMA_SRC_WIDTH_16            (1<<4)  
#define DMA_SRC_WIDTH_32            (2<<4)  
#define DMA_SRC_WIDTH_64            (3<<4)  
#define DMA_SRC_WIDTH_128           (4<<4)  
#define DMA_SRC_WIDTH_256           (5<<4)  

#define DMA_DSTADDR_INC                (0<<7)
#define DMA_DSTADDR_DEC                (1<<7)
#define DMA_DSTADDR_NO_CHANGE          (2<<7)      

#define DMA_SRCADDR_INC                (0<<9)
#define DMA_SRCADDR_DEC                (1<<9)
#define DMA_SRCADDR_NO_CHANGE          (2<<9)

/*
CTLx.SRC_MSIZE /
CTLx.DEST_MSIZE
Number of data items to be transferred
(of width CTLx.SRC_TR_WIDTH or CTLx.DST_TR_WIDTH)
000 1
001 4
010 8
011 16
100 32
101 64
110 128
111 256
*/
#define DMA_DST_MSIZE_1             (0<<11)                             
#define DMA_DST_MSIZE_4             (1<<11)                             
#define DMA_DST_MSIZE_8             (2<<11)                             
#define DMA_DST_MSIZE_16            (3<<11)                             
#define DMA_DST_MSIZE_32            (4<<11)                             
#define DMA_DST_MSIZE_64            (5<<11)                             
#define DMA_DST_MSIZE_128           (6<<11)                             
#define DMA_DST_MSIZE_256           (7<<11) 

#define DMA_SRC_MSIZE_1             (0<<14)                             
#define DMA_SRC_MSIZE_4             (1<<14)                             
#define DMA_SRC_MSIZE_8             (2<<14)                             
#define DMA_SRC_MSIZE_16            (3<<14)                             
#define DMA_SRC_MSIZE_32            (4<<14)                             
#define DMA_SRC_MSIZE_64            (5<<14)                             
#define DMA_SRC_MSIZE_128           (6<<14)                             
#define DMA_SRC_MSIZE_256           (7<<14)  


#define DMA_SRC_GATHER_EN        (1<<17)//Source gather enable bit:
#define DMA_SRC_GATHER_DIS       (0<<17)
#define DMA_DST_SCATTER_EN       (1<<18)//Destination scatter enable bit:
#define DMA_DST_SCATTER_DIS      (1<<18)

//DMA_TT_FC :20:22
/*
000 Memory to Memory DW_ahb_dmac
001 Memory to Peripheral DW_ahb_dmac
010 Peripheral to Memory DW_ahb_dmac
011 Peripheral to Peripheral DW_ahb_dmac
100 Peripheral to Memory Peripheral
101 Peripheral to Peripheral Source Peripheral
110 Memory to Peripheral Peripheral
111 Peripheral to Peripheral Destination Peripheral
*/
#define DMA_MEM2MEM                        (0<<20)
#define DMA_MEM2PER                        (1<<20)
#define DMA_PER2MEM                        (2<<20)
#define DMA_PER2PER                        (3<<20)

//23:24//Destination Master Select.
//25:26 //Source Master Select.

#define DMA_LLP_DST_EN                 (1<<27)//Block chaining is enabled on the destination side the LLP_DST_EN field is high
#define DMA_LLP_DST_DIS                (0<<27) 
#define DMA_LLP_SRC_EN                 (1<<28)//Block chaining is enabled on the source side the LLP_SRC_EN field is high
#define DMA_LLP_SRC_DIS                (0<<28)
//32-43bit      block transfer size
#define             LLI_CTL_DONE                  (1<<(44-32))

#define DMA_TRANSFER_SIZE 127
#define DMA_TRANSFER_SIZE_IN_BYTE   DMA_TRANSFER_SIZE*4

// link list
typedef struct {
  int SAR;       // src addr
  int DAR;       // dest addr
  int LLP;       // link list pointer
  int CTLl;      // control [31:0]
  int CTLh;      // control [63:32]
  int SSTAT;     // src status
  int DSTAT;     // dest status
} LLI;

#define xxxxxxxxxxxxxxxxxxxx
/*----------------------------------------------------------------------------------------*/

#endif  //_REG_H_
