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

/*
 * Function to test binding
 */
void procBind(void)
{
    int i, mbx;
    char buff[10];

    for(i = 255; i < 260; i++)
    {
    	mbx = p_bind(i);

    	if(mbx > 0)
    	{
    		UART0_TXStr("\nBound to mailbox ");
    		UART0_TXStr(my_itoa(mbx, buff, 10));
    	}
    	else if(mbx < 0)
    	{
    		UART0_TXStr("\nError {");
			UART0_TXStr(BIND_ERR_PRINTS[-mbx - 1]);
			UART0_TXStr("} when attempting to bind to mailbox ");
			UART0_TXStr(my_itoa(i, buff, 10));
    	}
    	else
		{
    		UART0_TXStr("\nSomehow bound to mailbox ");
    		UART0_TXStr(my_itoa(mbx, buff, 10));
		}
    }
}
