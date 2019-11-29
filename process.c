/*
 * process.c
 *
 *  Created on: Oct 17, 2019
 *      Author: Larry Hughes
 *      Editor: Finlay Miller
 *      		Derek Capone
 */

#include "process.h"

#define SEND_PROC_NUM 1
#define RECV_PROC_NUM 2
#define RAND_PROC_NUM 3

#define SLOW_TEXT 100000

extern char *BIND_ERR_PRINTS[3];

void procSendRecv(void)
{
	char 	msg[128];
	int 	mbx = p_bind(RECV_PROC_NUM);
	int 	rcode;


	rcode = p_recv(mbx, mbx, msg, 1);

    while(1){
        //UART0_TXChar('x');
        UART0_TXChar(msg[0]);
    }
}

void procSend(void)
{
    int     mbx = p_bind(SEND_PROC_NUM);

    int rcode = p_send(SEND_PROC_NUM, RECV_PROC_NUM, "K", 1);
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
    while(1){
        UART0_TXChar('a');
    }
}

/*
 * Function to test process
 */
void procB(void)
{
    while(1){
        UART0_TXChar('b');
    }
}

/*
 * Function to test process
 */
void procC(void)
{
    int i;
    for(i=0; i<100; i++){
        UART0_TXChar('c');
        waitTime(SLOW_TEXT);
    }

    nice(2);

    for(i=0; i<100; i++){
        UART0_TXChar('f');
        waitTime(SLOW_TEXT);
    }
}

void procD(void)
{
    int i;
    for(i=0; i<100; i++){
        UART0_TXChar('d');
        waitTime(SLOW_TEXT);
    }

    nice(2);

    for(i=0; i<100; i++){
        UART0_TXChar('y');
        waitTime(SLOW_TEXT);
    }
}

void procE(void)
{
    int i;
    for(i=0; i<100; i++){
        UART0_TXChar('e');
        waitTime(SLOW_TEXT);
    }

    nice(2);

    for(i=0; i<100; i++){
        UART0_TXChar('y');
        waitTime(SLOW_TEXT);
    }
}

void waitTime(int x)
{
    int i = x;
    while(i){
        i--;
    }
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
    enablePendSV(FALSE);

    volatile struct kcallargs arglist;

    /* Pass code and pkmsg to kernel in arglist structure */
    arglist . code 		= (enum SVC_CODES)code;
    arglist . arg1 		= arg;
    arglist . rtnvalue 	= -1;

    /* R7 = address of arglist structure */
    assignR7((unsigned long) &arglist);

    /* Call kernel */
    SVC();

    enablePendSV(TRUE);

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
 * Terminates process
 *
 * @param:		None
 * @returns:	None
 */
void p_terminate(void)
{
    pkcall(TERMINATE, NULL);
}

