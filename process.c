/*
 * process.c
 *
 *  Created on: Oct 17, 2019
 *      Author: Larry Hughes
 *      Editor: Finlay Miller
 *      		Derek Capone
 */

#include "process.h"

extern char *BIND_ERR_PRINTS[3];

void procSendRecv(void)
{
	char 	buff[128], msg[128];
	char	text[] = "qwerty";
	int 	id = p_get_id();
	int 	mbx = p_bind(id);
	int 	rcode, i = 0;
	char 	tstr[64];

	msg[0] = '\0';

	// send messages
	while(i < TRUE_STRLEN(text))
	{
		tstr[i] = text[i];
		tstr[++i] = '\0';
		rcode = p_send(mbx, mbx, tstr);
		UART0_TXStr("\tSend returned\t");
		UART0_TXStr(my_itoa(rcode, buff, 10));
	}

	// receive messages
	while(i > 0)
	{
		rcode = p_recv(mbx, mbx, msg, i-- + 1);
		UART0_TXStr("\nRecv returned\t");
		UART0_TXStr(my_itoa(rcode, buff, 10));
		UART0_TXStr("\tMessage is \"");
		UART0_TXStr(msg);
		UART0_TXStr("\"");
	}
}


void procBindUnbind(void)
{
	char buff[128];
	int i, mbx = 0;
	int id = p_get_id();

	// bind
	for(i = 50; i < 60; i++)
	{
		mbx = p_bind(i);

		if(mbx > 0)
		{
			UART0_TXStr("\nP");
			UART0_TXStr(my_itoa(id, buff, 10));
			UART0_TXStr(" bound to mailbox ");
			UART0_TXStr(my_itoa(mbx, buff, 10));
		}
		else
		{
			UART0_TXStr("\nBind with P");
			UART0_TXStr(my_itoa(id, buff, 10));
			UART0_TXStr(" and mailbox ");
			UART0_TXStr(my_itoa(i, buff, 10));
			UART0_TXStr(" returned error \"");
			UART0_TXStr(BIND_ERR_PRINTS[-mbx - 1]);
			UART0_TXStr("\"");
		}
	}

	// unbind
	for(i = 55; i < 65; i++)
	{
		mbx = p_unbind(i);

		if(mbx > 0)
		{
			UART0_TXStr("\nP");
			UART0_TXStr(my_itoa(id, buff, 10));
			UART0_TXStr(" unbound from mailbox ");
			UART0_TXStr(my_itoa(mbx, buff, 10));
		}
		else
		{
			UART0_TXStr("\nUnbind with P");
			UART0_TXStr(my_itoa(id, buff, 10));
			UART0_TXStr(" and mailbox ");
			UART0_TXStr(my_itoa(i, buff, 10));
			UART0_TXStr(" returned error \"");
			UART0_TXStr(BIND_ERR_PRINTS[-mbx -1]);
			UART0_TXStr("\"");
		}
	}
}



/*
 * Function to test process
 */
void procA(void)
{
	int i;
	char unique_char[] = "a";

	for(i = 0; i < 10; i++)
		UART0_TXStr(unique_char);

	p_nice(3);

	for(i = 0; i < 10; i++)
		UART0_TXStr(unique_char);

	p_nice(4);

	for(i = 0; i < 10; i++)
		UART0_TXStr(unique_char);
}

/*
 * Function to test process
 */
void procB(void)
{
	int i;
	char unique_char[] = "b";

	for(i = 0; i < 10; i++)
		UART0_TXStr(unique_char);

	p_nice(3);

	for(i = 0; i < 10; i++)
		UART0_TXStr(unique_char);

	p_nice(4);

	for(i = 0; i < 10; i++)
		UART0_TXStr(unique_char);
}


/*
 * Idle process for when no other processes are in priority queue
 */
void idleProc(void)
{
    while(1);
}


/*
 * Process-kernel call function.  Supplies code and kernel message to the
 * kernel is a kernel request.  Returns the result (return code) to the caller.
 *
 * @param:      code    - kernel request code as defined in trap.h
 * @param:      arg1    - first argument required by kernel
 * @param:      arg2    - second argument required by kernel. Can be pointer to
 *                        list of more arguments
 * @returns:    value returned by kernel, depends on call type.
 */
int pkcall(int code, unsigned int arg)
{
    volatile struct kcallargs arglist;

    /* Pass code and pkmsg to kernel in arglist structure */
    arglist . code 		= (enum SVC_CODES)code;
    arglist . arg1 		= arg;
    arglist . rtnvalue 	= -1;

    /* R7 = address of arglist structure */
    assignR7((unsigned long) &arglist);

    /* Call kernel */
    SVC();

    /* Return result of request to caller */
    return arglist . rtnvalue;
}


/*
 * Returns ID of current process
 *
 * @param:		None
 * @returns:	ID of the current process
 */
int p_get_id(void)
{
    return pkcall(GETID, NULL);
}

/*
 * Returns ID of current process
 *
 * @param:		None
 * @returns:	ID of the current process
 */
int p_nice(int priority)
{
    return pkcall(NICE, priority);
}


/*
 * Terminates process
 *
 * @param:		None
 * @returns:	None
 */
void p_terminate(void)
{
	pkcall(TERMINATE, NULL);
}

