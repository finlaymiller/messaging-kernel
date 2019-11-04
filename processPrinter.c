/*
 * processPrinter.c
 *
 *  Created on: Nov 2, 2019
 *      Author: Finlay Miller
 */

#include "processPrinter.h"

void p_printchar(void)
{
	UART0_TXChar('b');
}
