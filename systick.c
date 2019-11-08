/*
 * systick.c
 *
 * Defines all SysTick module functions
 * Defines constants for SysTick registers and bits
 *
 *  Created on: Oct 2, 2019
 *  Author: Derek Capone
 *
 */

#include "queue.h"
#include "systick.h"
#include "time.h"
#include "process.h"
#include "kernel.h"

SysTick* systick;

/*
 * Initializes SysTick
 *
 * @param:		None
 * @returns:	None
 */
void initSysTick(void)
{
    periodSysTick(PERIOD);
    intEnableSysTick();
    startSysTick();
}

/*
 * Starts SysTick timer
 *
 * @param:		None
 * @returns:	None
 */
void startSysTick(void)
{
    // Set the clock source to internal and enable the counter to interrupt
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

/*
 * Stops SysTick timer
 *
 * @param:		None
 * @returns:	None
 */
void stopSysTick(void)
{
    // Clear the enable bit to stop the counter
    ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

/*
 * Defines and sets the SysTick period
 *
 * @param:	Period:	Number of clock pulses per tick
 * @returns:		None
 */
void periodSysTick(unsigned long Period)
{
    // For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
    ST_RELOAD_R = Period - 1;  /* 1 to 0xff.ffff */
}

/*
 * Enables SysTick interrupts
 *
 * @param:		None
 * @returns:	None
 */
void intEnableSysTick(void)
{
    // Set the interrupt bit in STCTRL
    ST_CTRL_R |= ST_CTRL_INTEN;
}

/*
 * Disables SysTick interrupts
 *
 * @param:		None
 * @returns:	None
 */
void SysTickIntDisable(void)
{
    // Clear the interrupt bit in STCTRL
    ST_CTRL_R &= ~(ST_CTRL_INTEN);
}

/*
 * Interrupt handler for SysTick
 * Handles the SysTick interrupts by enqueuing a SysTick char onto SysTick queue
 *
 * @param:		None
 * @returns:	None
 */
void SysTickHandler(void)
{
    /* Enqueue characater onto systick queue */
    //enqueue(SYSTICK, SYS_CHAR);

	NVIC_INT_CTRL_R |= INT_CTRL_PENDSV;
}
