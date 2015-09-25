/*************************************************
 Copyright (C), 2013, BEIJING NUFRONT CO., LTD
 File name: os_dmem.c     
 Author: jun.chen@nufront.com                  
 Version: V1.0.0     
 Date:  2013-7-12     
 Description: This file contains Dynamic memory manegement implementation.
 Others: ...      
 Function List:     
     1. ...
 History: 
     1. Date: ...
       Author: ...
       Modification: ...
     2. ...
*************************************************/

#define  OS_CPU_GLOBALS
#include "arch/sys_arch.h"
#include "os_dmem.h"

INT32U        DMD_Total_Pools;

/* Control block of system memory pool */
DM_PCB OS_system_mempool;

INT8U  OS_Create_Memory_Pool(DM_PCB *pool_ptr, CHAR *name, 
                        void *start_address, UNSIGNED pool_size, 
                        UNSIGNED min_allocation)
{
	R1 DM_PCB      *pool;                       /* Pool control block ptr    */
	int             i;                                     /* Working index variable    */
	DM_HEADER      *header_ptr;          /* Partition block header ptr*/
#if OS_CRITICAL_METHOD == 3              /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0;
#endif

    /* Move input pool pointer into internal pointer.  */
    pool =  (DM_PCB *) pool_ptr;

	#if OS_ARG_CHK_EN > 0
	if (pool_ptr == NULL) return OS_ERR_INVALID_MEMPOOL;
	if ((start_address == NULL)||(start_address%4 != 0)) return OS_ERR_MEM_INVALID_ADDR;
	if (pool_size <= min_allocation) return OS_ERR_INVALID_MEMPOOL_SIZE;
	#endif

    /* First, clear the partition pool ID just in case it is an old 
       pool control block.  */
    pool->dm_id =             0;
    
    /* Fill in the partition pool name.  */
    for (i = 0; i < OS_DMEM_NAME_SIZE; i++)
        pool->dm_name[i] =  name[i];    

    /* Convert the pool's size into something that is evenly divisible by
       the sizeof an UNSIGNED data element.  */
    pool_size =  (pool_size/sizeof(UNSIGNED)) * sizeof(UNSIGNED);

    /* Save the starting address and size parameters in the dynamic memory 
       control block.  */
    pool->dm_start_address = start_address;
    pool->dm_pool_size = pool_size;
    pool->dm_min_allocation = ((min_allocation + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED)) * sizeof(UNSIGNED);
    
    /* Initialize link pointers.  */
    pool->dm_created.cs_previous =    NULL;
    pool->dm_created.cs_next =        NULL;

    /* Build a single block that has all of the memory.  */
    header_ptr =  (DM_HEADER *) start_address;

    /* Initialize the memory parameters.  */
    pool->dm_available =       pool_size - (2 * DM_OVERHEAD);
    pool->dm_memory_list =     header_ptr;
    pool->dm_search_ptr =      header_ptr;
    
    /* Build the block header.  */
    header_ptr->dm_memory_pool =  pool;
    header_ptr->dm_memory_free =  TRUE;
    header_ptr->dm_next_memory =  (DM_HEADER *) 
           (((CHAR *) header_ptr) + pool->dm_available + DM_OVERHEAD);     
    header_ptr->dm_previous_memory =  header_ptr->dm_next_memory;
    
    /* Build the small trailer block that prevents block merging when the
       pool wraps around.  Note that the list is circular so searching can
       wrap across the physical end of the memory pool.  */
    header_ptr =  header_ptr->dm_next_memory;
    header_ptr->dm_next_memory =  (DM_HEADER *) start_address;
    header_ptr->dm_previous_memory =  (DM_HEADER *) start_address;
    header_ptr->dm_memory_pool =  pool;
    header_ptr->dm_memory_free =  FALSE;

    /* Protect against access to the list of created memory pools.  */
    OS_ENTER_CRITICAL();
    
    /* At this point the dynamic memory pool is completely built.  The ID can 
       now be set and it can be linked into the created dynamic memory
       pool list. */
    pool->dm_id =  DM_DYNAMIC_ID;

    /* Link the memory pool into the list of created memory pools and 
       increment the total number of pools in the system.  */
//    CSC_Place_On_List(&DMD_Created_Pools_List, &(pool->dm_created));
    DMD_Total_Pools++;

    /* Release protection against access to the list of created memory
       pools.  */
    OS_EXIT_CRITICAL();

    /* Return successful completion.  */
    return(OS_ERR_NONE);
}


INT8U OS_DelMemPool(DM_PCB *pool_ptr)
{

	R1 DM_PCB      *pool;                       /* Pool control block ptr    */
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif

    /* Move input pool pointer into internal pointer.  */
    pool =  (DM_PCB *) pool_ptr;

    /* Protect against simultaneous access to the memory pool.  */
    OS_ENTER_CRITICAL();

    /* Clear the memory pool ID.  */
    pool->dm_id =  0;

    /* Remove the memory pool from the list of created memory pools.  */
//    CSC_Remove_From_List(&DMD_Created_Pools_List, &(pool->dm_created));

    /* Decrement the total number of created memory pools.  */
    DMD_Total_Pools--;

    /* Release protection against access to the list of created memory
       pools. */
    OS_EXIT_CRITICAL();

    /* Return a successful completion.  */
    return(OS_ERR_NONE);
}


INT8U OS_GetMem(DM_PCB *pool_ptr, void **return_pointer, 
                                        UNSIGNED size)
{

	R1 DM_PCB      *pool;                       /* Pool control block ptr    */
	R1 DM_HEADER   *memory_ptr;                 /* Pointer to memory         */
	R1 DM_HEADER   *new_ptr;                    /* New split block pointer   */
	UNSIGNED        free_size;                  /* Size of block found       */
	INT8U          status;                     /* Completion status         */
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif


    /* Move input pool pointer into internal pointer.  */
    pool =  (DM_PCB *) pool_ptr;

    /* Initialize the status as successful.  */
    status =  OS_ERR_NONE;

    /* Adjust the request to a size evenly divisible by the number of bytes
       in an UNSIGNED data element.  Also, check to make sure it is of the
       minimum size.  */
    if (size < pool->dm_min_allocation)
    
        /* Change size to the minimum allocation.  */
        size =  pool->dm_min_allocation;
    else
    
        /* Insure that size is a multiple of the UNSIGNED size.  */
        size = 
           ((size + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED)) * sizeof(UNSIGNED);

    /* Protect against simultaneous access to the memory pool.  */
    OS_ENTER_CRITICAL();
    
    /* Search the memory list for the first available block of memory that
       satisfies the request.  Note that blocks are merged during the 
       deallocation function.  */
    memory_ptr =  pool->dm_search_ptr;
    do
    {
    
        /* Determine if the block is free and if it can satisfy the request. */
        if (memory_ptr->dm_memory_free)
        
            /* Calculate the free block size.  */
            free_size =  (((CHAR *) (memory_ptr->dm_next_memory)) -
                           ((CHAR *) memory_ptr)) - DM_OVERHEAD;
        else
        
            /* There are no free bytes available.  */
            free_size =  0;
            
        /* Determine if the search should continue.  */
        if (free_size < size)
        
            /* Large enough block has not been found.  Move the search 
               pointer to the next block.  */
            memory_ptr =  memory_ptr->dm_next_memory;
    } while((free_size < size) && (memory_ptr != pool->dm_search_ptr));
    
    /* Determine if the memory is available.  */
    if (free_size >= size)
    {
    
        /* A block that satisfies the request has been found.  */
        
        /* Determine if the block needs to be split.  */
        if (free_size >= (size + DM_OVERHEAD + pool->dm_min_allocation))
        {
        
            /* Yes, split the block.  */
            new_ptr =  (DM_HEADER *) (((CHAR *) memory_ptr) + size +
                                                DM_OVERHEAD);

            /* Mark the new block as free.  */
            new_ptr->dm_memory_free =  TRUE;
            
            /* Put the pool pointer into the new block.  */
            new_ptr->dm_memory_pool =  pool;

            /* Build the necessary pointers.  */
            new_ptr->dm_previous_memory =  memory_ptr;
            new_ptr->dm_next_memory =      memory_ptr->dm_next_memory;
            (new_ptr->dm_next_memory)->dm_previous_memory =  new_ptr;
            memory_ptr->dm_next_memory =   new_ptr;
            
            /* Decrement the available byte count.  */
            pool->dm_available =  pool->dm_available - size - DM_OVERHEAD;
        }
        else
        
            /* Decrement the entire free size from the available bytes 
               count.  */
            pool->dm_available =  pool->dm_available - free_size;

        /* Mark the allocated block as not available.  */
        memory_ptr->dm_memory_free =  FALSE;

        /* Should the search pointer be moved?   */
        if (pool->dm_search_ptr == memory_ptr)
        
            /* Move the search pointer to the next free memory slot.  */
            pool->dm_search_ptr =  memory_ptr->dm_next_memory;
        
        /* Return a memory address to the caller.  */
        *return_pointer =  (VOID *) (((CHAR *) memory_ptr) + DM_OVERHEAD);
    }
    else 
    {
        
	    /* No suspension requested.  Simply return an error status.  */
	    status =            OS_ERR_MEM_FULL;        
	    *return_pointer =   NULL;
    }
    
    /* Release protection of the memory pool.  */
    OS_EXIT_CRITICAL();

    /* Return the completion status.  */
    return(status);
}



INT8U OS_FreeMem(VOID *memory)
{

	R1 DM_PCB      *pool;                       /* Pool pointer              */
	R1 DM_HEADER   *header_ptr;                 /* Pointer to memory hdr     */
	R1 DM_HEADER   *new_ptr;                    /* New memory block pointer  */
	INT8U          status;                     /* Completion status         */
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif



    /* Initialize the status as successful.  */
    status =  OS_ERR_NONE;

    /* Pickup the associated pool's pointer.  It is inside the header of
       each memory.  */
    header_ptr =  (DM_HEADER *) (((CHAR *) memory) - DM_OVERHEAD);
    pool =        header_ptr->dm_memory_pool;
    
    /* Protect against simultaneous access to the memory pool.  */
    OS_ENTER_CRITICAL();
    
    /* Mark the memory as available.  */
    header_ptr->dm_memory_free =  TRUE;

    /* Adjust the available number of bytes.  */
    pool->dm_available =  pool->dm_available +
                        (((CHAR *) (header_ptr->dm_next_memory)) -
                           ((CHAR *) header_ptr)) - DM_OVERHEAD;

    /* Determine if the block can be merged with the previous neighbor.  */
    if ((header_ptr->dm_previous_memory)->dm_memory_free)
    {
    
        /* Adjust the available number of bytes.  */
        pool->dm_available =  pool->dm_available + DM_OVERHEAD;

        /* Yes, merge block with previous neighbor.  */
        (header_ptr->dm_previous_memory)->dm_next_memory =  
                                header_ptr->dm_next_memory;
        (header_ptr->dm_next_memory)->dm_previous_memory =
                                header_ptr->dm_previous_memory;
                                
        /* Move header pointer to previous.  */
        header_ptr =  header_ptr->dm_previous_memory;
        
        /* Adjust the search pointer to the new merged block.  */
        pool->dm_search_ptr =  header_ptr;
    }
    
    /* Determine if the block can be merged with the next neighbor.  */
    if ((header_ptr->dm_next_memory)->dm_memory_free)
    {
    
        /* Adjust the available number of bytes.  */
        pool->dm_available =  pool->dm_available + DM_OVERHEAD;

        /* Yes, merge block with next neighbor.  */
        new_ptr =  header_ptr->dm_next_memory;
        (new_ptr->dm_next_memory)->dm_previous_memory =
                                                header_ptr;
        header_ptr->dm_next_memory = new_ptr->dm_next_memory;

        /* Adjust the search pointer to the new merged block.  */
        pool->dm_search_ptr =  header_ptr;
    }
    
    /* Release protection of the memory pool.  */
    OS_EXIT_CRITICAL();

    /* Return the completion status.  */
    return(status);
}


static INT32U sysemPool[OS_DMEM_POOL_SIZE/4];  // maybe need relocated

void OS_DMemInit(void)
{
    /* Initialize the total number of created pools to 0.  */
    DMD_Total_Pools =  0;
    
    /* Create a memory pool  */
    OS_Create_Memory_Pool(&OS_system_mempool, "Sys", (void *)sysemPool, OS_DMEM_POOL_SIZE, 16);
}

void *OSMMalloc(INT32U size)
{
	INT8U err;
	void *pointer;

	err = OS_GetMem(&OS_system_mempool, &pointer, size);
	return (err == OS_ERR_NONE?pointer : NULL);
}

void *OSMCalloc(INT32U count, INT32U size)
{
  void *p;

  /* allocate 'count' objects of size 'size' */
  p = OSMMalloc(count * size);
  if (p) {
    /* zero the memory */
    memset(p, 0, count * size);
  }
  return p;
}

void  OSMFree(void *ptr)
{
	OS_FreeMem(ptr);
}

