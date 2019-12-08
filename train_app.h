/*
 * train_app.h
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */

#ifndef TRAIN_APP_H_
#define TRAIN_APP_H_

/* Define locomotive numbers */
#define LOC1 0x01
#define LOC2 0x02
#define LOC_ALL 0xFF

/* Define directions */
#define CW 0x0
#define CCW 0x1

/* Define message codes */
#define C_MAGDIR 0xC0

/* Define message code lengths */
#define L_MAGDIR 3
#define L_ERR 255

/* Trainset messages */
struct t_message{
    unsigned char code;  /* Message code */
    unsigned char arg1;  /* First argument */
    unsigned char arg2;  /* Second argument */
};

/* Structure of magnitude/direction message */
struct mag_dir{
    unsigned char magnitude : 4;  /* Speed of train */
    unsigned char ignored : 3;    /* Reserved bits */
    unsigned char direction : 1;  /* 1 for CCW, 0 for CW */
};

/* Public Function Declarations */
struct t_message buildMagDirMessage(unsigned char loc_num, struct mag_dir mag_arg);
void transmitMagDir(char loc_num, char mag, char dir);

#endif /* TRAIN_APP_H_ */
