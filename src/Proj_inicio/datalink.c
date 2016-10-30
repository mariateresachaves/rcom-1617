#include "datalink.h"

char SET[5] = {FLAG, A_TRANSMITTER, C_SET, A_TRANSMITTER^C_SET, FLAG};
char DISC[5] = {FLAG, A_TRANSMITTER, C_DISC, A_TRANSMITTER^C_DISC, FLAG};
char UA[5] = {FLAG, A_RECEIVER, C_UA, A_RECEIVER^C_UA, FLAG};

char RR[5] = {FLAG, A_RECEIVER, C_RR, A_RECEIVER^C_RR, FLAG};
char RR_ACK[5] = {FLAG, A_RECEIVER, C_RR_ACK, A_RECEIVER^C_RR_ACK, FLAG};

char REJ[5] = {FLAG, A_RECEIVER, C_REJ, A_RECEIVER^C_REJ, FLAG};
char REJ_ACK[5] = {FLAG, A_RECEIVER, C_REJ_ACK, A_RECEIVER^C_REJ_ACK, FLAG};

char INFO_0[5] = {FLAG, A_TRANSMITTER, C_INFO_0, A_TRANSMITTER^C_INFO_0, FLAG};
char INFO_1[5] = {FLAG, A_TRANSMITTER, C_INFO_1, A_TRANSMITTER^C_INFO_1, FLAG};

int state = 0;

void printFlags(char * flags, char * type) {
	int n = 0;
	printf("%s: ", type);
	while(n<5) {
		printf("%x ", flags[n]);
		n++;
	}
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

void llopen() {
	printf("\nOpening port... \n");

	al.fd = open(ll.port, O_RDWR | O_NOCTTY );

	if (al.fd < 0) { perror(ll.port); exit(-1); }

	if ( tcgetattr(al.fd, &oldtio) == -1) { /* save current port settings */
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&newtio, sizeof(newtio));

	def_c_cflag();

	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = OPOST;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */

	tcflush(al.fd, TCIFLUSH);

	if ( tcsetattr(al.fd,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}
}

void llclose() {
	printf("\nClosing port... \n");

	if ( tcsetattr(al.fd,TCSANOW,&oldtio) == -1) {
		perror("tcsetattr");
		exit(-1);
  }

  close(al.fd);
}

void sm_command(char * type) {
	if (!strcmp(SET, type)) {
		state = 1;
		printf("A enviar um ");
		printFlags(type, "SET");
		// TODO: write the SET message
	}

	else if (!strcmp(DISC, type)) {
		state = 2;
		printf("A enviar um DISC\n");
		// TODO: write the DISC message
	}

	else if (!strcmp(INFO_0, type)) {
		state = 3;
		printf("A enviar um INFO_0\n");
		// TODO: write the INFO_0 message
	}

	else if (!strcmp(INFO_1, type)) {
		state = 3;
		printf("A enviar um INFO_1\n");
		// TODO: write the INFO_1 message
	}
}

// State machine to write a message
void state_machine(char side, char * type) {
	while (1) {
		switch (state) {
			case 0: // Send a command
				sm_command(type);
				break;

			case 1: // Receive a UA
				state = 4;
				break;

			case 2: // Receive a DISC
				state = 4;
				break;

			case 3: // Receive RR
				state = 4;
				break;

			case 4: // Exit
				break;
		}
		if (state == 4) break;
	}
}

void llwrite(char * packet, int packet_size) {

	state_machine(A_TRANSMITTER, SET);

}
