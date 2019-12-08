/*
 * register.c
 *
 *  Created on: Nov 8, 2019
 *      Author: Larry Hughes
 * 
 * 	Wraps assembly commands in C functions so we don't have to touch or think
 * 	about their yucky assembly innards.
 */

#include "register.h"

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

void returnPSP(void)
{
	/*
	 - Change the current LR to indicate return to Thread mode using the PSP
	 - Assembler required to change LR to FFFF.FFFD (Thread/PSP)
	 - BX LR loads PC from PSP stack (also, R0 through xPSR) - "hard pull"
	*/
	__asm(" movw    LR,#0xFFFD"); 	/* Lower 16 [and clear top 16] */
	__asm(" movt    LR,#0xFFFF"); 	/* Upper 16 only */
}

void assignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
    * simply a MOV from R0 to R7
    */
    __asm(" mov r7,r0");
}
