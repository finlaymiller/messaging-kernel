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

#define CLEAR_SCREEN    "\x1b[2J"
#define CURSOR_SAVE     "\x1b""7"
#define CURSOR_HOME     "\x1b[H"
#define CLEAR_LINE      "\x1b[2K"
#define HOME_COLOURS    "\x1b[0;30;47m"
#define CURSOR_MIDDLE   "\x1b[20C"
#define TERM_COLOURS    "\x1b[0;0;0m"
#define CURSOR_RESTORE  "\x1b""8"

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

/* structure to be used for repositioning cursor */
struct coord
{
    unsigned int x;
    unsigned int y;
}

/* function declarations */
void repositionCursor(unsigned int, unsigned int);
void initTerminal(void);
void drawTrainset(void);
void updateTrainPos(unsigned int);

#endif /* VT100_H_ */
