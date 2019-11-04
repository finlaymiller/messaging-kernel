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
#include "calls.h"
#include "uart.h"

#define NVIC_INT_CTRL_R			(*((volatile unsigned long *) 0xE000ED04))
#define NVIC_SYS_PRI3_R			(*((volatile unsigned long *) 0xE000ED20))
#define TRIGGER_PENDSV			0x10000000
#define PENDSV_LOWEST_PRIORITY	0x00E00000

#define PendSVMinPri()		(NVIC_SYS_PRI3_R |= PENDSV_LOWEST_PRIORITY)

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
void PendSV_Handler(void);

#endif /* TRAP_H_ */
