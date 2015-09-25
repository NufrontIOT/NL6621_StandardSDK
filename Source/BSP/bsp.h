/*
******************************************************************************
**
** Project     : 
** Description :    BSP.h
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/

#ifndef _BSP_H
#define _BSP_H

#include "reg.h"
#include "i2c.h"
#include "i2s.h"
#include "flash.h"
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include "dma.h"
#include "sdio.h"
#include "nvic.h"
#include "spi.h"

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

//*******************************************************************************************************
VOID  BSP_ClkInit (VOID);
VOID  BSP_Init(VOID);
VOID BSP_WakeupCpuIntISR(VOID);
VOID BSP_EfuseRead(UINT8 Offset, UINT8 Cnt, PUINT32 pEfuseInfo);
VOID BSP_ChipReset(VOID);
LARGE_INTEGER BSP_GetLaunchTicks(VOID);// get system launch ticks (1 tick time = 1/32.768KHz)

#define NST_WR_PHY_REG(Offset, Value)   /*0x40002000*/\
    {\
        (*(volatile UINT32 *)(_PHY_APB_BASE_ADR + Offset)) = (UINT32)(Value);\
    }

#define NST_RD_PHY_REG(Offset)  (*(volatile UINT32 *)(_PHY_APB_BASE_ADR + Offset))


#define NST_WR_IQ_CAL_REG(Offset, Value)   /*0x40006000*/\
    {\
        (*(volatile UINT32 *)(_IQ_CAL_BASE_ADR + Offset)) = (UINT32)(Value);\
    }

#define NST_RD_IQ_CAL_REG(Offset)  (*(volatile UINT32 *)(_IQ_CAL_BASE_ADR + Offset))

#define NST_WR_IQ_CAL_REG8(Offset, Value)   /*0x40006000*/\
    {\
        (*(volatile UINT8 *)(_IQ_CAL_BASE_ADR + Offset)) = (UINT8)(Value);\
    }

#define NST_RD_IQ_CAL_REG8(Offset)  (*(volatile UINT8 *)(_IQ_CAL_BASE_ADR + Offset))


extern UINT32 CpuClkFreq;
extern UINT32 ApbClkFreq;

#ifdef ADD_BSP_COUNTER
#define STATS_INC_BSP_CNT(pCounter)  *pCounter = *pCounter + 1;
#else
#define STATS_INC_BSP_CNT(pCounter)
#endif


/*
  *   NV INFO
  */
#define CHIP_ID_LEN     7
#define MAX_TX_PWR_LEN  9
#define ARRAY_SIZE_SP_B         3  
#define ARRAY_SIZE_LP_B         4
#define ARRAY_SIZE_G            8
#define ARRAY_SIZE_N            8
#define MAX_TXPOWER_ARRAY_SIZE  (ARRAY_SIZE_SP_B+ARRAY_SIZE_LP_B+ARRAY_SIZE_G+ARRAY_SIZE_N) // 3+4+8+8=23
#define DEVICE_TYPE_LEN 4
#define MAX_CH_NUM      14
#define MAC_ADDR_LEN    6 

typedef PACKED union   _ASIC_VER_ID_STRUC  {
    PACKED struct  {
        UINT8  ASICVer:4;     // 1000:6681, 0010:6621
        // 表征工艺信息（若工艺对射频影响过大，可以据此靠固件识别来调整RF_SPI）
        UINT8  Corner:4;      // 0000:TT, 0001:FF, 0010:FS, 0011:SF, 0100:SS
    }   field;
    UINT8  word;
}   ASIC_VER_ID_STRUC, *PASIC_VER_ID_STRUC;

typedef PACKED union _NIC_CINFIG1_STRUC {
    PACKED struct {
        UINT8      HardwareRadioControl:1;  // 1:enable, 0:disable
        UINT8      Rsv1:1;
        UINT8      ExternalLNA:1;           // external LNA enable for 2.4G
        UINT8      Rsv2:1;
        UINT8      PrivateDebug:1;          // 0:Debug PROHIBITTED
        UINT8      Efuse:1;                 // 0:flash, 1:efuse
        UINT8      Rsv3:2;
    }   field;
    UINT8          word;
}   NIC_CONFIG1_STRUC, *PNIC_CONFIG1_STRUC;

// 机台测试负责写ChipID和6681，其余的模组厂测负责写
typedef PACKED struct _NV_INFO     
{
    // 0 ~ 6: Year, Month, Day, Hour, Minute, Second, Site
    // 譬如<55:0>=0000 0011 - 0101 0111 - 0101 1001 - 0001 0011 - 0010 0001 - 0001 0001 - 0001 0011
    //            Site3    -   57秒    -   59分    -   13时    -   21日    -   11月    -   13年 
    UINT8  ChipId[CHIP_ID_LEN];

    // 7 ~ 12
    UINT8  MacAddr[MAC_ADDR_LEN];

    // 13 ~ 16
    UINT16  VendorID;
    UINT16  CardID;

    // 17 ~ 27
    UINT8  MaxTxPwrForEfuse[11];

    // 28
    ASIC_VER_ID_STRUC  AsicVerID;

    // 29
    NIC_CONFIG1_STRUC  NicConfig1;

    // 30 ~ 31
#ifdef NST_BIG_ENDIAN
    UINT16  VrefCtrlLdo2:3;
    UINT16  PllCal:13;
#else
    UINT16  PllCal:13;
    UINT16  VrefCtrlLdo2:3;
#endif /* !NST_BIG_ENDIAN */

    //
    // Efuse 32 Bytes, is fixed , do not change it !!!
    // 

    UINT8  TXGainLeve;
    UINT8  DeviceType[DEVICE_TYPE_LEN];
    UINT8  DeviceHwVer;
    UINT8  FwVersion;
    UINT8  ExtPaRamUpTime;

    // 40 ~ 361
    UINT8  MaxTxPwr[MAX_TXPOWER_ARRAY_SIZE][MAX_CH_NUM];
    
    UINT16  CheckSum;
} NV_INFO, *PNV_INFO;

extern NV_INFO EeBuffer;
extern const UINT8 DefaultTxPwrIdxTbl[MAX_TXPOWER_ARRAY_SIZE][MAX_CH_NUM]; 
extern UINT8   PermanentAddress[MAC_ADDR_LEN];

// firware source selection
#define SDIO_DN_BOOT           0  // SDIO/SPI boot
#define I2C_E2PROM_BOOT     1  // I2C boot
#define SPI_FLASH_BOOT         2  // SPI master
#define UART_BOOT                 3

#define QSPI_CLK_625KHZ   0x300        
#define QSPI_CLK_1250KHZ  0x180         
#define QSPI_CLK_2500KHZ  0x0C0         
#define QSPI_CLK_5000KHZ  0x060         
#define QSPI_CLK_10MHZ    0x030    
#define QSPI_CLK_20MHZ    0x018    
#define QSPI_CLK_40MHZ    0x00C    
#define QSPI_CLK_80MHZ    0x006    

#define QSPI_CLK_937KHZ   0x200        
#define QSPI_CLK_1875KHZ  0x100         
#define QSPI_CLK_3750KHZ  0x080         
#define QSPI_CLK_7500KHZ  0x040         
#define QSPI_CLK_15MHZ    0x020    
#define QSPI_CLK_30MHZ    0x010    
#define QSPI_CLK_60MHZ    0x008    
#define QSPI_CLK_120MHZ   0x004           

#define DEFAULT_QSPI_CLK             QSPI_CLK_20MHZ//QSPI_CLK_1250KHZ


// NULINK2 UART支持的baudrate (分频比8bit，)
#define UART_BAUDRATE_110            110
#define UART_BAUDRATE_300            300
#define UART_BAUDRATE_1200          1200
#define UART_BAUDRATE_2400          2400
#define UART_BAUDRATE_4800          4800
#define UART_BAUDRATE_9600          9600
#define UART_BAUDRATE_19200        19200
#define UART_BAUDRATE_38400        38400
#define UART_BAUDRATE_57600        57600
#define UART_BAUDRATE_115200      115200
#define UART_BAUDRATE_156250      156250
#define UART_BAUDRATE_250000      250000
#define UART_BAUDRATE_312500      312500
#define UART_BAUDRATE_500000      500000
#define UART_BAUDRATE_625000      625000
#define UART_BAUDRATE_1250000      1250000
#define UART_BAUDRATE_2500000      2500000

#define DEFAULT_UART_BAUDRATE    UART_BAUDRATE_115200


// 该结构体的大小定义为4字节整数倍
typedef PACKED struct _FW_HDR
{
    UINT8   StartFlag[8]; // Nu_link
    UINT32 FwSize; // total firmware image len, including fw hdr and tail
    UINT32 FwVerTime; /* Unix32 bit time (seconds since Jan 1 1970.).
                                    The version format is defined in [2]. It consists:
                                    o   Major: bit 27-31
                                    o   Minor: bit 20-16
                                    o   Patch: bit 12-19
                                    o   Build: bit 0-11
    */
    UINT32 AppEntryAdr;
    UINT16 CheckFlag;  //check flag, b1..0 , 0  not check 1: CRC, 2: SUM8
    UINT16 SdioBurstTransLen; // default 2K for SDIO, it must be multiple of 4 bytes
    UINT16 I2cSpeed; // 0: standard mode, 1: fast mode
    UINT16 SpimClkDiv; // SPI clk div, SPI_CLK=40MHZ/clk_div
    UINT32 UartBaudrate; // UART baudrate

    // bootloader升级时保持前面的不动，在后面追加字段
    // UINT8  Rsvd[];

#ifdef BURN_IMAGE
    UINT8  bBurnE2prom;
    UINT8  bBurnFlash;
#endif

} FW_HDR;

typedef PACKED struct _FW_TAIL
{
    UINT32 CheckWord; // CRC32/SUM32 value of the firmware payload.
    UINT8   EndFlag[8]; //  'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F'
}FW_TAIL;

#define CHECK_WORD_SIZE          (4)
#define FW_HDR_SIZE                  (sizeof(FW_HDR))
#define FW_TAIL_SIZE                  (sizeof(FW_TAIL))
#define MAX_FW_HDR_SIZE          0x100
#define SDIO_DMA_ALIGN    4   

/*
    download bin file image:
        | FW Header (32B) |rsvd (供将来bootloader升级用)|     FW data     |  FW Tail(12B) |
        |<--------------MAX_FW_HDR_SIZE(256B)----------->|
        |<-----------------------------------FW size--------------------------------->|
*/
#define APP_VEC_TABLE_BASE_ADR   NVIC_VectTab_ADR  // 0x00010100
// 中断向量表往前推256B为下载的首地址
#define DN_SRAM_START   (APP_VEC_TABLE_BASE_ADR - MAX_FW_HDR_SIZE) 
#define DN_SRAM_END       (_CODE_SRAM_BASE_ADR + CODE_SRAM_SIZE - 1)

#define FW_START_FLAG "Nu_link"
#define FW_MIN_SIZE     20
#define FW_MAX_SIZE     (DN_SRAM_END - DN_SRAM_START + 1)

#define FW_DNLDING    1

#define I2C_STANDARD_SPEED          0
#define I2C_FAST_SPEED                   1
#define I2C_HIGH_SPEED                   2
#define DEFAULT_I2C_SPEED             I2C_STANDARD_SPEED
#define EEPROM_BANK_MAX_NUM      (8)
#define BURN_SRC_ADR   DN_SRAM_START

#endif //_BSP_H
