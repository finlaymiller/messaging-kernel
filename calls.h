/*
 * calls.h
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller and Derek Capone
 * 
 * Headerfile for kernel calls
 */

#ifndef CALLS_H_
#define CALLS_H_

#include "kernel.h"
#include "systick.h"
#include "mail.h"
#include "trap.h"
#include "process.h"
#include "register.h"

struct message;	// necessary to shut the compiler up

/* function declarations */
int k_get_id(void);
int k_bind(unsigned int);
int k_unbind(unsigned int);
int k_send(struct message *);
int k_recv(struct message *);
int k_terminate(void);
int k_nice(int);

#endif /* CALLS_H_ */
