/*
 * mail.c
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller
 *
 *	TODO	Add support for process to own multiple mailboxes
 */

#include "mail.h"

struct mailbox mailroom[NUM_MAILBOXES];

void initMailbox(struct mailbox* mbox)
{
	mbox->pcb 			= NULL;
	mbox->message_list 	= NULL;
}

unsigned bind(unsigned int mailbox_number)
{
	return pkcall(BIND, mailbox_number);
}

unsigned unbind(unsigned int mailbox_number)
{
	return pkcall(UNBIND, mailbox_number);
}
