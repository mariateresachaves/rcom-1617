#ifndef DATALINK_H
#define DATALINK_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "utilities.h"

void printFlags(char * flags, char * type);
void def_c_cflag();
void llopen();
void llclose();
void bcc_generator(char * buf, int size);
void stuffing(char * buf, int * buf_size);
int write_packet(char side, char * type, char * data, int size);
int sm_command(char side, char * type, char * data, char size);
int sm_write(char side, char * type, char * data, char size);
void llwrite(char * packet, int packet_size);

#endif
