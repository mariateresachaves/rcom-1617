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

    strcpy(ll.port, argv[1]);

	ll.baudrate = BAUDRATE;
	ll.sequenceNumber = 0;
	ll.timeout = 3;
	ll.numTransmissions = 3;
	strcpy(ll.frame, "");

    al.fd = 0;

	printf("--- Welcome to RCOM best project ever ---\n\n");

    (void) signal(SIGALRM, timer_handler);

	al.fd = llopen();

	if (al.status == TRANSMITTER) {

		printf("String to send: ");
    	gets(buf);
		llwrite(buf);
		llread();

		printf("\nTransmitter ended successfully\n");

	} else { // RECEIVER
        char * aux = llread();
		memcpy(buf, aux, strlen(aux));
		llwrite(buf);

		printf("\nTransmitter ended successfully\n");

	}

	
	llclose();

}
