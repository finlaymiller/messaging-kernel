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

void phy_transmitMagDir(struct Packet packet)
{
    char frame[FRAME_LEN];

    char checksum = packet.control + packet.length + packet.message[0] + packet.message[1] + packet.message[2];
    checksum = ~checksum;

    frame[0] = STX;
    frame[1] = packet.control;
    frame[2] = DLE;
    frame[3] = packet.length;
    frame[4] = packet.message[0];
    frame[5] = packet.message[1];
    frame[6] = packet.message[2];
    frame[7] = checksum;
    frame[8] = ETX;

    while(1){
        UART1_TXStr(frame);
        waitTime(10 * SLOW_TEXT);
    }

}
