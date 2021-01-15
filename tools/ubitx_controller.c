/* ubitx_controller
 * Copyright (C) 2021 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <dirent.h>
#include <pthread.h>
#include <ctype.h>

#include "serial.h"
#include "ubitx_controller.h"

int main(int argc, char *argv[])
{
    int ssb_mode = UNDEFINED;
    int radio_type = RADIO_TYPE_ICOM;
    int frequency = 0;
    char serial_path[MAX_MODEM_PATH];
    int cw_mode = UNDEFINED;
    int serial_fd = -1;

    if (argc < 3)
    {
    manual:
        fprintf(stderr, "Usage modes: \n%s -s serial_device -r [icom,ubitx] -f frequency [-u -l] -c [on,off]\n", argv[0]);
        fprintf(stderr, "%s -h\n", argv[0]);
        fprintf(stderr, "\nOptions:\n");
        fprintf(stderr, " -s serial_device           Set the serial device file path\n");
        fprintf(stderr, " -f frequency               Sets the dial frequency (in Hz)\n");
        fprintf(stderr, " -c [on,off]                Set CW mode on or off\n");
        fprintf(stderr, " -l                         Sets LSB mode\n");
        fprintf(stderr, " -u                         Sets USB mode\n");
        fprintf(stderr, " -r [icom,ubitx]            Sets radio type (supported: icom or ubitx)\n");
        fprintf(stderr, " -h                         Prints this help.\n");
        exit(EXIT_FAILURE);
    }

    int opt;
    while ((opt = getopt(argc, argv, "hs:c:f:lur:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            goto manual;
            break;
        case 's':
            strcpy(serial_path, optarg);
            break;
        case 'l':
            ssb_mode = LSB;
            break;
        case 'u':
            ssb_mode = USB;
            break;
        case 'r':
            // icom is the default...
            if (!strcmp(optarg,"ubitx"))
                radio_type = RADIO_TYPE_UBITX;
            break;
        case 'f':
            frequency = atoi(optarg);
            break;
        case 'c':
            if (!strcmp(optarg,"on"))
                cw_mode = CW_ON;
            if (!strcmp(optarg,"off"))
                cw_mode = CW_OFF;
            break;
        default:
            goto manual;
        }
    }

    serial_fd = open_serial_port(serial_path);

    if (serial_fd == -1)
    {
        fprintf(stderr, "Could not open serial device.\n");
        return EXIT_FAILURE;
    }

    if (radio_type == RADIO_TYPE_ICOM){
        fprintf(stderr, "icom\n");
        set_fixed_baudrate("19200", serial_fd);
    }

    if (radio_type == RADIO_TYPE_UBITX){
        fprintf(stderr, "ubitx\n");
        set_fixed_baudrate("38400", serial_fd);
    }

    // mamma mia!!!!
    sleep(2);

    if (ssb_mode != UNDEFINED)
    {
        set_ssb_mode(serial_fd, radio_type, ssb_mode);
    }


}