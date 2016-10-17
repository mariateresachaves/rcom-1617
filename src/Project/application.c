#include "datalink.h"

int main(int argc, char** argv) {

	char buf[MAX_SIZE];

 	if ( (argc < 3) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 Receiver(0)/Transmitter(1)\n");
		exit(1);
    }
    
	if (strcmp(argv[2], "0") == 0)
		al.status = RECEIVER;
	else if (strcmp(argv[2], "1") == 0)
		al.status = TRANSMITTER;
	else {
		printf("Please select a valid option. For receiver choose 0 or transmitter choose 1.\n");
		exit(1);
	}

    memcpy(ll.port, argv[1], strlen(argv[1]));

	ll.baudrate = BAUDRATE;
	ll.sequenceNumber = 0;
	ll.timeout = 3;
	ll.numTransmissions = 3;
	memcpy(ll.frame, "", strlen(""));

    al.fd = 0;

	printf("--- Welcome to the best RCOM project ever ---\n\n");

	al.fd = llopen();

	if (al.status == TRANSMITTER) {

		printf("String to send: ");
    	gets(buf);
		llwrite(buf);
		llread();

		printf("\tTransmitter ended successfully\n");

	} else { // RECEIVER

		memcpy(buf, llread(), MAX_SIZE);
		llwrite(buf);
		
		printf("\nReceiver ended successfully\n");

	}

	
	llclose();

}
