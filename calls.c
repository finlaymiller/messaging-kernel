/*
 * calls.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller
 *
 *  All functions that processes need to access through supervisor calls are
 *  stored here. They are prefixed with 's_' to indicate that they are
 *  supervisor-level functions rather than process (p_) or kernel (k_).
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

int getid(void)
{
    volatile struct kcallargs getidarg; /* Volatile to actually reserve space on stack */
    getidarg . code = GETID;

    /* Assign address if getidarg to R7 */
    assignR7((unsigned long) &getidarg);

    SVC();

    return getidarg . rtnvalue;
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
 * Description
 *
 * @param:		Mailbox number to bind to. 0 if any.
 * @returns:	Mailbox that was bound to, or an error code:
 * 					-3 Mailbox in use by another process
 * 					-2 All mailboxes are in use
 * 					-1 Mailbox number is outside of supported range (1-256)
 */
int k_bind(unsigned int mailbox_number)
{
	unsigned int good_mailbox = 0, i;

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

	// update mailbox if one has been found
	if(good_mailbox > 0)
	{
		mailroom[good_mailbox].owner = getRunning();
	}

	return good_mailbox;
}


/*
 * Description
 *
 * @param:		Mailbox number to unbind from. 0 if all.
 * @returns:
 */
int k_unbind(unsigned int mailbox_number)
{

	return 0;
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
