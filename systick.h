/*
 * systick.h
 *
 *  Created on: Oct 2, 2019
 *      Author: finlay
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

/* required custom file links */
#include "uart.h"

/* systick struct definition */
typedef struct systick_struct
{
	unsigned int ticks;		// times ticked since last reset
	unsigned int cmp_val;	// number of ticks to run for
	unsigned int enabled;	// T/F whether or not the alarm is running
} systick_struct;

// SysTick Registers
// SysTick Control and Status Register (STCTRL)
#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// SysTick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

// SysTick defines
#define ST_CTRL_COUNT   	0x00010000  // Count Flag for STCTRL
#define ST_CTRL_CLK_SRC		0x00000004  // Clock Source for STCTRL
#define ST_CTRL_INTEN   	0x00000002  // Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE  	0x00000001  // Enable for STCTRL
#define MAX_WAIT        	0x01000000	// 2^24
#define CLK_PERIOD			0x00F42400	// 16 000 000
#define TICK_RATE			0x0000000A	// Ten times per second
#define TICK				'*'

// Function Declarations
void SysTickStart(void);
void SysTickStop(void);
void SysTickPeriod(unsigned long);
void SysTickIntEnable(void);
void SysTickIntDisable(void);
void SysTick_IntHandler(void);
void SysTickInit(void);
void SysTickReset(void);

#endif /* SYSTICK_H_ */
