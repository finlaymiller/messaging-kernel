/**
 * kernel.c
 */

#include "kernel.h"

/*
 * Registers process by:
 * - allocating memory for stack size and pcb
 * - initializing stack with initial register values
 * - initializing pcb values
 * - inserting into desired priority queue
 */
void reg_proc(void(*func_name)(), unsigned int pid, unsigned char priority)
{
    /* Initialize stack memory and push initial register values */
    unsigned long *stk = (unsigned long *)malloc(STACKSIZE);
    initStack(stk, func_name);

    /* Initialize pcb memory with starting values */
    struct pcb *new_pcb = (struct pcb *)malloc(sizeof(struct pcb));
    new_pcb->id = pid;
    new_pcb->next = NULL;
    new_pcb->prev = NULL;
}


/*
 * Initializes stack
 */
void initStack(unsigned long *stk, void(*func_name)())
{
    /* Generate initial stack values */
    struct stack_frame sf = initStackFrame(func_name);

    /* Copy stack frame into stack memory */
    memcpy(&stk[STACKSIZE - sizeof(sf)], &sf, sizeof(sf));
}


/*
 * Creates and returns initial stack frame values
 */
struct stack_frame initStackFrame(void(*func_name)())
{
    struct stack_frame sf;

    sf.r0 = 0x00000000;
    sf.r1 = 0x11111111;
    sf.r2 = 0x22222222;
    sf.r3 = 0x33333333;
    sf.r4 = 0x44444444;
    sf.r5 = 0x55555555;
    sf.r6 = 0x66666666;
    sf.r7 = 0;
    sf.r8 = 0;
    sf.r9 = 0;
    sf.r10 = 0;
    sf.r11 = 0;
    sf.r12 = 0;

    sf.lr = 0;  //terminate process routine
    sf.pc = (unsigned long)func_name;  //entry point for process
    sf.psr = 0x01000000;

    return sf;
}









