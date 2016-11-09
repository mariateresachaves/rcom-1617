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
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS0 Transmitter(1)/Receiver(0)\n");
		exit(1);
  }

	if (strcmp(argv[2], "1") == 0 || strcmp(argv[2], "0") == 0)
		STATUS = atoi(argv[2]);

	else {
		printf("Please select a valid option. For transmitter choose 0 or receiver choose 1.\n");
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
	scanf("%d", &NUMTRANSMISSIONS);

	printf("\nTime-out interval -> ");
	scanf("%d", &TIMEOUT);

	printf("\nMaximum information field size -> ");
	scanf("%d", &MAXINFO);

	NS = 0;

}

/**
 * @brief Função que  cria e escreve o pacote de controlo START.
 * @param fd - descritor da porta.
 * @param fileName - nome do ficheiro.
 * @param fileSize - tamanho do ficheiro.
 */
void start_control_packet(int fd, char* fileName, int fileSize) {
	int l1, l2, scp_size;
	char * fileSS, * start_cp;

	fileSS = malloc((int) (fileSize/256)+1);
	sprintf(fileSS, "%d", fileSize);

	l1 = strlen(fileSS);
	l2 = strlen(fileName);
	scp_size = l1 + l2 + 5;

	start_cp = malloc(40);

	start_cp[0] = START;
	start_cp[1] = 0x00;
	start_cp[2] = l1;
	strcpy(&start_cp[3], fileSS);
	start_cp[3 + l1] = 0x01;
	start_cp[4 + l1] = l2;
	strcpy(&start_cp[5 + l1], fileName);

	llwrite(fd, start_cp, l1 + l2 + 5);
}

/**
 * @brief Função que envia um pacote de dados.
 * @param fd - descritor da porta.
 * @param buf - dados a enviar.
 * @param bufSize - tamanho dos dados a enviar.
 */
void data_packet(int fd, char* buf, int bufSize) {
	int i = 0;
	char *packet = malloc(4+bufSize);

	packet[0] = DATA;
	packet[1] = NS;
	NS = (NS + 1) % 256;

	packet[2] = bufSize >> 8;
	packet[3] = bufSize & 0x0F;

	while (i != bufSize)
		packet[i+4] = buf[i++];

	llwrite(fd, packet, bufSize+4);
	free(packet);
}

/**
 * @brief Função que  cria e escreve o pacote de controlo END.
 * @param fd - descritor da porta.
 * @param fileName - nome do ficheiro.
 * @param fileSize - tamanho do ficheiro.
 */
void end_control_packet(int fd, char * fileName, int fileSize) {
	int l1, l2, scp_size;
	char * fileSS, * end_cp;

	fileSS = malloc((int) (fileSize/256)+1);
	sprintf(fileSS, "%d", fileSize);

	l1 = strlen(fileSS);
	l2 = strlen(fileName);
	scp_size = l1 + l2 + 5;

	end_cp = malloc(40);

	end_cp[0] = END;
	end_cp[1] = 0x00;
	end_cp[2] = l1;
	strcpy(&end_cp[3], fileSS);
	end_cp[3 + l1] = 0x01;
	end_cp[4 + l1] = l2;
	strcpy(&end_cp[5 + l1], fileName);

	llwrite(fd, end_cp, l1 + l2 + 5);
}

/**
 * Máquina de estados para receber pacotes.
 * @param fd - descritor da porta.
 */
void sm_receiver(int fd){
		int res, i, file_len=0, state=0, j=0;

		char * file_name = malloc(1);
		char * file_size = malloc(1);
		char * file = malloc(1);

		char store_name[MAX_SIZE];
		char packet[MAX_FRAME_SIZE];

		while (1) {
			if (state == START_PACKET) {
				res = llread(fd, packet);

				if (res>0 && packet[0] == START) {
					for (i=1; i<res; i += (packet[i+1]+2)) {
						//file size
						if (packet[i] == 0) {
							file_size=realloc(file_size, packet[i+1]+1);
							memcpy(file_size, &packet[i+2], packet[i+1]);
						}
						//file name
						else if (packet[i] == 1) {
							file_name=realloc(file_name, packet[i+1]+1);
							memcpy(file_name, &packet[i+2], packet[i+1]);
						}
					}
					state++;
				}

				else
					continue;
			}

			else if (state == DATA_PACKET) {
				res = llread(fd,packet);

				if (res>0 && packet[0] == DATA) {
					file = realloc(file, ((packet[2] << 8) + packet[3]) + file_len + 1);
					memcpy(file+file_len, &packet[4], (packet[2] << 8) + packet[3]);
					file_len += (packet[2] << 8) + packet[3];

					printf("\rReceiving file... ");
					fflush(stdout);
					printf("[%.0f%%]", (((float)file_len)/atof(file_size))*100);
					fflush(stdout);
				}

				else if (packet[0] == END)
					state++;
			}

			else if (state == END_PACKET)
				state++;

		 	else if (state == STORE_FILE) {

				struct stat st = {0};

				if (stat("files", &st) == -1) {
				    mkdir("files", 0777);
				}

				sprintf(store_name, "files/%s", file_name);
				FILE * f = fopen(store_name, "wb");
				fwrite(file, sizeof(char), file_len, f);
				fclose(f);

				printf("\nThe file %s was successfully stored at ./files/%s \n", file_name, file_name);

				statistics();

				return;
			}
		}
}

/**
 * Função para enviar o ficheiro, dividindo-o em vários pacotes.
 * @param fd - descritor da porta.
 * @param fileName - nome do ficheiro.
 * @param fileSize - tamanho do ficheiro.
 * @param fileContent - conteúdo do ficheiro.
 */
void send_all_packets(int fd, char * fileName, int fileSize, char * fileContent) {
	int i = 0;

	start_control_packet(fd, fileName, fileSize+1);

	while (1) {
		if ((fileSize - MAXINFO*i) > MAXINFO) {
			i++;
			data_packet(fd, fileContent, MAXINFO);
			memmove(&fileContent[0], &fileContent[MAXINFO], fileSize - MAXINFO*i);
		}

		else if ((fileSize - MAXINFO*i) > 0) {
			data_packet(fd, fileContent, fileSize - MAXINFO*i);
			break;
		}

		else
			break;
		
		printf("\rSending file... ");
		fflush(stdout);
		printf("[%.0f%%]", (((float)i)/(float)(fileSize))*1000);
		fflush(stdout);
	}

	end_control_packet(fd, fileName, fileSize);
}

int main(int argc, char** argv)
{

	int fd, fileFD, fileSize=0;
	char fileName[MAX_SIZE];
	char * buf = malloc(1);

	BAUDRATE = 0;
	NS = 0;
	NUMPACKETS = 0;

	check_args(argc,argv);

	print_header();

	ask_parameters();


	if(STATUS == 0) {
		print_receiver();
		fd=llopen(argv[1]);
		sm_receiver(fd);
		llclose(fd);
	}

	else if (STATUS == 1) {
		print_transmitter();

		printf("\nFile name? ");
		scanf("%s", fileName);

		fileFD = open(fileName, O_RDONLY | O_NOCTTY );

		while (read(fileFD, &buf[fileSize], 1)) {
			fileSize++;
			buf = realloc(buf, fileSize+1);

		}

		fd=llopen(argv[1]);
		printf("Fetching penguins from Antartica...\n");

		send_all_packets(fd, fileName, fileSize, buf);

		printf("File successfully delivered!\n");

		statistics();

		llclose(fd);
	}

	print_footer();
}

