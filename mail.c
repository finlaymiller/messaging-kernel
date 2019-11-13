/*
 * mail.c
 *
 *  Created on: Nov 5, 2019
 *      Author: Finlay Miller
 *
 *	TODO	Add support for process to own multiple mailboxes
 */

#include "mail.h"

struct mailbox mailroom[NUM_MAILBOXES] = { NULL };
struct message* mailpile;
char *BIND_ERR_PRINTS[4] = {
"Proc bound to max number of mailboxes", "Bad mailbox number",
"No mailbox free", "Mailbox in use"
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
int p_bind(unsigned int mailbox_number)
{
	return pkcall(BIND, mailbox_number);
}


/*
 * Description
 *
 * @param:		Mailbox number to unbind from. 0 if all.
 * @returns:
 */
int p_unbind(unsigned int mailbox_number)
{
	return pkcall(UNBIND, mailbox_number);
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int p_send(unsigned int src, unsigned int dst, char msg[MAX_MSG_LEN])
{
	struct message pmsg;

	// catch some basic errors before sending to kernel level
	if(dst > NUM_MAILBOXES)
		// catch receiver mailbox validity
		return BAD_RECVER;
	else if(strlen(msg) > MAX_MSG_LEN)
		// catch too-long message
		return BAD_SIZE;

	// load values into message struct
	pmsg . next = NULL;
	pmsg . dqid = dst;
	pmsg . sqid = src;
	memcpy(pmsg.body, msg, TRUE_STRLEN(msg));
	pmsg . size = TRUE_STRLEN(pmsg.body);

	return pkcall(SEND, (unsigned int)&pmsg);
}

/*
 * Description
 *
 * @param:
 * @returns:
 */
int p_recv(unsigned int src, unsigned int dst, char *buf, int size)
{
	struct message pmsg;
	int rtn_val;

	// catch some basic errors before sending to kernel level
	if((src > NUM_MAILBOXES) || (dst > NUM_MAILBOXES))
		// catch receiver mailbox validity
		return BAD_RECVER;

	// load values into message struct
	pmsg . next = NULL;
	pmsg . dqid	= dst;
	pmsg . sqid	= src;
	pmsg . body[0] = '\0';
	pmsg . size = size;

	rtn_val = pkcall(RECV, (unsigned int)&pmsg);

	memcpy(buf, pmsg.body, (size < TRUE_STRLEN(pmsg.body))?
							size : TRUE_STRLEN(pmsg.body));

	return rtn_val;
}


void k_copyMessage(struct message *dst_msg, struct message *src_msg)
{
	dst_msg->dqid = src_msg->dqid;
	dst_msg->sqid = src_msg->sqid;
	memcpy(dst_msg->body, src_msg->body, src_msg->size);
	dst_msg->size = src_msg->size;
}

void clearMessage(struct message *msg)
{
	msg->next = NULL;
	msg->dqid = NULL;
	msg->sqid = NULL;
	msg->body[0] = '\0';
	msg->size = NULL;
}

/*
 * A list of blank messages for use by k_send() and k_recv()
 *
 * @param:
 * @returns:
 */
struct message *initMessages(void)
{
	struct message *head, *msg;
	int i = 0;

	do {
		msg = malloc(sizeof(struct message));
		clearMessage(msg);
		msg->next = head;
		head = msg;
	} while(++i < MAILPILE_SIZE);

	return head;
}

/*
 * Description
 *
 * @param:
 * @returns:
 */
struct message *allocate(void)
{
	struct message *new_msg;
	if (mailpile == NULL)	// if list is empty, return
		return NULL;
	new_msg = mailpile; 	// New mbox entry gets head of list (not NULL)
	mailpile = mailpile->next; // Update list. Could be NULL
	new_msg->next = NULL;	// clear old pointer
	return new_msg;
}

/*
 * Description
 *
 * @param:
 * @returns:
 */
int deallocate(struct message *old_msg)
{
	old_msg -> next = mailpile;
	mailpile = old_msg;
	return 1; // Always successful
}

