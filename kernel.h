/*
 * kernel.h
 *
 *  Created on: Oct 17, 2019
 *      Author: Finlay Miller
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

#define UNBLOCKED -1

struct pri{
    unsigned long *head;    // holds pointer to first pcb in priority queue
    unsigned long *tail;    // holds pointer to last pcb in priority queue
};

/* function declarations */
/* initialization */
void initKernel(void);
void initStack(unsigned long *stk, void(*func_name)());
struct stack_frame initStackFrame(void(*func_name)());
void initRunning(void);
void initPriQueue(void);
void initMailroom(void);
/* process */
void reg_proc(void(*func_name)(), unsigned int pid, unsigned char priority);
void nextProcess(void);

void insertPriQueue(struct pcb *new_pcb, unsigned char priority);
/* running */
struct pcb* getNextRunning(void);
void setRunningSP(unsigned long*);
struct pcb* getRunning(void);
void setNextRunning(void);
void setRunning(struct pcb *new_running);
void removePriQueue(void);

void assignR7(volatile unsigned long);
int checkHighPriority(void);


#endif /* KERNEL_H_ */
