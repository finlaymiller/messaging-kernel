/*
 * VT100.c
 *
 *  Created on: Dec 3, 2019
 *      Author: Larry Hughes, Derek Capone, and Finlay Miller
 * 
 * VT100 is 
 */

#include "VT100.h"

static struct CUP cup = {ESC, '[', '0', '0', ';', '0', '0', 'H', NUL};

/*
 * Clears the text in the terminal, initializes cursor position
 * 
 * Arguments:
 *      None
 * Returns:
 *      None
 */
void initTerminal(void)
{
    repositionCursor(1, 1);

    int i;
    for(i = 0; i < MAX_ROWS * MAX_COLS; i++){
        UART0_TXChar(SPACE);
    }

    repositionCursor(1, 1);
}

/*
 * Repositions cursor to row, column provided
 * 
 * Arguments:
 *      [int] row to move to (0 - 24)
 *      [int] column to move to (0 - 80)
 * Returns:
 *      None
 */
void repositionCursor(unsigned int row, unsigned int col)
{
    enablePendSV(FALSE);

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

    enablePendSV(TRUE);
}
