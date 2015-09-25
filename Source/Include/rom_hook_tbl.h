#ifndef _ROM_FUNC_TBL_H
#define _ROM_FUNC_TBL_H


// DATA SRAM中预留2K用来放FW ROM function pointer table，不要改动下面这2个const
#define _ROM_HOOK_FUNC_BASE               (0x00080000)
#define ROM_HOOK_FUNC_TABLE_SIZE      0x800

// 如果修改table，ROM里的东西必须同步更新
typedef struct _ROM_HOOK_FUNC_TBL
{
#if OS_DEBUG_EN > 0u
void          (*OSDebugInit)(void);
#endif
void          (*OSInitHookBegin)(void);
void          (*OSInitHookEnd)(void);
void          (*OSTaskCreateHook)(OS_TCB          *ptcb);
void          (*OSTaskDelHook)(OS_TCB          *ptcb);
void          (*OSTaskIdleHook)(void);
void          (*OSTaskReturnHook)(OS_TCB          *ptcb);
void          (*OSTaskStatHook)(void);
OS_STK*    (*OSTaskStkInit)(void           (*task)(void *p_arg),
                                       void            *p_arg,
                                       OS_STK          *ptos,
                                       INT16U           opt);
#if OS_TASK_SW_HOOK_EN > 0u
void          (*OSTaskSwHook)(void);
#endif

void          (*OSTCBInitHook)(OS_TCB          *ptcb);

#if OS_TIME_TICK_HOOK_EN > 0u
void          (*OSTimeTickHook)(void);
#endif

OS_CPU_SR  (*OS_CPU_SR_Save) (void);           /* See OS_CPU_A.ASM                                  */ 
void       (*OS_CPU_SR_Restore) (OS_CPU_SR cpu_sr); 
void       (*OSCtxSw)(void); 
void       (*OSIntCtxSw)(void); 
void       (*OSStartHighRdy)(void);

signed char       (*_sys_mbox_new)(sys_mbox_t *mbox, int size);
void        (*_sys_mbox_free)(sys_mbox_t *mbox);
void        (*_sys_mbox_post)(sys_mbox_t *mbox, void *msg);
signed char       (*_sys_mbox_trypost)(sys_mbox_t *mbox, void *msg);
u32_t       (*_sys_arch_mbox_fetch)(sys_mbox_t *mbox, void **msg, u32_t timeout);
int         (*_sys_mbox_valid)(sys_mbox_t *mbox);
void        (*_sys_mbox_set_invalid)(sys_mbox_t *mbox);
signed char       (*_sys_sem_new)(sys_sem_t *sem, u8_t count);
void        (*_sys_sem_free)(sys_sem_t *sem);
u32_t       (*_sys_arch_sem_wait)(sys_sem_t *sem, u32_t timeout);
void        (*_sys_sem_signal)(sys_sem_t *sem);
int         (*_sys_sem_valid)(sys_sem_t *sem);
void        (*_sys_sem_set_invalid)(sys_sem_t *sem);
void        (*_sys_init)(void);
sys_thread_t(*_sys_thread_new)(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio);

}ROM_HOOK_FUNC_TBL;


extern ROM_HOOK_FUNC_TBL * const gpRomHookFuncTbl;

void InitRomHookFuncTbl(void);


#endif //_ROM_FUNC_TBL_H

