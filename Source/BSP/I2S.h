#ifndef _I2S_H
#define _I2S_H

//#define HW_I2S_SUPPORT

#ifdef HW_I2S_SUPPORT
#define NI2S_FIFO_BASE		    0x200
#define FCLK                    40000000//clk clock frequency
#define ASF                     22050//audio sample rate,
#define RESOLUTION              16//16 bit sample
#define ALMOSTFULLLEVEL         4
#define ALMOSTEMPTYLEVEL        4

//#define USE_PWM
#define USE_I2S_DMA
#define USE_I2S_DMA_INT
#define USE_I2S_16_BIT      //I2s output width 16bit/24bit 
#define TRAN_REC_I2S_LEFT_MODE   0x6db
#define TRAN_REC_I2S_MODE  0x249 
#define TRAN_REC_I2S_RIGHT_MODE  0x4d3 

#ifdef USE_I2S_DMA_INT
#define USE_DMA_INT        //enable DMA INT
#endif

#define NST_WR_I2S_REG(Value, Adr) \
    {\
        (*(volatile UINT32 *)(_I2S_BASE_ADR + Adr)) = (UINT32)(Value);\
    }

#define NST_RD_I2S_REG(Adr)  (*(volatile UINT32 *)(_I2S_BASE_ADR + Adr))


/*0~DEPTH-1 Determines threshold for almost empty flag in the transmit FIFO.*/
/*16~DEPTH+15 Determines threshold for aalmost full flag in the transmit FIFO.*/
#define FIFOC(ae,af)    (ae|(af<<16)) 

/* Control register */
//SFR
#define I2S_CTRL (0x00)  	//6'b000000 01900000H I2S Control Register
#define I2S_STAT (0x04)	//6'b000100 00001100H I2S Status Register
#define I2S_SRR  (0x08)	//6'b001000 00000000H I2S Channels Sample Rate & Resolution Configuration Register
#define I2S_CID_CTRL (0x0C)	//6'b001100 00000000H Clock, Interrupt and DMA Control Register
#define I2S_TFIFO_STAT (0x10)	//6'b010000 00000000H Transmit FIFO Status Register Indicates transmit FIFO level.
#define I2S_RFIFO_STAT (0x14)	//6'b010100 00000000H Receive FIFO Status Register Indicates receive FIFO level
#define I2S_TFIFO_CTRL (0x18)	//6'b011000 000F0000H1 Transmit FIFO Control Register
#define I2S_RFIFO_CTRL (0x1C)	//6'b011100 000F0000H1 Receive FIFO Control Register
#define I2S_DEV_CONF   (0x20)	//6'b100000 00000208H2 Device Configuration Register
#define I2S_PWM_CONF   (0x24)   //PWM MODULE Configuration Register

/* I2S Control Register */
/*I2S_CTRL*/
#define RESET_ALL_CHN       (0)
/*0 Enable bit for I2S channel 0. Value '0' causes reset
signal for this channel (i2s_rst_0), configuration SFR
bits for this channel are unchanged. Value '1' enables
channel. Sampled on the rising edge of the clock.*/
#define I2S_EN_CHN0         (1<<0)
#define I2S_EN_CHN1         (1<<1)
#define I2S_EN_CHN2         (1<<2)
#define I2S_EN_CHN3         (1<<3)
#define I2S_EN_CHN4         (1<<4)
#define I2S_EN_CHN5         (1<<5)
#define I2S_EN_CHN6         (1<<6)
#define I2S_EN_CHN7         (1<<7)

/*Transmitter (value '1') or receiver (value '0')
configuration bit*/
#define I2S_TX_EN_CHN0      (1<<8)
#define I2S_TX_EN_CHN1      (1<<9)
#define I2S_TX_EN_CHN2      (1<<10)
#define I2S_TX_EN_CHN3      (1<<11)
#define I2S_TX_EN_CHN4      (1<<12)
#define I2S_TX_EN_CHN5      (1<<13)
#define I2S_TX_EN_CHN6      (1<<14)
#define I2S_TX_EN_CHN7      (1<<15)
#define I2S_RX_EN_CHN0      (0<<8)
#define I2S_RX_EN_CHN1      (0<<9)
#define I2S_RX_EN_CHN2      (0<<10)
#define I2S_RX_EN_CHN3      (0<<11)
#define I2S_RX_EN_CHN4      (0<<12)
#define I2S_RX_EN_CHN5      (0<<13)
#define I2S_RX_EN_CHN6      (0<<14)
#define I2S_RX_EN_CHN7      (0<<15)
/*loop_back_0_1 16 Loop back test configuration bit. When '1', it configures
channels 0 and 1 into the loop-back mode. In this mode
channels 0 and 1 can work in both directions depending
on configuration bits. Default value '0' causes normal
operation without loop-back.*/
#define LOOP_BACK_CHN0_1    (1<<16) 
#define LOOP_BACK_CHN2_3    (1<<17)
#define LOOP_BACK_CHN4_5    (1<<18)
#define LOOP_BACK_CHN6_7    (1<<19)
/*sfr_rst 20 SFR block synchronous reset. When '0', all bits in SFR
registers are reset to default values.*/
#define I2S_SFR_RST         (0<<20)
#define I2S_SFR_NO_RST      (1<<20)
/*Master (value '1') or slave (value '0') configuration bit
for unit synchronizing all transmitters with I2S bus.*/
#define I2S_TX_MASTER       (1<<21)
#define I2S_TX_SLAVE        (0<<21)
/*Master (value '1') or slave (value '0') configuration bit
for unit synchronizing all receivers with I2S bus.*/
#define I2S_RX_MASTER       (1<<22)
#define I2S_RX_SLAVE        (0<<22)

#define TX_FIFO_RST         (0<<23)//Transmit FIFO reset
#define TX_FIFO_NO_RST      (1<<23)
#define RX_FIFO_RST         (0<<24)//Receive FIFO reset
#define RX_FIFO_NO_RST      (1<<24)
#define TX_SYNC_RST         (0<<25)//Reset for transmitter synchronizing unit
#define TX_SYNC_NO_RST      (1<<25)
#define RX_SYNC_RST         (0<<26)//Reset for receiver synchronizing unit
#define RX_SYNC_NO_RST      (1<<26)
/*When 1 (normal mode), the scki
and wsi inputs of the I2S module (configured to be
transmitter synchronization unit) are connected to the
external inputs tclki and twsi. When 0 (loop-back mode)
the scki and wsi inputs of the I2S module (configured to
be transmitter synchronization unit) are connected to
the receiver synchronization unit outputs rscko and
rwso.*/
#define TX_SYNC_NMLMD       (1<<27)
#define TX_SYNC_LPBK        (0<<27)
/*When 1 (normal mode), the scki and wsi inputs of
the I2S module (configured to be receiver
synchronization unit) are connected to the external
inputs rclki and rwsi. When 0 (loop-back mode) the scki
and wsi inputs of the I2S module (configured to be
receiver synchronization unit) are connected to the
transmitter synchronization unit outputs tscko and twso.*/
#define RX_SYNC_NMLMD       (1<<28)
#define RX_SYNC_LPBK        (0<<28)

/*I2S_STAT*/
#define TX_DATA_UNDER_RUN   (1<<0)//transmitter data underrun error
#define RX_DATA_OVER_RUN    (1<<4)//receiver data overrun error
#define TX_FIFO_EMPTY       (1<<8)//Transmit FIFO empty
#define TX_FIFO_AEMPTY      (1<<9)//Transmit FIFO almost empty
#define TX_FIFO_FULL        (1<<10)//Transmit FIFO full
#define TX_FIFO_AFULL       (1<<11)//Transmit FIFO almost full
#define RX_FIFO_EMPTY       (1<<12)//Receive FIFO empty
#define RX_FIFO_AEMPTY      (1<<13)//Receive FIFO almost empty
#define RX_FIFO_FULL        (1<<14)//Receive FIFO full
#define RX_FIFO_AFULL       (1<<15)//Receive FIFO almost full

/*I2S_SRR*/
/*tsample_rate 0-6 Transmitter sample rate.
Sampled on the rising edge of the clock.
- 7-10 Not implemented. Return ¡®0¡¯ when read.
tresolution 11-15 Transmitter resolution (0 to 31).
Sampled on the rising edge of the clock.
It simply should be assigned the value equal to the number
of valid bits minus one.
rsample_rate 16-22 Receiver sample rate.
Sampled on the rising edge of the clock.
- 23-26 Not implemented. Return ¡®0¡¯ when read.
rresolution 27-31 Receiver resolution (0 to 31).
Sampled on the rising edge of the clock.
It simply should be assigned the value equal to the number
of valid bits minus one.*/
#define SR(s,r)         (s|((r-1)<<11))
#define SSR(s,r)        (SR(s,r)|(SR(s,r)<<16))

/*I2S_CID_CTRL*/
/*Clock enable, channel 0.*/
#define CHN0_CLK_EN         (0<<0)
#define CHN0_CLK_DIS        (1<<0)
#define CHN1_CLK_EN         (0<<1)
#define CHN1_CLK_DIS        (1<<1)
#define CHN2_CLK_EN         (0<<2)
#define CHN2_CLK_DIS        (1<<2)
#define CHN3_CLK_EN         (0<<3)
#define CHN3_CLK_DIS        (1<<3)
#define CHN4_CLK_EN         (0<<4)
#define CHN4_CLK_DIS        (1<<4)
#define CHN5_CLK_EN         (0<<5)
#define CHN5_CLK_DIS        (1<<5)
#define CHN6_CLK_EN         (0<<6)
#define CHN6_CLK_DIS        (1<<6)
#define CHN7_CLK_EN         (0<<7)
#define CHN7_CLK_DIS        (1<<7)

/*Clock enable for the unit synchronizing transmitters*/
#define TX_SYNC_CLK_EN          (0<<8)
#define TX_SYNC_CLK_DIS         (1<<8)
/*Clock enable for the unit synchronizing receivers*/
#define RX_SYNC_CLK_EN          (0<<9)
#define RX_SYNC_CLK_DIS         (1<<9)
/*Bit masking all interrupt requests. When ¡®0¡¯ all interrupts are
masked, when ¡®1¡¯ interrupts use individual masks.*/
#define INTR_EN                 (1<<15)
#define INTR_DIS                (0<<15)
/*Bit masking interrupt request generation after underrun / overrun
condition occurrence in I2S channel 0. When LOW, masks
generation of interrupt request caused by the channel 0.*/
#define CHN0_INTR_EN            (1<<16)
#define CHN0_DIS                (0<<16)
#define CHN1_INTR_EN            (1<<17)
#define CHN1_DIS                (0<<17)
#define CHN2_INTR_EN            (1<<18)
#define CHN2_DIS                (0<<18)
#define CHN3_INTR_EN            (1<<19)
#define CHN3_DIS                (0<<19)
#define CHN4_INTR_EN            (1<<20)
#define CHN4_DIS                (0<<20)
#define CHN5_INTR_EN            (1<<21)
#define CHN5_DIS                (0<<21)
#define CHN6_INTR_EN            (1<<22)
#define CHN6_DIS                (0<<22)
#define CHN7_INTR_EN            (1<<23)
#define CHN7_DIS                (0<<23)
#define TX_FIFO_EMPTY_INT_EN       (1<<24)//Transmit FIFO empty intr enable
#define TX_FIFO_AEMPTY_INT_EN      (1<<25)//Transmit FIFO almost empty intr enable
#define TX_FIFO_FULL_INT_EN        (1<<26)//Transmit FIFO full intr enable
#define TX_FIFO_AFULL_INT_EN       (1<<27)//Transmit FIFO almost full intr enable
#define RX_FIFO_EMPTY_INT_EN       (1<<28)//Receive FIFO empty intr enable
#define RX_FIFO_AEMPTY_INT_EN      (1<<29)//Receive FIFO almost empty intr enable
#define RX_FIFO_FULL_INT_EN        (1<<30)//Receive FIFO full intr enable
#define RX_FIFO_AFULL_INT_EN       (1<<31)//Receive FIFO almost full intr enable


/* I2S Control Register */
#define PWM_CHN0                (0<<5)
#define PWM_CHN1                (1<<5)
#define PWM_CHN2                (2<<5)
#define PWM_CHN3                (3<<5)
#define PWM_CHN4                (4<<5)

#define PWM_MUTE                (1<<4)
#define PWM_EN                  (1<<3)

#define PWM_SAMPLE_RATE_220500    (0<<0)
#define PWM_SAMPLE_RATE_240000    (1<<0)
#define PWM_SAMPLE_RATE_320000    (2<<0)
#define PWM_SAMPLE_RATE_441000    (3<<0)
#define PWM_SAMPLE_RATE_480000    (4<<0)

void BSP_I2SInit(void);
void BSP_I2SStart(void);
void BSP_I2SHalt(void);
void BSP_I2SDeinit(void);
void BSP_I2SMute(void);
void BSP_I2SSound(void);
void BSP_I2SIntISR(void);
void BSP_I2SWrite(void * buf,  unsigned int len);
void BSP_I2SDMAWrite(void);

#endif  //HW_I2S_SUPPORT
#endif

