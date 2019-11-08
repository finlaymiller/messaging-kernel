/*
 * calls.h
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller
 */

#ifndef CALLS_H_
#define CALLS_H_

#include "kernel.h"
#include "mail.h"
#include "trap.h"

int k_get_id(void);
int k_nice(int);
int k_bind(unsigned int);
int k_unbind(unsigned int);
int k_send(void);
int k_recv(void);

int getid(void);

#endif /* CALLS_H_ */
