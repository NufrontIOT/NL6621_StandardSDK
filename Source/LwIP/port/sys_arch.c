/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#define SYS_ARCH_GLOBALS

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"

#include "include/arch/sys_arch.h"

static OS_MEM *pQueueMem;

const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;

static char pcQueueMemoryPool[MAX_QUEUES * sizeof(TQ_DESCR) + MEM_ALIGNMENT - 1];

//SYS_ARCH_EXT OS_STK LWIP_TASK_STK[LWIP_TASK_MAX][LWIP_STK_SIZE];

/* Message queue constants. */
#define archMESG_QUEUE_LENGTH	( 6 )
#define archPOST_BLOCK_TIME_MS	( ( unsigned portLONG ) 10000 )

/*-----------------------------------------------------------------------------------*/
//  Creates an empty mailbox.
signed char _sys_mbox_new(sys_mbox_t *mbox, int size)
{
    // prarmeter "size" can be ignored in your implementation.
    u8_t       ucErr;
    PQ_DESCR    pQDesc;
    
    *mbox = SYS_MBOX_NULL;
    
    pQDesc = OSMemGet( pQueueMem, &ucErr );
    LWIP_ASSERT("OSMemGet ", ucErr == OS_ERR_NONE );
    
    if( ucErr == OS_ERR_NONE ) 
    {
        if(size > MAX_QUEUE_ENTRIES ) // 邮箱最多容纳MAX_QUEUE_ENTRIES消息数目
            size = MAX_QUEUE_ENTRIES;
        
        pQDesc->pQ = OSQCreate( &(pQDesc->pvQEntries[0]), size ); 
        LWIP_ASSERT( "OSQCreate ", pQDesc->pQ != NULL );
        
        if( pQDesc->pQ != NULL ) 
        {
            *mbox = pQDesc;
            return ERR_OK; 
        }
        else
        {
            ucErr = OSMemPut( pQueueMem, pQDesc );
            return ERR_MEM;
        }
    }
    else 
        return ERR_MEM;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void _sys_mbox_free(sys_mbox_t *mbox)
{
    u8_t     ucErr;
    
    LWIP_ASSERT( "sys_mbox_free ", *mbox != SYS_MBOX_NULL );    
    
    //clear OSQ EVENT
    OSQFlush( (*mbox)->pQ );
    
    //del OSQ EVENT
    (void)OSQDel( (*mbox)->pQ, OS_DEL_NO_PEND, &ucErr);
    LWIP_ASSERT( "OSQDel ", ucErr == OS_ERR_NONE );
    
    //put mem back to mem queue
    ucErr = OSMemPut( pQueueMem, (*mbox) );
    LWIP_ASSERT( "OSMemPut ", ucErr == OS_ERR_NONE );  
}

/*-----------------------------------------------------------------------------------*/
//   Posts the "msg" to the mailbox.
void _sys_mbox_post(sys_mbox_t *mbox, void *msg)
{   
    u8_t ubErr,i=0;

    ubErr = ubErr; // remove warning
    
    if(msg == NULL ) 
        msg = (void*)&pvNullPointer;

    while((i<10) && ((ubErr = OSQPost( (*mbox)->pQ, msg)) != OS_ERR_NONE))
    {
        i++;//if full, try 10 times
        OSTimeDly(5);
    }
    //if (i==10) printf("sys_mbox_post error!\n");
}

// Try to post the "msg" to the mailbox.
signed char _sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    u8_t ubErr;

    ubErr = ubErr; // remove warning

    if(msg == NULL ) msg = (void*)&pvNullPointer;
    
    if((ubErr = OSQPost( (*mbox)->pQ, msg)) != OS_ERR_NONE)
        return ERR_MEM;

    return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP. 
*/
u32_t _sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    u8_t	ucErr;
    u32_t	ucos_timeout, timeout_new;
    void	*temp;

    if(timeout != 0)
    {
        ucos_timeout = (timeout * OS_TICKS_PER_SEC)/1000; //convert to timetick

        if(ucos_timeout < 1)
            ucos_timeout = 1;
        else if(ucos_timeout > 65535)    //ucOS only support u16_t timeout
            ucos_timeout = 65535;
    }
    else 
        ucos_timeout = 0;

    timeout = OSTimeGet();

    temp = OSQPend( (*mbox)->pQ, (u16_t)ucos_timeout, &ucErr );

    if(msg != NULL)
    {
        if( temp == (void*)&pvNullPointer )
            *msg = NULL;
        else
            *msg = temp;
    }   

    if(ucErr == OS_ERR_TIMEOUT ) 
        timeout = SYS_ARCH_TIMEOUT;
    else
    {
        LWIP_ASSERT( "OSQPend ", ucErr == OS_ERR_NONE );

        timeout_new = OSTimeGet();
        if (timeout_new>timeout) timeout_new = timeout_new - timeout;
        else timeout_new = 0xffffffff - timeout + timeout_new;

        timeout = timeout_new * 1000 / OS_TICKS_PER_SEC + 1; //convert to milisecond
    }

    return timeout; 
}

/*-----------------------------------------------------------------------------------*/
//  Creates and returns a new semaphore. The "count" argument specifies
//  the initial state of the semaphore. TBD finish and test
signed char _sys_sem_new(sys_sem_t *sem, u8_t count)
{
    *sem = OSSemCreate((u16_t)count);
    LWIP_ASSERT("OSSemCreate ",*sem != NULL );
    if(*sem != NULL)
        return ERR_OK;
    else
        return ERR_MEM;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t _sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    u8_t ucErr;
    u32_t ucos_timeout, timeout_new;
    
    // timeout 单位以ms计 转换为ucos_timeout 单位以TICK计
    if(timeout != 0)
    {
        ucos_timeout = (timeout * OS_TICKS_PER_SEC) / 1000; // convert to timetick
        if(ucos_timeout < 1)
            ucos_timeout = 1;
        else if(ucos_timeout > 65536) // uC/OS only support u16_t pend
            ucos_timeout = 65535; // 最多等待TICK数 这是uC/OS所能 处理的最大延时
    }
    else 
        ucos_timeout = 0;
    
    timeout = OSTimeGet(); // 记录起始时间
    
    OSSemPend ((OS_EVENT *)(*sem),(u16_t)ucos_timeout, (u8_t *)&ucErr);
    
    if(ucErr == OS_ERR_TIMEOUT)
        timeout = SYS_ARCH_TIMEOUT;    // only when timeout!
    else
    {    
        //LWIP_ASSERT( "OSSemPend ", ucErr == OS_ERR_NONE );
        //for pbuf_free, may be called from an ISR
        
        timeout_new = OSTimeGet(); // 记录终止时间
        if (timeout_new>=timeout) 
            timeout_new = timeout_new - timeout;
        else 
            timeout_new = 0xffffffff - timeout + timeout_new;
        
        timeout = (timeout_new * 1000 / OS_TICKS_PER_SEC + 1); //convert to milisecond 为什么加1？
    }
    
    return timeout;
}

/*-----------------------------------------------------------------------------------*/
// Signals a semaphore
void _sys_sem_signal(sys_sem_t *sem)
{
 //   u8_t ucErr;
    /*ucErr =*/ OSSemPost((OS_EVENT *)(*sem));
    
    // May be called when a connection is already reset, should not check...
    // LWIP_ASSERT( "OSSemPost ", ucErr == OS_ERR_NONE );
}

/*-----------------------------------------------------------------------------------*/
// Deallocates a semaphore
void _sys_sem_free(sys_sem_t *sem)
{
    u8_t     ucErr;
    (void)OSSemDel( (OS_EVENT *)(*sem), OS_DEL_ALWAYS, &ucErr );
    LWIP_ASSERT( "OSSemDel ", ucErr == OS_ERR_NONE );
}

/*-----------------------------------------------------------------------------------*/
// Initialize sys arch
void _sys_init(void)
{
    u8_t ucErr;

    // init mem used by sys_mbox_t, use ucosII functions
    // 指定内存起始地址以4字节对齐
    pQueueMem = OSMemCreate( 
                            (void*)((u32_t)((u32_t)pcQueueMemoryPool+MEM_ALIGNMENT-1) & ~(MEM_ALIGNMENT-1)), 
                            MAX_QUEUES, sizeof(TQ_DESCR), &ucErr 
                                );

    LWIP_ASSERT( "OSMemCreate ", ucErr == OS_ERR_NONE );
}
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
// TBD 
/*-----------------------------------------------------------------------------------*/
/*
  Starts a new thread with priority "prio" that will begin its execution in the
  function "thread()". The "arg" argument will be passed as an argument to the
  thread() function. The id of the new thread is returned. Both the id and
  the priority are system dependent.
*/
sys_thread_t _sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    u8_t ubPrio = 0;
    u8_t ucErr;

    arg = arg;

    if((prio > 0) && (prio <= LWIP_TASK_MAX))
    {
        ubPrio = (u8_t)(LWIP_START_PRIO + prio - 1);

        if(stacksize > LWIP_STK_SIZE)   // 任务堆栈大小不超过LWIP_STK_SIZE
        stacksize = LWIP_STK_SIZE;

#ifndef STK_CHECK
        OSTaskCreate(thread, (void *)arg, &LWIP_TASK_STK[prio-1][stacksize-1],ubPrio);
#else
        OSTaskCreateExt(thread, (void *)arg, &LWIP_TASK_STK[prio-1][stacksize-1],ubPrio
                    ,ubPrio,&LWIP_TASK_STK[prio-1][0],stacksize,(void *)0,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#endif
        OSTaskNameSet(ubPrio, (u8_t*)name, &ucErr);

        //        return ubPrio;
    }
    return ubPrio;
}

/** Check if an mbox is valid/allocated: return 1 for valid, 0 for invalid */
int _sys_mbox_valid(sys_mbox_t *mbox)
{
    return (*mbox != SYS_MBOX_NULL);
}

/** Set an mbox invalid so that sys_mbox_valid returns 0 */
void _sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    *mbox = SYS_MBOX_NULL;
}

/** Check if a sempahore is valid/allocated: return 1 for valid, 0 for invalid */
int _sys_sem_valid(sys_sem_t *sem)
{
    return (*sem != SYS_SEM_NULL);
}

/** Set a semaphore invalid so that sys_sem_valid returns 0 */
void _sys_sem_set_invalid(sys_sem_t *sem)
{
    *sem = SYS_SEM_NULL;
}


