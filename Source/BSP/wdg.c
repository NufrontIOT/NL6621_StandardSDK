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
** Returns      : ��
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
** Returns      : ��
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
** Description  : ��λӲ�����Ź���ÿ��tick����Ƿ������timeout�ˣ�                 ����ʱ��
                 ���������ܷ��ˡ�ֹͣι���ȴ�Ӳ�����Ź�����Զ���λϵͳ��
                 syscoreÿ����һ�λḴλ�������
** Arguments    : 
** Returns      : ��
** Author       :                                   
** Date         : 
**
******************************************************************************
*/
#define SOFT_WDT_TIMEOUT_PERIOD      200  // tick��one tick 10ms�� bcn period=100ms
// 200��tickû������������ֹͣι��
VOID  WDResetWDTimer( VOID )
{
    if ( WDSoftwareCounter < SOFT_WDT_TIMEOUT_PERIOD )
    {
        WDSoftwareCounter += 1;
        WDG_Feed();
    }
}

//��ʼ���������Ź�
//prer:��Ƶ��:0~7(ֻ�е�3λ��Ч!)
//��Ƶ����=4*2^prer.�����ֵֻ����256!
//rlr:��װ�ؼĴ���ֵ:��11λ��Ч.
//ʱ�����(���):Tout=40K/((4*2^prer)*rlr)ֵ.
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
    
    *Wdt0Cr = 1;  //ʹ�ܿ��Ź�
}


/*
******************************************************************************
**                            End Of File                                    **
******************************************************************************
*/

