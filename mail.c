/*
 * mail.c
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller
 *
 *	TODO	Add support for process to own multiple mailboxes
 */

#include "mail.h"

struct mailbox mailroom[NUM_MAILBOXES] = {{NULL}};
struct message* mailpile;
char *BIND_ERR_PRINTS[3] = {
"Bad mailbox number", "No mailbox free", "Mailbox in use"
};


/*
 * Description
 *
 * @param:		Mailbox number to bind to. 0 if any.
 * @returns:	Mailbox that was bound to, or an error code:
 * 					-3 Mailbox in use by another process
 * 					-2 All mailboxes are in use
 * 					-1 Mailbox number is outside of supported range (1-256)
 */
unsigned int p_bind(unsigned int mailbox_number)
{
	return pkcall(BIND, mailbox_number, NULL);
}


/*
 * Description
 *
 * @param:		Mailbox number to unbind from. 0 if all.
 * @returns:
 */
unsigned int p_unbind(unsigned int mailbox_number)
{
	return pkcall(UNBIND, mailbox_number, NULL);
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
unsigned int p_send(unsigned int dst, unsigned int src, void *msg, unsigned size)
{

	return pkcall(SEND, dst, NULL);
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
unsigned int p_recv(unsigned int dst, unsigned int src, void *msg, unsigned size)
{
	return pkcall(RECV, dst, NULL);
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
struct message *allocate()
{
	struct message *new_mbox;
	if (mailpile == NULL)	// if list is empty, return
			 return NULL;
	new_mbox = mailpile; 	// New mbox entry gets head of list (not NULL)
	mailpile = mailpile -> next; // Update list. Could be NULL
	return new_mbox;
}


