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
/******************* Abandon all hope, ye who enter here! ********************/
/*
 *  Process for testing sending and receiving functionality
 * 
 * Arguments:
 *      None
 * Returns:
 *      None
 */
void procSendRecv(void)
{
    char    msg[128];
    int     mbx = p_bind(RECV_PROC_NUM);
    int     rcode;
    int i;

    for(i=0; i<50; i++){
        UART0_TXChar('Q');
        waitTime(SLOW_TEXT);
    }

    rcode = p_recv(mbx, mbx, msg, 1);

    repositionCursor(5, 1);

    for(i=0; i<50; i++){
        UART0_TXChar(msg[0]);
        waitTime(SLOW_TEXT);
    }
}



/**************** Seriously, you don't have to read these... *****************/
/*
 *  Process to test sending functionality
 * 
 * Arguments:
 *      None
 * Returns:
 *      None
 */
void procSend(void)
{
    char ch = (char) p_get_id();
    int mbx = p_bind(SEND_PROC_NUM);
    int rcode = p_send(SEND_PROC_NUM, RECV_PROC_NUM, &ch, 1);
}


/*
 *  Process to test blocking on receive
 * 
 * Arguments:
 *      None
 * Returns:
 *      None
 */
void procBlockRecv(void)
{
    char    msg[128];
    int     mbx = p_bind(RECV_PROC_NUM);
    int     rcode;
    int     i;

    rcode = p_recv(mbx, mbx, msg, 1);
    repositionCursor(5, 1);

    for(i=0; i<50; i++){
        UART0_TXChar(msg[0]);
        waitTime(SLOW_TEXT);
    }
}


/*
 *  Process to test send with blocking recv on other end
 * 
 * Arguments:
 *      None
 * Returns:
 *      Still None
 */
void procBlockSend(void)
{
    int i;
    for(i=0; i<50; i++){
        UART0_TXChar('W');
        waitTime(SLOW_TEXT);
    }

    char ch = (char) p_get_id();

    int     mbx = p_bind(SEND_PROC_NUM);

    int rcode = p_send(SEND_PROC_NUM, RECV_PROC_NUM, &ch, 1);

    for(i=0; i<50; i++){
        UART0_TXChar('D');
        waitTime(SLOW_TEXT);
    }
}


/*
 *  Process to test binding and unbinding
 * 
 * Arguments:
 *      None
 * Returns:
 *      Guess...
 */
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
 *  Prints characters
 * 
 * Arguments:
 *      NONE
 * Returns:
 *      ZERO
 */
void procPrinter(void)
{
    int i;
    char ch = (char) p_get_id();
    for(i=0; i<50; i++){
        UART0_TXChar(ch);
        waitTime(SLOW_TEXT);
    }
}


/*
 *  Alright I'm done making these
 * 
 * Arguments:
 *      ZIP
 * Returns:
 *      ZILCH
 */
void procNiceA(void)
{
    /* Set base values for row and column */
    int b_row = 1;
    int b_col = 1;
    repositionCursor(b_row, b_col);

    int i;
    for(i=0; i<20; i++){
        UART0_TXChar('a');
        waitTime(SLOW_TEXT);
    }

    /* Lower priority */
    p_nice(1);

    /* Position cursor below initial position */
    repositionCursor(b_row+1, b_col);

    for(i=0; i<20; i++){
        repositionCursor(b_row+1, b_col);
        UART0_TXChar('a');
        b_col++;
        waitTime(SLOW_TEXT);
    }
}


void procNiceB(void)
{
    /* Set base values for row and column */
    int b_row = 5;
    int b_col = 1;
    repositionCursor(b_row, b_col);

    int i;
    for(i=0; i<20; i++){
        UART0_TXChar('b');
        waitTime(SLOW_TEXT);
    }

    /* Lower priority */
    p_nice(1);

    /* Position cursor below initial position */
    repositionCursor(b_row+1, b_col);

    for(i=0; i<20; i++){
        repositionCursor(b_row+1, b_col);
        UART0_TXChar('b');
        b_col++;
        waitTime(SLOW_TEXT);
    }
}


void procNiceC(void)
{
    /* Set base values for row and column */
    int b_row = 10;
    int b_col = 1;
    repositionCursor(b_row, b_col);

    int i;
    for(i=0; i<20; i++){
        UART0_TXChar('c');
        waitTime(SLOW_TEXT);
    }

    /* Lower priority */
    p_nice(1);

    /* Position cursor below initial position */
    repositionCursor(b_row+1, b_col);

    for(i=0; i<20; i++){
        repositionCursor(b_row+1, b_col);
        UART0_TXChar('c');
        b_col++;
        waitTime(SLOW_TEXT);
    }
}

void procDrawGUI(void)
{
	drawTrainset();
	while(1);
}


