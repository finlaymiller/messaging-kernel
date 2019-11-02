/*
 * queue.c
 *
 *  Created on: Sep 19, 2019
 * Modified on:	Oct 09, 2019
 *      Author: Finlay Miller
 *      		Derek Capone
 *
 *	Contains all circular-queue-related functions for monitor operation.
 */

#include "queue.h"

/* globals */
q_struct q_table[NUM_Q];
//extern systick_struct systick;
//extern int cmd_index;		// number of characters currently in command string


/*
 * Initializes array of queues filled with zeroes
 *
 * @param num_queues: 	Number of queues to fill array with
 * @return:				None
 */
//void initQTable(int num_queues)
//{
//    unsigned int i, j;
//    for(i = 0; i < num_queues; i++)
//    {
//    	for(j = 0; j < MAX_Q_LEN; j++)
//    		q_table[i].contents[j] = 0;
//
//    	q_table[i].head = 0;
//    	q_table[i].tail = 0;
//    }
//}

/*
 * Handler which is called when a character is received. dequeues a char from
 * the indicated queue, sends it to the checker functions, then sends it to
 *
 * @param: q_index:	Index position in the queue table of the queue that
 * 					received the character
 * @returns: 		None
 */
//void handleQ(int q_index)
//{
//    char data = deQ(q_index);	// dequeue char from buffer
//    systick_struct *stptr = &systick;
//
//    if(q_index == UART_RX)		// received character handler
//    {
//    	checkChar(data);		// process char
//    }
//    else if(q_index == SYSTICK)	// update SYSTICK
//    {
//    	timeIncrement();
//    	if(stptr->enabled)		// check alarm if one is set
//    	    	alarmCheck();
//    }
//}

/*
 * Checks whether or not queue is empty.
 *
 * @param q_index: 	Index of queue in q_table. See queue.h for which queue
 * 					is referred to by each position.
 * @return: 		1 if empty, 0 otherwise.
 */
int isQEmpty(int q_index)
{
	q_struct *queue = &q_table[q_index];

	return queue->head == queue->tail;
}

/*
 * Checks whether or not queue is full.
 *
 * @param q_index: 	Index of queue in q_table.
 * @return: 		1 if full, 0 otherwise.
 */
int isQFull(int q_index)
{
	q_struct *queue = &q_table[q_index];
	int temp_head = NEXT_PTR(queue->head);

	return temp_head == queue->tail;
}

/*
 * Adds value to next available place in queue, if space is available.
 * todo		add success/fail return value if queue is full
 *
 * @param q_index: 	Index of queue in q_table.
 * @param data: 	Value to give to the queue.
 * @return: 		None
 */
void enqueue(int q_index, char data)
{
	q_struct *queue = &q_table[q_index];

    // add character to queue if the queue has room for it
	if(!isQFull(q_index))
	{
		queue->contents[queue->head] = data;
		queue->head = NEXT_PTR(queue->head);
	}
}

/*
 * Removes oldest value at the tail of the queue.
 * todo		add success/fail return value if queue is empty, not just 0
 *
 * @param q_index:	Index of queue in q_table.
 * @return: 		Data from the tail of the queue.
 */
char dequeue(int q_index)
{
	q_struct *queue = &q_table[q_index];
	char data = 0;

	if(!isQEmpty(q_index))
	{
	    data = queue->contents[queue->tail];
	    queue->tail = NEXT_PTR(queue->tail);
    }

	return data;
}
