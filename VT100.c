/*
 * VT100.c
 *
 *  Created on: Dec 3, 2019
 *      Author: Derek/Larry Hughes
 */

#include <stdio.h>
#include "VT100.h"
#include "uart.h"
#include "str_conv_funcs.h"

#define NUL 0x00
#define ESC 0x1b
#define SPACE ' '

#define MAX_ROWS 24
#define MAX_COLS 80

#define MAX

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

/*
 * Clears the text in the terminal, initializes cursor position
 */
void initTerminal(void)
{
    repositionCursor(1, 1);

    int i;
    for(i=0; i<MAX_ROWS*MAX_COLS; i++){
        UART0_TXChar(SPACE);
    }

    repositionCursor(1, 1);
}

/*
 * Repositions cursor to row, column
 */
void repositionCursor(unsigned int row, unsigned int col)
{
    char buf[2];
    char *tmp;

    /* Catch excess rows/columns */
    if(row > 24 || col >80) return;

    /* Get and set row value */
    if(row > 9) {
        tmp = my_itoa(row, buf, 10);
    } else {
        buf[0] = '0';
        tmp = my_itoa(row, &buf[1], 10);
    }

    cup . line [0] = buf[0];
    cup . line [1] = buf[1];

    /* Get and set column value */
    if(col > 9){
        tmp = my_itoa(col, buf, 10);
    } else {
        buf[0] = '0';
        tmp = my_itoa(col, &buf[1], 10);
    }

    cup . col [0] = buf[0];
    cup . col [1] = buf[1];

    UART0_TXStr((char *)&cup);
}
