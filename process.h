/*
 * process.h
 *
 *  Created on: Oct 17, 2019
 *      Author: Larry Hughes
 *      Editor: Finlay Miller
 *      		Derek Capone
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <stdio.h>
#include "kernel.h"
#include "mail.h"
#include "trap.h"

#define PRIVATE 		static
#define MSP_RTN			0xFFFFFFF9	// link register exception return using MSP
#define PSP_RTN			0xFFFFFFFD	// link register exception return using PSP
#define STACKSIZE		1024
#define	NUM_PRI			5

/* Cortex default stack frame */
struct stack_frame{
	/* Registers saved explicitly by the software */
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	unsigned long r7;
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	/* Registers saved implicitly by the hardware */
	unsigned long r0;
	unsigned long r1;
	unsigned long r2;
	unsigned long r3;
	unsigned long r12;
	unsigned long lr;
	unsigned long pc;
	unsigned long psr;
};

/* process control block */
struct pcb{
    struct pcb* next;       // link to next pcb
    struct pcb* prev; 		// link to previous pcb
    unsigned long sp;   	// stack pointer - r13 (PSP)
    unsigned int  id;  		// process identifier
	unsigned long state;    // state of process
	unsigned char pri;      // priority of the process
	unsigned long *stk;
};

/* linked list structure */
struct linked_list{
    unsigned long* next;    // link to next struct pointer
    unsigned long* prev;    // link to prev struct pointer
};

/* function declarations */
extern void procA(void);
extern void procB(void);
extern void procC(void);
extern void idleProc(void);
int pkcall(int , unsigned int , unsigned int*);
int p_get_id(void);
void p_terminate(void);

#endif /* PROCESS_H_ */
