/*
 * mail.c
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller and Derek Capone
 *
 *	This file contains all of the functions related to the Inter-Process 
 * 	Communication system that are not the kernel-level side of kernel calls.
 * 	Functions which handle the process-level side of kernel calls which are
 *  related to the IPC system are prefixed with 'p_'.
 * 
 *  There are four main functions related to the IPC system:
 * 		bind() - allows a process to reserve a mailbox for its exclusive use
 * 	  unbind() - allows a process to free a mailbox so that it may be used by
 * 				 other processes
 * 		send() - allows a process to send data to another process
 * 	 receive() - allows a process to receive data from another process
 * 
 * 	Messages are sent using message structs that are allocated before processes
 * 	begin running to ensure for constant-time operation. When not in use the
 * 	messages are stored in the mailpile. Note that the processes themselves
 *  don't see the message structs, they just use mailbox numbers and messages. 
 */

#include "mail.h"

/* globals */
struct mailbox mailroom[NUM_MAILBOXES] = { NULL };	// array of mailboxes
struct message* mailpile;
char *BIND_ERR_PRINTS[4] = {
"Proc bound to max number of mailboxes", "Bad mailbox number",
"No mailbox free", "Mailbox in use"
};


/*
 * Wrapper for process-level binding. These functions will make more sense 
 * once you get to the send and receiving functions. This function calls the 
 * gobetween function for process to kernel communications (pkcall) with the 
 * request type (BIND) and the mailbox number to be bound to. No validity
 * checks are performed at this level.
 *
 * Arguments:
 * 		[int] Mailbox number to bind to. 0 if any.
 * Returns:	
 * 		[int] Mailbox that was bound to, or an error code:
 * 					-4 Mailbox in use by another process
 * 					-3 All mailboxes are in use
 * 					-2 Mailbox number is outside of supported range (1-127)
 * 					-1 Process has bound to the maximum number of mailboxes
 */
int p_bind(unsigned int mailbox_number)
{
	return pkcall(BIND, mailbox_number);
}


/*
 * Wrapper for process-level unbinding. This function calls the gobetween
 * function for process to kernel communications (pkcall) with the request 
 * type (UNBIND) and the mailbox number to be unbound from. No validity
 * checks are performed at this level.
 *
 * Arguments:
 * 		[int] Mailbox number to unbind from. 0 if all.
 * Returns:	
 * 		[int] Mailbox that was bound to, or an error code:
 * 				-3 Mailbox in use by another process
 * 				-2 All mailboxes are in use
 * 				-1 Mailbox number is outside of supported range (1-127)
 */
int p_unbind(unsigned int mailbox_number)
{
	return pkcall(UNBIND, mailbox_number);
}


/*
 * Wrapper for process-level message sending. This function performs some 
 * basic checks on the parameters of the send before putting the data in a pmsg
 * struct and passing it to the pkcall() function with the SEND request type.
 *
 * Arguments:
 * 		[int] Mailbox of the sending process
 * 		[int] Mailbox to send to
 * 		[char] Message to send
 * 		[int] Size of message to send
 * Returns:	
 * 		[int] The number of characters sent, or an error code:
 * 				-5 The sending process does not own the sender mailbox
 * 				-4 The mailbox being sent to has no owner
 * 				-3 The size of the message being sent is invalid (>64 chars)
 * 				-2 The mailbox being sent to is full
 */
int p_send(unsigned int src, unsigned int dst, char msg[MAX_MSG_LEN], int size)
{
	struct message pmsg;
	struct pcb *curr_running = getRunning();

	/* catch some basic errors before sending to kernel level */
	if(dst > NUM_MAILBOXES)
		// catch receiver mailbox validity
		return BAD_RECVER;
	else if(size <= 0 || size > MAX_MSG_LEN)
		// catch too-long message
		return BAD_SIZE;

	/* load values into message struct */
	pmsg . next = NULL;
	pmsg . dqid = dst;
	pmsg . sqid = src;
	memcpy(pmsg.body, msg, size);
	pmsg . size = size;

	int rtn_val = pkcall(SEND, (unsigned int)&pmsg);
	while(curr_running->pri_switch == TRUE);

	return rtn_val;
}

/*
 * Wrapper for process-level message receiving. This function performs some 
 * basic checks on the parameters of the receive before putting the data in a 
 * pmsg struct and passing it to the pkcall() function with the RECV request 
 * type. Any data beyond the expected size is not passed to the process.
 *
 * Arguments:
 * 		[int] Mailbox of the sending process
 * 		[int] Mailbox to receive from
 * 		[char] Buffer to write the received message to
 * 		[int] Size of message to receive
 * Returns:
 * 		[int] The number of bytes received, or the error code:
 * 				-4 Process doesn't own mailbox being checked
 * 				-3 Invalid message size
 */
int p_recv(unsigned int src, unsigned int dst, char *buf, int size)
{
	struct message pmsg;
	int rtn_val;

	// catch some basic errors before sending to kernel level
	if(dst > NUM_MAILBOXES) // catch receiver mailbox validity
		return BAD_RECVER;
	if(size <= 0)			// catch bad message size
		return BAD_SIZE;

	// load values into message struct
	pmsg . next = NULL;
	pmsg . dqid	= dst;
	pmsg . sqid	= src;
	pmsg . body[0] = '\0';
	pmsg . size = size;

	rtn_val = pkcall(RECV, (unsigned int)&pmsg);

	/* Catch blocked processes to wait to be put back into WTR queue */
	if(rtn_val == MBX_EMTY){
	    struct pcb *curr_running = getRunning();
	    while(curr_running->state != UNBLOCKED);

	    /* Reaches this point when unblocked, fill buffer with message from PCB */
	    memcpy(buf, curr_running->msg, curr_running->sz);
	} else	// copy shorter of the two
	    memcpy(buf, pmsg.body, (size < pmsg.size) ?
	                            size : pmsg.size);

	return rtn_val;
}


/*
 * Copy message data from one message struct to another
 *
 * Arguments:
 * 		[struct message] Pointer to source message
 * 		[struct message] Pointer to dest message
 * Returns:
 * 		None
 */
void k_copyMessage(struct message *dst_msg, struct message *src_msg)
{
	dst_msg->dqid = src_msg->dqid;
	dst_msg->sqid = src_msg->sqid;
	memcpy(dst_msg->body, src_msg->body, src_msg->size);
	dst_msg->size = src_msg->size;
}


/*
 * Clear message struct fields
 *
 * Arguments:
 * 		[struct message] Pointer to message to clear
 * Returns:
 * 		None
 */
void clearMessage(struct message *msg)
{
	msg->next = NULL;
	msg->dqid = NULL;
	msg->sqid = NULL;
	msg->body[0] = '\0';
	msg->size = NULL;
}


/*
 * A list of blank messages for use by k_send() and k_recv()
 *
 * Arguments:
 * 		None
 * Returns:
 * 		Pointer to head of message queue
 */
struct message *initMessages(void)
{
	struct message *head, *msg;
	int i = 0;

	do {
		msg = malloc(sizeof(struct message));
		clearMessage(msg);
		msg->next = head;
		head = msg;
	} while(++i < MAILPILE_SIZE);

	return head;
}


/*
 * Give caller a pointer to a usable message struct
 *
 * Arguments:
 * 		None
 * Returns:
 * 		[struct message] Pointer to available message struct
 */
struct message *allocate(void)
{
	struct message *new_msg;
	if (mailpile == NULL)		// if list is empty, return
		return NULL;
	new_msg = mailpile; 		// New mbox entry gets head of list (not NULL)
	mailpile = mailpile->next; 	// Update list. Could be NULL
	new_msg->next = NULL;		// clear old pointer
	return new_msg;
}

/*
 * Return message to mailpile
 *
 * Arguments:
 * 		[struct message] Message to return
 * Returns:
 * 		[int] 1 on success
 */
int deallocate(struct message *old_msg)
{
	old_msg -> next = mailpile;
	mailpile = old_msg;
	return 1; // Always successful
}
