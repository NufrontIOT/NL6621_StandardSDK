#ifndef _SDIO_REG_H
#define _SDIO_REG_H

// SDIO interrupts
#define  TRANS_DONE_INT           (0x1U << 0) // transfer complete
#define  DMA1_INT                 (0x1U << 1) /* RW on every page buffer boundary for DMA1*/
#define  DMA2_INT                 (0x1U << 2) /*RW on every page buffer boundary for DMA2*/
#define  WR_START_INT             (0x1U << 3) // write start
#define  RD_START_INT             (0x1U << 4) // read start
#define  PWD_SET_INT              (0x1U << 5) // set pwd for card
#define  PWD_RST_INT              (0x1U << 6) // reset password
#define  LOCK_CARD_INT            (0x1U << 7) // lock card
#define  UNLOCK_CARD_INT          (0x1U << 8) // unlock card
#define  FORCE_ERASE_INT          (0x1U << 9) // force erase
#define  ERASE_INT                (0x1U << 10) // erase
#define  CTL_ASSD_SYS_INT         (0x1U << 11) /*control ASSD system(host sends cmd37)*/
#define  CMD0_52_SOFT_RST_INT     (0x1U << 12) // CMD0/52 soft reset
#define  CMD6_CHECK_DONE_INT      (0x1U << 13) /* CMD6, On receiving this interrupt, the processor will read the Argument register to find CMD6 Argument from the Host*/
#define  CMD6_SWITCH_DONE_INT     (0x1U << 14) /* On receiving this interrupt, theprocessor will read the Argument register tofind CMD6 Argument from the Host*/
#define  PRG_CSD_INT              (0x1U << 15) /*update programmable bits in CSD, On receiving this Interrupt, the processor will read the CSD contents through Card Address and Card Data Registers (Indirect addressing)*/
#define  ACMD23_INT               (0x1U << 16) /* On receiving this Interrupt,the processor has to read the Argument register, to find number of blocks to be preerased before writing*/
#define  CMD20_INT                (0x1U << 17) /* On receiving this Interrupt, the processor has to read the Argument register, to find the Speed Class Control info*/
#define  CMD11_INT                (0x1U << 18) /*The processor controller will clear this interrupt only after receiving the cmd11 clock start interrupt*/
#define  CMD4_INT                 (0x1U << 19) /*On receiving this Interrupt, the processor will read the DSR contents through Card Address and Card Data Registers(Indirect addressing)*/
#define  BOOT_START_INT           (0x1U << 20) /*This Interrupt is Asserted based on two conditions.1. Host pulls the cmd line low for greater than or equal to 74 Clock cycles 2. CMD0 with Arg 0xFFFF_FFFA,On receiving the Interrupt, the Processor has to read the Extended CSD register to find the Boot Partition Area, BOOT_SIZE_MULT etc*/.
#define  FUNC1_RST_INT            (0x1U << 21) //
#define  FUNC2_RST_INT            (0x1U << 22) //
#define  CMD11_CLK_STOP_INT       (0x1U << 23) //
#define  CMD11_CLK_START_INT      (0x1U << 24) //
#define  PRG_START_INT            (0x1U << 25) /* program start, This is to notify the processor, that the operating Processor can now be loaded from the flash memory.*/
#define  CMD40_INT                (0x1U << 26) //
#define  CMD43_INT                (0x1U << 27) //
#define  CRC_END_ERR_INT          (0x1U << 28) //
#define  ABORT_INT                (0x1U << 29) //
#define  LRST_INT                 (0x1U << 30) //Used only in eSD mode
#define  BOOT_COMPLETE_INT        (0x1U << 31) //

// TODO
#define  HOST_INT_MASK            (0x1U)
#define  LOW_MAC_INT_MASK         (0x1U)

#define  HOST_INT_ALL_MASK                (0x00000000)
#define  HOST_INT_ALL_EABLE               (0xffffffff)
#define  HOST_INT_ALL_CLEAR               (0xffffffff)


// CSR registers
#define     _CSR_REG_BASE                          _SDIO_AHB_SLAVE_BASE_ADR
// CSR reg offset
#define     ADDR_CSR_CTL_REG                       (0x00)  
#define     ADDR_CSR_CMD_REG                       (0x04)
#define     ADDR_CSR_ARG_REG                       (0x08)
#define     ADDR_CSR_BLK_CNT_REG                   (0x0C)
#define     ADDR_CSR_DMA1_ADR_REG                  (0x10)
#define     ADDR_CSR_DMA1_CTL_REG                  (0x14)
#define     ADDR_CSR_DMA2_ADR_REG                  (0x18)
#define     ADDR_CSR_DMA2_CTL_REG                  (0x1C)
#define     ADDR_CSR_ERASE_WR_BLK_START_REG        (0x20)
#define     ADDR_CSR_ERASE_WR_BLK_END_REG          (0x24)
#define     ADDR_CSR_PWD_LEN_REG                   (0x28)
#define     ADDR_CSR_INT_STATUS_REG                (0x3C)
#define     ADDR_CSR_INT_STATUS_ENA_REG            (0x40)
#define     ADDR_CSR_INT_SIGNAL_ENA_REG            (0x44)
#define     ADDR_CSR_CARD_ADR_REG                  (0x48)
#define     ADDR_CSR_CARD_DATA_REG                 (0x4C)
#define     ADDR_CSR_IO_READY_REG                  (0x50)
#define     ADDR_CSR_FUNC1_CTL_REG                 (0x54)
#define     ADDR_CSR_FUNC2_CTL_REG                 (0x58)
#define     ADDR_CSR_CCCR_CTL_REG                  (0x5C)
#define     ADDR_CSR_FBR1_CTL_REG                  (0x60)
#define     ADDR_CSR_FBR2_CTL_REG                  (0x64)
#define     ADDR_CSR_FBR3_CTL_REG                  (0x68)
#define     ADDR_CSR_FBR4_CTL_REG                  (0x6C)
#define     ADDR_CSR_FBR5_CTL_REG                  (0x70)
#define     ADDR_CSR_FBR6_CTL_REG                  (0x74)
#define     ADDR_CSR_FBR7_CTL_REG                  (0x78)
#define     ADDR_CSR_CARD_SIZE_REG                (0x80)
#define     ADDR_CSR_CARD_OCR_REG                 (0x84)
#define     ADDR_CSR_DEBUG_MODE_REG            (0x8C)

//与host间自定义寄存器，基地址0x400C0100
#define     _HOST_CARD_REG_START                    0x100
#define     _HOST_CARD_REG_BASE                     (_CSR_REG_BASE + _HOST_CARD_REG_START)


// cmd index
#define  IO_RW_DIRECT            52  // CMD52
#define  IO_RW_EXTENDED          53  // CMD53

// cmd regs fields value
#define  APP_CMD                 1   // application cmd
#define  NON_APP_CMD             0   // no application cmd

#define    SDIO_BUS_WIDTH_1           0
#define    SDIO_BUS_WIDTH_4           1
#define    SDIO_BUS_WIDTH_8           2

#define    START_RECORDING            0
#define    CREATE_DIR                 1
#define    UPDATE_CI                  4

#define     CARD_STATE_IDLE           0
#define     CARD_STATE_READY          1   // ready
#define     CARD_STATE_IDENT          2
#define     CARD_STATE_STANDBY        3
#define     CARD_STATE_TRAN           4
#define     CARD_STATE_DATA           5
#define     CARD_STATE_RCV            6
#define     CARD_STATE_PRG            7
#define     CARD_STATE_DIS            8
#define     CARD_STATE_BSTS           9  // only for MMC Card
#define     CARD_STATE_SLP            10

#define     IGNORE_END_BIT_ERR   0x04

// argument reg fields
#define     SDIO_READ              0
#define  SDIO_WRITE                1

#define  BYTE_MODE                 0
#define  BLOCK_MODE                1

#define     FIX_ADR_MODE           0   // fixed address mode
#define     INC_ADR_MODE           1   // increment address mode

// dma control reg fields
#define     DMA_ADR_VALID          1
#define     DMA_BUF_4K             0
#define     DMA_BUF_8K             1
#define     DMA_BUF_16K            2
#define     DMA_BUF_32K            3
#define     DMA_BUF_64K            4
#define     DMA_BUF_128K           5
#define     DMA_BUF_256K           6
#define     DMA_BUF_512K           7


// SDIO CSR's IO Ready reg bits
#define  FUNC1_RDY                     (0x1U<<1)    
#define  FUNC2_RDY                     (0x1U<<2)     

// SDIO CSR's control reg bits
#define  PRG_DONE                      (0x1U<<0)    
#define  WR_LAST_BLK_BUSY              (0x1U<<1)     
#define  CARD_INIT_DONE                (0x1U<<2)     
#define  ADR_OUT_OF_RANGE              (0x1U<<3)    
#define  ADR_MISALIGN                  (0x1U<<4)     

// SDIO CSR's OCR reg bits
#define  CCCR_REV_1DOT00                    0
#define  CCCR_REV_1DOT10                    1
#define  CCCR_REV_2DOT00                    2
#define  CCCR_REV_3DOT00                    3

#define  SDIO_SPEC_REV_1DOT00                0
#define  SDIO_SPEC_REV_1DOT10                1
#define  SDIO_SPEC_REV_1DOT20                2
#define  SDIO_SPEC_REV_2DOT00                3
#define  SDIO_SPEC_REV_3DOT00                4

#define  SD_FORMAT_REV_1DOT01                0
#define  SD_FORMAT_REV_1DOT10                1
#define  SD_FORMAT_REV_2DOT00                2
#define  SD_FORMAT_REV_3DOT0X                3


typedef union _CSR_CTL_REG
{
    struct 
    {
        UINT32    PrgDone:1; /*The processor sets this bit after completion of
                            Programming / Erase Operation / CMD43 / CMD20
                            Interrupt.*/
        UINT32    WrLastBlkBusy:1;/*Write Last Block Busy Control
                                1 - SDMMCSDIODevice Controller will pull the data0 line low after the last data block. The data0
                                line will be pulled high only when processor sets
                                program_done bit to 1 in control register.
                                0 - The SDMMCSDIODevice Controller will pull the
                                data0 line low after the last data block. The data0
                                line will be pulled high immediately after
                                transferring the last byte of last data block in
                                system bus.
                                Note: If this bit is set to 1’b0, card will not move to
                                programming state and it is not required to set
                                program done bit.
                                */
        UINT32    CardInitDone:1; /*On Program Start Interrupt, the processor will start
                                programming the CSR and SD/MMC/SDIO card
                                registers. Processor set this bit to 1, once it is done
                                with the Initialization.
                                1 – Card is ready to Operate
                                0 – Card is busy
                                This bit reflects in bit31 of CMD1 and ACMD41
                                response.
                                Note Device Controller clear this bit on soft reset.*/
        UINT32    AdrOutOfRange:1;/*This bit reflects in Card Status Register.
                                1 - A multiple block or stream read/write operation
                                is (although started in a valid address) attempting
                                to read or write beyond the card capacity
                                0 – No Error
                                Note: The Processor set this bit, only for infinite or
                                stream transfers. Address out of range for R type is
                                handled inside the controller.*/
        UINT32    AdrMisalign:1;/*This bit reflects in Card Status Register.
                            1 - A multiple block read/write operation (although
                            started with a valid address/blocklength
                            combination) is attempting to read or write a data
                            block which does not align with the physical blocks
                            of the card.
                            0 – No Error
                            Note: Address Misalign for R type is handled inside
                            the controller. For SD Configuration, the alignment
                            is always done based on 512 byte blocks,
                            regardless of the CSD values, whereas for MMC,
                            Read Operation, the alignment is based on
                            READ_BL_LEN value and similarly for MMC write
                            operation, the alignment is based on
                            WRITE_BL_LEN value.*/
        UINT32    Rsvd1:1;
        UINT32    EraseParam:1;/*This bit reflects in Card Status Register. .
                            1 - An invalid selection of erase groups for erase occurred.
                            0 – No Error*/
        UINT32    CardEccFail:1;/*This bit reflects in Card Status Register.
                            1 - Card internal ECC was applied but failed to correct the data
                            0 – No Error*/
        UINT32    CcErr:1;/*This bit reflects in Card Status Register.
                        1 - A card error occurred, which is not related to the host command.
                        0 – No Error*/
        UINT32    Err:1; /*This bit reflects in Card Status Register.
                        A generic card error related to the (and detected
                        during) execution of the last host command (e.g.
                        read or write failures).
                        0 – No Error*/
        UINT32    MmcIrqTrigger:1;/*This bit is used in MMC Interrupt Mode.
                                Whenever this bit is set, the PE-SMID Device
                                Controller will send its response to the host. .
                                Note: The device controller will send the response
                                only in Interrupt mode. The device controller will
                                ignore the MMC_IRQ_Trigger events in other
                                states.*/
        UINT32    CmdDataOutputEdge:1; /*Command and Data Output Edge:
                                    The SD/MMC/SDIO Device controller drive the data
                                    and cmd lines based on this bit.
                                    0 – Drive the Command and Data line at the falling
                                    edge of SD/MMC/SDIO clock
                                    1 – Drive the Command and Data line at the Rising
                                    edge of SD/MMC/SDIO clock*/
        UINT32    Cmd32Cmd33Ena:1;/*0 – Disabled. The SD/MMC/SDIO Device Controller
                                    consider cmd32/cmd33 as illegal command.
                                    1 – Enabled. The SD/MMC/SDIO Device Controller
                                    accept the Erase sequence with cmd32 and
                                    cmd33. In other words, Host can issue both Erase
                                    sequences
                                    1. CMD32, CMD33, CMD38
                                    2. CMD35, CMD36, CMD38.
                                    Note: This field is applicable only to MMC mode.
                                    This is mainly to compatible with MMC3.31 spec.*/
        UINT32    BootSeqSupport:1;/*0 – Boot Sequence is not supported
                                    1 – Boot Sequence is supported*/
        UINT32    BootPartEna:1;
        UINT32    BootAck:1;
        UINT32    WpViolation:1;
        UINT32    WpEraseSkip:1;
        UINT32    CidCsdOverwrite:1;
        UINT32    AkeSeqErr:1;
        UINT32    CardEccDisable:1;
        UINT32    StreamThresholdSize:3;/*000 – 32Bytes
                                    001 – 64Bytes
                                    010 – 128Bytes
                                    011 – 256Bytes
                                    100 - 512Bytes
                                    101 – 1KBytes
                                    110 – 2KBytes
                                    111 – Reserved for Future Use
                                    The Internal DMA engine uses this threshold value
                                    to do Stream write or Stream read operations with
                                    the system memory. Instead of waiting for a block
                                    size amount of space or data (Read or Write
                                    operation), the internal dma engine wait for stream
                                    threshold amount of space or data or end of
                                    transaction for Stream read or write operations.*/
        UINT32    PermanentWrProt:1;/*0 – The card is not permanently Write protected
                                    1 – The card is permanently write protected
                                    This field is required for internal lock unlock logic.*/
        UINT32    TempWrProt:1; /*0 – The card is not Temporary Write protected
                                    1 – The card is Temporary write protected
                                    This field is required for internal lock unlock logic*/
        UINT32    WpCmdEna:1;
        UINT32    AllowAKE:1; /*This bit determines whether to allow AKE
                            commands or not. The Processor set this bit after
                            both GET_MKB and GET_MID were executed. The
                            Controller clear this bit after power on reset or
                            cmd0 soft reset.
                            1 - Allow AKE commands (ACMD45-48)
                            0 - Ignore (ACMD45 – 48) commands and treat
                            as illegal commands.*/
        UINT32    SecuredMode:1;/*This bit determines whether to allow protected area
                                access commands (ACMD18, ACMD25, ACMD26,
                                ACMD38, and ACMD49) or not. Processor set this
                                bit after successful AKE sequence. The Controller
                                clear this bit after ACMD45, ACMD46, ACMD47,
                                ACMD18, ACMD25, ACMD26, ACMD38, or
                                ACMD49
                                1 – Allow protected area access commands
                                (ACMD18, ACMD25, ACMD26, ACMD38,
                                ACMD49)
                                0 – Ignore protected area access commands and
                                treat as Illegal commands.*/
        UINT32    AkeSeqOK:1; /*The Processor set this bit after AKE was successful
                            upto ACMD47. The controller clears this bit, when
                            SECURED_MODE is cleared.
                            1 – AKE sequence is ok
                            0 – AKE sequence is not ok.
                            The controller set AKE_SEQ_ERROR in the R1
                            response of the next ACMD48 if AKE_SEQ_OK is
                            cleared. If AKE_SEQ_OK is set, then the Controller
                            clear AKE_SEQ_ERROR in the R1 response of the
                            next ACMD48.*/
        UINT32    AssdDisEna:1; /*0 – ASSD Commands are Disabled.1 – ASSD Commands are Enabled*/
        UINT32    BootDataRdy:1;/*0- Boot data is not ready from Firmware
                            1- Boot data is ready from Firmware*/
    } bit;
    UINT32    word;
} CSR_CTL_REG;

typedef union _CSR_CMD_REG
{
    struct 
    {
        UINT32    AppCmd:1;/*1 – Current command is an Application Command
                            0 – Not an Application Command.*/
        UINT32    BlockSize:12;/*This field denotes the size of the data block.
                            12’d 0 – Reserved
                            12’d 1 – 1 Byte
                            12’d 2 - 2 Byte ....*/
        UINT32    CmdIndex:6; /*This field denotes the Index of the Current command*/
        UINT32    CurBusWidth:2; /*Denotes the current Bus Width
                                00 - 1 Bit
                                01 - 4 Bits
                                10 - 8 Bits
                                11 – Reserved*/
        UINT32    CurSpeed:3; /*Defines the Speed Class Control Bits
                            0000b: Start Recording
                            0001b: Create DIR
                            0010b: Reserved for Future Use
                            0011b: Reserved for Future Use
                            0100b: Update CI
                            Others: Reserved*/
        UINT32    CardState:4; /*Defines the current state of the Controller.
                            0 = Idle
                            1 = Ready
                            2 = Ident
                            3 = Stby
                            4 = Tran
                            5 = Data
                            6 = Rcv
                            7 = Prg
                            8 = Dis
                            9 = Btst (Applicable only for MMC Card)
                            10 = Slp (Applicable only for MMC Card)
                            11–15 = reserved*/
        UINT32    EraseSeq:1;/*Erase Sequence
                            This bit reflects the Host Erase sequence
                            0 – Erase Sequence with CMD32, CMD33,
                            CMD38 has occurred
                            1 – Erase Sequence with CMD35, CMD36,
                            CMD38 has occurred.*/
        UINT32    Rsvd:3;
    } bit;
    UINT32    word;
} CSR_CMD_REG;

typedef union _CSR_CMD52_ARG_REG
{
    struct 
    {
        UINT32    WrData:8;
        UINT32    Stuff_1:4;
        UINT32    RegAdr:17;  // read/write start address in function's address space
        UINT32    Stuff_2:1;
        UINT32    RawFlag:1;  // read after write flag
        UINT32    FuncNum:3;
        UINT32    RWFlag:1;
    } bit;
    UINT32    word;
} CSR_CMD52_ARG_REG;

typedef union _CSR_CMD53_ARG_REG
{
    struct 
    {
        UINT32    ByteOrBlkCnt:9;  /* for byte mode, 0 means read/write 512 bytes. for block mode, 0 means infinite count.
        /the I/O blocks shall be transferred until the operation is aborted by writing to the I/O abort function selct bits (ASx) in
the CCCR*/
        UINT32    RegAdr:17;  // read/write start address in function's address space
        UINT32    OpCode:1;  // fixed address or incrementing address
        UINT32    BlkMode:1;
        UINT32    FuncNum:3;
        UINT32    RWFlag:1;
    } bit;
    UINT32    word;
} CSR_CMD53_ARG_REG;



typedef union _CSR_DMA_CTL_REG
{
    struct 
    {
        UINT32    DmaAdrVld:1; // DMA address valid
        UINT32    BufSize:3;/*DMA1 Buffer Size
                            000b 4K bytes (Detects A11 carry out)
                            001b 8K bytes (Detects A12 carry out)
                            010b 16K Bytes (Detects A13 carry out)
                            011b 32K Bytes (Detects A14 carry out)
                            100b 64K bytes (Detects A15 carry out)
                            101b 128K Bytes (Detects A16 carry out)
                            110b 256K Bytes (Detects A17 carry out)
                            111b 512K Bytes (Detects A18 carry out)*/
        UINT32    Rsvd:28;
    } bit;
    UINT32    word;
} CSR_DMA_CTL_REG;


typedef union _CSR_PWD_LEN_REG
{
    struct 
    {
        UINT32    PwdsLen:8; // password len
        UINT32    DisableLockUnlock:1; // Lock/unlock feature is enable/disable
        UINT32    Rsvd:23;
    } bit;
    UINT32    word;
} CSR_PWD_LEN_REG;


typedef union _CSR_CARD_ADR_REG
{
    struct 
    {
        UINT32    StartAdr:10; 
        UINT32    DataCount:8; 
        UINT32    Rsvd:14;
    } bit;
    UINT32    word;
} CSR_CARD_ADR_REG;


typedef union _CSR_FUNC_CTL_REG
{
    struct 
    {
        UINT32    ReadCount:16; 
        UINT32    Rsvd:16;
    } bit;
    UINT32    word;
} CSR_FUNC_CTL_REG;

typedef union _CSR_CCCR_CTL_REG
{
    struct 
    {
        UINT32    CccrFmtVer:4;   /*CCCR Format Version Number.
                                Value CCCR/FBR Format Version
                                00h CCCR/FBR defined in SDIO Version 1.00
                                01h CCCR/FBR defined in SDIO Version 1.10
                                02h CCCR/FBR defined in SDIO Version 2.00
                                1.20
                                03h CCCR/FBR defined in SDIO Version 3.00
                                04h-0Fh Reserved for Future Use*/

        UINT32    SdioSpecVer:4; /*SDIO Specification Revision Number
                                Value SDIO Specification
                                00h SDIO Specification Version 1.00
                                01h SDIO Specification Version 1.10
                                02h SDIO Specification Version 1.20
                                (unreleased)
                                03h SDIO Specification Version 2.00
                                04h SDIO Specification Version 3.00*/
        UINT32    SdxFmtVer:4; /*SD Format Version Number.
                                Value Physical Layer Specification
                                00h Physical Layer Specification Version 1.01
                                (March 2000)
                                01h Physical Layer Specification Version 1.10
                                (October 2004)
                                02h Physical Layer Specification Version 2.00
                                (May 2006)
                                03h Physical Layer Specification Version 3.0x
                                04h-0Fh Reserved for Future Use
                                */
        UINT32    S8B:1;//Support 8it bus mode.
        UINT32    SCSI:1;/*Support Continuous Support of Interrupt, 0 – Continuous SPI Interrupt is supported, 
                       1 – Continuous SPI Interrupt is not supported.*/
        UINT32    SDC:1;//Support Direct Command(CMD52)
        UINT32    SMB:1;//Multiple Block Transfer
        UINT32    SRW:1;//Support Read Wait
        UINT32    SBS:1;//Support Bus Control,0 – Suspend/Resume is not supported, 1 – Suspend/Resume is supported
        UINT32    S4MI:1; //Support Block Gap Interrupt
        UINT32    LSC:1;  //Low Speed Card
        UINT32    M4BLS:1;  // 4bit mode for low speed card
        UINT32    SMPC:1; //Support for Master Power Control
        UINT32    SHS:1;  //0 – High Speed is not supported  1 – High Speed is supported
        UINT32    SDR50:1;
        UINT32    SDR104:1;
        UINT32    DDR50:1;
        UINT32    SDTA:1;  //Driver Type A
        UINT32    SDTC:1;  //Driver Type C
        UINT32    SDTD:1; //Driver Type D
        UINT32    SAI:1;  //Asynchronous Interrupt
        UINT32    Rsvd:2;
    } bit;
    UINT32    word;
} CSR_CCCR_CTL_REG;


typedef union _CSR_FBRX_CTL_REG
{
    struct 
    {
        UINT32    StdFuncCode:4;  /*SDIO Standard Function Code.
                            0h: No SDIO standard interface supported by this function
                            1h: This function supports the SDIO Standard UART
                            2h: This function supports the SDIO Bluetooth Type-A standard interface
                            3h: This function supports the SDIO Bluetooth Type-B standard interface
                            4h: This function supports the SDIO GPS standard interface
                            5h: This function supports the SDIO Camera standard interface
                            6h: This function supports the SDIO PHS standard interface
                            7h: This function supports the SDIO WLAN interface
                            8h: This function supports the Embedded SDIO-ATA standard interface
                            (Embedded SDIO-ATA shall be implemented only on devices following the
                            "Embedded SDIO Specification").
                            9h: This function supports the SDIO Bluetooth Type-A AMP standard interface
                            (AMP: Alternate MAC PHY)
                            10h-Eh: Not assigned, reserved for future use
                            Fh: This function supports an SDIO standard interface number greater than Eh. In
                            this case, the value in byte 101h identifies the standard SDIO interfaces type.
                                    */
        UINT32    ExtStdFuncCode:8; /*Function X Extended Standard SDIO Function Interface Code.*/
        UINT32    SupportCSA:1;  //code storage area
        UINT32    SPS:1;
        UINT32    Rsvd:15;
    } bit;
    UINT32    word;
} CSR_FBRX_CTL_REG;


typedef union _CSR_CARD_OCR_REG
{
    struct 
    {
        UINT32    CardOCR:24; /*Card OCR – Operational Condition Register.
                                For Multiple OCRs, the processor has to
                                program the logical AND of the voltage
                                range(s) of all the IO functions (including SD
                                memory). The SMID Controller will send the
                                preset value of this field for
                                ACMD41/CMD5/CMD1 query commands.
                                14:0 - Reserved
                                15 - 2.7 to 2.8
                                16 – 2.8 to 2.9
                                17 – 2.9 to 3.0
                                18 – 3.0 to 3.1
                                19 – 3.1 to 3.2
                                20 – 3.2 to 3.3
                                21 – 3.3 to 3.4
                                22 – 3.4 to 3.5
                                23 – 3.5 to 3.6.
                                        */
        UINT32    SwitchTo1dot8V:1; /*1 - Switching to 1.8V accepted
                                    0 – Switching to 1.8V not accepted*/
        UINT32    Rsvd1:4;
        UINT32    AccessMode:1;/*Used only in MMC mode – tied to zero always
                                Based on bit30 only the modes are
                                differentiated (byte mode Vs sector mode)
                                Bit30 – 0 => byte mode
                                Bit30 – 1 => Sector mode
                                        */
        UINT32    AccessModeCCS:1; /*MMC Mode – Access mode
                                    SD Mode - Card Capacity Status*/
        UINT32    Rsvd2:1; /*Reserved for Future use
                            This bit is not used, instead bit2 of control
                            register (card_init_done) is used to drive bit31
                            of OCR register.*/
    } bit;
    UINT32    word;
} CSR_CARD_OCR_REG;

// SDIO regsiters macros
#define     REG_CSR_CTRL               SdioReg.csr_ctl
#define     REG_CSR_CMD                 SdioReg.csr_cmd_reg
//#define     REG_CSR_CMD52_ARG               SdioReg.csr_cmd52_arg
#define     REG_CSR_CMD53_ARG          SdioReg.csr_cmd53_arg
#define     REG_CSR_BLK_COUNT          SdioReg.csr_blk_count
#define     REG_CSR_DMA1_ADR           SdioReg.csr_dma1_adr
#define     REG_CSR_DMA1_CTL           SdioReg.csr_dma1_ctl
#define     REG_CSR_DMA2_ADR           SdioReg.csr_dma2_adr
#define     REG_CSR_DMA2_CTL           SdioReg.csr_dma2_ctl
#define     REG_CSR_ERASE_WR_BLK_START      SdioReg.csr_erase_wr_blk_start
#define     REG_CSR_ERASE_WR_BLK_END      SdioReg.csr_erase_wr_blk_end
#define     REG_CSR_PWD_LEN             SdioReg.csr_pwd_len
#define     REG_CSR_INT_STATUS          SdioReg.csr_int_status
#define     REG_CSR_INT_STATUS_ENA      SdioReg.csr_int_status_ena
#define     REG_CSR_INT_SIGNAL_ENA      SdioReg.csr_int_signal_ena
#define     REG_CSR_CARD_ADR            SdioReg.csr_card_adr
#define     REG_CSR_CARD_DATA           SdioReg.csr_card_data
#define     REG_CSR_IOREADY             SdioReg.csr_ioready
#define     REG_CSR_FUNC1_CTL           SdioReg.csr_func1_ctl
#define     REG_CSR_FUNC2_CTL           SdioReg.csr_func2_ctl
#define     REG_CSR_CCCR_CTL            SdioReg.csr_cccr_ctl
#define     REG_CSR_FBR1_CTL            SdioReg.csr_fbr1_ctl
#define     REG_CSR_FBR2_CTL            SdioReg.csr_fbr2_ctl
#define     REG_CSR_FBR3_CTL            SdioReg.csr_fbr3_ctl
#define     REG_CSR_FBR4_CTL            SdioReg.csr_fbr4_ctl
#define     REG_CSR_FBR5_CTL            SdioReg.csr_fbr5_ctl
#define     REG_CSR_FBR6_CTL            SdioReg.csr_fbr6_ctl
#define     REG_CSR_FBR7_CTL            SdioReg.csr_fbr7_ctl
#define     REG_CSR_CARD_SIZE           SdioReg.csr_card_size
#define     REG_CSR_CARD_OCR            SdioReg.csr_card_ocr


typedef struct _SDIO_REG_
{
    // SDIO CSR registers
    UINT32                    csr_ctl;
    CSR_CMD_REG             csr_cmd_reg;
    CSR_CMD53_ARG_REG        csr_cmd53_arg;
    UINT32                    csr_blk_count;
    UINT32                    csr_dma1_adr;
    CSR_DMA_CTL_REG         csr_dma1_ctl;
    UINT32                    csr_dma2_adr;
    CSR_DMA_CTL_REG         csr_dma2_ctl;
    UINT32                    csr_erase_wr_blk_start;
    UINT32                    csr_erase_wr_blk_end;
    CSR_PWD_LEN_REG         csr_pwd_len;
    UINT32                    csr_int_status;
    UINT32                    csr_int_status_ena;
    UINT32                    csr_int_signal_ena;
    CSR_CARD_ADR_REG        csr_card_adr;
    UINT32                    csr_card_data;
    UINT32                    csr_ioready;
    CSR_FUNC_CTL_REG        csr_func1_ctl;
    CSR_FUNC_CTL_REG        csr_func2_ctl;
    CSR_CCCR_CTL_REG        csr_cccr_ctl;
    CSR_FBRX_CTL_REG        csr_fbr1_ctl;
    CSR_FBRX_CTL_REG        csr_fbr2_ctl;
    CSR_FBRX_CTL_REG        csr_fbr3_ctl;
    CSR_FBRX_CTL_REG        csr_fbr4_ctl;
    CSR_FBRX_CTL_REG        csr_fbr5_ctl;
    CSR_FBRX_CTL_REG        csr_fbr6_ctl;
    CSR_FBRX_CTL_REG        csr_fbr7_ctl;
    UINT32                    csr_card_size;
    CSR_CARD_OCR_REG        csr_card_ocr;
} SDIO_REG, *PSDIO_REG;


//-----------------------------------------------------------------------------------------------------
// 读写SDIO device controller的CSR寄存器
#define NST_WR_SDIO_CSR_REG(Offset, Value)   /*0x400C0000*/ \
    {\
        (*(volatile UINT32 *)(_CSR_REG_BASE + Offset)) = (UINT32)(Value);\
    }

#define NST_RD_SDIO_CSR_REG(Offset)  (*(volatile UINT32 *)(_CSR_REG_BASE + Offset))

// 读写host与card之间的自定义寄存器
// 自定义寄存器都按16bit来访问*/
#define NST_WR_HOST_CARD_REG16(Offset, Value)  /*0x400C0100*/ \
    {\
        (*(volatile UINT16 *)(_HOST_CARD_REG_BASE + Offset)) = (UINT16)(Value);\
    }

#define NST_RD_HOST_CARD_REG16(Offset)   (*(volatile UINT16 *)(_HOST_CARD_REG_BASE + Offset))

#define NST_WR_HOST_CARD_REG(Offset, Value)  /*0x400C0100*/ \
                NST_WR_HOST_CARD_REG16(Offset, Value)

#define NST_RD_HOST_CARD_REG(Offset)   \
                NST_RD_HOST_CARD_REG16(Offset)

#define NST_WR_HOST_CARD_REG_BIT(Offset, BitNum, BitValue)  /*0x400C0100*/ \
{\
    MEM_ADDR(BITBAND((_HOST_CARD_REG_BASE + Offset), BitNum)) = (UINT32)(BitValue);\
}

#define NST_RD_HOST_CARD_REG_BIT(Offset, BitNum)  (MEM_ADDR(BITBAND((_CSR_REG_BASE + Offset), BitNum)))

VOID BSP_SDIOInit(VOID);
VOID BSP_HostIntISR(VOID);

//#define SDIO_TEST

#endif //_SDIO_REG_H
