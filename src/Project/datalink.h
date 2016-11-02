#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

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

#define MAX_FRAME_SIZE 4000

#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FLAG_INICIAL 0
#define FLAG_FINAL 1
#define BCC_DECODE 2
#define SEND_UA_BACK 3
#define EXIT_STATE_MACHINE 4

#define SEND_CMD 0
#define RCV_UA 1
#define RCV_DISC 2
#define RCV_I_BACK 3
#define EXIT_STATE_MACHINE_2 4

#define BCC1_FAIL 0
#define BCC2_FAIL 1
#define NO_ERROR 2


int CONNECTION;

int count_msg_dropped;
int duplicate_found;
int n_retransmissions;
int n_timeouts;


int llopen(char * porta);
int llwrite(int fd, char *buffer, int length);
int llread(int fd, char *buffer);
int llclose(int fd);

#endif
