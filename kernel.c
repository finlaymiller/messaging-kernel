/*
 * kernel.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller and Derek Capone
 *
 *  All kernel-level functions OTHER than kernel calls made by processes are
 *  located in this file.
 */

/* includes */
#include "kernel.h"

/* globals */
struct pcb *running;		// pointer to pcb of currently running process
struct pri pri_queue[NUM_PRI] = {NULL}; // list of all processes
extern struct message* mailpile;    // linked-list of all available empty msgs
extern struct mailbox mailroom[NUM_MAILBOXES];  // array of mailboxes

/*******************	 INITIALIZATION FUNCTIONS    *************************/
/*
 *  Initialize the kernel. Open UART0 connection, initialize priority process
 *  queues, allocate all messages to be used, and start idle process.
 *
 *  Arguments:
 *      None
 *  Returns:
 *      None
 */
void initKernel(void)
{
	initUART();
	mailpile = initMessages();
	reg_proc(&idleProc, 0, 0);
}


/*
 * Initializes stack of a process
 *
 *  Arguments:
 *      [long] Pointer to top of stack    
 * 		[function] Process function
 *  Returns:
 *      None
 */
void initStack(unsigned long *stk, void(*func_name)())
{
    /* Generate initial stack values */
    struct stack_frame sf = initStackFrame(func_name);

    /* Copy stack frame into stack memory */
    memcpy(&stk[STACKSIZE - sizeof(sf)], &sf, sizeof(sf));
}


/*
 *  Creates and returns initial stack frame values
 *
 *  Arguments:
 *      [function] Process function
 *  Returns:
 *      [struct stack_frame] The stack frame of the process
 */
struct stack_frame initStackFrame(void(*func_name)())
{
    struct stack_frame sf;

    sf.r0  = 0x00000000;
    sf.r1  = 0x11111111;
    sf.r2  = 0x22222222;
    sf.r3  = 0x33333333;
    sf.r4  = 0x44444444;
    sf.r5  = 0x55555555;
    sf.r6  = 0x66666666;
    sf.r7  = 0x77777777;
    sf.r8  = 0x88888888;
    sf.r9  = 0x99999999;
    sf.r10 = 0x10101010;
    sf.r11 = 0x11111111;
    sf.r12 = 0x12121212;
    sf.lr = (unsigned long)p_terminate;	// terminate process routine
    sf.pc = (unsigned long)func_name;  	// entry point for process
    sf.psr = 0x01000000;

    return sf;
}

/*
 *  Initializes running queue
 *
 *  Arguments:
 *      None
 *  Returns:
 *      None
 */
void initRunning(void)
{
    int i;
    for(i = NUM_PRI-1; i >= 0; i--){
        if(pri_queue[i].head != NULL){
            running = (struct pcb*) pri_queue[i].head;
            break;
        }
    }
}

/*
 *  Registers process by:
 *      - allocating memory for stack size and pcb
 *      - initializing stack with initial register values
 *      - initializing pcb values
 *      - inserting into desired priority queue
 *      - "priority" can be 1-4 (4 highest)
 *      - PID can be 1-65535
 * 
 *  Arguments:
 *      [function] Process function
 *      [int] Process ID
 *      [char] Priority level to run at
 *  Returns:
 *      None
 */
void reg_proc(void(*func_name)(), unsigned int pid, unsigned char priority)
{
	unsigned int i;

    /* catch bad PID */
    if(pid == 0)
        return -1;

    /* Initialize stack memory and push initial register values */
    unsigned long *stk = (unsigned long *)malloc(STACKSIZE);
    initStack(stk, func_name);

    /* Initialize pcb memory with starting values */
    struct pcb *new_pcb = (struct pcb *)malloc(sizeof(struct pcb));
    new_pcb->id = pid;

    /* Set stack to value of high stack mem - stack frame size */
    new_pcb->sp = (unsigned long) &stk[STACKSIZE - sizeof(struct stack_frame)];
    new_pcb->stk = stk;  //save top of stack to free memory later

    /* Set priority in PCB */
    new_pcb->pri = priority;
    new_pcb->pri_switch = 0;

    /* Set state to not blocked */
    new_pcb->state = UNBLOCKED;

    /* empty mailbox list */
    for(i = 0; i < NUM_MBX_PER_PROC; i++)
    	new_pcb->mbxs[i] = NULL;

    insertPriQueue(new_pcb, priority);
}

/*******************	PRIORITY RELATED FUNCTIONS    ************************/
/*
 * Changes "running" to the next process in the priority queue
 *
 *  Arguments:
 *      None
 *  Returns:
 *      None
 */
void nextProcess(void)
{
    if(running->state != UNBLOCKED) {
        /* If process was blocked, find next process to run */
        running = getNextRunning();
    } else if(running->pri_switch == TRUE) {
        /* If priority switch occured, reset flag and find next process to run */
        running->pri_switch = FALSE;
        running = getNextRunning();
    } else
        running = running->next;

    /* Set new stack pointer */
    setPSP(running->sp);
}

/*
 *  Insert process PCB into respective priority queue
 *
 *  Arguments:
 *      [struct pcb] PCB of process to enter into priority queue
 *      [char] Priority at which to run
 *  Returns:
 *      None
 */
void insertPriQueue(struct pcb *new_pcb, unsigned char priority)
{
    if(pri_queue[priority].head == NULL){
        /* If priority queue empty */
        new_pcb->next = new_pcb;
        new_pcb->prev = new_pcb;
        pri_queue[priority].head = (unsigned long*)new_pcb;
        pri_queue[priority].tail = (unsigned long*)new_pcb;
    } else {
        /* If priority queue not empty */
        new_pcb->next = (struct pcb*)pri_queue[priority].head;
        new_pcb->prev = (struct pcb*)pri_queue[priority].tail;

        /* Fix prev of head */
        struct pcb *tmp_h = (struct pcb*)pri_queue[priority].head;
        tmp_h->prev = new_pcb;

        /* Point tail to new pcb */
        struct pcb *tmp = (struct pcb*)pri_queue[priority].tail;
        tmp->next = new_pcb;

        /* Set up new tail */
        pri_queue[priority].tail = (unsigned long*)new_pcb;
    }
}

/*
 * Removes running PCB from priority queue
 *
 *  Arguments:
 *      None
 *  Returns: 
 *      None
 */
void removePriQueue(void)
{
    if(running->next == running){
        /* If this is the last process in the priority queue */
        pri_queue[running->pri].head = NULL;
        pri_queue[running->pri].tail = NULL;
    } else {
        /* Reset head or tail if necessary */
        if(pri_queue[running->pri].head == (unsigned long*)running){
            pri_queue[running->pri].head = (unsigned long*)running->next;
        } else if(pri_queue[running->pri].tail == (unsigned long*)running){
            pri_queue[running->pri].tail = (unsigned long*)running->prev;
        }

        /* Remove running from linked list */
        running->prev->next = running->next;
        running->next->prev = running->prev;
    }
}

/*
 *  Checks for processes in the highest priority
 *
 *  Arguments:
 *      None
 *  Returns: 
 *      [int] Highest priority with processes waiting to run, -1 if no process
 *              was found
 */
int checkHighPriority(void)
{
    int i;
    for(i = NUM_PRI - 1; i >= 0; i--)
        if(pri_queue[i].head != NULL) return i;

    return -1;  // Return error value if no processes found
}

/*******************	RUNNING RELATED FUNCTIONS    *************************/
/*
 *  Finds process at highest priority to run
 * 
 *  Arguments:
 *      None
 *  Returns:
 *      None
 */
void setNextRunning(void)
{
    struct pcb *new_running = getNextRunning();
    running = new_running;
    setPSP(running->sp);
}

/*
 *  Search process priority queues to find next process to run.
 *  This function assumes that all PCBs are already correctly linked.
 *
 *  Arguments:
 * 		None
 *  Returns:
 *  	[struct pcb] Pointer to PCB of next process to run
 */
struct pcb* getNextRunning(void)
{
	int i;
	struct pcb* next_to_run = NULL;

	for(i = NUM_PRI-1; i>=0; i--){
		if(pri_queue[i].head)
		{
			next_to_run = (struct pcb *)pri_queue[i].head;
			break;
		}
	}

	if(!next_to_run)	// no process found, switch to idle process
	{
		UART0_TXStr("\nNo process found during getNextRunning\nIdling...");
		next_to_run = (struct pcb *)pri_queue[0].head;
	}

	return next_to_run;
}

/*
 *  Sets running stack pointer value
 *
 *  Arguments:
 *      [long] Pointer to top of process stack
 *  Returns:
 *      None
 */
void setRunningSP(unsigned long* new_sp)
{
    running->sp = (unsigned long)new_sp;
}

/*
 *  Updates global `running` variable with PCB of running process
 *
 *  Arguments:
 *      [struct pcb] PCB to update variable with
 *  Returns:
 *      None
 */
void setRunning(struct pcb *new_running)
{
    running = new_running;
}

/*
 *  Gets PCB of running process
 *
 *  Arguments:
 *      None
 *  Returns:
 *      [struct pcb] Pointer to PCB of running process
 */
struct pcb* getRunning(void)
{
    return running;
}
