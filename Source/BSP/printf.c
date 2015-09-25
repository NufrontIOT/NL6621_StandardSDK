/*
******************************************************************************
**
** Project     : 
** Description :    Driver for Printing debug info
** Author      :    pchn                             
** Date        : 
**    
** UpdatedBy   : 
** UpdatedDate : 
**
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "includes.h"

#include "uart.h"

#pragma import(__use_no_semihosting)
struct __FILE { int handle;   /* Add whatever you need here */};
FILE __stdout;
FILE __stdin;


/*
** Retargeted I/O
** ==============
** The following C library functions make use of semihosting
** to read or write characters to the debugger console: fputc(),
** fgetc(), and _ttywrch().  They must be retargeted to write to
** the model's UART.  __backspace() must also be retargeted
** with this layer to enable scanf().  See the Compiler and
** Libraries Guide.
*/

/*
** These must be defined to avoid linking in stdio.o from the
** C Library
*/
int fputc(int ch, FILE *f)
{

    if(ch == '\n')
    {
        BSP_UartPutcPolled('\r');
    }
    BSP_UartPutcPolled(ch);

    return ch;
}

void _ttywrch(int ch)
{
    unsigned char tempch = ch;
    BSP_UartPutcPolled(tempch);
}


/* END of Retargeted I/O */

/*
** Exception Signaling and Handling
** ================================
** The C library implementations of ferror() uses semihosting directly
** and must therefore be retargeted.  This is a minimal reimplementation.
** _sys_exit() is called after the user's main() function has exited.  The C library
** implementation uses semihosting to report to the debugger that the application has
** finished executing.
*/

int ferror(FILE *f)
{
    return EOF;
}

void _sys_exit(int return_code)
{
    while(1);
}


/*
******************************************************************************
**                            End Of File                                    **
******************************************************************************
*/
