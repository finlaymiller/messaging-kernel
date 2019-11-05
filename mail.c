/*
 * mail.c
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller
 */

#include "mail.h"

struct mailbox mailroom[NUM_MAILBOXES];

void initMailbox(struct mailbox* mbox)
{
	mbox->pcb 			= NULL;
	mbox->message_list 	= NULL;
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
	enum BIND_ERR_CODES error = BIND_OK;
	unsigned int good_mailbox = 0, i;

	if((mailbox_number < 0) || (mailbox_number > NUM_MAILBOXES))
	{	// catch mailbox number outside of range
		error = BAD_MBX_NUM;
	}
	else if(mailroom[mailbox_number]->owner)
	{	// catch mailbox bound to by another process
		error = MBX_IN_USE;
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
			error = NO_MBX_FREE;
	}

	if(error)
		return error;
	else if(good_mailbox)
		return good_mailbox;
	else
		return mailbox_number;
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
