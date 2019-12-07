/*
 * train_dl.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */


#include "train_dl.h"
#include "train_phy.h"

/* Define static struct to keep track of Ns and Nr */
static union Ctrl control = {0};

void initControlUnion(void)
{
    control.c_control = 0;
}

void dl_transmitMagDir(struct t_message magdir)
{
    struct Packet packet;

    /* Define type of packet in static structure  */
    control.s_control.type = (enum PktType) ACK;

    /* Set control in packet to current value of static control struct */
    packet.control = (unsigned char)control.c_control;

    /* TODO: Create length lookup function for each type of packet */
    // packet.length = getPacketLen(control.type);
    packet.length = 3;

    /* Copy message into packet */
    memcpy(packet.message, (char *)&magdir, packet.length);

    /* Send to phy layer */
    phy_transmitMagDir(packet);

}
