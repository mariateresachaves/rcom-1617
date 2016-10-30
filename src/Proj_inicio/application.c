#include "datalink.h"

void check_args(int argc, char ** argv) {
	if ( (argc < 3) || ((strcmp("/dev/ttyS0", argv[1])!=0) &&
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
	  printf("|          ei11140          |        up201306842        |\n");
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

void ask_parameters() {
	print_baudrate();
	scanf("%d", &BAUDRATE);

	printf("\nMaximum number of retransmissions -> ");
	scanf("%d", &RETRANSMISSIONS);

	printf("\nTime-out interval -> ");
	scanf("%d", &INT_TIMEOUT);

	printf("\nMaximum information field size -> ");
	scanf("%d", &MAX_INFO);

	ll.baudrate = BAUDRATE;
	ll.sequenceNumber = 0;
	ll.timeout = INT_TIMEOUT;
	ll.numTransmissions = RETRANSMISSIONS;
	memcpy(ll.frame, "", strlen(""));
}

void start_control_packet(FILE * fileFD, char * fileName, int file_size) {
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
	int scp_size = l1+l2+5;

	char * start_cp = malloc(scp_size);

	start_cp[0] = START;
	start_cp[1] = 0x00;
	start_cp[2] = l1;
	memcpy(&start_cp[3], st, l1+1);
	start_cp[3+l1] = 0x01;
	start_cp[4+l1] = l2;
	memcpy(&start_cp[5+l1], fileName, l2+1);

	printf("Vou entrar no llwrite....\n");
	//llwrite(start_cp, scp_size);
	write(al.fd,0x05,4);
}

void send_all_dataPackets(char * buf, FILE * fileFD, char * fileName, int fileSize) {
	int i=0, str_size = strlen(buf);
	char aux[MAX_SIZE];
	ll.numPackets = 0;

	start_control_packet(fileFD, fileName, fileSize+1);

  /*while(i < str_size+1) {
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

		//data_packet(aux, send_bytes);
		//sleep(1);
		i+=MAX_INFO;
		ll.numPackets++;
	}*/

	//end_control_packet(fileFD, fileName);
}

void receive_packets(FILE *fileFD) {
	int packet = 0;
	int size;
	char buf[MAX_SIZE];

	char * file_name = malloc(1);
	char * file_size = malloc(1);

	int i;

	while (1) {
		switch (packet){
			case START_PACKET:
				size = read(al.fd,buf,3);

				if(size>0){
						printf("--- SIZE: %d ---\n", size);
						for (i=0;i<size;i++)
							printf("--- CONTENT: %x ---\n", buf[i]);
					if (buf[0]==START){
						printf("--- RECEIVING START PACKET ---\n");
						break;
						for (i=1; i<size; i += (buf[i+1]+2) ){
							if (buf[i]==0x00){
								file_size=realloc(file_size, buf[i+1]+1);
								memcpy(file_size, &buf[i+2], buf[i+1]);
							} else if (buf[i]==0x01){
								file_name=realloc(file_name, buf[i+1]+1);
								memcpy(file_name, &buf[i+2], buf[i+1]);
							}
						}
						packet++;
					}
				} else {
					continue;
				}
		}
		break;
	}

}

int main(int argc, char** argv) {
	char fileName[MAX_SIZE];
	char * buf = malloc(1);
	FILE * fileFD;
	int fileSize = 0;

 	check_args(argc, argv);

	print_header();

	// Update ll.port with argument
	memcpy(ll.port, argv[1], strlen(argv[1]));

	// Open port
	al.fd = 0;
	llopen();

	if (al.status == TRANSMITTER) {

		ask_parameters();

		print_transmitter();

		printf("\nFile Name -> ");
		scanf("%s", fileName);
		fileFD = fopen(fileName, "rb" );

		// Read file byte a byte
		while(fread(&buf[fileSize], sizeof(char), 1, fileFD)){
			fileSize++;
			buf = realloc(buf, fileSize+MAX_INFO);
		}

		send_all_dataPackets(buf, fileFD, fileName, fileSize);

		printf("\n\tTransmitter ended successfully\n");

	} else if(al.status == RECEIVER) {

		print_receiver();

		receive_packets(fileFD);

		printf("\tReceiver ended successfully\n");

	}

	print_footer();

	llclose();
}
