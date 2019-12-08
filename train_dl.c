/*
 * train_dl.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */


#include "train_dl.h"
#include "train_phy.h"
#include "train_app.h"

/* Define static struct to keep track of Ns and Nr */
static union Ctrl control = {0};

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
 * Returns error value if code is incorrect
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
