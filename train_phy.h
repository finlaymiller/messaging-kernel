/*
 * train_phy.h
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek Capone and Finlay Miller
 * 
 * Headerfile for all physical level trainset functions
 */

#ifndef TRAIN_PHY_H_
#define TRAIN_PHY_H_

#include "train_dl.h"
#include "uart.h"
#include "process.h"

#define SLOW_TEXT 10000
#define PADDING 5   //Number bytes on top of packet length in the frame (excluding DLE's)
#define FRAME_OFFSET 3  //offset of frame start to where packet message begins
#define BUFFER_SIZE 10  //buffer size for incoming characters

#define STX 0x02
#define ETX 0x03
#define DLE 0x10

/* State definitions */
#define WAIT_STX        0
#define WAIT_INBYTE1    1
#define WAIT_INBYTE2    2

/* Public Function Declarations */
void phy_transmitFrame(struct Packet packet);
char calculateChecksum(struct Packet packet);
char handleWaitSTX(char inbyte);
char handleWaitInbyte1(char inbyte);
char handleWaitInbyte2(char inbyte);

#endif /* TRAIN_PHY_H_ */
