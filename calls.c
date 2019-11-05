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

/*
 * Description
 *
 * @param:		None
 * @returns:	ID of the current process
 */
int k_get_id(void)
{
	// something like:
	// return running->id;

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
 * @param:
 * @returns:
 */
int k_terminate(void)
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


/*
 * TODO		Could this all be simplified by not using an enum and just
 * 			#defining the error codes? So the program would return [-2 to -1]
 * 			on error, or [1-256] on success? Probably.
 *
 * @param:		Mailbox number to bind to. 0 if any.
 * @returns:	Mailbox that was bound to, or an error code:
 * 					1. Mailbox in use by another process
 * 					2. All mailboxes are in use
 * 					3. Mailbox number is outside of supported range (1-256)
 */
int k_bind(unsigned int mailbox_number)
{
	unsigned int good_mailbox = 0, i;

	if((mailbox_number < 0) || (mailbox_number > NUM_MAILBOXES))
	{	// catch mailbox number outside of range
		return BAD_MBX_NUM;
	}
	else if(mailroom[mailbox_number]->owner)
	{	// catch mailbox bound to by another process
		return MBX_IN_USE;
	}
	else if(mailbox_number == 0)
	{	// catch bind to ANY mailbox
		// search mailroom for available box
		for(i = 0; i < NUM_MAILBOXES; i++)
		{
			if(!mailroom[i]->owner)
			{
				good_mailbox = i;
				break;
			}
		}

		if(i >= NUM_MAILBOXES)
			return NO_MBX_FREE;
	}
	else
		good_mailbox = mailbox_number

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
