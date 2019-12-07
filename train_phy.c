/*
 * train_phy.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */

#include "train_phy.h"
#include "uart.h"

void phy_transmitMagDir(struct Packet packet)
{
    UART1_TXChar('b');
}
