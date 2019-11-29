/*
 * register.h
 *
 *  Created on: Nov 8, 2019
 *      Author: Finlay Miller
 */

#ifndef REGISTER_H_
#define REGISTER_H_


// CPU interrupt enable/disable macros
#define InterruptMasterEnable()		__asm(" cpsie   i")
#define InterruptMasterDisable()	__asm(" cpsid 	i")

/* function definitions */
void setLR(volatile unsigned long);
unsigned long getPSP();
unsigned long getMSP();
unsigned long getSP();
void setPSP(volatile unsigned long);
void setMSP(volatile unsigned long);
void volatile saveRegisters();
void volatile loadRegisters();
void returnPSP(void);
void assignR7(volatile unsigned long);

#endif /* REGISTER_H_ */
