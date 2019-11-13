/*
 * mail.h
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller
 */

#ifndef MAIL_H_
#define MAIL_H_

#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "str_conv_funcs.h"
#include "trap.h"

#define NUM_MAILBOXES		64
#define MAX_MSG_LEN			64
#define MAX_NUM_MESSAGES	64
#define	MAILPILE_SIZE		128

#define TRUE_STRLEN(s)	(strlen(s) + 1)	// accounts for null terminator

// what should/can we use mailbox #0 for?
// a broadcast channel maybe? or it can just unused
enum BIND_ERR_CODES
{
	MBX_IN_USE = -4, NO_MBX_FREE, BAD_MBX_NUM, MAX_MBX_BOUND
};

enum SEND_ERR_CODES
{
	BAD_SENDER = -5, BAD_RECVER, BAD_SIZE, MBX_FULL, MBX_EMTY
};

struct message
{
	struct message* next;	// pointer to next message in list
	unsigned int 	dqid;	// the mailbox of the receving process
	unsigned int 	sqid;	// the mailbox of the sending process
	char body[MAX_MSG_LEN];	// message contents
	int				size;	// number of bytes in body
};

struct mailbox
{
	struct pcb*		owner;			// points to pcb of owner process
	struct message*	message_list;	// points to first message in mailbox
	unsigned int	num_messages;	// number of messages waiting
};

/* function definitions */
int p_bind(unsigned int);
int p_unbind(unsigned int);
int p_send(unsigned int, unsigned int, char msg[MAX_MSG_LEN]);
int p_recv(unsigned int, unsigned int, char *, int);

void k_copyMessage(struct message *, struct message *);
void clearMessage(struct message *);
struct message *initMessages(void);
struct message *allocate(void);
int deallocate(struct message *);

#endif /* MAIL_H_ */
