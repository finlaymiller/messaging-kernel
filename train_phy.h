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

void phy_transmitMagDir(struct Packet packet);
char calculateChecksum(struct Packet packet);

#endif /* TRAIN_PHY_H_ */
