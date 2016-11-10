#ifndef UTILITIES_H
#define UTILITIES_H

#define FALSE 0
#define TRUE 1

#define MODEMDEVICE "/dev/ttyS0"

#define RECEIVER 1
#define TRANSMITTER 0

#define _POSIX_SOURCE 1

#define MAX_SIZE 255

#define STOP_BYTE '\0'

#define DATA 1
#define START 2
#define END 3

#define START_PACKET 0
#define DATA_PACKET 1
#define END_PACKET 2
#define STORE_FILE 3

#define FLAG 0x7E
#define A_TRANSMITTER 0x03
#define A_RECEIVER 0x01

#define C_SET 0x03
#define C_DISC 0x0B
#define C_UA 0x07
#define C_RR 0x05
#define C_RR_ACK 0x85
#define C_REJ 0x01
#define C_REJ_ACK 0x81

#define C_INFO_0 0x00
#define C_INFO_1 0x40

#define BCC1 0x00
#define BCC2 0x02

#define FLAG_ESC 0x7D

int BAUDRATE;
int NS;
int NUMPACKETS;
int NUMTRANSMISSIONS;
int TIMEOUT;
int MAXINFO;

int STATUS;

struct termios oldtio, newtio;

struct stat s;

#endif

