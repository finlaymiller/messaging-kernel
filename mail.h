/*
 * mail.h
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller
 */

#ifndef MAIL_H_
#define MAIL_H_

#include "process.h"

#define NUM_MAILBOXES	256

// what should/can we use mailbox #0 for?
enum BIND_ERR_CODES
{
	MBX_IN_USE = -3, NO_MBX_FREE, BAD_MBX_NUM, BIND_OK
};

struct message
{
	struct message* next;	// pointers to next and previous messages in
	struct message* prev;	// mailbox for linked-list support
	unsigned int 	mqid;	// the mailbox of the current process
	unsigned int 	sqid;	// the mailbox of the message source or sender
	unsigned int*	addr;	// the address of the message in memory
	unsigned int 	size;	// the length of the message in bytes
};

struct mailbox
{
	struct pcb*	owner;		// points to pcb of owner process
	struct message*	message_list;	// points to first message in mailbox
};

void initMailbox(struct mailbox* mbox);
int k_bind(unsigned int);
int k_unbind(unsigned int);

#endif /* MAIL_H_ */
