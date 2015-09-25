/*
******************************************************************************
**
** Project     : 
** Description :    entry of firware
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/

/*
******************************************************************************
**                               Include Files
******************************************************************************
*/

#include "includes.h"


#pragma arm section zidata="main_only_cpu_access_mem"    /* 仅CPU访问*/
#ifdef LWIP_SUPPORT
static OS_STK   sAppStartTaskStack[NST_APP_START_TASK_STK_SIZE];  
#endif
#pragma arm section

/*
******************************************************************************
**                               FUNCTION DELEARATIONS
******************************************************************************
*/

// Prototypes

extern VOID SystemInit(VOID);

#ifdef LWIP_SUPPORT
extern VOID    TestAppMain(VOID * pParam);
#endif

/*
******************************************************************************
**                               FUNCTION PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
**                        INT32 main(VOID)
**
** Description  : Application entry point
** Arguments    : 无
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
INT32 main(VOID)
{
    /* system Init */
    SystemInit();

   /* create application tasks here */
#ifdef TEST_APP_SUPPORT
#ifdef LWIP_SUPPORT
    OSTaskCreate(TestAppMain, NULL, &sAppStartTaskStack[NST_APP_START_TASK_STK_SIZE -1],  NST_APP_TASK_START_PRIO); 
#endif // LWIP_SUPPORT
#endif //TEST_APP_SUPPORT

    /* Start Os */
    OSStart();
    
    return 1;
}


/*
******************************************************************************
**                            End Of File                                   **
******************************************************************************
*/


