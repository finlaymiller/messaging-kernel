/**
 * kernel.c
 */

#include "kernel.h"

extern struct pcb *running;
struct pcb* process_queue[NUM_PRI_LVLS];

void kernelInit(void)
{
	/* Initialize UART */
	UART0_Init();           // Initialize UART0

	PendSVMinPri();
}

/*
 * Priority Process Queue Initialization
 *
 * @param:		None
 * @returns:	None
 */
void initProcessTable(void)
{
	int i;
	for(i = 0; i < NUM_PRI_LVLS; i++)
		process_queue[i] = NULL;
}

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

/*
 * Search process priority queues to find next process to run.
 * This function assumes that all pcbs are already correctly linked.
 *
 * Remove debugging prints before handing in
 *
 * @param:		None
 * @returns:	Pointer to PCB of next process to run
 */
struct pcb* getNextRunning(void)
{
	struct pcb* next_to_run = NULL;
	int i;

	for(i = 0; i < NUM_PRI_LVLS; i++)
	{
		if(process_queue[i])
		{
			next_to_run = process_queue[i];
			UART0_TXStr("\nSwitching to priority level ");
			UART0_TXChar((char)i);
		}
	}

	if(!next_to_run)	// no process found, switch to idle process
	{
		UART0_TXStr("\nNo process found during getNextRunning\nIdling...");
		next_to_run = process_queue[0];
	}

	return next_to_run;
}

/*
 * Move to next process AT SAME PRIORITY LEVEL
 *
 *
 * @param:		None
 * @returns:	None
 */
void nextProcess(void)
{
	running->sp = getPSP();			// save current SP

	if(running->next == running)	// process is alone on level
	{
		// handle process deletion, link breaking, etc, here
		running = getNextRunning();
	}
	else
		running = running->next;	// move to next process

	setPSP(running->sp);			// start process
}
