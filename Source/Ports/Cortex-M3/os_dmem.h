/*************************************************
 Copyright (C), 2013, BEIJING NUFRONT CO., LTD
 File name: os_dmem.h     
 Author: jun.chen@nufront.com                  
 Version: V1.0.0     
 Date:  2013-7-12
 Description: This file contains dynamic memory manegement structures and APIs.
 Others: ...      
 Function List:     
     1. ...
 History: 
     1. Date: ...
       Author: ...
       Modification: ...
     2. ...
*************************************************/
#ifndef __OS_DMEM_H__
#define __OS_DMEM_H__

#define OS_DMEM_NAME_SIZE         4    /*     Determine the size of a memory partition name       */

#define OS_DMEM_INVALID_MEMPOOL		    150u
#define OS_DMEM_INVALID_MEMPOOL_SIZE	    151u

typedef struct  CS_NODE_STRUCT
{
    struct CS_NODE_STRUCT  *cs_previous;
    struct CS_NODE_STRUCT  *cs_next;
}CS_NODE;

typedef unsigned int   UNSIGNED;
typedef unsigned char  CHAR; 
typedef unsigned char  DATA_ELEMENT; 
#ifndef VOID
typedef void           VOID;
#endif

#ifndef TRUE
#define TRUE               1
#endif

#ifndef FALSE
#define FALSE              0
#endif

#ifndef NULL
#define NULL               0
#endif

/* Define constants local to this component.  */
#define         DM_DYNAMIC_ID          0x44594e41UL
#define         DM_OVERHEAD            ((sizeof(DM_HEADER) + sizeof(UNSIGNED) \
                                        - 1)/sizeof(UNSIGNED)) *    \
                                        sizeof(UNSIGNED)

#define KL_MAX_NAME   8
#define PAD_1         3
#define R1 	register

/* Define the Dynamic Pool Control Block data type.  */
typedef struct DM_PCB_STRUCT 
{
    CS_NODE             dm_created;            /* Node for linking to    */
                                               /* created dynamic pools  */
    UNSIGNED            dm_id;                 /* Internal PCB ID        */
    CHAR                dm_name[OS_DMEM_NAME_SIZE];  /* Dynamic Pool name      */
    VOID               *dm_start_address;      /* Starting pool address  */
    UNSIGNED            dm_pool_size;          /* Size of pool           */
    UNSIGNED            dm_min_allocation;     /* Minimum allocate size  */
    UNSIGNED            dm_available;          /* Total available bytes  */
    struct DM_HEADER_STRUCT    
                       *dm_memory_list;        /* Memory list            */
    struct DM_HEADER_STRUCT
                       *dm_search_ptr;         /* Search pointer         */
} DM_PCB;    


/* Define the header structure that is in front of each memory block.  */
typedef struct DM_HEADER_STRUCT
{
    struct DM_HEADER_STRUCT
                       *dm_next_memory,        /* Next memory block      */
                       *dm_previous_memory;    /* Previous memory block  */
    DATA_ELEMENT        dm_memory_free;        /* Memory block free flag */
#if     PAD_1
    DATA_ELEMENT        dm_padding[PAD_1];
#endif 
    DM_PCB             *dm_memory_pool;        /* Dynamic pool pointer   */
} DM_HEADER;

void    OS_DMemInit(void); 
void *OSMMalloc(INT32U size);
void  OSMFree(void *ptr);
INT8U OS_GetMem(DM_PCB *pool_ptr, void **return_pointer, UNSIGNED size);
INT8U OS_FreeMem(VOID *memory);
INT8U  OS_Create_Memory_Pool(DM_PCB *pool_ptr, CHAR *name, 
                        void *start_address, UNSIGNED pool_size, 
                        UNSIGNED min_allocation);
INT8U OS_DelMemPool(DM_PCB *pool_ptr);

#endif//__OS_DMEM_H__
