/*
 * trap.h
 *
 *  Created on: Nov 2, 2019
 *      Author: Finlay Miller
 */

#ifndef TRAP_H_
#define TRAP_H_

#include <stdio.h>
#include "kernel.h"
#include "process.h"
#include "calls.h"
#include "uart.h"
#include "register.h"

#define SVC()					__asm(" SVC #0")
#define NVIC_INT_CTRL_R			(*((volatile unsigned long *) 0xE000ED04))
#define NVIC_SYS_PRI3_R			(*((volatile unsigned long *) 0xE000ED20))
#define TRIGGER_PENDSV			0x10000000
#define PENDSV_LOWEST_PRIORITY	0x00E00000
#define INT_CTRL_PENDSV     (1<<28)     // bit for generating PendSV interrupt
#define INT_CTRL_UNPENDSV   (1<<27)     // bit to remove pending state for PendSV

#define PendSVMinPri()		(NVIC_SYS_PRI3_R |= PENDSV_LOWEST_PRIORITY)

enum SVC_CODES
{
	GETID, NICE, TERMINATE, SEND, RECV, BIND, UNBIND
};

struct kcallargs
{
	enum SVC_CODES	code;
	int 			rtnvalue;
	unsigned int 	arg1;
	unsigned int*	arg2;
};

void SVCall(void);
void PendSV_Handler(void);

#endif /* TRAP_H_ */
