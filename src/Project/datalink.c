#include "datalink.h"

volatile int STOP = FALSE;
char buf[MAX_SIZE];
int times = 0, flag_timer = 0;

int SUCCESS_UA = 0;

char SET[5] = {FLAG, A, C_SET, A^C_SET, FLAG};
char DISC[5] = {FLAG, A, C_DISC, 0, FLAG};
char UA[5] = {FLAG, A, C_UA, 0, FLAG};
char RR[5] = {FLAG, A, C_RR, 0, FLAG};
char REJ[5] = {FLAG, A, C_REJ, 0, FLAG};
char RR_ACK[5] = {FLAG, A, C_RR_ACK, 0, FLAG};
char REJ_ACK[5] = {FLAG, A, C_REJ_ACK, 0, FLAG};
char INFO_0[5] = {FLAG, A, C_INFO_0, 0, FLAG};
char INFO_1[5] = {FLAG, A, C_INFO_1, 0, FLAG};

void timer_handler() {
	printf("<--- Alarm number %d --->\n", times);
	times++;
	flag_timer = 1;
}

void printFlags(char * flags, char * type) {
	int n = 0;
	printf("%s: ", type);
	while(n<5) {
		printf("%x ", flags[n]);
		n++;
	}
}

int open_port(char * port) {

	(void) signal(SIGALRM, timer_handler);

	al.fd = open(port, O_RDWR | O_NOCTTY );
	if (al.fd <0) {perror(port); exit(-1); }

	if ( tcgetattr(al.fd,&oldtio) == -1) { /* save current port settings */
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&newtio, sizeof(newtio));

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

	return al.fd;

}

int llopen() {

	printf("\nOpening port... \n");

	al.fd = open_port(ll.port);

  return al.fd;

}

int llclose() {

	printf("\nClosing port... \n");

	if ( tcsetattr(al.fd,TCSANOW,&oldtio) == -1) {
		perror("tcsetattr");
		exit(-1);
  }

  close(al.fd);

}

char * llread(int type) {

	char aux[MAX_SIZE];

	memset(aux, 0, MAX_SIZE);

	while (STOP==FALSE) {       /* loop for input */
		receive_message(aux);
	}

	//if(type == 1)
		//llwrite(aux);

	//stateMachine();

	return aux;
}

int llwrite(char * buf, int buf_size) {

	int res;

	res = write(al.fd, buf, buf_size);

	printf("RES: %d", res);

	ll.sequenceNumber = (ll.sequenceNumber + 1) % 256;

	return res;

	/*int i=0, res;
	char aux[MAX_SIZE];

	// String size
	int num = strlen(buf);

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

		res = send_message(aux, send_bytes);

		printf("Sent: %s:%d\n", aux, send_bytes);

		sleep(1);

		i+=MAX_INFO;

	}

	return res;*/
}

void sendFlags(char * flagToSend, char * type) {
	write(al.fd, flagToSend, 5);
	printf("<--- A enviar Flag ");
	printFlags(flagToSend, type);
	printf(" --->\n");

}

int send_message(char * message, int send_bytes) {

    int res=0;


    /*printf("\nTrying to send SET.....\n\n");

    sendFlags(SET, "SET");*/

    //res = read(al.fd, buf, MAX_INFO);

    //printFlags(buf, "UA");

	//printf("\nIa enviar uma mensagem, mas não dá jeito agora.....\n\n");
    res = write(al.fd, message, send_bytes);

    return res;

}

void receive_message(char * aux) {

    int res=0;
		while(!res){
			res = read(al.fd, buf, MAX_INFO);
		}

		buf[res] = 0;

    printf("Received: %s:%d\n", buf, res);

    if(buf[res-1] == STOP_BYTE)
		STOP = TRUE;

    strcat(aux, buf);
}

int bcc_generator(char * buf, int size){
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
		return 0;
	}

	return buf[3];
}

int BCC_check(char *buff, int buf_size){
	char bcc=0x00;
	int i=1;

	// BCC1
	while(i<4)
		bcc^=buff[i++];

	if(bcc!=0x00) return -1; // fail BCC1

	bcc = 0x00;

	// BCC2
	while(i<buf_size)
		bcc^=buff[i++];

	if(bcc!=0x00) return -2; // fail BCC2

	return 0; // Success
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

void destuffing(char * buf, int * buf_size) {

	int i = 1;

	while(i < (*buf_size)) {

		if (buf[i] == 0x7D && buf[i+1] == 0x5E) {
			memmove(&buf[i], &buf[i+1], (*buf_size)+1-i);
			buf[i] = FLAG;
			(*buf_size)--;
		}

		else if (buf[i] == 0x7D && buf[i+1] == 0x5D) {
			memmove(&buf[i], &buf[i+1], (*buf_size)+1-i);
			buf[i] = FLAG_ESC;
			(*buf_size);
		}

		i++;
	}

}

int stateMachine() {

	int res=0;
	int currentState = 0;

	while (1){

	while(!res) {
		res = read(al.fd, buf, MAX_INFO);
	}

	switch (currentState) {
		case 0: // FLAG
			if (buf[currentState] != FLAG) {

				printf("[ERRO] Não recebi flag FLAG correta!\n");
				break;
			} else
				currentState++;
		case 1: // A
			if (buf[currentState] != A){
				printf("[ERRO] Não recebi flag A correta!\n");
				break;
			} else
				currentState++;
		case 2: // CONTROLO
			switch (buf[currentState]) {
				case C_INFO_0:
					//destuff de pacote de dados
					//processar pacote de dados
					//enviar receiver ready (RR)
					break;
				case C_INFO_1:
					//destuff de pacote de dados
					//processar pacote de dados
					//enviar receiver ready (RR_ACK)
					break;
				case C_SET:
					printf("<--- Recebi um SET --->\n");
					//envia UA
					sendFlags(UA,"UA");
					break;
				case C_DISC:
					//verifica se é recetor ou emissor
					//se for recetor, envia novo DISC
					//se for emissor, envia UA
					break;
				case C_UA:
					printf("<--- Recebi um UA --->\n");
					//verifica se é recetor ou emissor
					if (al.status == TRANSMITTER) {
						//se for emissor, envia INFO_0
						//printf("------A enviar INFO_0----------\n");
						sendFlags(INFO_0,"INFO_0");
					} else  if (al.status == RECEIVER) {
						//se for recetor, termina leitura
						printf("------A terminar leitura----------\n");
					}

					break;
				case C_RR:
					//enviar trama com informação (INFO_1)
					break;
				case C_RR_ACK:
					//verifica se existem mais tramas
					//caso exista, repete ciclo
					//caso não exista, envia disconnect (DISC)
					break;
				case C_REJ:
					break;
				case C_REJ_ACK:
					break;
				default:
					printf("[ERRO] Flag do campo de controlo desconhecida - %x\n",	buf[currentState]);
					return NULL;
			}
			currentState++;
		case 3: // BCC
			bcc_generator(buf,res);
			switch (buf[currentState]) {
				case A ^ C_UA:
					break;
				case A ^ C_SET:
					break;
				default:
					if (buf[currentState-1] == C_INFO_0)
						printf("Encher chouriços");
						//enviar C_REJ
					else if (buf[currentState-1] == C_INFO_1)
						printf("Encher chouriços");
						//enviar C_REJ_ACK
					printf("[ERRO] Flag BCC errada!");
					return NULL;
			}
			currentState++;
		case 4:
			if (buf[currentState != FLAG]) {
				printf("[ERRO] Não recebi flag final FLAG correta!\n");
				break;
			} else
				//TODO
				//enviar dados se trama de informação
				break;
		}
	}
}
