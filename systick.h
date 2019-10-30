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

void SysTickInit(void);
void SysTickStart(void);
void SysTickStop(void);
void SysTickIntEnable(void);
void SysTickIntDisable(void);
void SysTickHandler(void);
void SysTickPeriod(unsigned long Period);

#endif /* SYSTICK_H_ */
