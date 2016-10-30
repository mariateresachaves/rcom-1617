/*! \file */

#ifndef UTILITIES_H
#define UTILITIES_H

#define FALSE 0
#define TRUE 1

//#define BAUDRATE B9600
int BAUDRATE;
int RETRANSMISSIONS;
int INT_TIMEOUT;
int MAX_INFO;
#define MODEMDEVICE "/dev/ttyS1"

#define RECEIVER 0
#define TRANSMITTER 1

#define _POSIX_SOURCE 1

#define MAX_SIZE 255
#define STOP_BYTE '\0'

#define DATA 1
#define START 2
#define END 3

#define START_PACKET 0
#define DATA_PACKET 1
#define END_PACKET 2


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

#define BCC1 0x00 // A ^ C_TRANSMITTER
#define BCC2 0x02 // A ^ C_RECEIVER

#define FLAG_ESC 0x7D

/**
 * @brief Contains some information about the application layer.
 */
struct ApplicationLayer{
	int fd; 	///< Descriptor of the active port.
	int status;	///< Transmitter or Receiver.
} al;

/**
 * @brief Contains some information about the data link layer.
 */
struct LinkLayer {
	char port[20];					///< /dev/ttyS0
	int baudrate;					///< Transition velocity.
	unsigned int sequenceNumber;	///< Sequence number of the packet
	unsigned int timeout;			///< Timer value
	unsigned int numTransmissions; 	///< Number of retransmissions in case of fail.
	unsigned int numPackets;		///< Number of packets.

	char frame[MAX_SIZE]; 			///< Packet.
} ll;

struct termios oldtio, newtio;

#endif
