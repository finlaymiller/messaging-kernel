/*
 * Sample kernel call using SVC
 * Note use of stack to pass arguments to kernel
 * ECED 4402
 */
#include <stdio.h>
#include "KernelCalls.h"
#include "process.h"

int getid()
{
    volatile struct kcallargs getidarg; /* Volatile to actually reserve space on stack */
    getidarg . code = GETID;

    /* Assign address if getidarg to R7 */
    assignR7((unsigned long) &getidarg);

    SVC();

    return getidarg . rtnvalue;

}

void assignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
    * simply a MOV from R0 to R7
    */
    __asm(" mov r7,r0");
}
