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


int nice(int priority)
{
    volatile struct kcallargs getidarg; /* Volatile to actually reserve space on stack */
    getidarg . code = NICE;
    getidarg . arg1 = priority;

    /* Assign address if getidarg to R7 */
    assignR7((unsigned long) &getidarg);

    SVC();

    return getidarg . rtnvalue;
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
	int good_mailbox = 0, i = 0, found_mailbox = 0;
	struct pcb* curr_running = getRunning();

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
		mailroom[good_mailbox].owner = curr_running;	// update mailbox
		for(i = 0; i < NUM_MBX_PER_PROC; i++)			// update pcb
		{	// get first zero/null mailbox in list
			if(curr_running->mbxs[i] == 0)
			{
				curr_running->mbxs[i] = good_mailbox;
				found_mailbox = 1;
				break;
			}
		}
	}

	// process having already bound to the maximum number of mailboxes
	// is the last error to catch
	return (found_mailbox) ? good_mailbox : MAX_MBX_BOUND;
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
		mailroom[mailbox_number].owner = NULL;	// update mailbox
		for(i = 0; i < NUM_MBX_PER_PROC; i++)	// update pcb
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
 * Description
 *
 * @param:
 * @returns:
 */
int k_send(struct message *msg)
{
	char b[128];
	struct pcb* curr_running = getRunning();
	struct message *kmsg;

	// debugging prints
	UART0_TXStr("\nKernel received a message with the following data:");
	UART0_TXStr("\nMQID\t\t");
	UART0_TXStr(my_itoa(msg->dqid, b, 10));
	UART0_TXStr("\tSQID\t\t");
	UART0_TXStr(my_itoa(msg->sqid, b, 10));
	UART0_TXStr("\nBODY\t\t");
	UART0_TXStr(msg->body);
	UART0_TXStr("\nSIZE\t\t");
	UART0_TXStr(my_itoa(msg->size, b, 10));

	// mailroom/box error checks
	if(mailroom[msg->dqid].owner != curr_running)
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

	// copy over message data and add to message queue
	kmsg = allocate();				// get fresh message struct from mailpile
	k_copyMessage(kmsg, msg);		// copy message contents
	kmsg->next = mailroom[msg->dqid].message_list;	// add to mailbox ll
	mailroom[msg->dqid].message_list = kmsg;
	mailroom[msg->dqid].num_messages++;	// update number of messages in mailbox

	return TRUE_STRLEN(kmsg->body);
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int k_recv(struct message *msg)
{
	char b[128];
	struct pcb* curr_running = getRunning();
	struct message *kmsg;

	// debugging prints
	UART0_TXStr("\nKernel looking for a message with the following data:");
	UART0_TXStr("\nMQID\t\t");
	UART0_TXStr(my_itoa(msg->dqid, b, 10));
	UART0_TXStr("\tSQID\t\t");
	UART0_TXStr(my_itoa(msg->sqid, b, 10));
	UART0_TXStr("\nMAX SIZE\t");
	UART0_TXStr(my_itoa(msg->size, b, 10));

	// mailroom/box error checks
	if(mailroom[msg->dqid].owner != curr_running)
	{	// catch sender mailbox validity
		return BAD_RECVER;
	}
	else if(mailroom[msg->sqid].owner == NULL)
	{		// catch receiver mailbox validity???
			return BAD_SENDER;
	}
	else if (mailroom[msg->dqid].num_messages == 0)
	{	// catch mailbox empty

	    removePriQueue();
	    running->state = msg->sqid;  //TODO: verify that this is the right value
	    startNextProcess();

		return MBX_EMTY;
	}

	kmsg = mailroom[msg->dqid].message_list;	// for readability
	// DO SOME MORE CHECKS ON THE ACTUAL MESSAGE STRUCT HERE

	memcpy(msg->body, kmsg->body,	// copy message body
		  (msg->size < TRUE_STRLEN(kmsg->body)) ?
		   msg->size : TRUE_STRLEN(kmsg->body));

	// remove message from mailbox
	if(kmsg->next == NULL)
		mailroom[msg->dqid].message_list = NULL;
	else
		mailroom[msg->dqid].message_list = kmsg->next;

	deallocate(kmsg);
	mailroom[msg->dqid].num_messages--;

	return TRUE_STRLEN(msg->body);
}

