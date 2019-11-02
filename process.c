/*
 * process.c
 *
 *  Created on: Oct 17, 2019
 *      Author: Larry Hughes
 *      Editor: Finlay Miller
 *      		Derek Capone
 */

#include "process.h"


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
