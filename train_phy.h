/*
 * train_phy.h
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */

#ifndef TRAIN_PHY_H_
#define TRAIN_PHY_H_

#include "train_dl.h"

#define STX 0x02
#define ETX 0x03
#define DLE 0x10

/* State definitions */
#define WAIT_STX        0
#define WAIT_INBYTE1    1
#define WAIT_INBYTE2    2

void phy_transmitFrame(struct Packet packet);
char calculateChecksum(struct Packet packet);
char handleWaitSTX(char inbyte);
char handleWaitInbyte1(char inbyte);
char handleWaitInbyte2(char inbyte);

#endif /* TRAIN_PHY_H_ */
