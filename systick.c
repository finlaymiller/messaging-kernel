/*
 * systick.c
 *
 * Defines all SysTick module functions
 * Defines constants for SysTick registers and bits
 *
 *  Created on: Oct 2, 2019
 *  Author: Derek Capone
 */

#include "queue.h"
#include "systick.h"
#include "time.h"
#include "process.h"
#include "kernel.h"

//PendSV defines
#define INT_CTRL   (*((volatile unsigned long *)0xE000ED04))
#define INT_CTRL_PENDSV     (1<<28)     //bit for generating PendSV interrupt
#define INT_CTRL_UNPENDSV   (1<<27)     //bit to remove pending state for PendSV

SysTick* systick;

/*
 * Initializes SysTick
 */
void systickInit(void)
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

    INT_CTRL |= INT_CTRL_PENDSV;
}


void PendSV_Handler(void)
{
    InterruptMasterDisable();

    if(getRunning()) saveRegisters();
    setRunningSP((unsigned long*)getPSP());

    nextProcess();
    loadRegisters();

    InterruptMasterEnable();

    __asm(" movw    LR,#0xFFFD");  /* Lower 16 [and clear top 16] */
    __asm(" movt    LR,#0xFFFF");  /* Upper 16 only */
    __asm(" bx  LR");          /* Force return to PSP */
}

