/*
 * str_conv_funcs.h
 *
 *  Created on: Nov 7, 2019
 *      Author: Finlay Miller and Derek Capone
 * 
 * Headerfile for string conversion functions
 */

#ifndef STR_CONV_FUNCS_H_
#define STR_CONV_FUNCS_H_

#include <string.h>

#define MAX_STR_LEN 512

/* function declarations */
int my_atoi(char *, int *);
char *my_itoa(int, char *, int);
void reverse(char *);

#endif /* STR_CONV_FUNCS_H_ */
