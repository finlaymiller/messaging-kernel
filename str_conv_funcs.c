/*
 * str_conv_funcs.c
 *
 *  Created on: Nov 7, 2019
 *      Author: Finlay Miller and Derek Capone
 * 
 * Custom versions of the somewhat standard C functions, itoa and atoi.
 */

#include "str_conv_funcs.h"

const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/*
 * Converts provided ASCII string to integers and checks for non-integer chars
 *
 * Arguments:
 *  	[char*] The string to be converted
 * 	     [int] The integer result of the conversion
 * Returns:
 * 		[int] 1 if conversion is successful, 0 if a char was detected
 */
int my_atoi(char *str, int *num)
{
	unsigned int total = 0;

	while(*str)
	{
		if(*str >= '0' && *str <= '9')
			total = total * 10 + (*str++ - '0');

		else	// non-integer character detected
			return 0;
	}

	*num = total;
	return 1;
}


/*
 * Converts integer to string
 *
 * Arguments:
 * 	    [int] Number to convert
 * 		[char*] Buffer to write string to
 * 		[int]	Base of number (most commonly 10)
 * Returns:
 * 		[int] Pointer to head of string
 */
char *my_itoa(int number, char *buff, int base)
{
    char *result = (buff == NULL || base > strlen(digits) || base < 2)
                   ? NULL : buff;
    char sign = 0;

    if (number < 0) sign = '-';

    if (result != NULL)
    {
        do
        {
            *buff++ = digits[abs(number % (base ))];
            number /= base;
        } while (number);
        if(sign) *buff++ = sign;
        if (!*result) *buff++ = '0';
        *buff = 0;
        reverse(result);
    }

    return result;
}


/*
 * Reverse string in place
 *
 * Arguments:
 *  	[char*]	String to reverse
 * Returns:	
 *      None
 */
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
