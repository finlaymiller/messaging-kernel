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
#include "queuing.h"
#include "systick.h"
#include "kernel.h"
#include "SVC_example.h"

/*
 * Enables all interrupts on the CPU
 */
void InterruptEnable(unsigned long InterruptIndex)
{
/* Indicate to CPU which device is to interrupt */
if(InterruptIndex < 32)
    NVIC_EN0_R = 1 << InterruptIndex;       // Enable the interrupt in the EN0 Register
else
    NVIC_EN1_R = 1 << (InterruptIndex - 32);    // Enable the interrupt in the EN1 Register
}

/*
 * Executes assembly instruction to enable interrupts
 */
void InterruptMasterEnable(void)
{
    /* enable CPU interrupts */
    __asm(" cpsie   i");
}

void InterruptMasterDisable(void)
{
    __asm(" cpsid i");
}

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
