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

#include <stdio.h>

#define NUL	0x00
#define ESC	0x1b

/* Cursor position string */

/* Define the cursor position data structure */

struct CUP
{
	char esc;
	char sqrbrkt;
	char line[2];	/* 01 through 24 */
	char semicolon;
	char col[2];	/* 01 through 80 */
	char cmdchar;
	char nul;
};

/* Define and initialize an escape sequence for cursor position */

struct CUP cup = {ESC, '[', '0', '0', ';', '0', '0', 'H', NUL};

/*
main()
{

 - To position the cursor at line 3 and column 75 
 - Note that this should be done dynamically for any line/column pair


cup . line [0] = '0';
cup . line [1] = '3';
cup . col [0] = '7';
cup . col [1] = '5';


 - To test on screen - change ESC to 'E' 
 - If removed when using a VT-100 emulator, cursor will be reposition

cup . esc = 'E';

printf("%s\n", &cup);

}
*/
