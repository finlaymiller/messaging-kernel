/*
 * kernel.h
 *
 *  Created on: Oct 17, 2019
 *      Author: Finlay Miller
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "uart.h"
#include "systick.h"
#include "trap.h"

#define TRUE  1
#define FALSE 0

void kernelInit(void);
void reg_proc(void(*func_name)(), unsigned int pid, unsigned char priority);
void initStack(unsigned long *stk, void(*func_name)());
struct stack_frame initStackFrame(void(*func_name)());
struct pcb* getNextRunning(void);
void nextProcess(void);

#endif /* KERNEL_H_ */
