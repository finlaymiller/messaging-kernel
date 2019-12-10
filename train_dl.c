/*
 * train_dl.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek Capone and Finlay Miller\
 * 
 * This file contains all of the data-link-layer train related functions
 */


#include "train_dl.h"

#define LEN_IND  2  //index for length of message in frame
#define MSG_IND  3  // index for beginning of message in frame

/* Define static struct to keep track of Ns and Nr */
static union Ctrl control = {0};

/*
 * Check validity of packet, send to application layer if packet is valid
 * 
 * Arguments:
 *      [char *] pointer to data frame
 *      [char] length of frame
 *      [char] checksum of frame
 * Returns:
 *      None
 */
void unpackFrame(char *frame, char len, char chksum)
{
    char dl_buffer[len];

    /* Copy memory into local buffer so that UART can continue receiving */
    memcpy(dl_buffer, frame, len);

    /* First ensure chksum is correct */
    if(dl_buffer[len-1] != chksum){
        //if incorrect, return and disregard buffer
        return;
    }

    /* Check control byte */
    union Ctrl n_ctrl;
    n_ctrl.c_control = dl_buffer[1];  //0th byte is STX, 1st byte is control byte
    if(checkCtrl(n_ctrl.s_control) == CTRL_INVALID) return;   //disregard packet if control is invalid

    /* Fill application layer message struct and allow process to take over */
    fillMessage(&dl_buffer[MSG_IND], dl_buffer[LEN_IND]);
}

/*
 * Check Nr of incoming packet to see if value is correct
 * TODO: Check value depending on type of packet
 * 
 * Arguments:
 *      [struct Control] control byte of packet
 * Returns:
 *      [char]  1 if packet is valid
 *              0 if packet is invalid
 */
char checkCtrl(struct Control ctrl)
{
    return ((control.s_control.ns == ctrl.nr) ? CTRL_VALID : CTRL_INVALID);
}

/*
 * Send mag dir update request from DLL to PL
 * 
 * Arguments:
 *      [struct t_message] message to send
 * Returns:
 *      None
 */
void dl_transmitMagDir(struct t_message magdir)
{
    struct Packet packet;

    /* Define type of packet in static structure  */
    control.s_control.type = (enum PktType) DATA;

    /* Set control in packet to current value of static control struct */
    packet.control = (unsigned char)control.c_control;

    /* Get and set message length (return if error in code) */
    char len = getMessageLen(magdir.code);
    if(len == L_ERR) return;  // Dont transmit if code is incorrect
    packet.length = len;

    /* Copy message into packet */
    memcpy(packet.message, (char *)&magdir, packet.length);

    /* Send to phy layer */
    phy_transmitFrame(packet);
}

/*
 * Gets the message length to be set in packet
 * Returns error value if code is incorrect. We know that this is hilariously
 * inefficient as-is. The struct was used so that we could later incorporate 
 * checks for each type of message length but due to lack of time we didn't get
 * to it.
 * 
 * Arguments:
 *      [unsigned char] the code to check the length of
 * Returns:
 *      [char] the length of the message
 */
char getMessageLen(unsigned char code)
{
    char rtn;

    /* Find code and return length */
    switch(code){
    case C_MAGDIR:
        rtn = L_MAGDIR;
        break;
    default:
        rtn = L_ERR;
        break;
    }

    return rtn;
}
