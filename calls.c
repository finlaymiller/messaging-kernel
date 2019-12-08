/*
 * calls.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller and Derek Capone
 *
 *  All functions that processes need to access through supervisor calls are
 *  stored here. They are prefixed with 'k_' to indicate that they are
 *  kernel-level functions rather than process (p_).
 */

#include "calls.h"

extern struct mailbox mailroom[NUM_MAILBOXES];	// list of mailboxes
extern struct pcb *running;				// pointer to pcb of running process
extern struct pri pri_queue[NUM_PRI];	// list of all processes

/*
 * 	Returns the unique identifier of the currently running process
 *
 * 	Arguments:
 * 		None
 * 	Returns:
 * 		[int] ID of the current process
 */
int k_get_id(void)
{
    struct pcb *running = getRunning();
    return running->id;
}

/*
 *	Binds mailbox to process. The rules for binding are as follows:
 *		- One process per mailbox.
 * 		- Processes are allowed to bind to up to 5 mailboxes.
 *		- Mailbox numbers are between 1 and 63.
 *		- Processes may call a bind() with a mailbox number of 0 if they wish
 *		to bind to any mailbox. The available mailbox with the lowest number 
 *		will be bound to.
 *		- Processes cannot bind to a mailbox that has already been bound to.
 *
 * Note that the "all mailboxes in use" error will only occur when all
 * mailboxes have been bound to AND bind() is called with an argument of 0.
 * Otherwise, "mailbox in use" will be returned.
 *
 * Arguments:
 * 		[int] Mailbox number to bind to. 0 if any.
 * Returns:
 * 		[int] Mailbox that was bound to, or an error code:
 * 				-4 Mailbox in use by another process
 * 				-3 All mailboxes are in use
 * 				-2 Mailbox number is outside of supported range (1-127)
 * 				-1 Process has bound to the maximum number of mailboxes
 * 				 0 The machine has broken free of its chains and is running
 * 				   wild. Destroy it before it gains sentience!
 * 				   (this should never happen)
 */
int k_bind(unsigned int mailbox_number)
{
	int good_mailbox = 0, i;
	struct pcb* curr_running = getRunning();	// PCB of current process

	// search mailroom for available mailbox
	if(mailbox_number > NUM_MAILBOXES - 1)
	{	// catch mailbox number outside of range
		// no need to check < 0 since variable is unsigned
		return BAD_MBX_NUM;
	}
	else if(mailbox_number == 0)
	{	// catch bind to ANY mailbox
		for(i = 1; i < NUM_MAILBOXES; i++)
		{	// search mailroom for available box
			if(!mailroom[i].owner)
			{	// mailbox found, save its ID to bind to
				good_mailbox = i;
				break;
			}
		}

		// catch all mailboxes have been bound to
		if(i >= NUM_MAILBOXES)
			return NO_MBX_FREE;
	}
	else if(mailroom[mailbox_number].owner)
	{	// catch mailbox bound to by another process
		return MBX_IN_USE;
	}
	else if((sizeof(curr_running->mbxs) / sizeof(curr_running->mbxs[0])) 
			>= NUM_MBX_PER_PROC)
	{
		return MAX_MBX_BOUND;
	}
	else
		good_mailbox = mailbox_number;	// requested mailbox is available

	// update mailbox and pcb if one has been found
	if(good_mailbox > 0)
	{
		mailroom[good_mailbox].owner = curr_running;	// update mailbox
		for(i = 0; i < NUM_MBX_PER_PROC; i++)			// update PCB
		{	// get first zero/null mailbox in list
			if(curr_running->mbxs[i] == 0)
			{
				curr_running->mbxs[i] = good_mailbox;
				break;
			}
		}
	}

	return good_mailbox;
}


/*
 * 	Unbind mailbox(es) from process. The rules for unbinding are as follows:
 * 		- The process must own the mailbox being unbound from.
 * 		- Mailbox numbers are between 1 and 127.
 * 		- A process may unbind from all of its mailboxes by calling unbind() 
 * 		with the mailbox number 0.
 * 		- Messages are not deleted when a mailbox is unbound from! It is the
 * 		responsibility of the process to do so.
 *
 * 	Arguments:
 * 		[int] Mailbox number to unbind from. 0 if all.
 * 	Returns:	
 * 		[int] Number of mailbox unbound from. Or an error code:
 * 				-3 Mailbox in use by another process
 * 				-2 All mailboxes are in use
 * 				-1 Mailbox number is outside of supported range (1-127)
 */
int k_unbind(unsigned int mailbox_number)
{
	int old_mailbox = 0, i = 0;
	struct pcb* curr_running = getRunning();

	// search mailroom for mailbox
	if(mailbox_number > NUM_MAILBOXES - 1)
	{	// catch mailbox number outside of range
		// no need to check < 0 since variable is unsigned
		return BAD_MBX_NUM;
	}
	else if(mailbox_number == 0)
	{	// catch unbind from ALL mailboxes
		for(i = 0; i < NUM_MBX_PER_PROC; i++)	// update PCB
			curr_running->mbxs[i] = NULL;

		for(i = 1; i < NUM_MAILBOXES; i++)		// update mailbox
			if(mailroom[i].owner == curr_running)
			   mailroom[i].owner = NULL;
	}
	else if(mailroom[mailbox_number].owner != curr_running)
	{	// catch mailbox bound to by another process
		return MBX_IN_USE;
	}
	else	// ok now we can unbind from the mailbox
	{
		mailroom[mailbox_number].owner = NULL;	// update mailbox
		for(i = 0; i < NUM_MBX_PER_PROC; i++)	// update PCB
		{	// get first zero/null mailbox in list
			if(curr_running->mbxs[i] == mailbox_number)
			{
				curr_running->mbxs[i] = 0;
				break;
			}
		}

		old_mailbox = mailbox_number;
	}
	
	return old_mailbox;
}


/*
 * 	Send message from a process to a mailbox. The rules for sending messages 
 * 	are as follows:
 * 		- The sender must be bound to a mailbox
 * 		- The receiving mailbox must have an owner
 * 		- The sending and receiving mailbox IDs must be between 1 and 127
 * 		- The message being sent must be between 1 and 64 characters long
 * 		- The receiving mailbox must have fewer than 64 messages waiting
 *
 *	Arguments:
 *		[struct message] The message to be sent 
 * 	Returns:
 * 		[int] The number of bytes sent, or an error code:
 * 				-5 The sending process does not own the sender mailbox
 * 				-4 The mailbox being sent to has no owner
 * 				-2 The mailbox being sent to is full
 */
int k_send(struct message *msg)
{
	struct message *kmsg;
	struct pcb* curr_running = getRunning();

	// mailroom/box error checks
	if(mailroom[msg->sqid].owner != curr_running)
	{	// catch sender mailbox validity
		return BAD_SENDER;
	}
	else if(mailroom[msg->dqid].owner == NULL)
	{		// catch receiver mailbox validity
			return BAD_RECVER;
	}
	else if (mailroom[msg->dqid].num_messages >= MAX_NUM_MESSAGES)
	{	// catch mailbox full
		return MBX_FULL;
	}

	// handle blocked processes if mailbox is waiting for 
	// ANY or desired destination message
	if(mailroom[msg->dqid].owner->state == msg->dqid || 
	   mailroom[msg->dqid].owner->state == 0)
	{   // point PCB to the message, fill size in PCB, and unblock
        mailroom[msg->dqid].owner->msg = (char *)msg->body;
        mailroom[msg->dqid].owner->sz = msg->size;
        mailroom[msg->dqid].owner->state = UNBLOCKED;

	    // force that PCB back into the priority queue
	    insertPriQueue(mailroom[msg->dqid].owner, 
					   mailroom[msg->dqid].owner->pri);

	    running->pri_switch = TRUE;
	} else 
	{   // copy over message data and add to mailbox message queue
        kmsg = allocate();			// get fresh message struct from mailpile
        k_copyMessage(kmsg, msg);		// copy message contents
        kmsg->next = mailroom[msg->dqid].message_list;	// add to mailbox ll
        mailroom[msg->dqid].message_list = kmsg;
        mailroom[msg->dqid].num_messages++;	// update number of messages in mox
	}

	return kmsg->size;
}


/*
 * 	Receive a message from a mailbox. Receiving works as follows:
 * 		- Receiver must own a mailbox
 * 		- If no message is waiting, receiving process will block 
 * 		until one arrives
 * 		- Between the two potential message sizes (expected by process and 
 * 		present in message), the shorter of the two is copied over
 * 		
 *
 * 	Arguments:
 * 		[struct message] The message to be returned to the calling process
 * 	Returns:
 * 		[int] The number of bytes received, or the error code:
 * 				-4 Process doesn't own mailbox being checked
 * 				-1 No message waiting
 */
int k_recv(struct message *msg)
{
	struct message *kmsg;
	struct pcb* curr_running = getRunning();

	// mailroom/box error checks
	if(mailroom[msg->dqid].owner != curr_running)
	{	// catch sender mailbox validity
		return BAD_RECVER;
	}
	else if (mailroom[msg->dqid].num_messages == 0)
	{	// catch mailbox empty
	    removePriQueue();
	    running->state = msg->sqid;
        enablePendSV(TRUE); // Re-enable pendSV handler before being blocked

		return MBX_EMTY;
	}

	kmsg = mailroom[msg->dqid].message_list;	// for readability

	/* Get message size, then fill message body */
	msg->size = ((msg->size < kmsg->size) ? msg->size : kmsg->size);
	memcpy(msg->body, kmsg->body, msg->size);	// copy message body

	// remove message from mailbox
	if(kmsg->next == NULL)
		mailroom[msg->dqid].message_list = NULL;
	else
		mailroom[msg->dqid].message_list = kmsg->next;

	// return message to mailpile for use by other processes
	deallocate(kmsg);
	mailroom[msg->dqid].num_messages--;

	return msg->size;
}


/*
 * 	Terminate the currently running process. Process is removed from running
 * 	process list, all links are broken and all memory is freed.
 *
 * 	Arguments:
 * 		None
 * 	Returns:
 * 		[int] 0 on success
 */
int k_terminate(void)
{
    InterruptMasterDisable();

    if(running->next == running)
	{   // If this is the last process in the priority queue
        pri_queue[running->pri].head = NULL;
        pri_queue[running->pri].tail = NULL;

        running = getNextRunning();	// Set new running 
    } else 
	{	// Reset head or tail if necessary
        if(pri_queue[running->pri].head == (unsigned long*)running){
            pri_queue[running->pri].head = (unsigned long*)running->next;
        } else if(pri_queue[running->pri].tail == (unsigned long*)running){
            pri_queue[running->pri].tail = (unsigned long*)running->prev;
        }

        /* set up temporary struct for next running pcb */
        struct pcb *next_run = running->next;

        /* Remove running from linked list */
        running->prev->next = running->next;
        running->next->prev = running->prev;

        /* Deallocate memory for stack and pcb */
        free(running->stk);
        free(running);

        /* Set new running */
        running = next_run;
    }

    /* Set new stack pointer, load registers */
    setPSP(running->sp);
    loadRegisters();

    InterruptMasterEnable();
    enablePendSV(TRUE);

    returnPSP();
    __asm(" bx      lr");

    return 0;
}


/*
 * 	Change priority level of currently running process. Valid process levels
 * 	are 1-4. 0 is reserved for the idle process.
 *
 * 	Arguments:
 * 		[int] Priority level to switch to
 * 	Returns:
 * 		[int] Priority level that was switched to
 */
int k_nice(int priority)
{
    // remove process from current linked list
    if(running->next == running){
        /* If this is the last process in the priority queue */
        pri_queue[running->pri].head = NULL;
        pri_queue[running->pri].tail = NULL;
    } else 
	{	/* Reset head or tail if necessary */
        if(pri_queue[running->pri].head == (unsigned long*)running){
            pri_queue[running->pri].head = (unsigned long*)running->next;
        } else if(pri_queue[running->pri].tail == (unsigned long*)running){
            pri_queue[running->pri].tail = (unsigned long*)running->prev;
        }

        /* Remove running from linked list */
        running->prev->next = running->next;
        running->next->prev = running->prev;
    }

    // insert process into desired priority queue
    insertPriQueue(running, priority);
    running->pri = priority;
    running->pri_switch = TRUE;

    return running->pri;
}
