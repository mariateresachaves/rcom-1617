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

void timer_handler();
void printFlags(char * flags);
int open_port();
int llopen();
int llclose();
char * llread();
int llwrite(char * buf);
void sendFlags(char * flagToSend, char * type);

#endif
