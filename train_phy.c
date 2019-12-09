/*
 * train_phy.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */

#include "train_phy.h"
#include "train_dl.h"
#include "uart.h"
#include "process.h"

#define SLOW_TEXT 10000
#define PADDING 5   //Number bytes on top of packet length in the frame (excluding DLE's)
#define FRAME_OFFSET 3  //offset of frame start to where packet message begins
#define BUFFER_SIZE 10  //buffer size for incoming characters

/* Static variables for phy layer */
static char rx_buffer[BUFFER_SIZE];
static char len = 0;
static char chksum = 0;

/*
 * Handles WAIT_STX state
 * Returns new state to go into
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
 */
char handleWaitInbyte1(char inbyte)
{
    if(inbyte == ETX){
        /* Transmission complete, send to datalink layer */
        unpackFrame(rx_buffer, len+1, chksum);  //Note: len+1 because length is incremented before adding bytes
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

