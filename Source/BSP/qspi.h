#ifndef _QSPI_H
#define _QSPI_H


#define MRST_QSPI_TIMEOUT       0x200000
#define MRST_REGBASE_QSPI0      0xff128000
#define MRST_REGBASE_QSPI1      0xff128400
#define MRST_CLK_QSPI0_REG      0xff11d86c

/* Bit fields in CTRLR0 */
#define QSPI_DFS_OFFSET         0

#define QSPI_FRF_OFFSET         4  // frame format
#define QSPI_FRF_QSPI           0x0
#define QSPI_FRF_SSP            0x1
#define QSPI_FRF_MICROWIRE      0x2
#define QSPI_FRF_RESV           0x3

#define QSPI_MODE_OFFSET            6
#define QSPI_SCPH_OFFSET            6
#define QSPI_SCOL_OFFSET            7
#define QSPI_TMOD_OFFSET            8
#define QSPI_TMOD_TR            0x0     /* xmit & recv */
#define QSPI_TMOD_TO            0x1     /* xmit only */
#define QSPI_TMOD_RO            0x2     /* recv only */
#define QSPI_TMOD_EPROMREAD     0x3     /* eeprom read mode */

#define QSPI_SLVOE_OFFSET       10
#define QSPI_SRL_OFFSET         11
#define QSPI_CFS_OFFSET         12
#define QSPI_SLV_OE         0x1
/* Bit fields in SR, 7 bits */
#define QSPI_SR_MASK                0x7f        /* cover 7 bits */
#define QSPI_SR_BUSY                (1 << 0)
#define QSPI_SR_TF_NOT_FULL         (1 << 1)
#define QSPI_SR_TF_EMPT         (1 << 2)
#define QSPI_SR_RF_NOT_EMPT         (1 << 3)
#define QSPI_SR_RF_FULL         (1 << 4)
#define QSPI_SR_TX_ERR          (1 << 5)
#define QSPI_SR_DCOL                (1 << 6)

// 速率可以再调低点，偶数分频
#define QSPI_MODE_0     0
#define QSPI_MODE_3     3

#define QSPI_FRM_SIZE  8

#define QSPI_RD_MODE_OFFSET         16
#define QSPI_STD_RD_MODE                      0
#define QSPI_STD_FAST_RD_MODE             1
#define QSPI_DUAL_FAST_RD_MODE           2
#define QSPI_QUAD_FAST_RD_MODE           3
#define QSPI_DUAL_IO_FAST_RD_MODE      4
#define QSPI_QUAD_IO_FAST_RD_MODE     5
#define QSPI_QUAD_IO_WORD_RD_MODE   6
#define QSPI_OTHER_RD_MODE                  7

#define QSPI_RD_PREFETCH_OFFSET     19


typedef PACKED struct _QSPI_REGS {
    VUINT32 CTRLR0;      // 0x00
    VUINT32 CTRLR1;      // 0x04
    VUINT32 SSIENR;    // 0x08
    VUINT32 BAUDR;     // 0x0C
    VUINT32 TXFTLR;   // 0x10 TX FIFO threshold level
    VUINT32 RXFTLR;   // 0x14 RX FIFO threshold level
    VUINT32    QSPI_TXFLR; // 0x18
    VUINT32    QSPI_RXFLR; // 0x1C
    VUINT32 SR; // 0x20 status reg
    VUINT32 IMR;  // 0x24
    VUINT32 ISR;  // 0x28
    VUINT32 RISR;  // 0x2C
    VUINT32 TXOICR;  // 0x30
    VUINT32 RXOICR;  // 0x34
    VUINT32 RXUICR;  // 0x38
    VUINT32 AHBICR;  // 0x3C
    VUINT32 ICR;       // 0x40
    VUINT32 HOLD_WP;       // 0x44
    VUINT32 READ_CMD;       // 0x48
    VUINT32 PGM_CMD;       // 0x4C
    VUINT32 CACHE_FLUSH;       // 0x50
    VUINT32 CACHE_DIS_UPDATE;       // 0x54
    VUINT32    RSVD[20];  // 
    VUINT32 DR;      // 
}  QSPI_REGS;

#ifndef EXT
  #define EXT extern
#endif /* EXT */

/*------------------------ Non Debug Mode ------------------------------------*/
#ifndef  DEBUG
#define NST_QSPI  ((QSPI_REGS *) _QSPI_BASE_ADR)
#else
/*------------------------ Debug Mode ----------------------------------------*/
EXT QSPI_REGS  *NST_QSPI; // 0x40140000
#endif


#define NST_WR_QSPI_REG(Value, Adr) \
    {\
        (*(volatile UINT32 *)(_QSPI_BASE_ADR + Adr)) = (UINT32)(Value);\
    }

#define NST_RD_QSPI_REG(Adr)  (*(volatile UINT32 *)(_QSPI_BASE_ADR + Adr))

VOID BSP_QSpiWait(VOID);
VOID BSP_QSpiInit(UINT16 SpiClkDiv);
VOID  BSP_QSpiWriteByte(UINT8 Byte);
VOID BSP_QSpiRead(UINT32 RdCnt, UINT8* pBuf);
VOID BSP_QSpiDirectReadInit(UINT16 SpiClkDiv);
VOID BSP_QSpiDirectWriteInit(UINT16 SpiClkDiv);


#endif

