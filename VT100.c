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

