/*
 - Example of VT-100 command
 - All commands are ASCII strings that the VT-100 or VT-100 emulator
   interpret and then act on
 - A command is an "escape sequence" starting with ESC and followed by a
   string of characters
 - Note that the NUL character terminates the string for C; it is not 
   transmitted
 - VT-100 screen size: 24 lines by 80 columns (1..24 and 1..80)
 - Example shown here is cursor position (CUP)
 - ECED 4402
 - 25 Feb 2013
*/

#include "VT100.h"

extern char *BIND_ERR_PRINTS[3];
struct CUP cup		= {ESC, '[', '0', '0', ';', '0', '0', 'H', NUL};
char CLEAR_SCREEN[] = {"\x1b[2J"};
char CURSOR_SAVE[] 	= {"\x1b""7"};
char CURSOR_HOME[] 	= {"\x1b[H"};
char CLEAR_LINE[]  	= {"\x1b[2K"};
char CURSOR_LOAD[] 	= {"\x1b""8"};


/*
 * Draws rectangle from top left to bottom right
 *
 */
unsigned int v_drawWindow(unsigned int pid, struct coord upper_left, struct coord bottom_right)
{
	char buf[32];
	int i = 0, width, height, temp1, temp2, printed_pid = 0;
	struct CUP start = cup, middle = cup;

	// store coords in CUP
	memcpy(start.line, my_itoa(upper_left.y, buf, 10), 2 * sizeof(char));
	memcpy(start.col,  my_itoa(upper_left.x, buf, 10), 2 * sizeof(char));

	// calculate width and height, no need to catch errors
	width 	= bottom_right.x - upper_left.x;
	height	= bottom_right.y - upper_left.y;

	if(width < 8 || height < 3)
		return 1;

	UART0_TXStr((char *)&start);

	// make top bar
	while(i++ < (width - 6) / 2)
	{					// ^ 6 spaces required for process number header
		UART0_TXStr("-");
		// print process number header
		if ((i >= ((width - 6) / 2)) && !printed_pid)
		{
			UART0_TXStr(" P");
			if(pid < 100)
				if(pid < 10)
					UART0_TXChar('0');
				UART0_TXChar('0');
			UART0_TXStr(my_itoa(pid, buf, 10));
			UART0_TXStr(" ");
			printed_pid++;
			i = 0;
		}	// continue printing top bar
	}

	// print walls
	for(i = 1; i < (height - 1); i++)
	{					 //  ^ 2 to account for top and bottom bars of window
		// move to next line
		memcpy(start.line, my_itoa(upper_left.y + i, buf, 10), 2 * sizeof(char));
		UART0_TXStr((char *)&start);

		UART0_TXStr("|");
		for(i = 0; i < (width - 2); i++)
			UART0_TXStr("\x20");	// for some reason using the ASCII
		UART0_TXStr("|");
	}

	// move to next line
	memcpy(start.line, my_itoa(upper_left.y + i, buf, 10), 2 * sizeof(char));
	UART0_TXStr((char *)&start);

	// print bottom bar
	for(i = 0; i < width; i++)
		UART0_TXStr("-");

	// move cursor to middle of rectangle
	memcpy(middle.line, my_itoa((width/2), buf, 10), 2 * sizeof(char));
	memcpy(middle.col, my_itoa((height/2), buf, 10), 2 * sizeof(char));
	UART0_TXStr((char *)&middle);
	UART0_TXStr((char *)&CURSOR_SAVE);

	return 0;
}

void v_clearScreen(void)
{
	UART0_TXStr("\x1b[2J");
}
