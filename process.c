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
struct CUP cup = {ESC, '[', '0', '0', ';', '0', '0', 'H', NUL};
char CLEAR_SCREEN[]     = {"\x1b[2J"};
char CURSOR_SAVE[] 		= {"\x1b""7"};
char CURSOR_HOME[] 		= {"\x1b[H"};
char CLEAR_LINE[]       = {"\x1b[2K"};
char HOME_COLOURS[]     = {"\x1b[2;30;47m"};
char CURSOR_MIDDLE[] 	= {"\x1b[20C"};
char TERM_COLOURS[]     = {"\x1b[0;0;0m"};
char CURSOR_RESTORE[] 	= {"\x1b""8"};

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
	char buf[32];
	int i = 0;
	struct CUP p =
	{	.esc 		= ESC,
		.sqrbrkt	= '[',
		.line		= "10",
		.semicolon	= ';',
		.col		= "10",
		.cmdchar	= 'H',
		.nul		= NUL
	};

	UART0_TXStr(CLEAR_SCREEN);
	drawWindow(p_get_id());
	UART0_TXStr((char *)&p);
	UART0_TXStr("HERE");
	while(1);

}


void drawWindow(unsigned int pid)
{
	char buf[32];
	int i;
	UART0_TXStr(CURSOR_HOME);
	// make top bar
	UART0_TXStr("------- P");
	if(pid < 100)
		if(pid < 10)
			UART0_TXChar('0');
		UART0_TXChar('0');
	UART0_TXStr(my_itoa(pid, buf, 10));
	UART0_TXStr(" -------\n");
	// fill out walls
	for(i = 0; i < 20; i++)
		UART0_TXStr("|                  |\n");
	// make bottom bar
	UART0_TXStr("--------------------");
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
 * Terminates process
 *
 * @param:		None
 * @returns:	None
 */
void p_terminate(void)
{
	pkcall(TERMINATE, NULL);
}

