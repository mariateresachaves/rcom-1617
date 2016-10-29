#include "datalink.h"

// Sequence number
int Ns = 0;

void data_packet(char * buf, int buf_size) {

	int i = 4;
	int dp_size = buf_size + 4;
	char * data_packet = malloc(dp_size);

	data_packet[0] = 1;
	data_packet[1] = Ns;
	data_packet[2] = buf_size >> 8;
	data_packet[3] = buf_size & 0xFF;

	Ns = (Ns + 1) % 256;

	while(i != buf_size)
		data_packet[i] = buf[i++];

	//llwrite(data_packet, dp_size);
}

int main(int argc, char** argv) {

	char * buf;
	char buf_reader[MAX_SIZE];
	char fileName[MAX_SIZE];
	int fileFD, fileSize = 0;

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
		//printf("String to send: ");
    	//gets(buf);
		printf("File Name: ");
		scanf("%s", fileName);
		fileFD = open(fileName, O_RDONLY | O_NOCTTY );
		buf = malloc(1);
		
		// le o ficheiro byte a byte
		while(read(fileFD, &buf[fileSize], 1)){
			fileSize++;
			buf = realloc(buf, fileSize+newtio.c_cc[VMIN]);
		}

		// start_control_packet

			// enviar para cada pacote de dados
				// data_packet

				// llwrite

		// end_control_packet

		llwrite(buf);
		llread();

		printf("\tTransmitter ended successfully\n");

	} else if(al.status == RECEIVER) { // RECEIVER


		// Verificar qual o tipo de pacote que esta a receber
		// Dependendo do tipo de pacote tratar os dados recebidos

		memcpy(buf_reader, llread(), MAX_SIZE);
		llwrite(buf_reader);
		
		printf("\nReceiver ended successfully\n");

	}

	
	llclose();

}
