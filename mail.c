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
char *BIND_ERR_PRINTS[3] =
{
 "Bad mailbox number", "No mailbox free", "Mailbox in use"
};

unsigned int p_bind(unsigned int mailbox_number)
{
	return pkcall(BIND, mailbox_number, NULL);
}

unsigned int p_unbind(unsigned int mailbox_number)
{
	return pkcall(UNBIND, mailbox_number, NULL);
}

unsigned int p_send(unsigned int dst, unsigned int src, void *msg, unsigned size)
{

	return pkcall(SEND, dst, NULL);
}

unsigned int p_recv(unsigned int dst, unsigned int src, void *msg, unsigned size)
{
	return pkcall(RECV, dst, NULL);
}

void p_terminate(void)
{
	pkcall(TERMINATE, NULL, NULL);
}
