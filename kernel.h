/*
 * kernel.h
 *
 *  Created on: Oct 17, 2019
 *      Author: Finlay Miller, Derek Capone
 * 
 * Headerfile for all non-call kernel-related functions.
 */

#ifndef KERNEL_H_
#define KERNEL_H_

struct pcb;   //forward declaration for struct

#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "uart.h"
#include "systick.h"
#include "trap.h"

#define UNBLOCKED -1    // used in PCB status field

struct pri{
    unsigned long *head;    // holds pointer to first pcb in priority queue
    unsigned long *tail;    // holds pointer to last pcb in priority queue
};

/* function declarations */
/* initialization functions */
void initKernel(void);
void initStack(unsigned long *stk, void(*func_name)());
struct stack_frame initStackFrame(void(*func_name)());
void initRunning(void);
void reg_proc(void(*func_name)(), unsigned int, unsigned char);
/* priority related fucntions */
void nextProcess(void);
void insertPriQueue(struct pcb *, unsigned char);
void removePriQueue(void);
int checkHighPriority(void);
/* running related functions */
void setNextRunning(void);
struct pcb* getNextRunning(void);
void setRunningSP(unsigned long*);
void setRunning(struct pcb *);
struct pcb* getRunning(void);
void setNextRunning(void);
void setRunning(struct pcb *new_running);
void removePriQueue(void);

#endif /* KERNEL_H_ */
