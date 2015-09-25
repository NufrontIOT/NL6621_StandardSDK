#ifndef _GPIO_H
#define _GPIO_H
#include "includes.h"


#define NST_WR_GPIO_REG(Offset, Value)  /*0x40040000*/\
    {\
        (*(volatile UINT32 *)(_GPIO_BASE_ADR + Offset)) = (UINT32)Value;\
    }

#define NST_RD_GPIO_REG(Offset)   (*(volatile UINT32 *)(_GPIO_BASE_ADR + Offset))

//GPIO level
#define GPIO_LOW_LEVEL                0
#define GPIO_HIGH_LEVEL               1

//GPIO direction
#define GPIO_DIRECTION_INPUT             0
#define GPIO_DIRECTION_OUTPUT            1

//GPIO interrupt type level
#define GPIO_INT_LEVEL_SENSITIVE        0
#define GPIO_INT_EDGE_SENSITIVE         1

//GPIO interrupt polarity
//level sensitve: active-low   active-high
//dege sensitive: falling-edge  raiseing-edge
#define GPIO_INT_ACTIVE_LOW              0   
#define GPIO_INT_ACTIVE_HIGH             1

//GPIO interrupt mask
#define GPIO_INT_UNMASK                    0
#define GPIO_INT_ENMASK                    1


VOID  BSP_GPIOInit (VOID);
VOID  BSP_GPIOPinMux (int portNum);
VOID  BSP_GPIOSetDir ( int portNum, BOOLEAN dir);
VOID  BSP_GPIOSetValue (int portNum, BOOLEAN bitValue);
int  BSP_GPIOGetValue (int portNum);
VOID  BSP_GPIOIntEn (int portNum, BOOLEAN typeLev, BOOLEAN polarity);
VOID  BSP_GPIOIntDisable (int portNum);
VOID  BSP_GPIOIntMask (int portNum, BOOLEAN mask);
VOID  BSP_GPIOIntISR (int portNum);

#endif // _GPIO_H

