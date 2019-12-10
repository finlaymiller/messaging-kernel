 /*
 * mail.h
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller and Derek Capone
 * 
 * Headerfile for the mail-related functions
 */

#ifndef MAIL_H_
#define MAIL_H_

#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "str_conv_funcs.h"
#include "trap.h"

/* global definitions */
#define NUM_MAILBOXES		128
#define MAX_MSG_LEN			64
#define MAX_NUM_MESSAGES	64
#define	MAILPILE_SIZE		128

// macro to find string length + null terminator
// why did I even do this?
#define TRUE_STRLEN(s)	(strlen(s) + 1)	// accounts for null terminator

/* error codes that can be thrown when
 * binding, unbinding, sending, and receiving
 */
enum BIND_ERR_CODES
{
	MBX_IN_USE = -4, NO_MBX_FREE, BAD_MBX_NUM, MAX_MBX_BOUND
};

enum SEND_ERR_CODES
{
	BAD_SENDER = -5, BAD_RECVER, BAD_SIZE, MBX_FULL, MBX_EMTY
};

/* structure definitions */
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
/* process wrappers */
int p_bind(unsigned int);
int p_unbind(unsigned int);
int p_send(unsigned int src, unsigned int dst, char msg[MAX_MSG_LEN], int size);
int p_recv(unsigned int, unsigned int, char *, int);
/* message handlers */
void k_copyMessage(struct message *, struct message *);
void clearMessage(struct message *);
struct message *initMessages(void);
struct message *allocate(void);
int deallocate(struct message *);

#endif /* MAIL_H_ */
