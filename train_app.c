/*
 * train_app.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek Capone and Finlay Miller
 * 
 * This file contains all of the application-layer train related functions
 */


#include "train_app.h"
#include "train_dl.h"

/* Declare static variables */
static struct t_message message;
static char message_rdy = 0;

/*
 * Fills message struct from packet
 * 
 * Arguments:
 *      [char *] The received message
 *      [char] The length of the message
 * Returns:
 *      None
 */
void fillMessage(char *msg, char len)
{
    message.code = msg[0];
    message.arg1 = msg[1];

    /* Fill arg2 only if length of msg is 3 */
    if(len == 3) message.arg2 = msg[2];

    /* Indicate that message is now ready to be processed */
    message_rdy = 1;
}

/*
 * Process function to handle application incoming messages
 * 
 * Arguments:
 *      None
 * Returns:
 *  None
 */
void handleMessages(void)
{
    while(1){

        /* Wait for datalink layer to fill message struct */
        while(!message_rdy);

        // handleMessage();
        /* Reset message ready flag */
        message_rdy = 0;
    }
}

/*
 * Process to send magnitude direction messages
 * 
 * Arguments:
 *      None
 * Returns:
 *      None
 */
void magDirProcess(void)
{
    transmitMagDir(LOC_ALL, 0xA, CW);
}

/*
 * Transmit requested magnitude and direction update to DLL
 * 
 * Arguments:
 *      [char] number of the locomotive to update
 *      [char] magnitude to travel at
 *      [char] direction to travel in
 * Returns:
 *      None
 */
void transmitMagDir(char loc_num, char mag, char dir)
{
    struct mag_dir magdir;
    magdir.ignored = 0x0;
    magdir.direction = dir;
    magdir.magnitude = mag;

    /* Build mag_dir message */
    struct t_message new_mess = buildMagDirMessage(loc_num, magdir);

    /* Send to datalink layer */
    dl_transmitMagDir(new_mess);
}

/*
 * Pack magnitude and diucrection update request into a message struct
 * 
 * Arguments:
 *      [unsigned char] number of train to update
 *      [struct mag_dir] requested magnitude and direction
 * Returns:
 *      [struct t_message] Request in trainset message form
 */
struct t_message buildMagDirMessage(unsigned char loc_num, struct mag_dir mag_arg)
{
    struct t_message message;

    /* Set locomotive number and code */
    message.code = C_MAGDIR;
    message.arg1 = loc_num;

    /* Set magnitude/direction argument (Casting to pointer to shut compiler up) */
    unsigned char *tmp_mag = (unsigned char *)&mag_arg;
    message.arg2 = (unsigned char) *tmp_mag;

    return message;
}

