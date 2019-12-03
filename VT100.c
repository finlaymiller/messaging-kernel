/*
 * VT100.c
 *
 *  Created on: Dec 3, 2019
 *      Author: Derek
 */

#include <stdio.h>
#include "VT100.h"
#include "uart.h"

#define NUL 0x00
#define ESC 0x1b

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

static struct CUP cup = {ESC, '[', '0', '0', ';', '0', '0', 'H', NUL};

void repositionCursor(int x, int y)
{
    /* Reposition cursor to line 3, column 75 */
    cup . line [0] = '0';
    cup . line [1] = '3';
    cup . col [0] = '7';
    cup . col [1] = '5';

    //cup . esc = 'E';

    UART0_TXStr((char *)&cup);
}
