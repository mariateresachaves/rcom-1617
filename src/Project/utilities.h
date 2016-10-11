#ifndef UTILITIES_H
#define UTILITIES_H

#define FALSE 0
#define TRUE 1

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"

#define RECEIVER 0
#define TRANSMITTER 1

#define _POSIX_SOURCE 1

#define MAX_SIZE 255
#define STOP_BYTE '\0'


#define FLAG 0x7E
#define A 0x03
#define C_TRANSMITTER 0x03
#define C_RECEIVER 0x01
#define BCC1 0x00

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
