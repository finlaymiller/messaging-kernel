/*
 * queue.c
 *
 *  Created on: Sep 19, 2019
 *      Author: Finlay Miller and Derek Capone
 *
 *	Contains all circular-queue-related functions for monitor operation.
 *  For this assignment queues are only used in UART functions. Legacy code!
 */

#include "queue.h"

/* globals */
q_struct q_table[NUM_Q];


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
