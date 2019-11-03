/*
 * systick.c
 *
 * Defines all SysTick module functions
 * Defines constants for SysTick registers and bits
 *
 *  Created on: Oct 2, 2019
 *  Author: Derek Capone
 */


#include "systick.h"
#include "time.h"
#include "queuing.h"

// SysTick Registers
// SysTick Control and Status Register (STCTRL)
#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// Systick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

// SysTick defines
#define ST_CTRL_COUNT      0x00010000  // Count Flag for STCTRL
#define ST_CTRL_CLK_SRC    0x00000004  // Clock Source for STCTRL
#define ST_CTRL_INTEN      0x00000002  // Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE     0x00000001  // Enable for STCTRL

// Maximum period
#define MAX_WAIT           0x1000000   /* 2^24 */
#define PERIOD             0x186A00
#define SYS_CHAR 'x' /* Character to fill in systick queue */

/* Global to signal SysTick interrupt */
volatile int elapsed;
// global variable to count number of interrupts on PORTF0 (falling edge)
volatile int count = 0;

/*
 * Initializes SysTick
 */
void SysTickInit(void)
{
    SysTickPeriod(PERIOD);
    SysTickIntEnable();
    SysTickStart();
}

/*
 * Starts SysTick timer
 */
void SysTickStart(void)
{
    // Set the clock source to internal and enable the counter to interrupt
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

/*
 * Stops SysTick timer
 */
void SysTickStop(void)
{
    // Clear the enable bit to stop the counter
    ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

/*
 * Defines and sets the SysTick period
 */
void SysTickPeriod(unsigned long Period)
{
    /*
     For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
    */
    ST_RELOAD_R = Period - 1;  /* 1 to 0xff.ffff */
}

/*
 * Enables SysTick interrupts
 */
void SysTickIntEnable(void)
{
    // Set the interrupt bit in STCTRL
    ST_CTRL_R |= ST_CTRL_INTEN;
}

/*
 * Disables SysTick interrupts
 */
void SysTickIntDisable(void)
{
    // Clear the interrupt bit in STCTRL
    ST_CTRL_R &= ~(ST_CTRL_INTEN);
}

/*
 * Interrupt handler for SysTick
 * Handles the SysTick interrupts by enqueuing a SysTick char onto SysTick queue
 */
void SysTickHandler(void)
{
    /* Enqueue characater onto systick queue */
    //enqueue(SYSTICK, SYS_CHAR);
}

