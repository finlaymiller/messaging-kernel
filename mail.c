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

void initMailbox(struct mailbox* mbox)
{
	mbox->owner			= NULL;
	mbox->message_list 	= NULL;
}

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

/*
 * Function to test binding
 */
void procBind(void)
{
    char i, mbx;

    for(i = 0; i < 5; i++)
    {
    	mbx = p_bind(i);

    	if(mbx > 0)
    	{
    		UART0_TXStr("\nBound to mailbox ");
    		UART0_TXChar(mbx);
    	}
    	else if(mbx < 0)
    	{
    		UART0_TXStr("\nError ");
			UART0_TXChar(mbx);
			UART0_TXStr("when attempting to bind to queue ");
			UART0_TXChar(i);
    	}
    	else
		{
    		UART0_TXStr("\nSomehow bound to mailbox ");
    		UART0_TXChar(mbx);
		}
    }
}
