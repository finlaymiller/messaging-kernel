/*
 * systick.h
 *
 * Declares public functions for SysTick module
 *
 *  Created on: Oct 2, 2019
 *  Author: Derek Capone
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "trap.h"

// SysTick Registers
// SysTick Control and Status Register (STCTRL)
#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// Systick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

// SysTick defines
#define ST_CTRL_COUNT   0x00010000	// Count Flag for STCTRL
#define ST_CTRL_CLK_SRC 0x00000004	// Clock Source for STCTRL
#define ST_CTRL_INTEN   0x00000002	// Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE  0x00000001	// Enable for STCTRL
#define MAX_WAIT        0x01000000	// 2^24
#define PERIOD          0x00027100	// 160 000
#define SYS_CHAR 		'x' 		// Character to fill in systick queue

typedef struct systick_
{
	unsigned int ticks;		// times ticked since last reset
	unsigned int cmp_val;	// number of ticks to run for
	unsigned int enabled;	// T/F whether or not the alarm is running
} SysTick;

/* function declarations */
void initSysTick(void);
void startSysTick(void);
void stopSysTick(void);
void periodSysTick(unsigned long);
void intEnableSysTick(void);
void intDisableSysTick(void);
void SysTickHandler(void);
void enablePendSV(char en);

#endif /* SYSTICK_H_ */
