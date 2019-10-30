/*
 * queuing.h
 *
 * Defines constants for string lengths and queue size, idle/busy
 * Defines struct for queue
 * Declares public functions for queuing.c
 *
 *  Created on: Sep 19, 2019
 *  Author: Derek Capone
 */

#ifndef QUEUING_H_
#define QUEUING_H_

#define MAX_STRING_LEN  100
#define MAX_COMMAND_LENGTH 100
#define QSIZE 64
#define EXIT_CHAR   '\e'
#define BACKSPACE '\x7f'

/* Indicates the status of the UART */
#define IDLE 0
#define BUSY 1

/* Indicates which queue to use */
#define UART_RX 0
#define UART_TX 1
#define SYSTICK 2

/* Struct for queue */
struct Queue{
    char data[QSIZE];
    char head; /* Holds index of empty element where next enqueue occurs */
    char tail; /* Holds index of next character to dequeue */
};

/* Public function declarations */
void init_queues(void);
void enqueue(char q_num, char data);
char dequeue(char q_num);
char rx_char_waiting(char q_val);
char get_tx_queue_busy(void);
void set_tx_queue_busy(char busy);
void fill_tx_queue(char *message, char length);

#endif /* QUEUING_H_ */
