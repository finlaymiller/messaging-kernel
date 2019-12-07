/*
 * train_dl.h
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */

#ifndef TRAIN_DL_H_
#define TRAIN_DL_H_

#include "train_app.h"

#define MAX_MESS_LEN 10  /* TODO: Fix this value */

/* Type of packets to send/receive */
enum PktType{DATA = 0, ACK, NACK};

/* Define structure for packets */
struct Packet{
    unsigned char control;  /* Control field of packet */
    unsigned char length;   /* Length of message */
    unsigned char message[MAX_MESS_LEN]; /* Message */
};

struct Control{
    unsigned char nr : 3;   /* Nr for packet */
    unsigned char ns : 3;   /* Ns for packet */
    enum PktType type : 2;  /* Type of packet */
};

union Ctrl{
    struct Control s_control;
    char c_control;
};

void initControlUnion(void);
void dl_transmitMagDir(struct t_message magdir);


#endif /* TRAIN_DL_H_ */
