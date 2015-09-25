#ifndef MPU_H_
#define MPU_H_

#include "types_def.h"

extern VOID MPU_init(VOID);

/* Masks for MPU Memory Region Sizes */
#define REGION_32B      0x08
#define REGION_64B      0x0A
#define REGION_128B     0x0C
#define REGION_256B     0x0E
#define REGION_512B     0x10
#define REGION_1K       0x12
#define REGION_2K       0x14
#define REGION_4K       0x16
#define REGION_8K       0x18
#define REGION_16K      0x1A
#define REGION_32K      0x1C
#define REGION_64K      0x1E
#define REGION_128K     0x20
#define REGION_256K     0x22
#define REGION_512K     0x24
#define REGION_1M       0x26
#define REGION_2M       0x28
#define REGION_4M       0x2A
#define REGION_8M       0x2C
#define REGION_16M      0x2E
#define REGION_32M      0x30
#define REGION_64M      0x32
#define REGION_128M     0x34
#define REGION_256M     0x36
#define REGION_512M     0x38
#define REGION_1G       0x3A
#define REGION_2G       0x3C
#define REGION_4G       0x3E

/* General MPU Masks */
#define REGION_Enabled  0x1
#define REGION_Valid    0x10

#define SHAREABLE       0x40000
#define CACHEABLE       0x20000
#define BUFFERABLE      0x10000

/* Region Permissions */

#define NOT_EXEC        0x10000000 /* All Instruction fetches abort */

#define NO_ACCESS       0x00000000 /* Privileged No Access, Unprivileged No Access */
#define P_NA_U_NA       0x00000000 /* Privileged No Access, Unprivileged No Access */
#define P_RW_U_NA       0x01000000 /* Privileged Read Write, Unprivileged No Access */
#define P_RW_U_RO       0x02000000 /* Privileged Read Write, Unprivileged Read Only */
#define P_RW_U_RW       0x03000000 /* Privileged Read Write, Unprivileged Read Write */
#define FULL_ACCESS     0x03000000 /* Privileged Read Write, Unprivileged Read Write */
#define P_RO_U_NA       0x05000000 /* Privileged Read Only, Unprivileged No Access */
#define P_RO_U_RO       0x06000000 /* Privileged Read Only, Unprivileged Read Only */
#define RO              0x07000000 /* Privileged Read Only, Unprivileged Read Only */


typedef struct 
{
    VUINT32 Type;
    VUINT32 Ctrl;
    VUINT32 RegionNumber;
    VUINT32 RegionBaseAddr;
    VUINT32 RegionAttrSize;
} MPU_TypeDef;



#define MPU_BASE    ((UINT32)0xE000ED90)

#ifndef  DEBUG
#define NST_MPU            ((MPU_TypeDef *) MPU_BASE)
#else
/*------------------------ Debug Mode ----------------------------------------*/
extern MPU_TypeDef  *NST_MPU;
#endif

VOID MPU_Init(void);

#endif /* MPU_H_*/
