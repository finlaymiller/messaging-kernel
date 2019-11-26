/**
 * kernel.c
 */

/* Includes */
#include "kernel.h"

/* globals */
struct pcb *running;		// pointer to pcb of currently running process
struct pri pri_queue[NUM_PRI] = {NULL};
extern struct message* mailpile;
extern struct mailbox mailroom[NUM_MAILBOXES];

/*******************	 INITIALIZATION FUNCTIONS    *************************/

/*
 * Description
 *
 * @param:
 * @returns:
 */
void initKernel(void)
{

	initUART();
	initPriQueue();
	mailpile = initMessages();

	reg_proc(&idleProc, 0, 0);
}

/*
 * Initializes stack of process
 *
 * @param:		stk       -
 * 				func_name -
 * @returns:	None
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
 *
 * @param:
 * @returns:
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
 * Description
 *
 * @param:
 * @returns:
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

/* TODO: do this at compile time */
void initPriQueue(void)
{
    char i;
    for(i=0; i<NUM_PRI; i++){
        pri_queue[i].head = NULL;
        pri_queue[i].tail = NULL;
    }
}

/*******************	PROCESS-RELATED FUNCTIONS    *************************/

/*
 * Registers process by:
 * - allocating memory for stack size and pcb
 * - initializing stack with initial register values
 * - initializing pcb values
 * - inserting into desired priority queue
 * "priority" can be 1-4 (4 highest)
 */
void reg_proc(void(*func_name)(), unsigned int pid, unsigned char priority)
{
	unsigned int i;

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

    /* empty mailbox list */
    for(i = 0; i < NUM_MBX_PER_PROC; i++)
    	new_pcb->mbxs[i] = NULL;

    insertPriQueue(new_pcb, priority);
}

/*
 * Changes "running" to the next process in the priority queue
 *
 * @param:
 * @returns:
 */
void nextProcess(void)
{
    running = running->next;

    /* Set new stack pointer */
    setPSP(running->sp);
}

/*
 * Insert pcb into respective priority queue
 *
 * @param:
 * @returns:
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

        /* Point tail to new pcb */
        struct pcb *tmp = (struct pcb*)pri_queue[priority].tail;
        tmp->next = new_pcb;

        /* Set up new tail */
        pri_queue[priority].tail = (unsigned long*)new_pcb;
    }
}

/*******************	RUNNING-RELATED FUNCTIONS    *************************/

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

	for(i = NUM_PRI-1; i>=0; i--){
		if(pri_queue[i].head)
		{
			next_to_run = (struct pcb *)pri_queue[i].head;
			UART0_TXStr("\nSwitching to priority level ");
			UART0_TXChar((char)i);
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
 * Sets running stack pointer value
 *
 * @param:
 * @returns:
 */
void setRunningSP(unsigned long* new_sp)
{
    running->sp = (unsigned long)new_sp;
}


/*
 * Description: Checks for processes in the highest priority
 *
 * @param:
 * @returns: Highest priority with processes waiting to run
 */
int checkHighPriority(void)
{
    int i;
    for(i=NUM_PRI-1; i>=0; i--){
        if(pri_queue[i].head != NULL) return i;
    }

    /* Return error value if no processes found */
    return -1;
}

/*
 * Gets running pointer value
 *
 * @param:
 * @returns:
 */
struct pcb* getRunning(void)
{
    return running;
}

void printPriQueue(void)
{
	char buf[32] = {0};
	int i;
	struct pcb *pcbptr = NULL;


	UART0_TXStr("\nPrinting current priority queue...");
	for(i = NUM_PRI; i >= 0; i--)
	{
		UART0_TXStr("\n[Level ");
		UART0_TXStr(my_itoa(i, buf, 10));
		UART0_TXStr("]\t");

		if(pri_queue[i].head != NULL)
		{
			pcbptr = (struct pcb *)pri_queue[i].head;

			while(pcbptr != NULL)
			{
				// print process IDs
				UART0_TXStr("P");
				if(pcbptr->id < 100)	// zero-padding
					UART0_TXStr("0");
					if(pcbptr->id < 10)
								UART0_TXStr("0");
				UART0_TXStr(my_itoa(pcbptr->id, buf, 10));

				UART0_TXStr(" -> ");
				pcbptr = pcbptr->next;
			}
		}
		else UART0_TXStr("NO RUNNING PROCESSES");
	}
}
