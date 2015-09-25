#ifndef APP_CFG_H_
#define APP_CFG_H_

// platform define
#define NULINK2_SOC
#define RF_NU680X
#define ADD_IQ_CALIBRATION

#define USE_NV_INFO
//#define CHECK_AUTH_CODE

#define CONFIG_STA_SUPPORT		
//#define WSC_STA_SUPPORT			/* only could uncommented when not support softap */

#define CONFIG_AP_SUPPORT
//#define WSC_AP_SUPPORT			/* only could uncommented when not support sta */

#define LWIP_SUPPORT				/* necessary for Stand-alone Firmware */


#define DHCP_SERVER_SUPPORT  		/* only could uncommented when support softap */
#define DNS_SERVER_SUPPORT    		/* only could uncommented when support softap */

#define TEST_APP_SUPPORT

/* define wsc feature */
#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
#define STA_WSC_INCLUDED
#endif // WSC_STA_SUPPORT //
#endif // CONFIG_STA_SUPPORT //

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
#define AP_WSC_INCLUDED
#define WSC_REGISTRAR_SUPPORT
#endif // WSC_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

#if defined(STA_WSC_INCLUDED)||defined(AP_WSC_INCLUDED)
#define WSC_INCLUDED
#endif

#define WPA_SUPPORT
#define DEFAULT_PHY_CHNL 6

//#define DOT11_N_SUPPORT

/*
********************************************************************************************************* 
*                                            任务ID和优先级                                                   
********************************************************************************************************* 
*/         
#define  NST_SYS_CORE_TASK_PRIO                 (OS_TASK_TMR_PRIO + 1)   

#define  NST_MAC_MNG_TASK_PRIO                  (OS_TASK_TMR_PRIO + 2)

#define  NST_SYS_MNG_TASK_PRIO                 (OS_TASK_TMR_PRIO + 3)   

#define  NST_RX_TASK_PRIO                       (OS_TASK_TMR_PRIO + 5)  

#define  NST_TX_TASK_PRIO                       (OS_TASK_TMR_PRIO + 7)    

#define  NST_APP_TASK_START_PRIO                 (OS_TASK_TMR_PRIO + 8)   

#define  LWIP_TASK_START_PRIO                       (OS_TASK_TMR_PRIO + 9)      
#define  LWIP_TASK_END_PRIO                           (OS_TASK_TMR_PRIO + 13)  


/*                                                                                                        
********************************************************************************************************* 
*                                              栈的大小                                                   
*                            Size of the task stacks (# of OS_STK entries)                                
********************************************************************************************************* 
*/    
// stack size unit is OS_STK (4B)
#define  NST_SYS_CORE_TASK_STK_SIZE                 512       // IQ calibration  //2K                                                 
#define  NST_RX_TASK_STK_SIZE                       512//256      //2K                                                  
#define  NST_MAC_MNG_TASK_STK_SIZE           512*3   //8k                                                      
#define  NST_TX_TASK_STK_SIZE                       512                                                        
#define  NST_DEBUG_TASK_STK_SIZE                    512                                                        
//#define  NST_PRINTF_TASK_STK_SIZE                   256        // Define the Uart printf stack length 

#ifdef LWIP_SUPPORT
#define  NST_APP_START_TASK_STK_SIZE      256//    128      
#define  NST_TEST_APP_TASK_STK_SIZE        256//    128      
#define  DNS_APP_START_TASK_STK_SIZE        256
#endif

#define  NST_SYS_MNG_TASK_STK_SIZE          256       // 1K                                                 

#define SYS_CORE_TSK_MSG_Q_SIZE                     80
#define RX_TSK_MSG_Q_SIZE                           80
#define TX_TSK_MSG_Q_SIZE                           80
#define MAC_MNG_TSK_MSG_Q_SIZE                      80
#define DEBUG_TSK_MSG_Q_SIZE                        80
#define SYS_MNG_TSK_MSG_Q_SIZE                      30


/* 堆内存定义*/
#define OS_DMEM_POOL_SIZE         1024*10    /*     Determine the size of dynamic memory pool     */

/*
********************************************************************************************************* 
*                                            测试示例选择                                                   
********************************************************************************************************* 
*/ 
#ifdef TEST_APP_SUPPORT

//#define TEST_DIRECT_CONFIG
//#define TEST_SOFTAP_CONFIG
//#define TEST_SNIFFER
//#define TEST_UART_UPDATE_FW
//#define TEST_OTA_UPDATE_FW
#define TEST_AIRKISS

#define TEST_TCP_SERVER
//以下选项每次测试时只可打开其中之一
#define TEST_TCP_CLIENT
//#define TEST_UDP_SERVER
//#define TEST_UDP_CLIENT

#endif //TEST_APP_SUPPORT//

//#define SPI_SDIO_CMD_TEST
//#define SDIO_TEST

#endif /*APP_CFG_H_*/


