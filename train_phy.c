/*
 * train_phy.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */

#include "train_phy.h"
#include "uart.h"
#include "process.h"

#define FRAME_LEN 9 // TODO: update this to non-magic number
#define SLOW_TEXT 10000
#define PADDING 5   //Number bytes on top of packet length in the frame (excluding DLE's)
#define FRAME_OFFSET 3  //offset of frame start to where packet message begins

/*
 * Transmits magnitude direction frame
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
