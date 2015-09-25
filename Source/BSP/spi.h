#ifndef _SPI_H
#define _SPI_H


#define MRST_SPI_TIMEOUT        0x200000
#define MRST_REGBASE_SPI0        0xff128000
#define MRST_REGBASE_SPI1        0xff128400
#define MRST_CLK_SPI0_REG        0xff11d86c

/* Bit fields in CTRLR0 */
#define SPI_DFS_OFFSET            0

#define SPI_FRF_OFFSET            4
#define SPI_FRF_SPI            0x0
#define SPI_FRF_SSP            0x1
#define SPI_FRF_MICROWIRE        0x2
#define SPI_FRF_RESV            0x3

#define SPI_MODE_OFFSET            6
#define SPI_SCPH_OFFSET            6
#define SPI_SCOL_OFFSET            7
#define SPI_TMOD_OFFSET            8
#define    SPI_TMOD_TR            0x0        /* xmit & recv */
#define SPI_TMOD_TO            0x1        /* xmit only */
#define SPI_TMOD_RO            0x2        /* recv only */
#define SPI_TMOD_EPROMREAD        0x3        /* eeprom read mode */

#define SPI_SLVOE_OFFSET        10
#define SPI_SRL_OFFSET            11
#define SPI_CFS_OFFSET            12
#define SPI_SLV_OE            0x1
/* Bit fields in SR, 7 bits */
#define SR_MASK                0x7f        /* cover 7 bits */
#define SR_BUSY                (1 << 0)
#define SR_TF_NOT_FULL            (1 << 1)
#define SR_TF_EMPT            (1 << 2)
#define SR_RF_NOT_EMPT            (1 << 3)
#define SR_RF_FULL            (1 << 4)
#define SR_TX_ERR            (1 << 5)
#define SR_DCOL                (1 << 6)


#define SPI_CLK_DIV     2  // Fsclk_out = Fssi_clk/SCKD, Fssi_clk (APB CLK 40Mhz)
#define SPI_MODE_0     0
#define SPI_MODE_3     3

#define SPI_FRM_SIZE  8


typedef PACKED struct _SPI_REGS {
    VUINT32    CTRLR0;      // 0x00
    VUINT32    CTRLR1;      // 0x04
    VUINT32    SSIENR;    // 0x08
    VUINT32    MWCR;     // 0x0C
    VUINT32    SER;         // 0x10
    VUINT32    BAUDR;     // 0x14
    VUINT32    TXFTLR;   // 0x18 TX FIFO threshold level
    VUINT32    RXFTLR;   // 0x1C RX FIFO threshold level
    VUINT32    SPI_TXFLR; //0X20
    VUINT32    SPI_RXFLR; // 0X24
    VUINT32    SR; // 0X28 status reg
    VUINT32    IMR;  // 0x2C
    VUINT32    ISR;  // 0x30
    VUINT32    RISR;  // 0x34
    VUINT32    TXOICR;  // 0x38
    VUINT32    RXOICR;  // 0x3C
    VUINT32    RXUICR;  // 0x40
    VUINT32    MSTICR;  // 0x44
    VUINT32    ICR;       // 0x48
    VUINT32    DMACR;  // 0x4C
    VUINT32    DMATDLR;  // 0x50
    VUINT32    DMARDLR;  // 0x54
    VUINT32    IDR;          // 0x58
    VUINT32    VERSION;   // 0x5C

    /* Currently operates as 32 bits, though only the low 16 bits matter */
    VUINT32    DR;      // 0x60
}  SPI_REGS;

#ifndef EXT
  #define EXT extern
#endif /* EXT */

/*------------------------ Non Debug Mode ------------------------------------*/
#ifndef  DEBUG
#define NST_SPI  ((SPI_REGS *) _SPI_BASE_ADR)
#else
/*------------------------ Debug Mode ----------------------------------------*/
EXT SPI_REGS  *NST_SPI;
#endif


#define NST_WR_SPI_REG(Value, Adr) \
    {\
        (*(volatile UINT32 *)(_SPI_BASE_ADR + Adr)) = (UINT32)(Value);\
    }

#define NST_RD_SPI_REG(Adr)  (*(volatile UINT32 *)(_SPI_BASE_ADR + Adr))

VOID BSP_SpiWait(VOID);
VOID BSP_SpiInit(VOID);
VOID  BSP_SpiWriteByte(UINT8 Byte);
VOID BSP_SpiRead(UINT32 RdCnt, UINT8* pBuf);

#ifdef SPI_SDIO_CMD_TEST
VOID  BSP_SpiEum(VOID);
VOID BSP_SpiCmd53Read(UINT16 bytecnt,UINT8* pBuf);
VOID BSP_SpiCmd53Write(UINT16 bytecnt,UINT8* pBuf);
VOID BSP_SpiSdioCmdTest(VOID);
#endif

#endif

