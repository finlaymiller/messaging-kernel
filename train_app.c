/*
 * train_app.c
 *
 *  Created on: Dec 7, 2019
 *      Author: Derek
 */


#include "train_app.h"
#include "train_dl.h"

void transmitMagDir(char loc_num, char mag, char dir)
{
    struct mag_dir magdir;
    magdir.ignored = 0x0;
    magdir.direction = dir;
    magdir.magnitude = mag;

    /* Build mag_dir message */
    struct t_message new_mess = buildMagDirMessage(loc_num, magdir);

    /* Send to datalink layer */
    dl_transmitMagDir(new_mess);
}

struct t_message buildMagDirMessage(unsigned char loc_num, struct mag_dir mag_arg)
{
    struct t_message message;

    /* Set locomotive number and code */
    message.code = C_MAGDIR;
    message.arg1 = loc_num;

    /* Set magnitude/direction argument (Casting to pointer to shut compiler up) */
    unsigned char *tmp_mag = (unsigned char *)&mag_arg;
    message.arg2 = (unsigned char) *tmp_mag;

    return message;
}

