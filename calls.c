/*
 * calls.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller
 *
 *  All functions that processes need to access through supervisor calls are
 *  stored here. They are prefixed with 'k_' to indicate that they are
 *  kernel-level functions rather than process (p_).
 *
 *
 *  todo	write nice() function
 *  todo	write send() function
 *  todo	write recv() function
 *  todo	write bind() function
 *  todo	write unbind() function
 *  todo	write terminate() function
 */

#include "calls.h"

extern struct mailbox mailroom[NUM_MAILBOXES];
extern struct pcb *running;
extern struct pri pri_queue[NUM_PRI];

/*
 * Description
 *
 * @param:		None
 * @returns:	ID of the current process
 */
int k_get_id(void)
{
    struct pcb *running = getRunning();
    return running->id;
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int k_nice(int priority)
{

	return 0;
}


/*
 * Bine mailbox to process
 *
 * @param:		Mailbox number to bind to. 0 if any.
 * @returns:	Mailbox that was bound to, or an error code:
 * 					-3 Mailbox in use by another process
 * 					-2 All mailboxes are in use
 * 					-1 Mailbox number is outside of supported range (1-256)
 */
int k_bind(unsigned int mailbox_number)
{
	unsigned int good_mailbox = 0, i = 0;
	struct pcb* curr_running;

	// search mailroom for available mailbox
	if(mailbox_number > NUM_MAILBOXES - 1)
	{	// catch mailbox number outside of range
		// no need to check < 0 since variable is unsigned
		return BAD_MBX_NUM;
	}
	else if(mailbox_number == 0)
	{	// catch bind to ANY mailbox
		// search mailroom for available box
		for(i = 1; i < NUM_MAILBOXES; i++)
		{
			if(!mailroom[i].owner)
			{
				good_mailbox = i;
				break;
			}
		}

		if(i >= NUM_MAILBOXES)
			return NO_MBX_FREE;
	}
	else if(mailroom[mailbox_number].owner)
	{	// catch mailbox bound to by another process
		return MBX_IN_USE;
	}
	else
		good_mailbox = mailbox_number;

	// update mailbox and pcb if one has been found
	if(good_mailbox > 0)
	{
		curr_running = getRunning();
		mailroom[good_mailbox].owner = curr_running;	// update mailbox
		for(i = 0; i < NUM_MBX_PER_PROC; i++)			// update pcb
		{	// get first zero/null mailbox in list
			if(!curr_running->mbxs[i])
			{
				curr_running->mbxs[i] = good_mailbox;
				break;
			}
		}
	}

	return good_mailbox;
}


/*
 * Unbind mailbox(es) from process
 *
 * @param:		Mailbox number to unbind from. 0 if all.
 * @returns:	Number of mailbox unbound from. Or an error code:
 * 				-3 Mailbox in use by another process
 * 				-2 All mailboxes are in use
 * 				-1 Mailbox number is outside of supported range (1-256)
 */
int k_unbind(unsigned int mailbox_number)
{
	unsigned int old_mailbox = 0, i = 0;
	struct pcb* curr_running = getRunning();
	struct message* msg;	// to be used for emptying mailbox

	// search mailroom for mailbox
	if(mailbox_number > NUM_MAILBOXES - 1)
	{	// catch mailbox number outside of range
		// no need to check < 0 since variable is unsigned
		return BAD_MBX_NUM;
	}
	else if(mailbox_number == 0)
	{	// catch unbind from ALL mailboxes
		for(i = 0; i < NUM_MBX_PER_PROC; i++)
			curr_running->mbxs[i] = NULL;

		for(i = 1; i < NUM_MAILBOXES; i++)
			if(mailroom[i].owner == curr_running)
			   mailroom[i].owner = NULL;
	}
	else if(mailroom[mailbox_number].owner != curr_running)
	{	// catch mailbox bound to by another process
		return MBX_IN_USE;
	}
	else	// ok now we can unbind from the mailbox
	{
		msg = mailroom[mailbox_number].message_list;
		mailroom[mailbox_number].owner = NULL;
		while(msg->next)		// free messages
		{
			msg = msg->next;	// move to next message
			free(mailroom[mailbox_number].message_list);	// free first message
			mailroom[mailbox_number].message_list = msg;
		}

		old_mailbox = mailbox_number;
	}

	return old_mailbox;
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int k_send(void)
{

	return 0;
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int k_recv(void)
{

	return 0;
}

/*
 * Description
 *
 * @param:
 * @returns:
 */
int k_terminate(void)
{
    if(running->next == running){
        /* If this is the last process in the priority queue */
        pri_queue[running->pri].head = NULL;
        pri_queue[running->pri].tail = NULL;

        //terminate process

        /* Set new running */
        running = getNextRunning();
    } else {
        /* Reset head or tail if necessary */
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
    returnPSP();

    return 0;
}
