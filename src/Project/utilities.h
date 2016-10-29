#ifndef UTILITIES_H
#define UTILITIES_H

#define FALSE 0
#define TRUE 1

//#define BAUDRATE B9600
int BAUDRATE;
#define MODEMDEVICE "/dev/ttyS1"

#define RECEIVER 0
#define TRANSMITTER 1

#define _POSIX_SOURCE 1

#define MAX_SIZE 255
#define STOP_BYTE '\0'

#define DATA 1
#define START 2
#define END 3


#define FLAG 0x7E
#define A 0x03

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

struct ApplicationLayer{
	int fd; 			//fd - descritor da porta ativa
	int status;			//transmissor ou recetor
} al;

struct LinkLayer {
	char port[20];					// /dev/ttyS0
	int baudrate;					//velocidade de transmissão
	unsigned int sequenceNumber;	//Número de sequência da trama
	unsigned int timeout;			//Valor do timer
	unsigned int numTransmissions; 	//Numero de tentativas em caso de falha

	char frame[MAX_SIZE]; 			// Trama
} ll;

struct termios oldtio, newtio;

#endif
