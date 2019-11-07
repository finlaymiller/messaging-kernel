/*
 * calls.h
 *
 *  Created on: Nov 3, 2019
 *      Author: Finlay Miller
 */

#ifndef CALLS_H_
#define CALLS_H_

#include "trap.h"

int s_get_id(void);
int s_nice(int);
int s_terminate(void);
int s_send(void);
int s_recv(void);
int s_bind(unsigned int);
int s_unbind(unsigned int);

int getid(void);
void p_terminate(void);

#endif /* CALLS_H_ */
