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
typedef struct stackframe
{
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
} stack_frame;

/* process control block */
typedef struct process_control_block
{
	unsigned long sp;	// stack pointer - r13 (PSP)
	pcb* next;			// link to next pcb
	pcb* prev;			// link to previous pcb
} pcb;

/* function declarations */
void setLR(volatile unsigned long);
unsigned long getPSP();
unsigned long getMSP();
unsigned long getSP();
void setPSP(volatile unsigned long);
void setMSP(volatile unsigned long);\
void volatile saveRegisters();
void volatile loadRegisters();

#endif /* PROCESS_H_ */
