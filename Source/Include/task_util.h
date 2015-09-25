/*
******************************************************************************
**
** Project     : 
** Description :    RX/TX header file
** Author      :    NST                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/
#ifndef _TSK_UTIL_H
#define _TSK_UTIL_H

#include "util.h"


/*管理内存缓冲池的控制单元*/
typedef struct _NST_Cell
{
    struct _NST_Cell *pprev; /*指向前一个节点*/
    struct _NST_Cell *pnext; /*指向下一个节点*/
    /*pbufHead为指向内存缓存池数据的指针，初始化后再也不能修改该
    指针的值，否则会导致内存资源丢失，操作这片内存时最好定义
    一个本地的指针来操作,防止不消息修改了该指针*/    
    VOID* pbufHead; 
}  NST_Cell, *PNST_Cell;

#define MlmeBuf_SubType_CMD     0
#define MlmeBuf_SubType_RX      1
/*MLME对外接口队列的节点单元*/
typedef struct _NST_MLMEIntfCell //不要修改下列顺序
{
    struct _NST_MLMEIntfCell *pprev; /*指向前一个节点*/
    struct _NST_MLMEIntfCell *pnext; /*指向下一个节点*/
    /*pbufHead为指向内存缓存池数据的指针，初始化后再也不能修改该
    指针的值，否则会导致内存资源丢失，操作这片内存时最好定义
    一个本地的指针来操作,防止不消息修改了该指针*/    
    VOID* pbufHead; 
    // removed by pchn, use pbufHead's first two bytes as pMLMEBuf's offset from pbufHead
//    VOID* pMLMEBuf; // MLME使用这个指针获得接口数据
}  NST_MLMEIntfCell, *PNST_MLMEIntfCell;

typedef  NST_list_head  NST_CellQueue;

typedef NST_CellQueue QUEUE_HEADER;
typedef NST_CellQueue* PQUEUE_HEADER;
typedef NST_Cell*  PQUEUE_ENTRY;

typedef void                * PNDIS_PACKET;

/*
******************************************************************************
**        static INLINE UINT8 NST_GetMsg (OS_EVENT  *pMsgQ, VOID **pMess)
**
** Description    : get message from msg queue head
** Arguments    : 
                  pMsgQ:  msg queue
                  pMess:   msg to be returned
** Returns        : 无
** Author        :                                     
** Date         : 
**
******************************************************************************
*/
static INLINE UINT8 NST_GetMsg (OS_EVENT  *pMsgQ, VOID **pMess)
{
    UINT8 err;

    *pMess = OSQPend(pMsgQ, 0, &err);

  //  ASSERT(*pMess);
    
    if(err != OS_ERR_NONE)
        DBGPRINT(DEBUG_ERROR, "ERR:  NST_GetMsg FAILED 0x%x\n", pMsgQ);

    return err;
    
}

/*
******************************************************************************
**        static INLINE UINT8 NST_SendMsg (OS_EVENT  *pMsgQ, VOID  *pMess)
**
** Description    : send msg into msg queue
** Arguments    : 
                  pMsgQ: msg queue
                  pMess: msg to be inserted
                  
** Returns        : 无
** Author        :                                     
** Date         : 
**
******************************************************************************
*/
static INLINE UINT8 NST_FreeTskMsg(NST_TskMsg* pTskMsg);
static INLINE NST_TskMsg* NST_AllocTskMsg(VOID);

extern UINT32 SendMsgErrCnt;

static INLINE UINT8 NST_SendMsg (OS_EVENT  *pMsgQ, VOID  *pMess)
{
    UINT8 retCode;
    NST_TskMsg* pMsg = (NST_TskMsg*)pMess;
 //   UINT16 MsgId = pMsg->msgId;
    ASSERT(pMess);
    retCode = OSQPost(pMsgQ, pMess);
    
 //   DBGPRINT(DEBUG_INFO, " @@@@ INFO:  NST_SendMsg , MsgId_%d !\n", MsgId);
    
    if(retCode != OS_ERR_NONE)
    {   
        SendMsgErrCnt++;
     //   DBGPRINT(DEBUG_ERROR, "ERR:  NST_SendMsg FAILED, MsgId = %d retCode = %d !\n", pMsg->msgId, retCode);
        NST_FreeTskMsg(pMsg); //TODO if msg's body need release
    }

    return retCode;
}

static INLINE UINT8 NST_SendHighPrioMsg (OS_EVENT  *pMsgQ, VOID  *pMess)
{
    UINT8 retCode;
    NST_TskMsg* pMsg = (NST_TskMsg*)pMess;
 //   UINT16 MsgId = pMsg->msgId;
    ASSERT(pMess);
    retCode = OSQPostFront(pMsgQ, pMess);
    
 //   DBGPRINT(DEBUG_INFO, " @@@@ INFO:  NST_SendMsg , MsgId_%d !\n", MsgId);
    
    if(retCode != OS_ERR_NONE)
    {   
        SendMsgErrCnt++;
       // DBGPRINT(DEBUG_ERROR, "ERR:  NST_SendMsg FAILED, MsgId = %d retCode = %d !\n", pMsg->msgId, retCode);
        NST_FreeTskMsg(pMsg); //TODO if msg's body need release
    }

    return retCode;
}

static INLINE UINT8 NST_PostHighPrioMsg (OS_EVENT  *pMsgQ, UINT16 MsgId)
{
    NST_TskMsg* pMsg;
    
    pMsg = NST_AllocTskMsg();
    if(pMsg)
    {
        pMsg->msgId = MsgId;
        return NST_SendHighPrioMsg(pMsgQ, pMsg);
    }

    return OS_ERR_MEM_NO_FREE_BLKS;
}
/*
******************************************************************************
**                          VOID NST_TskMsgMemCreate()
**
** Description    : create task message memory
** Arguments    : 无
** Returns        : 无
** Author        :                                    
** Date         : 
**
******************************************************************************
*/
extern OS_MEM* gpTskMsgMem;

static INLINE VOID NST_TskMsgMemCreate(VOID   *memAdr,
                                            INT32U  nblks,
                                            INT32U  blksize)
{
    UINT8 err;
    gpTskMsgMem = OSMemCreate(memAdr, nblks, blksize, &err);
}

/*
******************************************************************************
**                          NST_TskMsg* NST_AllocTskMsg(VOID)
**
** Description    : allocate a message from task message memory
** Arguments    : 无
** Returns        : message
** Author        :                                    
** Date         : 
**
******************************************************************************
*/

static INLINE NST_TskMsg* NST_AllocTskMsg(VOID)
{
    NST_TskMsg* pTskMsg = NULL;
    UINT8 err;
    
    pTskMsg = (NST_TskMsg*)OSMemGet(gpTskMsgMem, &err);
    if(err == OS_ERR_NONE)
        return pTskMsg;
    if(err != OS_ERR_NONE)
        DBGPRINT(DEBUG_ERROR, " ERR:Task#%d NST_AllocTskMsg: ERR code=%d\n", OSTCBCur->OSTCBPrio,  err);
    
    return NULL;
}

/*
******************************************************************************
**                          UINT8 NST_FreeTskMsg(NST_TskMsg* pTskMsg)
**
** Description    : restore a message back into task message memory
** Arguments    : pTskMsg
** Returns        : err code
** Author        :                                    
** Date         : 
**
******************************************************************************
*/
static INLINE UINT8 NST_FreeTskMsg(NST_TskMsg* pTskMsg)
{
    UINT8 err;
    
 //   DBGPRINT(DEBUG_INFO, " @@@@ INFO:  NST_FreeTskMsg , MsgId_%d !\n", pTskMsg->msgId);
    err = OSMemPut (gpTskMsgMem, pTskMsg);
//    if(err != OS_ERR_NONE)
//        DBGPRINT(DEBUG_ERROR, " ERR:     NST_FreeTskMsg: ERR code=%d\n", err);
    
    return err;
}
/*
******************************************************************************
**                VOID NST_InitQue(NST_CellQueue* pQueHead, BOOL_T lock_required)
**
** Description    : init queue
** Arguments    : pQueHead
**               lock_required: if need lock for opeartions on the queue
** Returns        : 
** Author        :                                    
** Date         : 
**
******************************************************************************
*/

static INLINE VOID NST_InitQue(NST_CellQueue* pQueHead, BOOL_T lock_required)
{
    util_init_list_head(pQueHead, lock_required);
}


/*
******************************************************************************
**                    NST_Cell* NST_GetCell(NST_CellQueue* pQueHead)
**
** Description    : get cell from queue's head, the cell is removed from queue
** Arguments    : pQueHead
** Returns        : cell
** Author        :                                    
** Date         : 
**
******************************************************************************
*/

static INLINE NST_Cell* NST_GetCell(NST_CellQueue* pQueHead)
{
    NST_Cell* cell = NULL;

    cell = (NST_Cell *) util_dequeue_list((NST_list_head*)pQueHead);

//    if(cell == NULL)
//        DBGPRINT(DEBUG_WARN, " NST_GetCell: FAILED\n");
    
    return cell;
}

/*
******************************************************************************
**            NST_Cell* NST_PeekCell(NST_CellQueue* pQueHead)
**
** Description    : try to get cell from queue's head, don't remove the cell from the queue
** Arguments    : pQueHead
** Returns        : cell
** Author        :                                    
** Date         : 
**
******************************************************************************
*/

static INLINE NST_Cell* NST_PeekCell(NST_CellQueue* pQueHead)
{
    NST_Cell* cell = NULL;

    cell = (NST_Cell *) util_peek_list((NST_list_head*)pQueHead);

//    if(cell == NULL)
        //DBGPRINT(DEBUG_WARN, " NST_PeekCell: FAILED\n");
    
    return cell;
}


/*
******************************************************************************
**        VOID NST_FreeCell(NST_CellQueue* pFreeQue, NST_Cell* pCell)
**
** Description    : restore cell back into queue's tail
** Arguments    : pFreeQueHead, pCell
** Returns        : 
** Author        :                                    
** Date         : 
**
******************************************************************************
*/

static INLINE VOID NST_FreeCell(NST_CellQueue* pFreeQueHead, NST_Cell* pCell)
{
    util_enqueue_list_tail((NST_list_head*)pFreeQueHead, (NST_linked_list*)pCell);
}

/*
******************************************************************************
**        VOID NST_RemoveCell(NST_CellQueue* pQueHead, NST_Cell* pCell)
**
** Description    : remove specific cell from queue
** Arguments    : pQuehead, pCell
** Returns        : 
** Author        :                                    
** Date         : 
**
******************************************************************************
*/

static INLINE VOID NST_RemoveCell(NST_CellQueue* pQueHead, NST_Cell* pCell)
{
    if(pCell != NULL && pCell->pnext !=NULL && pCell->pprev !=NULL)
        util_unlink_list((NST_list_head*)pQueHead, (NST_linked_list*)pCell);
    else
        DBGPRINT(DEBUG_WARN, "WARN:     NST_RemoveCell: pCell is not in the queue\n");
}

/*
******************************************************************************
**        VOID NST_InsertCell(NST_CellQueue* pQueHead, NST_Cell* pCell, BOOL_T addTail)
**
** Description    : insert cell into queue
** Arguments    : pQueHead, pCell,
**               addTail: TRUE-insert at tail, FALSE-insert at head
** Returns        : 
** Author        :                                    
** Date         : 
**
******************************************************************************
*/

static INLINE VOID NST_InsertCell(NST_CellQueue* pQueHead, NST_Cell* pCell, BOOL_T addTail)
{
    if(addTail == NST_TRUE)
        util_enqueue_list_tail((NST_list_head*)pQueHead, (NST_linked_list*)pCell);
    else
        util_enqueue_list_head((NST_list_head*)pQueHead, (NST_linked_list*)pCell);
}


#define NST_QueSize(pQueHead)  ((pQueHead)->count)
#define NST_IsQueEmpty(pQueHead)  (((pQueHead)->count == 0) ? NST_TRUE : NST_FALSE)

/* concatinate 把pInsertedQueHead队列接在pQueHead后面或者插入pQueHead前面*/
static INLINE VOID NST_InsertCellQue(NST_CellQueue* pQueHead, NST_CellQueue* pInsertedQueHead, BOOL_T AddTail)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = (OS_CPU_SR)0;
#endif 

    if(pInsertedQueHead->count == 0)
        return;
    #if 1
    if(pQueHead->lock_required)
        OS_ENTER_CRITICAL();

    if(AddTail == NST_TRUE)
    { /* pQueHead                   pInsertedQueHead 
                       pQueLastCell  pInsertedQueFirstCell
    */
        NST_Cell* pQueLastCell = pQueHead->pprev;
        NST_Cell* pInsertedQueFirstCell = pInsertedQueHead->pnext;
        NST_Cell* pInsertedQueLastCell = pInsertedQueHead->pprev;
        
        pQueLastCell->pnext = pInsertedQueFirstCell;
        pInsertedQueFirstCell->pprev= pQueLastCell;
        
        pQueHead->pprev = pInsertedQueLastCell;
        pInsertedQueLastCell->pnext = pQueHead;
    } 
    else
    {
        /* pInsertedQueHead                           pQueHead 
           pInsertedQueFirstCell  pInsertedQueLastCell  pQueFirstCell
           */
        NST_Cell* pQueFirstCell = pQueHead->pnext;
        NST_Cell* pInsertedQueLastCell = pInsertedQueHead->pprev;
        NST_Cell* pInsertedQueFirstCell = pInsertedQueHead->pnext;
        
        pInsertedQueLastCell->pnext = pQueFirstCell;
        pQueFirstCell->pprev= pInsertedQueLastCell;

        pQueHead->pnext = pInsertedQueFirstCell;
        pInsertedQueFirstCell->pprev= pQueHead;
    }
        
    pQueHead->count += pInsertedQueHead->count;
    
    pInsertedQueHead->pnext = pInsertedQueHead->pprev = pInsertedQueHead;
    pInsertedQueHead->count = 0;
    if(pQueHead->lock_required)
        OS_EXIT_CRITICAL();

    #else

   if(AddTail == NST_TRUE)
   {
       while(pInsertedQueHead->count)
       {
            NST_Cell* pCell = NST_GetCell(pInsertedQueHead);
            
            NST_InsertCell(pQueHead, pCell, NST_TRUE);
       }
   }
   else
   {
       while(pQueHead->count)
       {
            NST_Cell* pCell = NST_GetCell(pQueHead);
            
            NST_InsertCell(pInsertedQueHead, pCell, NST_TRUE);
       }
       pQueHead->pnext = pInsertedQueHead->pnext;
       pInsertedQueHead->pnext->pprev = pQueHead;
       pQueHead->count += pInsertedQueHead->count;
   }
#endif
}


#define InsertTailQueue(QueueHeader, QueueEntry)                \
            NST_InsertCell(QueueHeader, QueueEntry, NST_TRUE)

#define RemoveHeadQueue(QueueHeader)                \
            NST_GetCell(QueueHeader)

#define InsertHeadQueue(QueueHeader, QueueEntry)            \
            NST_InsertCell(QueueHeader, QueueEntry, NST_FALSE)

#define InitializeQueueHeader(QueueHeader)              \
            NST_InitQue(QueueHeader, NST_TRUE);


#endif

