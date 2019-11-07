/*
 * calls.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller
 *
 *  All functions that processes need to access through supervisor calls are
 *  stored here. They are prefixed with 's_' to indicate that they are
 *  supervisor-level functions rather than process (p_) or kernel (k_).
 *
 *
 *  todo	write nice() function
 *  todo	write send() function
 *  todo	write recv() function
 *  todo	write bind() function
 *  todo	write unbind() function
 *  todo	write terminate() function
 */

#include "calls.h"

/*
 * Description
 *
 * @param:		None
 * @returns:	ID of the current process
 */
int s_get_id(void)
{
    struct pcb *running = getRunning();
    return running->id;
}

int getid(void)
{
    volatile struct kcallargs getidarg; /* Volatile to actually reserve space on stack */
    getidarg . code = GETID;

    /* Assign address if getidarg to R7 */
    assignR7((unsigned long) &getidarg);

    SVC();

    return getidarg . rtnvalue;

}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int s_nice(int priority)
{

	return 0;
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int s_terminate(void)
{

	return 0;
}

void p_terminate(void)
{
    return;
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int s_send(void)
{

	return 0;
}


/*
 * Description
 *
 * @param:
 * @returns:
 */
int s_recv(void)
{

	return 0;
}


/*
 * Description
 *
 * @param:		Mailbox number to bind to. 0 if any.
 * @returns:
 */
int s_bind(unsigned int mailbox_number)
{

	return 0;
}


/*
 * Description
 *
 * @param:		Mailbox number to unbind from. 0 if all.
 * @returns:
 */
int s_unbind(unsigned int mailbox_number)
{

	return 0;
}
