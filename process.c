/*
 * process.c
 *
 *  Created on: Oct 17, 2019
 *      Author: Larry Hughes
 *      Editor: Finlay Miller
 *      		Derek Capone
 */

#include "process.h"


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
int pkcall(int code, unsigned int arg1, unsigned int* arg2)
{
    volatile struct kcallargs arglist;

    /* Pass code and pkmsg to kernel in arglist structure */
    arglist . code = (enum SVC_CODES)code;
    arglist . arg1 = arg1;
    arglist . arg2 = arg2;

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
    return pkcall(GETID, NULL, NULL);
}


/*
 * Terminates process
 *
 * @param:		None
 * @returns:	None
 */
void p_terminate(void)
{
	pkcall(TERMINATE, NULL, NULL);
}
