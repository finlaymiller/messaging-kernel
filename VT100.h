/*
 * VT100.h
 *
 *  Created on: Dec 3, 2019
 *      Author: Derek Capone and Finlay Miller
 * 
 * Headerfile for VT100 related functions and data
 */

#ifndef VT100_H_
#define VT100_H_

#include <stdio.h>
#include "uart.h"
#include "str_conv_funcs.h"

/* global definitions */
#define NUL 0x00
#define ESC 0x1b
#define SPACE ' '
#define MAX_ROWS 24
#define MAX_COLS 80

/* structure to be used for moving the cursor around */
struct CUP
{
    char esc;
    char sqrbrkt;
    char line[2];   /* 01 through 24 */
    char semicolon;
    char col[2];    /* 01 through 80 */
    char cmdchar;
    char nul;
};

/* function declarations */
void repositionCursor(unsigned int x, unsigned int y);
void initTerminal(void);

#endif /* VT100_H_ */
