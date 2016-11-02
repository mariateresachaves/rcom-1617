#include "datalink.h"

int count_msg_dropped = 0;
int duplicate_found = 0;
int n_retransmissions = 0;
int n_timeouts = 0;

int count = 0;
int flagTimer = 0;

char * buf_duplicate_aux;
int size_duplicate_aux=0;

void alarm_handler() {
	flagTimer = 1;
}

void def_c_cflag() {
	if (BAUDRATE == 300)
		newtio.c_cflag = B300 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 1200)
		newtio.c_cflag = B1200 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 2400)
		newtio.c_cflag = B2400 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 4800)
		newtio.c_cflag = B4800 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 9600)
		newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 19200)
		newtio.c_cflag = B19200 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 38400)
		newtio.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 57600)
		newtio.c_cflag = B57600 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 115200)
		newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	else if (BAUDRATE == 230400)
		newtio.c_cflag = B230400 | CS8 | CLOCAL | CREAD;
}

int open_port(char * port) {
	int fd, c, res, i;

	(void) signal(SIGALRM, alarm_handler);

  /*
    Open serial port device for reading and writing and not as flagCling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
    fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(port); exit(-1); }
    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      return -1;
    }
    bzero(&newtio, sizeof(newtio));

    def_c_cflag();

    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      return -1;
    }

    return fd;
}

int close_port(int fd) {
	sleep(1);
  tcsetattr(fd, TCSANOW, &oldtio);

  if (close(fd) < 0)
  	return -1;

  return 0;
}

int validate_bcc(char * buf, int size) {
	char bcc=0x00;
	int i=4;

	bcc = buf[2] ^ buf[3];

	if(bcc != 0x00)
		return 0;

	while(i < size)
		bcc^=buf[i++];

	if(bcc != 0x00)
		return 1;

	return 2;
}

char generate_bcc(char * buf, int size){
 	int i=4;
 	buf[3]=0x00;

	buf[3]^=buf[2];

	if (size > 4) {
		buf[size-1] = 0x00;

    while(i < size-1)
    	buf[size-1] ^= buf[i++];

    return 0;
	}

	return buf[3];
}

void stuffing(char * buf, int * size){
	int i=1;

	while (i < (*size)) {
		if (buf[i] == FLAG){
			memmove(&buf[i+2], &buf[i+1], (*size)-(i-1));
			buf[i] = FLAG_ESC;
			buf[i+1]=0x5E;
			(*size)++;
		}

		else if (buf[i] == FLAG_ESC) {
			memmove(&buf[i+2], &buf[i+1], (*size)-(i-1));
			buf[i] = FLAG_ESC;
			buf[i+1] = 0x5D;
			(*size)++;
		}
		i++;
	}
}

void destuffing(char * buf, int * size){
	int i=1;

	while (i < (*size)) {
		if (buf[i] == FLAG_ESC && buf[i+1] == 0x5D) {
			memmove(&buf[i], &buf[i+1], (*size)-(i-1));
			buf[i] = FLAG_ESC;
			(*size)--;
		}

		else if(buf[i] == FLAG_ESC && buf[i+1] == 0x5E){
			memmove(&buf[i], &buf[i+1], (*size)-(i-1));
			buf[i] = FLAG;
			(*size)--;
		}
		i++;
	}
}

int send_flags(int fd, char type, char flagA, char * data, int size){
	char buf[MAX_FRAME_SIZE];
	int size_i=size;
	int i=4;

	buf[0] = FLAG;
	buf[1] = flagA;
	buf[2] = type;

	if (size > 4) {
	    while (i < size_i-1) {
			  buf[i] = data[i-4];
				i++;
	    }
	}

	buf[size] = FLAG;

	generate_bcc(buf, size_i);
	stuffing(buf, &size_i);

	return write(fd, buf, size_i+1);
}

int check_duplicate(char * buf, int size) {
	int i=0, check=0;

	if (size == size_duplicate_aux) {
		check=1;

		while(i <= size) {
			if (buf_duplicate_aux[i] != buf[i]) {
				check=0;
				break;
			}

			i++;
		}
	}

	if (check) {
		return 1;
	}

	else {
		size_duplicate_aux = size;
		buf_duplicate_aux = (char*) realloc (buf_duplicate_aux, (size+1) * sizeof(char));

		while(i <= size)
			buf_duplicate_aux[i] = buf[i++];

		return 0;
	}
}

int get_message(int fd, char * type, char * flagA, char * flagC, char * data, int mode){
	int size=0;
	char buf[MAX_FRAME_SIZE];
	int res = 0, bcc;
	int state=0;

	while (1) {
		if (state == FLAG_INICIAL) {
			if (flagTimer == 1) {
				alarm(0);	//turn off alarm
				return 0;
			}

			if (!read(fd,&buf[size],1)) {
				continue;
			}

			if(buf[size] == FLAG )
				state++;
		}
		else if (state == FLAG_FINAL) {
			if (!read(fd, &buf[++size], 1)) {
				size--;
				continue;
			}

			if (buf[size] == FLAG && size < 4) { //protection for packets who start with 2 falgs
					size=0;
					continue;
			}

			else if (buf[size] == FLAG) {
				if ((buf[2] == C_INFO_0 || buf[2] == C_INFO_1) && size < 7) // check if frame is type info
					continue;

				destuffing(buf, &size);
				state++;
			}
		}

		else if (state == BCC_DECODE) {
			res=validate_bcc( buf, size);

			if (res == NO_ERROR)
				state++;

			else if (res == BCC1_FAIL) {
				state=0;
				size=0;
				count_msg_dropped++;
				printf("Not a valid BCC1.\n");
				fflush(stdout);
			}

			else if (res == BCC2_FAIL) {
				state=0;
				size=0;
				count_msg_dropped++;

				if(buf[2] == C_INFO_0) {
					printf("Not a valid BCC2.\n");

					if (send_flags(fd, C_REJ_ACK, A_TRANSMITTER, NULL, 4) <= 0) {
						printf("Fail to send REJ_ACK\n");
						fflush(stdout);
					}

					else
						printf("REJ_ACK successfully sent.\n");
				}

				else if (buf[2] == C_INFO_1) {
					printf("Not a valid BCC2.\n");

					if (send_flags(fd, C_REJ, A_TRANSMITTER, NULL, 4) <= 0) {
						printf("Fail to send REJ.\n");
						fflush(stdout);
					}

					else
						printf("REJ successfully sent.\n");
				}
			}
		}

		else if (state == SEND_UA_BACK) {
			switch ((unsigned char) buf[2]) {
				case C_SET:
					strcpy(type, "SET");
					*flagA=buf[1];
					*flagC=buf[2];
					CONNECTION=1;

					if(mode == RECEIVER) {
						if (send_flags(fd, C_UA, A_TRANSMITTER, NULL, 4)  <=0)
							printf("Fail to sent UA\n");
					}
					break;
				case C_DISC:
					if(mode==RECEIVER){
						if (send_flags(fd, C_DISC, A_TRANSMITTER, NULL, 4) <=0)
							printf("Fail to send DISC.\n");

						while (count < NUMTRANSMISSIONS) {
							alarm(TIMEOUT);	//turn on alarm

							res = get_message(fd, type, flagA, flagC, data, TRANSMITTER);
							flagTimer=0;

							alarm(0); 			//turn off alarm

							if (res > 0) {
								if (strcmp(type, "UA") == 0)
									break;
							}

							count++;
							printf("Fail to receive DISC. Try = %d\n", count);
						}

						if (count == NUMTRANSMISSIONS)
							printf("Fail to receive UA.\n");
					}

					CONNECTION=0;
					count=0;

					strcpy(type, "DISC");
					*flagA=buf[1];
					*flagC=buf[2];

					break;
				case C_UA:
					strcpy(type, "UA");
					*flagA=buf[1];
					*flagC=buf[2];

					break;
				case C_INFO_0:
					strcpy(type, "C_INFO_0");
					*flagA=buf[1];
					*flagC=buf[2];
					memcpy(data, buf+4, size-5);

					if (mode == RECEIVER) {
						if (send_flags(fd, C_RR_ACK, A_TRANSMITTER, NULL, 4) <= 0)
							printf("Fail to send RR_ACK.\n");
					}

					break;
				case C_INFO_1:
					strcpy(type, "I2");
					*flagA=buf[1];
					*flagC=buf[2];
					memcpy(data, buf+4, size-5);

					if (mode == RECEIVER) {
						if (send_flags(fd, C_RR, A_TRANSMITTER, NULL, 4) <=0)
							printf("Fail to send RR\n");
					}

					break;
				default:
					strcpy(type, "UNKOWN");
					*flagA=buf[1];
					*flagC=buf[2];

					if(mode == RECEIVER) {
						if ((send_flags(fd, C_UA, A_TRANSMITTER, NULL, 4))  <=0)
							printf("Fail to send UA.\n");
					}

					break;
			}
			//send unnumbered acknowledgment
			if (mode == RECEIVER) {
				if (check_duplicate(buf, size)) {
					printf("Duplicate %s received.\n", type);

					duplicate_found++;
					size=0;
					state = FLAG_INICIAL;
				}

				else
					state=4;
			}

			else
				state=4;
		}

		else if (state == 4)
			break;
	}

	return size+1;
 }

int write_message(int fd, char * type, char flagA, char flagC, char * data, int size) {
	int res = 0, bcc;
	int state=0;
	char type_2[10]={0}, flagA_2=0, flagC_2=0;
	char * data_2 = malloc(sizeof(char));
	int count=0;

	int terminateStateMachineFlag = 0;

	while (1) {
		switch (state) {
			case 0:
				switch ((int) (flagC)) {
					case C_SET:
						if (send_flags(fd, C_SET, A_TRANSMITTER, NULL, 4) <=0 )
							return -1;
						state=1;
						continue;
					case C_DISC:
						if (send_flags(fd, C_DISC, A_TRANSMITTER, NULL, 4) <= 0)
							return -1;
						state=2;
						continue;
					case C_INFO_0:
						if (send_flags(fd, C_INFO_0, A_TRANSMITTER, data, size+5) <= 0)
							return -1;
						state=3;
						continue;
					case C_INFO_1:
						if (send_flags(fd, C_INFO_1, A_TRANSMITTER, data, size+5) <= 0)
							return -1;
						state=3;
						continue;
				}
				break;
			case 1:
				flagTimer=0;
				alarm(TIMEOUT);	//turn on alarm

				get_message(fd, type_2, &flagA_2,  &flagC_2, data_2, TRANSMITTER);

				flagTimer=0;	//turn off alarm
				alarm(0);

				if (!strcmp(type_2, "UA"))
					state=4;

				else if (count < NUMTRANSMISSIONS) {
					printf("Failed to receive UA. Try = %d\n", count+1);
					state=0;
					n_timeouts++;
					n_retransmissions++;
					count++;
					continue;
				}

				else {
					printf("Failed to receive UA.\n");
					state=4;
					return -1;
				}
				break;
			case 2:
				flagTimer=0;
				alarm(TIMEOUT);	//turn on alarm

				res = get_message(fd, type_2, &flagA_2,  &flagC_2, NULL, TRANSMITTER);

				flagTimer=0;	//turn off alarm
				alarm(0);

				if (res && !strcmp(type_2, "DISC")) {
					send_flags(fd, C_UA, A_RECEIVER, NULL, 4);
					state=4;
				}

				else if (count < NUMTRANSMISSIONS) {
					printf("Failed to receive DISC. Try = %d\n", count+1);
					state=0;
					n_timeouts++;
					n_retransmissions++;
					count++;
					continue;
				}

				else {
					printf("Failed to receive DISC.\n");
					state=4;
					return -1;
				}
				break;
			case 3:
				flagTimer=0;
				alarm(TIMEOUT);	//turn on alarm

				res = get_message(fd, type_2, &flagA_2,  &flagC_2, NULL, TRANSMITTER);

				flagTimer=0;	//turn off alarm
				alarm(0);

			if (res > 0) {
				  switch ((unsigned char) (flagC_2)) {
						case C_RR:
							if (flagC == C_INFO_1)
							  state = 4;

							else if (count < NUMTRANSMISSIONS) {
								printf("Failed to receive INFO_1. Try = %d\n", count+1);
								state = 0;
								n_timeouts++;
								n_retransmissions++;
								count++;
							}

							else {
							  	printf("Failed to receive INFO_1.\n");
							  	state = 4;
							  	return -1;
							}
						  break;
						case C_RR_ACK:
							if (flagC == C_INFO_0)
								state = 4;

							else if (count < NUMTRANSMISSIONS) {
								printf("Failed to receive INFO_0. Try = %d\n", count+1);
								state = 0;
								count++;
								n_timeouts++;
								n_retransmissions++;
							}

							else {
							  	printf("Failed to receive INFO_0.\n");
							  	state = 4;
							  	return -1;
							}
						  break;
						case C_REJ:
						case C_REJ_ACK:
							if (count < NUMTRANSMISSIONS) {
								printf("Failed. Received REJ. Try = %d\n", count+1);
								state = 0;
								n_retransmissions++;
								count++;
							}

							else {
								  printf("Failed. Received REJ.\n");
								  state = 4;
								  return -1;
							}
						  break;
					}
				}

				else {
					if (count < NUMTRANSMISSIONS) {
						printf("Failed. INFO not received. Try = %d\n", count+1);
						state = 0;
						n_timeouts++;
						n_retransmissions++;
						count++;
					}

					else {
						printf("Failed. INFO not received.\n");
						n_timeouts++;
						state = 4;
						return -1;
					}
				}
				break;
			case 4:
				if (flagC == C_INFO_0 || flagC == C_INFO_1 )
					return size;

				terminateStateMachineFlag = 1;

				break;
		}

		if (terminateStateMachineFlag)
			break;
	}

	return 1;
}


int llopen(char * port){
	int fd;
	char type[10], flagA, flagC;

	fd = open_port(port);

	if (fd == -1)
		return -1;

	if (STATUS == 1) {
		if (write_message(fd, "SET", A_TRANSMITTER, C_SET, NULL, TRANSMITTER)) {
			printf("Behold! The port has opened! Creatures will come forth!\n");
			return fd;
		}

		else
			return -1;
	}

	else {
		if (get_message(fd, type, &flagA, &flagC, NULL, RECEIVER) != -1) {
			if (flagC == C_SET) {
				printf("Behold! The port has opened! Creatures will come forth!\n");
				return fd;
			}

			else
				return -1;
		}

		else
			return -1;
	}
}

int llwrite(int fd, char * buffer, int size) {
	int res=0;
	char type[10], flagA, flagC, *data;

	res = write_message(fd, type, A_TRANSMITTER, NUMPACKETS%2 ? C_INFO_1 : C_INFO_0, buffer, size);

	if(res>0) {
		NUMPACKETS++;
		return res;
	}

	else
		return -1;
}

int llread(int fd, char * buf) {
	char type[10], flagC=2, flagA;
	int nrecebidas;

	while (flagC != C_INFO_0 && flagC != C_INFO_1) {
		if ((nrecebidas = get_message(fd, type, &flagA, &flagC, buf, RECEIVER))) {
			NUMPACKETS++;

			if (flagC==C_DISC)
				return -2;

			if (flagC==C_SET)
				continue;

			return nrecebidas-6;
		}

		else
			return -1;
	}
}

int llclose(int fd) {
	char type[10], flagA, flagC;

	if (STATUS == 1) {
		if (write_message(fd, "DISC", A_TRANSMITTER, C_DISC, NULL, TRANSMITTER))
			printf("The port is closing! Quick! Run for your life!\n");

		else
			return -1;
	}

	else {
		if (get_message(fd, type, &flagA, &flagC, NULL, RECEIVER)) {
			if (!strcmp(type, "DISC"))
				printf("The port is closing! Quick! Run for your life!\n");

			else
				return -1;
		}

		else
			return -1;
	}

	close_port(fd);

	return 1;
}

void statistics() {

	printf(" _____________________\n");
	printf("|      Statistics     |\n");
	printf(".-----------------.---.\n");
	printf("|Retransmissions  | %d |\n", n_retransmissions);
	printf("|Timeouts         | %d |\n", n_timeouts);
	printf("|REJs sent        | %d |\n", count_msg_dropped);
	printf("|Duplicates found | %d |\n", duplicate_found);
	printf("'-----------------'---'\n");
}
