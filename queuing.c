/*
 * queuing.c
 *
 * Defines all queuing functions:
 * - Initializes queues
 * - defines enqueuing and dequeuing functions
 * - defines peripheral queue functions
 *
 *  Created on: Sep 19, 2019
 *  Author: Derek Capone
 */


/* Header includes */
#include "queuing.h"
#include "uart.h"

/* Private function declarations */
char check_rx_char(char data);
char queue_full(char q_num, char head_val);


/* Local variables */
struct Queue queues[3];
char tx_busy;

/* Function definitions */

/*
 * Initializes the 3 queues: UART_RX, UART_TX, SysTick
 * Initializes tx_busy flag
 */
void init_queues(void)
{
    queues[UART_RX].head = 0;
    queues[UART_RX].tail = 0;

    queues[UART_TX].head = 0;
    queues[UART_TX].tail = 0;

    queues[SYSTICK].head = 0;
    queues[SYSTICK].tail = 0;

    tx_busy = 0;
}

/*
 * Indicates whether rx queue has character ready
 */
char rx_char_waiting(char q_val)
{
    return (queues[q_val].head != queues[UART_RX].tail);
}

/*
 * Sets the transmit queue to busy
 */
void set_tx_queue_busy(char busy)
{
    tx_busy = busy;
}

/*
 * Returns value indicating if transmit queue is busy
 */
char get_tx_queue_busy(void)
{
    return tx_busy;
}

/*
 * Enqueues data onto one of the three queues
 * "q_num" indicates the queue to be enqueued to
 * "data" is the data to enqueue
 */
void enqueue(char q_num, char data)
{
    /* Calculate new head value */
    char new_head = (queues[q_num].head + 1) & (QSIZE-1);

    /* Return if new head reaches tail */
    if(!queue_full(q_num, new_head)) return;

    /* Add data to element, then move head */
    queues[q_num].data[queues[q_num].head] = data;
    queues[q_num].head = new_head;
}

/*
 * Dequeues data from one of the three queues
 * "q_num" indicates the queue to be dequeued
 * Returns the dequeued character, 0 if queue is empty
 */
char dequeue(char q_num)
{
    /* Return if empty */
    if(queues[q_num].head == queues[q_num].tail) {
        if(q_num == UART_TX) tx_busy = IDLE;
        return 0;
    }

    /* Get data and move tail */
    char data = queues[q_num].data[queues[q_num].tail];
    queues[q_num].tail = (queues[q_num].tail+1) & (QSIZE-1);

    return data;
}

/*
 * Returns 1 if queue full, 0 if not full
 */
char queue_full(char q_num, char head_val)
{
    return (queues[q_num].tail != head_val);
}

