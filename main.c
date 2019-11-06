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
#include "process.h"
#include "queue.h"
#include "systick.h"
#include "kernel.h"
#include "trap.h"

/*
 * Calls initialization functions for all modules
 * Begins polling the input queue
 */
void main (void)
{
    /* Initialize UART */
     UART0_Init();           // Initialize UART0
    InterruptEnable(INT_VEC_UART0);       // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts

    /* Enable CPU interrupts */
    InterruptMasterEnable();

    initPriQueue();

    reg_proc(&procA, 100, 4);
    reg_proc(&procB, 50, 4);
    reg_proc(&procC, 25, 4);

    initRunning();

    SVC();

    while(1)
    {
    }

}
