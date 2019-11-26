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
#include "mail.h"
#include "queue.h"
#include "systick.h"
#include "kernel.h"
#include "trap.h"
#include "register.h"

/*
 * Calls initialization functions for all modules
 * Begins polling the input queue
 */
void main (void)
{
	initKernel();

    // init procs here
    reg_proc(&procSendRecv, 1, 3);

    reg_proc(&procSend, 2, 3);

    initRunning();

    UART0_TXStr("\n\n---------- REBOOT ----------\n\n");

    SVC();

    while(1);

}
