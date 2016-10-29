#include "datalink.h"

volatile int STOP = FALSE;
char buf[MAX_SIZE];
int times = 0, flag_timer = 0;

int SUCCESS_UA = 0;

char SET[5] = {FLAG,A,C_SET,0,FLAG};
char DISC[5] = {FLAG,A,C_DISC,0,FLAG};
char UA[5] = {FLAG,A,C_UA,0,FLAG};
char RR[5] = {FLAG,A,C_RR,0,FLAG};
char REJ[5] = {FLAG,A,C_REJ,0,FLAG};
char RR_ACK[5] = {FLAG,A,C_RR_ACK,0,FLAG};
char REJ_ACK[5] = {FLAG,A,C_REJ_ACK,0,FLAG};
char INFO_0[5] = {FLAG,A,C_INFO_0,0,FLAG};
char INFO_1[5] = {FLAG,A,C_INFO_1,0,FLAG};

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
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = OPOST;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 3;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */


    ll.timeout=3;
    ll.numTransmissions=3;

    tcflush(al.fd, TCIFLUSH);

    if ( tcsetattr(al.fd,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
    }

    return al.fd;

}

int llopen() {

	printf("Opening port... \n");

	al.fd = open_port(ll.port);

    return al.fd;

}

int llclose() {

	printf("Closing port... \n");

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

	if(type == 1)
		llwrite(aux);

	//stateMachine();

	return aux;
}

int llwrite(char * buf) {

	int i=0, res;
	char aux[MAX_SIZE];

	// String size
    int num = strlen(buf);

	// Envia a mensagem de newtio.c_cc[VMIN] em newtio.c_cc[VMIN] bytes
    while(i < num+1) {

		int send_bytes = 0;

		memset(aux, 0, newtio.c_cc[VMIN]);

		if(newtio.c_cc[VMIN] > strlen(buf)-i) {
			memcpy(aux, buf + i, strlen(buf)-i);
			send_bytes = strlen(buf)-i;

			aux[send_bytes] = 0;
			send_bytes++;
		}
		else {
			memcpy(aux, buf + i, newtio.c_cc[VMIN]);
			send_bytes = newtio.c_cc[VMIN];

			aux[send_bytes] = 0;
		}

		res = send_message(aux, send_bytes);

		printf("Sent: %s:%d\n", aux, send_bytes);

		sleep(1);

		i+=newtio.c_cc[VMIN];

	}

	return res;
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

    //res = read(al.fd, buf, newtio.c_cc[VMIN]);

    //printFlags(buf, "UA");

	//printf("\nIa enviar uma mensagem, mas não dá jeito agora.....\n\n");
    res = write(al.fd, message, send_bytes);

    return res;

}

void receive_message(char * aux) {

    int res;

   	res = read(al.fd, buf, newtio.c_cc[VMIN]);

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

void stuffing(char * buf, int * buf_size) {

	int i = 1;

	while(i < (*buf_size)) {

		if (buf[i] == FLAG) {
			buf[i] = 0x7D;
			buf[i+1] = 0x5E;
			(*buf_size)++;
		}

		else if  (buf[i] == FLAG_ESC) {
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
			buf[i] = FLAG;
			(*buf_size)--;
		}

		else if (buf[i] == 0x7D && buf[i+1] == 0x5D) {
			buf[i] = FLAG_ESC;
			(*buf_size);
		}

		i++;
	}

}

int stateMachine() {

	int res;
	int currentState = 0;

	while (1){

	res = read(al.fd, buf, newtio.c_cc[VMIN]);

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
