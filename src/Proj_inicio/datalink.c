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

	if (al.status == TRANSMITTER)
		sm_write(A_TRANSMITTER, SET, "", 0);

	else if (al.status == RECEIVER) {
		// TODO: Se for o receiver recebe um SET e tenta enviar um UA
	}
}

void llclose() {

	if (al.status == TRANSMITTER)
		sm_write(A_TRANSMITTER, DISC, "", 0);

	else if (al.status == RECEIVER)
		// TODO: Se for o receiver recebe um DISC e tenta enviar um DISC

	printf("\nClosing port... \n");

	if ( tcsetattr(al.fd,TCSANOW,&oldtio) == -1) {
		perror("tcsetattr");
		exit(-1);
  }

  close(al.fd);
}

void bcc_generator(char * buf, int size) {
	int i=4;

	//bcc1
	buf[3] = buf[1] ^ buf[2];

	//bcc2
	if (size > 4){
		buf[size-1] = 0;

		while (i<size-1){
			buf[size-1] ^= buf[i];
			i++;
		}
	}
}

void stuffing(char * buf, int * buf_size) {
	int i = 1;

	while(i < (*buf_size)) {
		if (buf[i] == FLAG) {
			memmove(&buf[i+2], &buf[i+1], (*buf_size)+1-i);
			buf[i] = 0x7D;
			buf[i+1] = 0x5E;
			(*buf_size)++;
		}

		else if  (buf[i] == FLAG_ESC) {
			memmove(&buf[i+2], &buf[i+1], (*buf_size)+1-i);
			buf[i] = 0x7D;
			buf[i+1] = 0x5D;
			(*buf_size)++;
		}

		i++;
	}
}

int write_packet(char side, char * type, char * data, int size) {
	char packet[MAX_SIZE];
	int i=0, res;

	packet[0] = FLAG;
	packet[1] = side;
	packet[2] = *type;

	if(size > 4)
		for (i = 4; i < size-1; i++)
			packet[i] = data[i-4];

	packet[size] = FLAG;

	bcc_generator(packet, size);
	stuffing(packet, &size);

	res = write(al.fd, packet, size+1);

	return res;
}

int sm_command(char side, char * type, char * data, char size) {
	int res;

	if (!strcmp(SET, type)) {
		state = 1;

		res = write_packet(A_TRANSMITTER, SET, "", 4);

		if (res <= 0) {
			printf("Cannot send a SET at the moment!\n");
			return -1;
		}
	}

	else if (!strcmp(DISC, type)) {
		state = 2;

		res = write_packet(A_TRANSMITTER, DISC, "", 4);

		if (res <= 0) {
			printf("Cannot send a DISC at the moment!\n");
			return -1;
		}
	}

	else if (!strcmp(INFO_0, type)) {
		state = 3;

		res = write_packet(A_TRANSMITTER, INFO_0, data, size+5);

		if (res <= 0) {
			printf("Cannot send a INFO_0 at the moment!\n");
			return -1;
		}
	}

	else if (!strcmp(INFO_1, type)) {
		state = 3;

		res = write_packet(A_TRANSMITTER, INFO_1, data, size+5);

		if (res <= 0) {
			printf("Cannot send a INFO_1 at the moment!\n");
			return -1;
		}
	}
}

// State machine to write a message
int sm_write(char side, char * type, char * data, char size) {
	int e;

	while (1) {
		switch (state) {
			case 0: // Send a command
				e = sm_command(side, type, data, size);
				if (e == -1) return e;

				state++;
				continue;
				break;

			case 1: // Receive a UA
				state = 4;

				//receive_ua(side, type, data, );
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
	while (ll.numPackets > 0) {
		printf("A ENVIAR ------------ %s\n", packet);
		if (ll.numPackets % 2)
			sm_write(A_TRANSMITTER, INFO_1, packet, packet_size);

		else
			sm_write(A_TRANSMITTER, INFO_0, packet, packet_size);

		ll.numPackets--;
	}
}

int llread(int fd, char *buf){
	int res=0;
	while(!res){
		res = read(fd, buf, MAX_INFO);
	}

	buf[res] = 0;

	printf("Received: %s:%d\n", buf, res);

	//if(buf[res-1] == STOP_BYTE)
	//STOP = TRUE;
}
