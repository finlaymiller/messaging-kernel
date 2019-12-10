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
#include "VT100.h"

/*
 * Calls initialization functions for all modules
 * Begins polling the input queue
 */
void main (void)
{
    initKernel();

    initTerminal();

    /* Init processes here */
    reg_proc(&procBlockRecv, 10, 3);
    reg_proc(&procBlockSend, 'T', 2);

    initRunning();

    SVC();

    while(1);

}
