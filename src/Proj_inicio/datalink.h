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
void sm_command(char * type);
void state_machine(char side, char * type);
void llwrite(char * packet, int packet_size);

#endif
