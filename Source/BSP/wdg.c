/*
******************************************************************************
**
** Project     : 
** Description :    watchdog
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/

#include "wdg.h"
#include "board.h"


/*
******************************************************************************
**                        VOID  WDResetSWWatchdog( VOID )
**
** Description  : Reset software watchdog
** Arguments    : 
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
UINT16 WDSoftwareCounter = 0;

VOID  WDResetSWWatchdog( VOID )
{
    WDSoftwareCounter = 0;
}

/*
******************************************************************************
**              VOID WDG_Feed(VOID)
**
** Description  : feed watchdog
** Arguments    : 
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
VOID WDG_Feed(VOID)
{
    *Wdt0Crr = 0x76;
}


/*
******************************************************************************
**              VOID  WDResetWDTimer( VOID )
**
** Description  : 复位硬件看门狗，每个tick检查是否软件狗timeout了，                 若超时了
                 则程序可能跑飞了。停止喂狗等待硬件看门狗溢出自动复位系统。
                 syscore每调度一次会复位软件狗。
** Arguments    : 
** Returns      : 无
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
#define SOFT_WDT_TIMEOUT_PERIOD      200  // tick，one tick 10ms， bcn period=100ms
// 200个tick没有任务工作，则停止喂狗
VOID  WDResetWDTimer( VOID )
{
    if ( WDSoftwareCounter < SOFT_WDT_TIMEOUT_PERIOD )
    {
        WDSoftwareCounter += 1;
        WDG_Feed();
    }
}

//初始化独立看门狗
//prer:分频数:0~7(只有低3位有效!)
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=40K/((4*2^prer)*rlr)值.
VOID WDG_Init(VOID) 
{/*
Where i = TOP_INIT and t = timeout period
For i = 0 to 15
if WDT_USE_FIX_TOP==1
    t = 2(16 + i)
else
    t = WDT_USER_TOP_INIT_(i)
*/
    #define INITIAL_TIMEOUT_PERIOD    0
    #define TIMEOUT_PERIOD            0

    *Wdt0Cr = 0;
    
    *Wdt0Torr = (INITIAL_TIMEOUT_PERIOD << 4) |TIMEOUT_PERIOD;
    
//    ASSERT(INITIAL_TIMEOUT_PERIOD <= *Wdt0CompParams4);
//    ASSERT(TIMEOUT_PERIOD <= *Wdt0CompParams5);
    
    *Wdt0Cr = 1;  //使能看门狗
}


/*
******************************************************************************
**                            End Of File                                    **
******************************************************************************
*/

