/*
** Copyright (C) ARM Limited, 2006. All rights reserved.
*/

/*
** This file contains SCS Registers, MPU Masks
** and SCS Initialisation example.
*/

#include "mpu.h"

/*
 * Setup MPU Basic regions and non execute for stack/heap
 */
VOID MPU_Init(void)
{
    /***Example MPU initialisation code***/

// 0x00000000-0x0000FFFF   bootloader + fwROMÖ»¶Á64K
// 0x00010000-0x0003FFFF   code SRAM ¿É¶ÁÐ´192K
// 0x00080000-0x00098000   DATA SRAM ¿É¶ÁÐ´96K
// 0x20000000-0x20018000   BUF SRAM0 ¿É¶ÁÐ´96K
// 0x20040000-0x20050000   BUF SRAM1 ¿É¶ÁÐ´64K

    /* Configure region 1 to cover ROM (Executable, Read-only) */
    /* Start address, Region field valid, Region number */
    NST_MPU->RegionBaseAddr = 0x00000000 | 0x10 | 1;
    /* Access control bits, Size, Enable */
    NST_MPU->RegionAttrSize = 0x06030000 | REGION_64K | 0x1;

    /* Configure a region to cover code RAM (Executable, Read-Write) */
    NST_MPU->RegionBaseAddr = 0x00010000 | 0x10 | 2;
    NST_MPU->RegionAttrSize = 0x03030000 | REGION_64K | 0x1;

    /* Configure a region to cover code RAM (Executable, Read-Write) */
    NST_MPU->RegionBaseAddr = 0x00020000 | 0x10 | 2;
    NST_MPU->RegionAttrSize = 0x03030000 | REGION_128K | 0x1;

    /* Configure a region to cover data SRAM (Not Executable, Read-Write) */
    NST_MPU->RegionBaseAddr = 0x00080000 |0x3F00 | 0x10 | 3; // high 32K null memory
    NST_MPU->RegionAttrSize = 0x13030000 | REGION_128K| 0x1;

    /* Configure a region to cover buf SRAM0 (Not Executable, Read-Write) */ 
    NST_MPU->RegionBaseAddr = 0x20000000 |0x3F00 | 0x10 | 3; // high 32K null memory
    NST_MPU->RegionAttrSize = 0x13030000 | REGION_128K| 0x1;

    /* Configure a region to cover buf SRAM1 (Not Executable, Read-Write) */
    NST_MPU->RegionBaseAddr = 0x20040000 | 0x10 | 3;
    NST_MPU->RegionAttrSize = 0x13030000 | REGION_64K| 0x1;


    /* Configure a region to cover peripherial (Not Executable, Read-Write) */
    NST_MPU->RegionBaseAddr = 0x40000000 | 0x10 | 4;
    NST_MPU->RegionAttrSize = 0x13030000 | REGION_1G | 0x1;

    /* Configure a region to cover SCB (Not Executable, Read-Write) */
    NST_MPU->RegionBaseAddr = 0xE0000000 | 0x10 | 4;
    NST_MPU->RegionAttrSize = 0x13030000 | REGION_32M | 0x1;

    /* Enable the MPU */
    NST_MPU->Ctrl |= 1;

    /* Force Memory Writes before continuing */
    __dsb(0xf);
    /* Flush and refill pipeline with updated permissions */
    __isb(0xf);
    
}




