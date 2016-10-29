#include "datalink.h"

char SET2[5] = {FLAG, A, C_SET, A^C_SET, FLAG};

void start_control_packet(FILE * fileFD, char * fileName, int file_size) {


	char *stamanho = malloc((int) (file_size/256)+1);


	sprintf(stamanho, "%d", file_size);

	int sizestamanho = strlen(stamanho);
	int sizenome = strlen(fileName);

	char* pacote_start = malloc(40);

	pacote_start[0] = START;
	pacote_start[1] = 0x0;
	pacote_start[2] = sizestamanho;
	strcpy(&pacote_start[3], stamanho);
	pacote_start[3+sizestamanho] = 0x1;
	pacote_start[3+sizestamanho+1] = sizenome;
	strcpy(&pacote_start[3+sizestamanho+2], fileName);

	llwrite(pacote_start, 3+sizestamanho+2+sizenome);

	printf("File size: %d bytes\n", file_size);
	printf("st[0] = %s\n", stamanho);
	printf("l1 = %d\n", sizestamanho);
	printf("l2 = %d\n", sizenome);
	printf("pacote_start[0] = %d\n", pacote_start[0]);
	printf("pacote_start[1] = %x\n", pacote_start[1]);
	printf("pacote_start[2] = %d\n", pacote_start[2]);
	printf("pacote_start[3] = %d\n", pacote_start[3]);
	printf("pacote_start[4] = %x\n", pacote_start[4]);
	printf("pacote_start[5] = %d\n", pacote_start[5]);
	printf("pacote_start[6] = %s\n", &pacote_start[6]);

/*	int fsize;

	fseek(fileFD, 0, SEEK_END);
	fsize = ftell(fileFD);

	printf("fsize = %d", fsize);

	char * st = malloc(4);

	// LITTLE ENDIAN
	st[0] = fsize % 0x100;
	st[1] = (fsize/0x100) % 0x100;
	st[2] = (fsize/0x10000) % 0x100;
	st[3] = fsize/0x1000000;



	int l1 = strlen(st);
	int l2 = strlen(fileName);

	char * start_cp = malloc(l1+l2+5);

	start_cp[0] = START;
	start_cp[1] = 0x0;
	start_cp[2] = l1;
	memcpy(&start_cp[3], st, l1+1);
	start_cp[3+l1] = 0x1;
	start_cp[4+l1] = l2;
	memcpy(&start_cp[5+l1], fileName, l2+1);

	llwrite(start_cp, l1+l2+5);
	*/
}

void end_control_packet(FILE * fileFD, char * fileName) {

	int fsize;

	fseek(fileFD, 0, SEEK_END);
	fsize = ftell(fileFD);

	char * et = malloc(4);

	// LITTLE ENDIAN
	et[0] = fsize % 0x100;
	et[1] = (fsize/0x100) % 0x100;
	et[2] = (fsize/0x10000) % 0x100;
	et[3] = fsize/0x1000000;

	int l1 = strlen(et);
	int l2 = strlen(fileName);

	char * end_cp = malloc(l1+l2+5);

	end_cp[0] = END;
	end_cp[1] = 0x0;
	end_cp[2] = l1;
	strcpy(&end_cp[3], et);
	end_cp[3+l1] = 0x1;
	end_cp[4+l1] = l2;
	memcpy(&end_cp[5+l1], fileName, l2);

	llwrite(end_cp, l1+l2+5);
}

void data_packet(char * buf, int buf_size) {

	int i = 4;
	int dp_size = buf_size + 4;
	char * data_packet = malloc(dp_size);

	data_packet[0] = DATA;
	data_packet[1] = ll.sequenceNumber;
	data_packet[2] = buf_size >> 8;
	data_packet[3] = buf_size & 0x0F;

	ll.sequenceNumber = (ll.sequenceNumber + 1) % 256;

	while(i != buf_size){
		data_packet[i] = buf[i];
		i++;
	}


	llwrite(data_packet, dp_size);
}

// PRINT FUNCTIONS

void print_header() {
	printf("\n.--------------------------.--------------------------.\n");
		printf("|    RCOM FIRST PROJECT    |    DATA LINK PROTOCOL    |\n");
		printf("'--------------------------'--------------------------'\n\n");
}

void print_footer() {
	printf("\n.---------------------------.---------------------------.\n");
	  printf("|    Carlos Miguel Lucas    |    Maria Teresa Chaves    |\n");
	  printf("|            -              |            -              |\n");
	  printf("|       	 ei11140          |       up201306842         |\n");
	  printf("'---------------------------'---------------------------'\n\n");
}

void print_transmitter() {
	printf("\n.-------------------.\n");
		printf("|    TRANSMITTER    |\n");
		printf("'-------------------'\n\n");
}

void print_receiver() {
	printf("\n.----------------.\n");
		printf("|    RECEIVER    |\n");
		printf("'----------------'\n\n");
}

void print_baudrate() {
	printf(" _________________________________________\n");
	printf("|   Baudrate values supported by RS-232   |\n");
	printf(" -----------------------------------------\n");
	printf("|   300 |  1200 |  2400 |   4800 |   9600 |\n");
	printf("| 19200 | 38400 | 57600 | 115200 | 230400 |\n");
	printf(" -----------------------------------------\n\n");
	printf("Baudrate -> ");
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

	print_header();

	memcpy(ll.port, argv[1], strlen(argv[1]));

	al.fd = 0;

	al.fd = llopen();


	if (al.status == TRANSMITTER) {

		print_baudrate();
		scanf("%d", &BAUDRATE);

		printf("\nMaximum number of retransmissions -> ");
		scanf("%d", &RETRANSMISSIONS);

		printf("\nTime-out interval -> ");
		scanf("%d", &INT_TIMEOUT);

		printf("Maximum information field size -> ");
		scanf("%d", &MAX_INFO);

		ll.baudrate = BAUDRATE;
		ll.sequenceNumber = 0;
		ll.timeout = INT_TIMEOUT;
		ll.numTransmissions = RETRANSMISSIONS;
		memcpy(ll.frame, "", strlen(""));

		print_transmitter();

		sendFlags(SET2, "SET");

		printf("File Name: ");
		scanf("%s", fileName);
		fileFD = fopen(fileName, "rb" );

		// le o ficheiro byte a byte
		buf = malloc(1);

		while(fread(&buf[fileSize], sizeof(char), 1, fileFD)){
			fileSize++;
			buf = realloc(buf, fileSize+MAX_INFO);
		}

		int i=0;
		char aux[MAX_SIZE];

		// String size
		int num = strlen(buf);



		start_control_packet(fileFD, fileName, fileSize+1);

		// Envia a mensagem de MAX_INFO em MAX_INFO bytes
	  while(i < num+1) {

			int send_bytes = 0;

			memset(aux, 0, MAX_INFO);

			if(MAX_INFO > strlen(buf)-i) {
				memcpy(aux, buf + i, strlen(buf)-i);
				send_bytes = strlen(buf)-i;

				aux[send_bytes] = 0;
				send_bytes++;
			}
			else {
				memcpy(aux, buf + i, MAX_INFO);
				send_bytes = MAX_INFO;

				aux[send_bytes] = 0;
			}

			data_packet(aux, send_bytes);

			sleep(1);

			i+=MAX_INFO;

		}

		end_control_packet(fileFD, fileName);

		// start_control_packet


			// enviar para cada pacote de dados
				// data_packet

		// end_control_packet

		// so para testar
		//start_control_packet(fileFD, fileName);
		//end_control_packet(fileFD, fileName);

		//llwrite(buf);
		//llread(0);

		printf("\tTransmitter ended successfully\n");

	} else if(al.status == RECEIVER) { // RECEIVER

		print_receiver();

		// Verificar qual o tipo de pacote que esta a receber
		// Dependendo do tipo de pacote tratar os dados recebidos

		// memcpy(buf_reader, llread(), MAX_SIZE);
		// llwrite(buf_reader); //not working i dont know why!!!

		//llread(1);
		printf("\tReceiver ended successfully\n");

	}

	print_footer();

	llclose();

}
