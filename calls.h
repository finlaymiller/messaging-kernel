/*
 * calls.h
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller
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

int k_get_id(void);
int k_nice(int);
int k_bind(unsigned int);
int k_unbind(unsigned int);
int k_terminate(void);

int k_send(struct message *);
int k_recv(struct message *);

#endif /* CALLS_H_ */
