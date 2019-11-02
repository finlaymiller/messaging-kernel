/*
 * main.c
 *
 *  Created on: Sep 19, 2019
 *  Author: Derek Capone
 *
 *  Calls all initialization functions
 *  Polls input queues
 */

#include "queue.h"
#include "uart.h"
#include "systick.h"
#include "kernel.h"
#include "processPrinter.h"

/*
 * Calls initialization functions for all modules
 * Begins polling the input queue
 */
void main (void)
{
    /* Initialize UART */
    UART0_Init();           // Initialize UART0

    /* Initialize Systick */
    //SysTickInit();

    reg_proc(NULL, 100, 4);

    while(1)
    {
        UART0_TXChar('a');
    }

}
