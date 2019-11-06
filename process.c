/*
 * process.c
 *
 *  Created on: Oct 17, 2019
 *      Author: Larry Hughes
 *      Editor: Finlay Miller
 *      		Derek Capone
 */

#include "process.h"


/*
 * Process-kernel call function.  Supplies code and kernel message to the
 * kernel is a kernel request.  Returns the result (return code) to the caller.
 *
 * @param:	code	- kernel request code as defined in trap.h
 * @param:	arg1	-
 */
int pkcall(int code, unsigned int arg1, unsigned int* arg2)
{
	volatile struct kcallargs arglist;

	/* Pass code and pkmsg to kernel in arglist structure */
	arglist . code = code;
	arglist . arg1 = arg1;
	arglist . arg2 = arg2;

	/* R7 = address of arglist structure */
	assignR7((unsigned long) &arglist);

	/* Call kernel */
	SVC();

	/* Return result of request to caller */
	return arglist . rtnvalue;
}

unsigned long getPSP(void)
{
	/* returns contents of PSP (current process stack) */
	__asm("	mrs		r0, psp");
	__asm("	bx		lr");
	return 0;
}

unsigned long getMSP(void)
{
	/* returns contents of MSP (main stack) */
	__asm("	mrs		r0, msp");
	__asm("	bx		lr");
	return 0;
}

unsigned long getSP(void)
{
	/* leading space required for label */
	__asm("	mov		r0, sp");
	__asm("	bx		lr");
	return 0;
}

void setPSP(volatile unsigned long process_stack)
{
	/* set PSP to process_stack */
	__asm("	msr		psp, r0");
}

void setMSP(volatile unsigned long main_stack)
{
	/* set PSP to process_stack */
	__asm("	msr		msp, r0");
}

void volatile saveRegisters(void)
{
	/* save r4-r11 on process stack */
	__asm("	mrs		r0, psp");
	__asm("	stmdb	r0!, {r4-r11}");
	__asm("	msr		psp, r0");
}

void volatile loadRegisters(void)
{
	/* load r4-r11 from stack to CPU */
	__asm("	mrs		r0, psp");
	__asm("	ldmia	r0!, {r4-r11}");
	__asm("	msr		psp, r0");
}

void volatile loadLR(void)
{
    __asm(" movw     lr, #0xfffd");
    __asm(" movt     lr, #0xffff");
}


/*
 * Executes assembly instruction to enable interrupts
 */
void InterruptMasterEnable(void)
{
    /* enable CPU interrupts */
    __asm(" cpsie   i");
}

void InterruptMasterDisable(void)
{
    __asm(" cpsid i");
}
