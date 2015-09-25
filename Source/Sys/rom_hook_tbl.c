/*
******************************************************************************
**
** Project     : 
** Description :    ROM functions table
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/
//------------------------------------------------------------------------------------------------------
#include "lwip/sys.h"

//------------------------------------------------------------------------------------------------------

void InitRomHookFuncTbl(void)
{
    OS_MemClr((unsigned char*)gpRomHookFuncTbl, ROM_HOOK_FUNC_TABLE_SIZE);
    
    gpRomHookFuncTbl->OSDebugInit = OSDebugInit;
    gpRomHookFuncTbl->OSInitHookBegin = OSInitHookBegin;
    gpRomHookFuncTbl->OSInitHookEnd = OSInitHookEnd;
    gpRomHookFuncTbl->OSTaskCreateHook = OSTaskCreateHook;
    gpRomHookFuncTbl->OSTaskDelHook = OSTaskDelHook;
    gpRomHookFuncTbl->OSTaskIdleHook = OSTaskIdleHook;
    gpRomHookFuncTbl->OSTaskReturnHook = OSTaskReturnHook;
    gpRomHookFuncTbl->OSTaskStatHook = OSTaskStatHook;
    gpRomHookFuncTbl->OSTaskStkInit = OSTaskStkInit;
    gpRomHookFuncTbl->OSTaskSwHook = OSTaskSwHook;
    gpRomHookFuncTbl->OSTCBInitHook = OSTCBInitHook;
    gpRomHookFuncTbl->OSTimeTickHook = OSTimeTickHook;
    gpRomHookFuncTbl->OS_CPU_SR_Save = OS_CPU_SR_Save;
    gpRomHookFuncTbl->OS_CPU_SR_Restore = OS_CPU_SR_Restore;
    gpRomHookFuncTbl->OSCtxSw = OSCtxSw;
    gpRomHookFuncTbl->OSIntCtxSw = OSIntCtxSw;
    gpRomHookFuncTbl->OSStartHighRdy = OSStartHighRdy;

    gpRomHookFuncTbl->_sys_mbox_new = _sys_mbox_new;
    gpRomHookFuncTbl->_sys_mbox_free = _sys_mbox_free;
    gpRomHookFuncTbl->_sys_mbox_post = _sys_mbox_post;   
    gpRomHookFuncTbl->_sys_mbox_trypost = _sys_mbox_trypost;      
    gpRomHookFuncTbl->_sys_arch_mbox_fetch = _sys_arch_mbox_fetch;
    gpRomHookFuncTbl->_sys_mbox_valid = _sys_mbox_valid;
    gpRomHookFuncTbl->_sys_mbox_set_invalid = _sys_mbox_set_invalid;   
    gpRomHookFuncTbl->_sys_sem_new = _sys_sem_new;
    gpRomHookFuncTbl->_sys_sem_free = _sys_sem_free; 
    gpRomHookFuncTbl->_sys_arch_sem_wait = _sys_arch_sem_wait;
    gpRomHookFuncTbl->_sys_sem_signal = _sys_sem_signal;
    gpRomHookFuncTbl->_sys_sem_valid = _sys_sem_valid;
    gpRomHookFuncTbl->_sys_sem_set_invalid = _sys_sem_set_invalid;    
    gpRomHookFuncTbl->_sys_init = _sys_init;
    gpRomHookFuncTbl->_sys_thread_new = _sys_thread_new;
    
}




