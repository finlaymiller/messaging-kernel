/*
 * systick.c
 *
 *	Modified by: Finlay Miller
 *	Modified on: Oct 09, 2019
 *
 *  - SysTick sample code
 *	- Originally written for the Stellaris (2013)
 *	- Will need to use debugger to "see" interrupts
 *	- Code uses bit-operations to access SysTick bits
 */

#include "systick.h"

/* globals */
systick_struct systick;
extern volatile int got_data;


void SysTickStart(void)
{
	// Set the clock source to internal and enable the counter to interrupt
	ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

void SysTickStop(void)
{
	// Clear the enable bit to stop the counter
	ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

void SysTickPeriod(unsigned long Period)
{
	// For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
	ST_RELOAD_R = Period - 1;  /* 1 to 0xff.ffff */
}

void SysTickIntEnable(void)
{
	// Set the interrupt bit in STCTRL
	ST_CTRL_R |= ST_CTRL_INTEN;
}

void SysTickIntDisable(void)
{
	// Clear the interrupt bit in STCTRL
	ST_CTRL_R &= ~(ST_CTRL_INTEN);
}	

void SysTick_IntHandler(void)
{
	systick_struct *stptr = &systick;

	stptr->ticks++;			// increment global time counter

	enQ(SYSTICK, TICK);
	got_data = TRUE;
}

void SysTickInit(void)
{  
	SysTickPeriod(CLK_PERIOD/TICK_RATE);	// set clock rate
	SysTickIntEnable();						// enable interrupts
	SysTickReset();							// initialize struct
	SysTickStart();							// start ticking
}

/*
 * Resets all systick struct components to zero
 *
 * @param:		None
 * @returns:	None
 */
void SysTickReset(void)
{
	systick_struct *stptr = &systick;

	stptr->ticks = 0;
	stptr->cmp_val = 0;
	stptr->enabled = 0;
}
