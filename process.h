/*
 * process.h
 *
 *  Created on: Oct 17, 2019
 *      Author: Larry Hughes
 *      Editor: Finlay Miller
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <stdio.h>
#include "kernel.h"

#define PRIVATE 	static
#define SVC()		__asm(" SVC #0")
#define disable()	__asm(" cpsid i")
#define enable()	__asm(" cpsie i")
#define MSP_RTN		0xFFFFFFF9	// link register exception return using MSP
#define PSP_RTN		0xFFFFFFFD	// link register exception return using PSP
#define STACKSIZE	1024

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
    struct pcb* next;           // link to next pcb
    struct pcb* prev;           // link to previous pcb
    unsigned long sp;   // stack pointer - r13 (PSP)
	unsigned int id;  // process identifier
	unsigned long state;    // state of process
};

/* Linked list structure */
struct linked_list{
    unsigned long* next;    // link to next struct pointer
    unsigned long* prev;    // link to prev struct pointer
};

/* function declarations */
void setLR(volatile unsigned long);
unsigned long getPSP();
unsigned long getMSP();
unsigned long getSP();
void setPSP(volatile unsigned long);
void setMSP(volatile unsigned long);\
void volatile saveRegisters();
void volatile loadRegisters();
void volatile loadLR(void);


void SVCHandler(struct stack_frame *argptr);

#endif /* PROCESS_H_ */
