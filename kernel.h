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

#define TRUE  1
#define FALSE 0

void initKernel(void);
void reg_proc(void(*func_name)(), unsigned int pid, unsigned char priority);
void initStack(unsigned long *stk, void(*func_name)());
struct stack_frame initStackFrame(void(*func_name)());
struct pcb* getNextRunning(void);
void nextProcess(void);

struct pri{
    unsigned long *head;    //holds pointer to first pcb in priority queue
    unsigned long *tail;    //holds pointer to last pcb in priority queue
};

void reg_proc(void(*func_name)(), unsigned int pid, unsigned char priority);
void initStack(unsigned long *stk, void(*func_name)());
struct stack_frame initStackFrame(void(*func_name)());
void setRunningSP(unsigned long*);
void initRunning(void);
struct pcb* getRunning(void);
void initPriQueue(void);
void insertPriQueue(struct pcb *new_pcb, unsigned char priority);
void nextProcess(void);
void assignR7(volatile unsigned long);

extern void procA(void);
extern void procB(void);
extern void procC(void);

#endif /* KERNEL_H_ */
