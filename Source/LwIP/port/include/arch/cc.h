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
#ifndef __CC_H__
#define __CC_H__

#include "cpu.h"
#include "stdio.h"

/*-------------data type------------------------------------------------------*/

typedef unsigned   char    u8_t;    /* Unsigned 8 bit quantity         */
typedef signed     char    s8_t;    /* Signed    8 bit quantity        */
typedef unsigned   short   u16_t;   /* Unsigned 16 bit quantity        */
typedef signed     short   s16_t;   /* Signed   16 bit quantity        */
typedef unsigned   long    u32_t;   /* Unsigned 32 bit quantity        */
typedef signed     long    s32_t;   /* Signed   32 bit quantity        */
typedef u32_t mem_ptr_t;            /* Unsigned 32 bit quantity        */
//typedef int sys_prot_t;

/*-------------critical region protection (depends on uC/OS-II setting)-------*/

#define SYS_ARCH_DECL_PROTECT(lev)	u32_t lev
#define SYS_ARCH_PROTECT(lev)    	lev = gpRomHookFuncTbl->OS_CPU_SR_Save()
#define SYS_ARCH_UNPROTECT(lev)    	gpRomHookFuncTbl->OS_CPU_SR_Restore(lev)

#define sys_mbox_new(mbox, size)    	gpRomHookFuncTbl->_sys_mbox_new(mbox, size)
#define sys_mbox_free(mbox)             gpRomHookFuncTbl->_sys_mbox_free(mbox)
#define sys_mbox_post(mbox, msg)        gpRomHookFuncTbl->_sys_mbox_post(mbox, msg)
#define sys_mbox_trypost(mbox, msg)     gpRomHookFuncTbl->_sys_mbox_trypost(mbox, msg)
#define sys_arch_mbox_fetch(mbox, msg, tm)    gpRomHookFuncTbl->_sys_arch_mbox_fetch(mbox, msg, tm)
#define sys_mbox_valid(mbox)            gpRomHookFuncTbl->_sys_mbox_valid(mbox)
#define sys_mbox_set_invalid(mbox)      gpRomHookFuncTbl->_sys_mbox_set_invalid(mbox)

#define sys_sem_new(sem, s)            gpRomHookFuncTbl->_sys_sem_new(sem, s)
#define sys_sem_free(sem)      gpRomHookFuncTbl->_sys_sem_free(sem)
#define sys_arch_sem_wait(sem, t)      gpRomHookFuncTbl->_sys_arch_sem_wait(sem, t)
#define sys_sem_signal(sem)      gpRomHookFuncTbl->_sys_sem_signal(sem)
#define sys_sem_valid(sem)      gpRomHookFuncTbl->_sys_sem_valid(sem)
#define sys_sem_set_invalid(sem)      gpRomHookFuncTbl->_sys_sem_set_invalid(sem)
#define sys_init()      gpRomHookFuncTbl->_sys_init()
#define sys_thread_new(n, t, a, s, p)      gpRomHookFuncTbl->_sys_thread_new(n, t, a, s, p)


/*----------------------------------------------------------------------------*/

/* define compiler specific symbols */
#if defined (__ICCARM__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT 
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES

#elif defined (__CC_ARM)

#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT 
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined (__GNUC__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined (__TASKING__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#endif

/*---define (sn)printf formatters for these lwip types, for lwip DEBUG/STATS--*/

#define U16_F "4d"
#define S16_F "4d"
#define X16_F "4x"
#define U32_F "8ld"
#define S32_F "8ld"
#define X32_F "8lx"

#define LWIP_NOASSERT

/*--------------macros--------------------------------------------------------*/
#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) \
    do \
    {   printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
    } while(0)
#endif

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do {printf x;} while(0)
#endif

#define RX_PBUF    1
#define TX_PBUF     2

#endif /* __CC_H__ */
