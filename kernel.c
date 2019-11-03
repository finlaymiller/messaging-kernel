/**
 * kernel.c
 */

/* Includes */
#include "kernel.h"

/* Private defines */
#define NUM_PRI 5   /* Number of priority queues */

static struct pcb *running;
static struct pri pri_queue[NUM_PRI];

/* TODO: do this at compile time */
void initPriQueue(void)
{
    char i;
    for(i=0; i<NUM_PRI; i++){
        pri_queue[i].head = NULL;
        pri_queue[i].tail = NULL;
    }
}

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
    /* Initialize stack memory and push initial register values */
    unsigned long *stk = (unsigned long *)malloc(STACKSIZE);
    initStack(stk, func_name);

    /* Initialize pcb memory with starting values */
    struct pcb *new_pcb = (struct pcb *)malloc(sizeof(struct pcb));
    new_pcb->id = pid;

    /* Set stack to value of high stack mem - stack frame size */
    new_pcb->sp = (unsigned long) &stk[STACKSIZE - sizeof(struct stack_frame)];

    insertPriQueue(new_pcb, priority);
}

/*
 * Insert pcb into respective priority queue
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

/*
 * Sets running pointer value
 */
void setRunning(void)
{
    char i;
    for(i=NUM_PRI-1; i>=0; i--){
        if(pri_queue[i].head != NULL){
            running = (struct pcb*) pri_queue[i].head;
            break;
        }
    }
}

struct pcb* getRunning(void)
{
    return running;
}


/*
 * Initializes stack of process
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

    sf.lr = 0xfffffffd;  //terminate process routine
    sf.pc = (unsigned long)func_name;  //entry point for process
    sf.psr = 0x01000000;

    return sf;
}

/*
 * Function to test process
 */
void procA(void)
{
    while(1){
        UART_force_out_char('a');
    }
}

/*
 * Function to test process
 */
void procB(void)
{
    while(1){
        UART_force_out_char('b');
    }
}




