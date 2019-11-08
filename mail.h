/*
 * mail.h
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller
 */

#ifndef MAIL_H_
#define MAIL_H_

#include <stdlib.h>
#include "process.h"
#include "str_conv_funcs.h"
#include "trap.h"

#define NUM_MAILBOXES		256
#define MAX_MESSAGE_LEN		256
#define NUM_MBX_PER_PROC	4

// what should/can we use mailbox #0 for?
// a broadcast channel maybe? or it can just unused
enum BIND_ERR_CODES
{
	MBX_IN_USE = -3, NO_MBX_FREE, BAD_MBX_NUM
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
	struct pcb*		owner;			// points to pcb of owner process
	struct message*	message_list;	// points to first message in mailbox
};

/* function definitions */
unsigned int p_bind(unsigned int);
unsigned int p_unbind(unsigned int);
unsigned int p_send(unsigned int, unsigned int, void*, unsigned int);
unsigned int p_recv(unsigned int, unsigned int, void*, unsigned int);

#endif /* MAIL_H_ */
