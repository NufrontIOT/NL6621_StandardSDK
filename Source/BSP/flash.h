#ifndef __FLASH_H
#define __FLASH_H    

#include "spi.h"
#include "qspi.h"

// Flash芯片选择，只能选择一个
//#define AT45DB041D
#define MX25L2005

#define DUMMY_BYTE                  0x55
#define MAX_FLASH_RD_SIZE    65536

#if(defined AT45DB041D)
//AT45DB041D default page size 264B
#define FLASH_PAGE_SIZE         264
#else
#define FLASH_PAGE_SIZE         256
#define FLASH_SECTOR_SIZE     4096
#define FLASH_BLOCK64_SIZE       1024*64
#define FLASH_BLOCK32_SIZE       1024*32
#endif

// Flash 命令定义
#define  FLASH_CMD_SEQ_READ  0x03
#if(defined AT45DB041D)
//AT45DB041D指令表
#define FLASH_ERASE_CMD0             0xC7
#define FLASH_ERASE_CMD1             0x94
#define FLASH_ERASE_CMD2             0x80
#define FLASH_ERASE_CMD3             0x9A

#define  FLASH_CMD_PAGE_WRITE    0x82   // Main mem page program through buffer, page size=264
#define  FLASH_CMD_RD_SR              0xD7 //0xD7/0x57 read status register
#define  FLASH_SR_READY                   (1 << 7)  // 1 ready  0 busy
#else
/* W25X40BL(chip erase 0x60/0xC7) 
  & MX25L2005(chip erase 0x60/0xC7) 
  & S25FL404A (chip erase 0xC7)
  & GD25Q40(chip erase 0x60/0xC7) 
*/
#define  FLASH_CMD_WREN         0x06
#define  FLASH_CMD_WRDIS        0x04
#define  FLASH_CMD_CHIP_ERASE     0xC7 // chip erase
#define  FLASH_CMD_SECTOR_ERASE     0x20 // sector erase, 0x20
#define  FLASH_CMD_BLOCK64_ERASE     0xD8 // block erase, 64K(0xD8) 
#define  FLASH_CMD_BLOCK32_ERASE     0x52 // block erase, 32K(0x52)
#define  FLASH_CMD_PAGE_WRITE    0x02
#define  FLASH_CMD_RD_SR         0x05 // read status register
#define  FLASH_CMD_WR_SR        0x01 // read status register
#define  FLASH_SR_BUSY             (1 << 0)  // 1: busy, 0: ready
#define  FLASH_SR_WEL               (1 << 1)  // 1: write enable 
#endif


/* Flash 地址空间分配*/
#define NVM_MAX_SIZE				0x80000   // 4Mbits 
//#define NVM_MAX_SIZE				0x100000 // 8Mbits 

#define FW_IMG_SIZE_MAX		  	0x30000	// max firmware image size 192K(include image header 256 bytes)

// firmware image area
#define NVM_FW_MAIN_OFFSET        	0

// auto test result area 
#define NV_INFO_START		(0x32000) // take 1 sector

#define NVM_FW_BAK_OFFSET          	(0x34000)

// config param
#define NVM_PARAM_OFFSET			(NVM_FW_BAK_OFFSET + FW_IMG_SIZE_MAX)
#define NVM_PARAM_PARTITION_NUM_MAX   3   // each param partition take 1 sector

// file system
#define NVM_FILE_SYSTEM_OFFSET	(NVM_PARAM_OFFSET + FLASH_SECTOR_SIZE * NVM_PARAM_PARTITION_NUM_MAX) // for 8 Mbits Flash
//#define NVM_FILE_SYSTEM_OFFSET 0 // debug with 4Mbits Flash,  fatfs require at least 128 sectors(4M bits)

/* flash io interface */
VOID SpiFlashEraseChip(VOID);
VOID SpiFlashRead(UINT32 ReadStartPos, UINT8* pBuf, UINT32 Len);  
VOID SpiFlashWriteOnePage(UINT32 Adr, UINT8* pBuf);
VOID SpiFlashEraseSector(UINT32 SectorAddr);

VOID QSpiFlashRead(UINT32 ReadStartPos, UINT8* pBuf, UINT32 Len); 
VOID QSpiFlashEraseChip(VOID);
VOID QSpiFlashWriteOnePage(UINT32 Adr, UINT8* pBuf);
VOID QSpiFlashEraseSector(UINT32 SectorAddr);
INT32 QSpiWriteOneSector(UINT32 SectorAddr, UINT8* pBuf);

#endif

