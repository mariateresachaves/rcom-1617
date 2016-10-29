#include "datalink.h"

// Sequence number
int Ns = 0;

void start_control_packet(FILE * fileFD, char * fileName) {

	printf("FILENAME: %s", fileName);

	int fsize;

	fseek(fileFD, 0, SEEK_END);
	fsize = ftell(fileFD);

	char * st = malloc(4);

	// LITTLE ENDIAN
	st[0] = fsize % 0x100;
	st[1] = (fsize/0x100) % 0x100;
	st[2] = (fsize/0x10000) % 0x100;
	st[3] = fsize/0x1000000;

	int l1 = strlen(st);
	int l2 = strlen(fileName);

	char * start_cp = malloc(l1+l2+5);

	start_cp[0] = 0x2;
	start_cp[1] = 0x0;
	start_cp[2] = l1;
	strcpy(&start_cp[3], st);
	start_cp[3+l1] = 0x1;
	start_cp[4+l1] = l2;
	memcpy(&start_cp[5+l1], fileName, l2);

	//llwrite(start_cp, l1+l2+5);

	printf("File size: %d bytes\n", fsize);
	printf("st[0] = %x\n", st[0]);
	printf("st[1] = %x\n", st[1]);
	printf("st[2] = %x\n", st[2]);
	printf("st[3] = %x\n", st[3]);
	printf("l1 = %d\n", l1);
	printf("l2 = %d\n", l2);
	printf("start_cp[0] = %x\n", start_cp[0]);
	printf("start_cp[1] = %x\n", start_cp[1]);
	printf("start_cp[2] = %d\n", start_cp[2]);
	printf("start_cp[3] = %d\n", start_cp[3]);
	printf("start_cp[4] = %x\n", start_cp[3+l1]);
	printf("start_cp[5] = %d\n", start_cp[3+l1+1]);
	printf("start_cp[6] = %d\n", start_cp[3+l1+2]); // TODO: Porque que isto e' um int???

}

void data_packet(char * buf, int buf_size) {

	int i = 4;
	int dp_size = buf_size + 4;
	char * data_packet = malloc(dp_size);

	data_packet[0] = 1;
	data_packet[1] = Ns;
	data_packet[2] = buf_size >> 8; // TODO:
	data_packet[3] = buf_size & 0xFF; // TODO:

	Ns = (Ns + 1) % 256;

	while(i != buf_size)
		data_packet[i] = buf[i++];

	//llwrite(data_packet, dp_size);
}

int main(int argc, char** argv) {

	char * buf;
	char buf_reader[MAX_SIZE];
	char fileName[MAX_SIZE];
	int fileSize = 0;
	FILE * fileFD;

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
		fileFD = fopen(fileName, "rb" );
		buf = malloc(1);
		
		// le o ficheiro byte a byte
		while(fread(&buf[fileSize], sizeof(char), 1, fileFD)){
			fileSize++;
			buf = realloc(buf, fileSize+newtio.c_cc[VMIN]);
		}

		// start_control_packet

			// enviar para cada pacote de dados
				// data_packet

				// llwrite

		// end_control_packet

		// so para testar
		//start_control_packet(fileFD, fileName);

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
