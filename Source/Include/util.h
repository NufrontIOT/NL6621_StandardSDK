/*
******************************************************************************
**
** Project     : 
** Description :    This file contains wrappers for linked-list
** Author      :    NST                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/

#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <stdarg.h>

#define MEM_ALIGNMENT    4  /*4字节对齐*/
#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))  /* >=size而且为4的倍数的所有整数中最小的一个*/

//
//  Memory
//
//#define NST_MALLOC_MEM(size)   malloc(size)
//#define NST_FREE_MEM(pMem)     free(pMem)

//#define NST_MOVE_MEM(Destination, Source, Length)      memmove(Destination, Source, Length)
//#define NST_ZERO_MEM(Destination, Length)                   memset(Destination, 0, Length)
//#define NST_EQUAL_MEM(Source1, Source2, Length)        (!memcmp(Source1, Source2, Length))                            
#define NST_MOVE_MEM(Destination, Source, Length)     __aeabi_memcpy((INT8U*)Destination, (INT8U*)Source, (INT16U)Length) 
#define NST_ZERO_MEM(Destination, Length)                OS_MemClr((INT8U*)Destination, (INT16U)Length)
#define NST_EQUAL_MEM(Source1, Source2, Length)    (!NST_MemEqual((UINT8*)Source1, (UINT8*)Source2, (UINT16)Length))                          
#define NST_MoveBigMem  NST_MOVE_MEM

/*
    ========================================================================
    
    Routine Description:
        Compare two memory block

    Arguments:
        pSrc1       Pointer to first memory address
        pSrc2       Pointer to second memory address
        
    Return Value:
        0:          memory is equal
        1:          pSrc1 memory is larger
        2:          pSrc2 memory is larger

    IRQL = DISPATCH_LEVEL
    
    Note:
        
    ========================================================================
*/
static INLINE UINT32   NSTCompareMemory(
    IN  PVOID   pSrc1,
    IN  PVOID   pSrc2,
    IN  UINT32   Length)
{
    PUINT8  pMem1;
    PUINT8  pMem2;
    UINT32   Index = 0;

    pMem1 = (PUINT8) pSrc1;
    pMem2 = (PUINT8) pSrc2;

    for (Index = 0; Index < Length; Index++)
    {
        if (pMem1[Index] > pMem2[Index])
            return (1);
        else if (pMem1[Index] < pMem2[Index])
            return (2);
    }

    // Equal
    return (0);
}

#define NST_DELAY(TimeLen)   OSTimeDly(TimeLen) /*tick*/
//
//  Timer
//
typedef OS_TMR NST_TIMER;

VOID NST_InitTimer(
    IN  NST_TIMER **    pTimer,
    IN  OS_TMR_CALLBACK     pTimerFunc,
    IN  PVOID                   pData,
    IN  BOOL_T      Repeat);
    
VOID NST_DelTimer(
    IN  NST_TIMER **    pTimer,
    OUT BOOL_T      *pCancelled);

VOID NST_SetTimer(
    IN  NST_TIMER *    pTimer,
    IN  UINT32      Value);

VOID NST_CancelTimer(
    IN  NST_TIMER *    pTimer,
    OUT BOOL_T          *pCancelled);
    
VOID    NST_ModTimer(
    IN    NST_TIMER    *pTimer,
    IN    UINT32                    Value);

//
//  Lock
//
typedef OS_EVENT    NST_LOCK;

VOID NST_ALLOC_LOCK(
    OUT  NST_LOCK **  Lock);

VOID NST_FREE_LOCK(
    IN  NST_LOCK *  Lock);

VOID NST_AQUIRE_LOCK(
    IN  NST_LOCK *  Lock);

VOID NST_RELEASE_LOCK(
    IN  NST_LOCK *  Lock);


#define NST_SCHED_LOCK()   OSSchedLock()
#define NST_SCHED_UNLOCK()   OSSchedUnlock()

#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
#define  CPU_SR_ALLOC()             OS_CPU_SR  cpu_sr = (OS_CPU_SR)0
#else
#define  CPU_SR_ALLOC()
#endif

#define NST_IRQ_LOCK() OS_ENTER_CRITICAL()
#define NST_IRQ_UNLOCK() OS_EXIT_CRITICAL()

//
//  Endian byte swapping codes
//
// Endian byte swapping codes
#define SWAP16(x) \
    ((UINT16)( \
    (((UINT16)(x) & (UINT16) 0x00ffU) << 8) | \
    (((UINT16)(x) & (UINT16) 0xff00U) >> 8) ))

#define SWAP32(x) \
    ((UINT32)( \
    (((UINT32)(x) & (UINT32) 0x000000ffUL) << 24) | \
    (((UINT32)(x) & (UINT32) 0x0000ff00UL) <<  8) | \
    (((UINT32)(x) & (UINT32) 0x00ff0000UL) >>  8) | \
    (((UINT32)(x) & (UINT32) 0xff000000UL) >> 24) ))
    
#ifdef NST_BIG_ENDIAN
#define cpu2le64(x) SWAP64((x))
#define le2cpu64(x) SWAP64((x))
#define cpu2le32(x) SWAP32((x))
#define le2cpu32(x) SWAP32((x))
#define cpu2le16(x) SWAP16((x))
#define le2cpu16(x) SWAP16((x))
//#define cpu2be64(x) ((UINT64)(x))
//#define be2cpu64(x) ((UINT64)(x))
#define cpu2be32(x) ((UINT32)(x))
#define be2cpu32(x) ((UINT32)(x))
#define cpu2be16(x) ((UINT16)(x))
#define be2cpu16(x) ((UINT16)(x))
#else   // Little_Endian
//#define cpu2le64(x) ((UINT64)(x))
//#define le2cpu64(x) ((UINT64)(x))
#define cpu2le32(x) ((UINT32)(x))
#define le2cpu32(x) ((UINT32)(x))
#define cpu2le16(x) ((UINT16)(x))
#define le2cpu16(x) ((UINT16)(x))
#define cpu2be64(x) SWAP64((x))
#define be2cpu64(x) SWAP64((x))
#define cpu2be32(x) SWAP32((x))
#define be2cpu32(x) SWAP32((x))
#define cpu2be16(x) SWAP16((x))
#define be2cpu16(x) SWAP16((x))
#endif  // NST_BIG_ENDIAN

//
// Time Relation
//
#define NST_TIME_AFTER_EQ(a,b) ((INT32)(a) - (INT32)(b) >= 0)
#define NST_TIME_BEFORE(a,b)   NST_TIME_AFTER_EQ(b,a)
#define NST_TIME_AFTER(a,b) ((INT32)(a) - (INT32)(b) > 0)


//
// Debug Print
//
VOID DBGPRINT_MGMT(UINT8 level, UINT8 *fmt, ...);
void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);

#define DEBUG_ON         1   // Global debug on-off
#define DEBUG_LEVEL      DEBUG_TRACE  // control output level, lower values indicate higher urgency
#define DEBUG_OFF	     0
#define DEBUG_ERROR     1
#define DEBUG_WARN      2
#define DEBUG_TRACE     3
#define DEBUG_INFO      4

// Submodule debug on-off
//#define DEBUG_AP
//#define DEBUG_SCAN  
//#define DEBUG_CONN  
//#define DEBUG_WPA
//#define DEBUG_ACTION
//#define DEBUG_WSC
//#define DEBUG_DATA
//#define DEBUG_ENCRY
//#define DEBUG_DH
//#define DEBUG_CFG
//#define DEBUG_RATE
//#define DEBUG_PS
//#define DEBUG_LWIP
//#define DEBUG_DC

#define DBGPRINT DBGPRINT_MGMT

#ifdef DEBUG_AP
#define DBGPRINT_AP DBGPRINT_MGMT
#else
#define DBGPRINT_AP(level, fmt, ...)
#endif // DEBUG_AP

#ifdef DEBUG_SCAN
#define DBGPRINT_SCAN DBGPRINT_MGMT
#else
#define DBGPRINT_SCAN(level, fmt, ...)
#endif  //  DEBUG_SCAN

#ifdef DEBUG_CONN
#define DBGPRINT_CONN DBGPRINT_MGMT
#else
#define DBGPRINT_CONN(level, fmt, ...)
#endif  //  DEBUG_CONN

#ifdef DEBUG_ACTION
#define DBGPRINT_ACTION DBGPRINT_MGMT
#else
#define DBGPRINT_ACTION(level, fmt, ...)
#endif  //  DEBUG_ACTION

#ifdef DEBUG_WPA
#define DBGPRINT_WPA DBGPRINT_MGMT
#else
#define DBGPRINT_WPA(level, fmt, ...)
#endif  //  DEBUG_ACTION

#ifdef DEBUG_WSC
#define DBGPRINT_WSC DBGPRINT_MGMT
#else
#define DBGPRINT_WSC(level, fmt, ...)
#endif  //  DEBUG_WSC

#ifdef DEBUG_DATA
#define DBGPRINT_DATA DBGPRINT_MGMT
#else
#define DBGPRINT_DATA(level, fmt, ...)
#endif  //  DEBUG_DATA

#ifdef DEBUG_ENCRY
#define DBGPRINT_ENCRY DBGPRINT_MGMT
#else
#define DBGPRINT_ENCRY(level, fmt, ...)
#endif  //  DEBUG_ENCRY

#ifdef DEBUG_DH
#define DBGPRINT_DH DBGPRINT_MGMT
#else
#define DBGPRINT_DH(level, fmt, ...)
#endif  //  DEBUG_DH

#ifdef DEBUG_CFG
#define DBGPRINT_CFG DBGPRINT_MGMT
#else
#define DBGPRINT_CFG(level, fmt, ...)
#endif  //  DEBUG_CFG

#ifdef DEBUG_RATE
#define DBGPRINT_RATE DBGPRINT_MGMT
#else
#define DBGPRINT_RATE(level, fmt, ...)
#endif  //  DEBUG_RATE

#ifdef DEBUG_PS
#define DBGPRINT_PS DBGPRINT_MGMT
#else
#define DBGPRINT_PS(level, fmt, ...)
#endif  //  DEBUG_PS

#ifdef DEBUG_LWIP
#define DBGPRINT_LWIP DBGPRINT_MGMT
#else
#define DBGPRINT_LWIP(level, fmt, ...)
#endif  //  DEBUG_LWIP

#ifdef DEBUG_DC
#define DBGPRINT_DC DBGPRINT_MGMT
#else
#define DBGPRINT_DC(level, fmt, ...)
#endif  //  DEBUG_DC

#define ASSERT_ON         0   // control assert on/off

#if ASSERT_ON
#define ASSERT(exp) do { if(!(exp))   DBGPRINT_MGMT(DEBUG_ERROR, "!!! ERR: %s(%d) ASSERT FAILED!\n", __FILE__, __LINE__);} while (0)
#else
/* ASSERT could cause segmentation fault on GCC3.1, use empty instead */
#define ASSERT(exp)
#endif              /* GCC_VERSION > 30100 */

#ifndef MIN
#define MIN(_a, _b)     (((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef MAX
#define MAX(_a, _b)     (((_a) > (_b)) ? (_a) : (_b))
#endif

#define Nst_Islower(c)	('a' <= (c) && (c) <= 'z')
#define Nst_ToUpper(c)	(Nst_Islower(c) ? ((c) - 'a' + 'A') : (c))


/* lib c string operation helper */
char *strcpy(char *dest, const char *src);
char *strcat (char *dest, const char *src);
INT32U strspn (const char *s, const char *accept);
char *strncpy (char *s1, const char *s2, INT32U n);
char *strpbrk(const char *s1, const char *s2);

VOID __aeabi_memcpy(UINT8 *pDest, UINT8  *pSrc, UINT32  Size);
VOID __aeabi_memcpy4(UINT8 *pDest, UINT8  *pSrc, UINT32  Size);

VOID NST_MoveMemOvlap(UINT8  *pDst, UINT8  *pSrc, UINT16  Size);
//VOID NST_MoveBigMem(UINT8  *pDst, UINT8  *pSrc, UINT16  Size);
VOID NST_DmaMoveMem(UINT8  *pDst, UINT8  *pSrc, UINT16  Size);

//
//  Return Status Definitions
//
#define NST_STATUS                             INT32

#define NST_STATUS_SUCCESS                         0x00
#define NST_STATUS_FAILURE                         0x01
#define NST_STATUS_INVALID_DATA            0x02
#define NST_STATUS_RESOURCES                   0x03
#define NST_STATUS_PENDING                         0x04
#define NST_STATUS_NOT_SUPPORTED           0x05
#define NST_STATUS_NOT_ACCEPTED            0x06
#define NST_STATUS_BUFFER_TOO_SHORT    0x07
#define NST_STATUS_INVALID_LENGTH          0x08

/** Circular doubly linked list */
typedef struct _NST_linked_list
{
    /** Pointer to previous node */
    struct _NST_linked_list *pprev;
    /** Pointer to next node */
    struct _NST_linked_list *pnext;
} NST_linked_list, *pNST_linked_list;

/** List head */
typedef struct _NST_list_head
{
    /** Pointer to previous node */
    struct _NST_linked_list *pprev;
    /** Pointer to next node */
    struct _NST_linked_list *pnext;

    UINT16   count;/* 链表中的元素个数*/

    BOOL_T   lock_required; /*是否给队列操作加锁*/
} NST_list_head, *pNST_list_head;

/*
******************************************************************************
**      static INLINE VOID util_init_list(NST_linked_list* phead)
**
** Description  : init list
** Arguments    : 
                  phead  queue's header pointer
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE VOID util_init_list(NST_linked_list* phead)
{
    /* Both next and prev point to self */
    phead->pprev= phead->pnext = phead;
}

/*
******************************************************************************
**      static INLINE VOID util_init_list_head(NST_list_head* phead, BOOL_T lock_required)
**
** Description  : init list
** Arguments    : 
                  phead   list head
                  lock_required    A flag for lock requirement
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE VOID util_init_list_head(NST_list_head* phead, BOOL_T lock_required)
{
    /* Both next and prev point to self */
    util_init_list((NST_linked_list*) phead);
    
    phead->count = 0;
    phead->lock_required = lock_required;
}

/*
******************************************************************************
**      static INLINE VOID util_free_list_head(NST_list_head* phead)
**
** Description  : free a list
** Arguments    : 
                  phead  list head
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE VOID util_free_list_head(NST_list_head* phead)
{
    phead->pprev = phead->pnext = 0;
}

/*
******************************************************************************
**      static INLINE NST_linked_list* util_peek_list(NST_list_head* phead)
**
** Description  : peek a node from the list
** Arguments    : 
                  phead:  list head
                  
** Returns      : list node
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE NST_linked_list* util_peek_list(NST_list_head* phead)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = (OS_CPU_SR)0;
#endif 
    NST_linked_list* pnode = NULL;
    
    if(phead->lock_required)
        OS_ENTER_CRITICAL();

    if (phead->pnext != (NST_linked_list*) phead) 
    {
        pnode = phead->pnext;
    }

    if(phead->lock_required)
        OS_EXIT_CRITICAL();

    return pnode;
}

/*
******************************************************************************
**      static INLINE VOID util_enqueue_list_tail(NST_list_head* phead, NST_linked_list* pnode)
**
** Description  : queues a node at the list tail
** Arguments    : 
                  phead:  list head
                  pnode:  list node to be inserted
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE VOID util_enqueue_list_tail(NST_list_head* phead, NST_linked_list* pnode)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = (OS_CPU_SR)0;
#endif 
    NST_linked_list* pold_last;
    
    if(phead->lock_required)
        OS_ENTER_CRITICAL();

    pold_last = phead->pprev;
    pnode->pprev = pold_last;
    pnode->pnext = (NST_linked_list*) phead;
    
    phead->pprev = pold_last->pnext = pnode;
    phead->count++;
    
    if(phead->lock_required)
        OS_EXIT_CRITICAL();
}

/*
******************************************************************************
**      static INLINE VOID util_enqueue_list_head(NST_list_head* phead, NST_linked_list* pnode)
**
** Description  : queues a node at the list header
** Arguments    : 
                  phead:  list head
                  pnode:  list node to be inserted
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE VOID util_enqueue_list_head(NST_list_head* phead, NST_linked_list* pnode)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = (OS_CPU_SR)0;
#endif 
    NST_linked_list* pold_first;
    
    if(phead->lock_required)
        OS_ENTER_CRITICAL();
    
    pold_first = phead->pnext;
    pnode->pprev = (NST_linked_list*) phead;
    pnode->pnext = pold_first;
    
    phead->pnext = pold_first->pprev = pnode;
    phead->count++;
    
    if(phead->lock_required)
        OS_EXIT_CRITICAL();
}

/*
******************************************************************************
**      static INLINE VOID util_unlink_list( NST_list_head* phead,  pNST_linked_list pnode)
**
** Description  : 
** Arguments    : 
                  phead:  list head
                  pnode:  list node to be removed
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE VOID util_unlink_list( NST_list_head* phead,  NST_linked_list* pnode)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = (OS_CPU_SR)0;
#endif 
    NST_linked_list* pmy_prev;
    NST_linked_list* pmy_next;

    if(phead->lock_required)
        OS_ENTER_CRITICAL();
    
    pmy_prev = pnode->pprev;
    pmy_next = pnode->pnext;
    pmy_next->pprev = pmy_prev;
    pmy_prev->pnext = pmy_next;
    phead->count--;
    
    pnode->pnext = pnode->pprev = 0;
    if(phead->lock_required)
        OS_EXIT_CRITICAL();
}

/*
******************************************************************************
**      static INLINE NST_linked_list* util_dequeue_list(NST_list_head* phead)
**
** Description  : dequeues a node from the list's head
** Arguments    : 
                  phead:  list head
                  
** Returns      : list node to be removed
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE NST_linked_list* util_dequeue_list(NST_list_head* phead)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = (OS_CPU_SR)0;
#endif 
    NST_linked_list *pnode;
    
    if(phead->lock_required)
        OS_ENTER_CRITICAL();

    pnode = phead->pnext;
    if (pnode && (pnode != (NST_linked_list*) phead)) 
    {
        util_unlink_list(phead, pnode);
    } 
    else 
    {
        pnode = NULL;
    }
    
    if(phead->lock_required)
        OS_EXIT_CRITICAL();

    return pnode;
}

/*
******************************************************************************
**      static INLINE VOID util_init_list(NST_list_head* phead)
**
** Description  : init list
** Arguments    : 
                  phead  queue's header pointer
                  
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE BOOL_T util_is_list_empty(NST_list_head* phead)
{
    return phead->count == 0 ? NST_TRUE : NST_FALSE;
}

/*
******************************************************************************
**      static INLINE BOOL_T  NST_MemEqual (UINT8  *pSrc1, UINT8  *pSrc2, UINT16  Len)
**
** Description  : memory compare
** Arguments    : 
** Returns      : 
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
static INLINE BOOL_T  NST_MemEqual (UINT8  *pSrc1, UINT8  *pSrc2, UINT16  Len)
{
    while (Len-- > 0u) 
    {
        if(*pSrc1++  != *pSrc2++)
            return  NST_TRUE;
    }

    return NST_FALSE;
}

#define xxxxxxxxxxxxxxxxxxxx

INLINE VOID GetSystemUpTime(UINT32 *time)
{    
    UINT32 temp = 0;
    
    //LSB_TIME
    temp= NST_RD_PWM_REG(ADDR_LSB_TIME);
    //MSB_TIME bit 12:0
    //NST_RD_PWM_REG(ADDR_MSB_TIME) & 0x1FFF;

    //128us  -> 1ms
    *time = temp/8;
}

INLINE VOID NSTusecDelay(UINT32 usec)
{
    UINT32 i;
    UINT8 CNT = 30;

    if (CpuClkFreq == CPU_CLK_FREQ_160M)
        CNT = 30;
    else if (CpuClkFreq == CPU_CLK_FREQ_120M)
        CNT = 23;
    else if (CpuClkFreq == CPU_CLK_FREQ_80M)
        CNT = 15;
    else if (CpuClkFreq == CPU_CLK_FREQ_40M)
        CNT = 8;
        
    for (i = 0; i < (usec*CNT); i++)
        ;
}

INT8U NST_GetCrc8(INT8U *ptr, INT32U len);

INT32 NST_GetCrc32(UINT8 *pData, INT32 DataSize);

UINT8 BtoH(char ch);

void AtoH(PSTRING src, PUINT8 dest, int destlen);

#endif //_UTIL_H
