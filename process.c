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


void procA(void)
{
	char unique[] = "a";

	UART0_TXStr(unique);
	p_nice(2);
	UART0_TXStr(unique);
	p_nice(3);
	UART0_TXStr(unique);
}

ProcA prints

void procB(void)
{
	char unique[] = "b";

	UART0_TXStr(unique);
	p_nice(2);
	UART0_TXStr(unique);
	p_nice(3);
	UART0_TXStr(unique);
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
	UART0_TXStr("\nIdling");
    while(1)
    {
    	UART0_TXStr(".");
    	waitTime(SLOW_TEXT * 5);
    }
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
 * Change current process priority
 *
 * @param:		Desired priority
 * @returns:	priority of process after switch
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

