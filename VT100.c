/*
 * VT100.c
 *
 *  Created on: Dec 3, 2019
 *      Author: Larry Hughes, Derek Capone, and Finlay Miller
 * 
 * VT100 is 
 */

#include "VT100.h"

// generic structure for cursor position update
static struct CUP cup = {ESC, '[', '0', '0', ';', '0', '0', 'H', NUL};

/* 
 * This is a table of the positions of the hall sensor indicators ('x') 
 * in the terminal. It accounts for the whitespace offset (+8, +4), which
 * is stored in element 0 so that the indices correspond exactly to the hall
 * sensors and that other functions can have an easy offset for accessing the
 * trainset drawing.
 * 
 * e.g. The coordinates of the x icon representing the 12th hall sensor on the
 *      terminal screen is (10, 20)
 */
struct coord hall_pos[26] = 
{
    {08, 04}, {19, 08}, {19, 14}, {19, 20}, {19, 26}, 
    {19, 32}, {19, 38}, {16, 41}, {13, 41}, {10, 38}, 
    {10, 32}, {10, 26}, {10, 20}, {10, 14}, {10, 08}, 
    {13, 05}, {16, 05}, {21, 10}, {21, 36}, {08, 36}, 
    {08, 10}, {17, 25}, {17, 34}, {12, 22}, {12, 13} 
};

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
	UART0_TXStr(CLEAR_SCREEN);
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

/*
 * Draws the ASCII version of the trainset.
 *      x indicates a Hall sensor
 *      o indicates the last Hall sensor which detected the train
 *          (not used until updateTrainPos() below)
 *      - / \ | indicate track (\x5c is the ASCII hex code for '\')
 * All labels are the same as on the graphic provided to us
 * 
 * Arguments:
 *      None
 * Returns:
 *      None
 */
void drawTrainset(void)
{
	int i;
	char *pic[] = {
	"     x------------------------x\n",
	" S1 / 20     S2              19 \x5c S3\n",
	"14 x-----x-----x-----x-----x-----x\n",
	"  /      13   12  /  11   10    9 \x5c\n",
	" /      x--------x                 \x5c\n",
	"x 15    24      23                8 x\n",
	"|                                   |\n",
	"|                                   |\n",
	"x 16                   21      22 7 x\n",
	" \x5c                  x--------x    /\n",
	"  \x5c   2     3   /   4     5   6 /\n",
	" 1 x-----x-----x-----x-----x-----x\n",
	" S6 \x5c 17       S5         18 / S4\n",
	"     x------------------------x"
	};

    repositionCursor(1, 1);     // draw upper bar
    for(i = 0; i < MAX_COLS; i++)
    	UART0_TXChar('=');

    UART0_TXStr("\n");          // draw trainset
    for(i = 0; i < 14; i++)
    {
    	repositionCursor(i + 8, 4);	// 8 and 4 are for whitespace
    	UART0_TXStr(pic[i]);
    }

    repositionCursor(MAX_ROWS, 1);  // draw lower bar
    for(i = 0; i < MAX_COLS; i++)
        	UART0_TXChar('=');
}

/*
 * Update trainset graphic by changing the icon of the hall sensor which last
 * detected the train from a 'x' to a 'o'.
 * 
 * Arguments:
 *      [unsigned int] The number of the hall sensor
 * Returns:
 *      None
 */
void updateTrainPos(unsigned int hall)
{
    repositionCursor(hall_pos[hall].x, hall_pos[hall].y);
    UART0_TXStr('o');
}
