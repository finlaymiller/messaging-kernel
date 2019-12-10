/*
 * train_phy.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek Capone and Finlay Miller
 * 
 * This file contains all of the physical layer train related functions.
 * 
 * Refer to the physical layer receive state diagram in the assignment
 * description and our design document for a graphical representation of what
 * WAIT_STX, WAIT_INBYTE1, and WAIT_INBYTE2 are and how they work together.
 */

#include "train_phy.h"

/* Static variables for phy layer */
static char rx_buffer[BUFFER_SIZE];
static char len = 0;
static char chksum = 0;

/*
 * Handles WAIT_STX state
 * Returns new state to go into
 * 
 * Arguments:
 *      [char] Input byte
 * Returns:
 *      [char]  0 if to go to wait_inbyte1 state
 *              1 otherwise
 */
char handleWaitSTX(char inbyte)
{
    if(inbyte == STX){
        len = 0;
        chksum = 0;
        rx_buffer[len] = inbyte;
        return WAIT_INBYTE1;
    } else {
        return WAIT_STX;
    }
}

/*
 * Handles WAIT_INBYTE1 state
 * Returns new state to go into
 * 
 * Arguments:
 *      [char] Input byte
 * Returns:
 *      [char]  1 if to go to wait_inbyte1 state
 *              2 if to go to wait_inbyte2 state
 */
char handleWaitInbyte1(char inbyte)
{
    if(inbyte == ETX){
        /* Transmission complete, send to datalink layer */
        // Note: len+1 because length is incremented before adding bytes
        unpackFrame(rx_buffer, len+1, chksum);  
    } else if(inbyte == DLE){
        /* Discard character and move to WAIT_INBYTE2 */
        return WAIT_INBYTE2;
    } else {
        /* Regular character, add to buffer */
        chksum += inbyte;
        len++;
        rx_buffer[len] = inbyte;
        return WAIT_INBYTE1;
    }
}

/*
 * Handles WAIT_INBYTE2 state
 * Returns new state to go into
 * 
 * Arguments:
 *      [char] Input byte
 * Returns:
 *      [char]  1 to go to wait_inbyte1 state
 */
char handleWaitInbyte2(char inbyte)
{
    chksum += inbyte;
    len++;
    rx_buffer[len] = inbyte;
    return WAIT_INBYTE1;
}

/*
 * Transmits magnitude direction frame
 * 
 * Arguments:
 *      [struct packet] packet to transmit
 * Returns:
 *      None
 */
void phy_transmitFrame(struct Packet packet)
{
    char frame_len = packet.length + PADDING;

    char frame[frame_len];

    /* Add constant values to frame */
    frame[0] = STX;
    frame[1] = packet.control;
    frame[2] = packet.length;
    frame[frame_len-2] = calculateChecksum(packet);
    frame[frame_len-1] = ETX;

    /* Fill packet message into frame */
    char i;
    for(i=0; i<packet.length; i++){
        frame[i + FRAME_OFFSET] = packet.message[i];
    }

    while(1){
        UART1_TXStr(frame, frame_len);
        waitTime(10 * SLOW_TEXT);
    }
}

/*
 * Calculates checksum of the packet
 * 
 * Arguments:
 *      [struct Packet] packet to calculate checksum of
 * Returns:
 *      [char] checksum of packet
 */
char calculateChecksum(struct Packet packet)
{
    char checksum, i;

    /* Add control and length to checksum */
    checksum = packet.control + packet.length;

    /* Add packet message to checksum */
    for(i=0; i<packet.length; i++){
        checksum += packet.message[i];
    }

    /* Return one's complement of sum */
    return ~checksum;
}
