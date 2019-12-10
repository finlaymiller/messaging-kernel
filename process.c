/*
 * process.c
 *
 *  Created on: Oct 17, 2019
 *      Author: Larry Hughes
 *      Editor: Finlay Miller and Derek Capone
 * 
 * This file contains the process-side functions for process-kernel calls that
 * are not related to messaging, as well as the processes used for testing.
 */

#include "process.h"

/* local definitions */
#define SEND_PROC_NUM 1
#define RECV_PROC_NUM 2
#define RAND_PROC_NUM 3
#define SLOW_TEXT 100000
#define IDLE_TEXT 200000

/* globals */
extern char *BIND_ERR_PRINTS[3];


/*
 * Process-kernel call function.  Supplies code and kernel message to the
 * kernel is a kernel request.  Returns the result (return code) to the caller.
 *
 * Arguments:
 *      [int] Kernel request code as defined in trap.h
 *      [int] First argument required by kernel
 * Returns:    
 *      [int] Value returned by kernel, depends on call type.
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
 * Arguments:
 * 		None
 * Returns:
 *  	ID of the current process
 */
int p_get_id(void)
{
    return pkcall(GETID, NULL);
}

/*
 * Change current process priority
 *
 * Arguments:
 * 		Desired priority
 * Returns:
 * 	    Priority of process after switch
 */
int p_nice(int priority)
{
	return pkcall(NICE, priority);
}

/*
 * Terminate current process
 *
 * Arguments:
 * 		None
 * Returns:
 *  	None
 */
void p_terminate(void)
{
    pkcall(TERMINATE, NULL);
}

/*
 * Slow down! This function is only used for testing purposes.
 * 
 * Arguments:
 *      [int] How long to wait for with 0 being no time at all and 65535
 *      being a bit longer than you had hoped. Very precise, we know.
 * Returns:
 *      None 
 */
void waitTime(unsigned int x)
{
    unsigned int i = x;
    while(i){
        i--;
    }
}

/*
 * Idle process for when no other processes are in priority queue
 * 
 * Arguments:
 *      None
 * Returns:
 *      None
 */
void idleProc(void)
{
    char ch = 'a';
    while(1){
        repositionCursor(0, 80);
        UART0_TXChar(ch);
        waitTime(IDLE_TEXT);
        ch++;
        if(ch > 'z') ch = 'a';  // loop chars
    }
}

/**************************** Processes for Testing **************************/
void procDrawGUI(void)
{
    int mbx, rcode, hall;
    char msg_buf[128] = '\x0';

    /* get ready to receive hall updates */
    mbx = p_bind(0);
    if (mbx <= 0) return;

	drawTrainset(); // draw initial trainset diagram

    /* block on receive, when hall update is received, uodate GUI */
	while(1)
    {   
        // receive hall number update                 
        // HALL_..._SRVR is the mailbox of the process that would be sending
        // hall sensor updates, but alas, we ran out of time and this was never
        // fully implemented
        rcode = p_recv(mbx, HALL_UPDATE_SRVR, msg_buf, 10);
        my_atoi(msg_buf, (int *)hall);  // convert to integer
        updateTrainPos(hall);   // draw GUI
    }
}
