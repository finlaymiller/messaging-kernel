/*
 * trap.h
 *
 *  Created on: Nov 2, 2019
 *      Author: Finlay Miller
 */

#ifndef TRAP_H_
#define TRAP_H_

#include <stdio.h>
#include "process.h"

struct kcallargs
{
	unsigned int code;
	unsigned int rtnvalue;
	unsigned int arg1;
	unsigned int arg2;
};

enum SVC_CODES
{
	GETID, NICE, TERMINATE, SEND, RECV, BIND, UNBIND
};

void SVCall(void);
void SVCHandler(struct stack_frame *);
int s_get_id(void);
int s_nice(int);
int s_terminate(void);
int s_send(void);
int s_recv(void);
int s_bind(unsigned int);
int s_unbind(unsigned int);

#endif /* TRAP_H_ */
