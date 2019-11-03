/*
 * trap.c
 *
 *  Created on: Nov 2, 2019
 *      Author: Finlay Miller
 *
 *
 *  NOTE: All the comments I deleted from SVCHandler are still present in the
 *  original file, SVC_example.c.
 */

#include "trap.h"

extern void systick_init();

struct pcb *running;


/*
 * Supervisor call (trap) entry point
 * Using MSP - trapping process either MSP or PSP (specified in LR)
 * Source is specified in LR: F1 (MSP) or FD (PSP)
 * Save r4-r11 on trapping process stack (MSP or PSP)
 * Restore r4-r11 from trapping process stack to CPU
 * SVCHandler is called with r0 equal to MSP or PSP to access any arguments
 *
 * @param: 		None
 * @returns:	None
 */
void SVCall(void)
{
	/* Save LR for return via MSP or PSP */
	__asm(" 	PUSH 	{LR}");

	/* Trapping source: MSP or PSP? */
	__asm(" 	TST 	LR,#4");	/* Bit #3 (0100b) indicates MSP (0) or PSP (1) */
	__asm(" 	BNE 	RtnViaPSP");

	/* Trapping source is MSP - save r4-r11 on stack (default, so just push) */
	__asm(" 	PUSH 	{r4-r11}");
	__asm(" 	MRS	r0,msp");
	__asm(" 	BL	SVCHandler");	/* r0 is MSP */
	__asm(" 	POP	{r4-r11}");
	__asm(" 	POP 	{PC}");

	/* Trapping source is PSP - save r4-r11 on psp stack (MSP is active stack) */
	__asm("RtnViaPSP:");
	__asm(" 	mrs 	r0,psp");
	__asm("		stmdb 	r0!,{r4-r11}");	/* Store multiple, decrement before */
	__asm("		msr	psp,r0");
	__asm(" 	BL	SVCHandler");	/* r0 Is PSP */

	/* Restore r4..r11 from trapping process stack  */
	__asm(" 	mrs 	r0,psp");
	__asm("		ldmia 	r0!,{r4-r11}");	/* Load multiple, increment after */
	__asm("		msr	psp,r0");
	__asm(" 	POP 	{PC}");
}


/*
 * Supervisor call handler
 * Handle startup of initial process
 * Handle all other SVCs such as getid, terminate, etc.
 * Assumes first call is from startup code
 * Argptr points to (i.e., has the value of) either:
 * - the top of the MSP stack (startup initial process)
 * - the top of the PSP stack (all subsequent calls)
 * Argptr points to the full stack consisting of both hardware and software
 * register pushes (i.e., R0..xPSR and R4..R10); this is defined in type
 * stack_frame
 * Argptr is actually R0 -- setup in SVCall(), above.
 * Since this has been called as a trap (Cortex exception), the code is in
 * Handler mode and uses the MSP
 *
 * @param:		Pointer to the the top of the process stack
 * @returns:	None
 */
void SVCHandler(struct stack_frame *argptr)
{
	static int firstSVCcall = TRUE;
	struct kcallargs *kcaptr;

	if (firstSVCcall)
	{
		setPSP(running -> sp + 8 * sizeof(unsigned int));

		firstSVCcall = FALSE;
		/* Start SysTick */
		systickInit();

		__asm("	movw 	LR,#0xFFFF");  /* Lower 16 [and clear top 16] */
		__asm("	movt 	LR,#0xFFFD");  /* Upper 16 only */
		__asm("	bx 		LR");          /* Force return to PSP */
	}
	else /* Subsequent SVCs */
	{
	#ifdef FOR_KERNEL_ARGS
		kcaptr = (struct kcallargs *) argptr -> r7;
		switch(kcaptr -> code)
		{
		case GETID:
			kcaptr -> rtnvalue = s_get_id();
		break;
		case NICE:		// arg1 should be priority level to switch to
			kcaptr -> rtnvalue = s_nice(kcaptr->arg1);
			break;
		case TERMINATE:	// no arguments requred
			kcaptr -> rtnvalue = s_terminate();
			break;
		case SEND:
			kcaptr -> rtnvalue = s_send();
			break;
		case RECV:
			kcaptr -> rtnvalue = s_recv();
			break;
		case BIND:		// arg1 should be the mailbox to bind to
			kcaptr -> rtnvalue = s_bind(kcaptr->arg1);
			break;
		case UNBIND:	// arg1 should be the mailbox to unbind from
			kcaptr -> rtnvalue = s_unbind(kcaptr->arg1);
			break;
		default:
			kcaptr -> rtnvalue = -1;
		}
	#endif
	}
}

/*
 * Pending Supervisor Call Handler
 *
 * @param:		None
 * @returns:	None
 */
void PendSVHandler(void)
{
	INTERRUPT_MASTER_DISABLE();

	if(running)
		saveRegisters();	// save registers if there is a process running

	nextProcess();			// move to next process
	loadRegisters();		// restore process registers

	INTERRUPT_MASTER_ENABLE();
}
