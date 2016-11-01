/*! \file */

#include "datalink.h"

/**
 * @brief Function to verify the user-inserted arguments.
 * @param argc - number of arguments inserted.
 * @param argv - arguments inserted.
 */
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

/**
 * @brief Prints the header of the project.
 */
void print_header() {
	printf("\n.--------------------------.--------------------------.\n");
		printf("|    RCOM FIRST PROJECT    |    DATA LINK PROTOCOL    |\n");
		printf("'--------------------------'--------------------------'\n\n");
}

/**
 * @brief Prints the footer of the project.
 */
void print_footer() {
	printf("\n.---------------------------.---------------------------.\n");
	  printf("|    Carlos Miguel Lucas    |    Maria Teresa Chaves    |\n");
	  printf("|            -              |            -              |\n");
	  printf("|          ei11140          |        up201306842        |\n");
	  printf("'---------------------------'---------------------------'\n\n");
}

/**
 * @brief Print a message to indicate that this side is the transmitter.
 */
void print_transmitter() {
	printf("\n.-------------------.\n");
		printf("|    TRANSMITTER    |\n");
		printf("'-------------------'\n\n");
}

/**
 * @brief Print a message to indicate that this side is the receiver.
 */
void print_receiver() {
	printf("\n.----------------.\n");
		printf("|    RECEIVER    |\n");
		printf("'----------------'\n\n");
}

/**
 * @brief Print a table with baudrate values supported by the RS-232.
 */
void print_baudrate() {
	printf(" _________________________________________\n");
	printf("|   Baudrate values supported by RS-232   |\n");
	printf(" -----------------------------------------\n");
	printf("|   300 |  1200 |  2400 |   4800 |   9600 |\n");
	printf("| 19200 | 38400 | 57600 | 115200 | 230400 |\n");
	printf(" -----------------------------------------\n\n");
	printf("Baudrate -> ");
}

/**
 * @brief Asks the user to insert the baudrate, number of retransmissions, time-out interval and the maximum size of information field.
 */
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

/**
 * @brief Sends a start control packet.
 * @param fileFD - file descriptor of the file.
 * @param fileName - file name.
 * @param file_size - file size.
 */
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

	//llwrite(start_cp, scp_size);
	write(al.fd, start_cp, scp_size);
	free(start_cp);
}

/**
 * @brief Sends a data packet.
 * @param buf - data to send.
 * @param buf_size - data size.
 */
void data_packet(char * buf, int buf_size) {
	int i;
	char *packet = malloc(buf_size+4);

	packet[0] = DATA;
	packet[1] = ll.sequenceNumber;
	ll.sequenceNumber = (ll.sequenceNumber + 1) % 256;
	packet[2] = buf_size >> 8;
	packet[3] = buf_size & 0x0F;


	for (i = 0; i < buf_size; i++)
		packet[i+4] = buf[i];

	llwrite(packet, buf_size+4);
	free(packet);
}

/**
 * @brief Sends a end control packet.
 * @param fileFD - file descriptor of the file.
 * @param fileName - file name.
 * @param file_size - file size.
 */
void end_control_packet(FILE * fileFD, char * fileName, int file_size) {
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
	int ecp_size = l1+l2+5;

	char * end_cp = malloc(ecp_size);

	end_cp[0] = END;
	end_cp[1] = 0x00;
	end_cp[2] = l1;
	memcpy(&end_cp[3], et, l1+1);
	end_cp[3+l1] = 0x01;
	end_cp[4+l1] = l2;
	memcpy(&end_cp[5+l1], fileName, l2+1);

	//llwrite(end_cp, scp_size);
	write(al.fd, end_cp, ecp_size);
	free(end_cp);
}

/**
 * @brief Sends all the fragments of the file.
 * @param buf - file content.
 * @param fileFD - file descriptor of the file.
 * @param fileName - file name.
 * @param fileSize - file size.
 */
void send_all_dataPackets(char * buf, FILE * fileFD, char * fileName, int fileSize) {
	int i=0, str_size = strlen(buf);
	char aux[MAX_SIZE];
	ll.numPackets = 0;

	start_control_packet(fileFD, fileName, fileSize+1);

  while(i < str_size+1) {
		int send_bytes = 0;
		memset(aux, 0, MAX_INFO);

		if(MAX_INFO > fileSize-i) {
			memcpy(aux, buf + i, fileSize-i);
			send_bytes = fileSize-i;
			aux[send_bytes] = 0;
			send_bytes++;
		}

		else {
			memcpy(aux, buf + i, fileSize);
			send_bytes = fileSize;
			aux[send_bytes] = 0;
		}

		data_packet(aux, send_bytes);
		//sleep(1);
		i+=MAX_INFO;
		ll.numPackets++;
	}

	end_control_packet(fileFD, fileName, fileSize+1);
}

/**
 * @brief Function to receive the packets.
 * @param fileFD - file descriptor of the file.
 */
void receive_packets(FILE *fileFD) {
	int packet = 0;
	int size;
	char * buf = malloc(MAX_SIZE);
	//char buf_data[MAX_SIZE]

	char * file_name = malloc(MAX_SIZE);
	char * file_size = malloc(1);
	char * file = malloc(1);

	int len = 0;

	int i;

	while (1) {

		size = read(al.fd, buf, MAX_SIZE);

		switch (packet){
			case START_PACKET:
				if(size>0){
					if (buf[0]==START){
						printf("\n--- RECEIVING START PACKET ---\n");
						for (i=1; i<size; i += (buf[i+1]+2) ){
							if (buf[i]==0x00){
								file_size=realloc(file_size, buf[i+1]+1);
								memcpy(file_size, &buf[i+2], buf[i+1]);
								printf("SIZE ------- %d\n",file_size);
							} else if (buf[i]==0x01){
								file_name=realloc(file_name, buf[i+1]+1);
								memcpy(file_name, &buf[i+2], buf[i+1]);
								printf("NAME ------- %s\n",file_name);
							}
						}
						for (i=0;i<size;i++)
							printf("--- CONTENT: %x ---\n", buf[i]);
						packet++;
						//break;
					}
					printf("\n--- END OF START PACKET ---\n");
				} else {
					continue;
				}
			case DATA_PACKET:
				//memset(buf, 0, size);
				size = read(al.fd, buf, MAX_SIZE);
				printf("--- SIZE: %d ---\n", size);
				//for (i=0;i<size;i++)
					//printf("--- CONTENT: %x ---\n", buf[i]);
				if(size>0){
					if (buf[0]==DATA){
						printf("--- RECEIVING DATA PACKET ---\n");
						file=realloc(file, ((buf[2] << 8) + buf[3] )+len+1);
						memcpy(file+len, &buf[4], (buf[2] << 8) + buf[3] );
						len += (buf[2] << 8) + buf[3];
					}
				}
		}
		break;
	}
	printf("--- RECEIVING PACKETS  ---\n");
}

/**
 * @brief Main function.
 * @param argc - number of arguments inserted.
 * @param argv - arguments inserted.
 */
void main(int argc, char** argv) {
	char fileName[MAX_SIZE];
	char * buf = malloc(1);
	char * buf_r = malloc(MAX_SIZE);
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
		//write(al.fd, buf, fileSize);

		printf("\n\tTransmitter ended successfully\n");

	} else if(al.status == RECEIVER) {

		int res=0;

		print_receiver();

		//receive_packets(fileFD);
		while(!res)
			res = read(al.fd, buf, fileSize);

		printf("\tReceiver ended successfully\n");

	}

	print_footer();

	llclose();
}
