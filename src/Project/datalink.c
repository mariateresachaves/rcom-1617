#include "datalink.h"

volatile int STOP = FALSE;
char buf[MAX_SIZE];
//int tries=0;
//int flag_timer=0;

char set[5] = {FLAG, A, C_TRANSMITTER, BCC1, FLAG};
char ua[5] = {FLAG, A, C_RECEIVER, BCC1, FLAG};

/*void timer_handler() {
	printf("alarme # %d\n", tries);
	flag_timer = 1;
	tries++;
}*/

void printFlags(char * flags) {
	int n = 0;
	while(n<5) {
		printf("%x ", flags[n]);
		n++;
	}
	printf("\n");
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
   
    printFlags(ua);
   
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

char * llread() {

	int res;

	while (STOP==FALSE) {       /* loop for input */
		res = read(al.fd, buf, newtio.c_cc[VMIN]); /* returns after 5 chars have been input */
		
		buf[res]=0;  /* so we can printf... */

		printf(":%s:%d\n", buf, res);

		if (buf[res-1] == STOP_BYTE)
			STOP = TRUE;

		res=write(al.fd, buf, res);        
	}

	sleep(1);
	
	return buf;

}

int llwrite(char * buf) {

	int i=0, res;
	char aux[MAX_SIZE];

	// String size
    int num = strlen(buf);
    
    while(i < num+1) {

		int send_bytes = 0;

		memset(aux, 0, newtio.c_cc[VMIN]);
		
		/*while(tries <= ll.numTransmissions){
		   if(flag_timer){
			  alarm(newtio.c_cc[VTIME]);                 // activa alarme de 3s
			  flag_timer = 0;
		   } //else
 				//res = read(fd, buf, newtio.c_cc[VMIN]);   	/* returns after 5 chars have been input */

		//}

		if(newtio.c_cc[VMIN] > strlen(buf)-i) {
			memcpy(aux, buf + i, strlen(buf)-i);
			send_bytes = strlen(buf)-i;
			
			aux[send_bytes] = '\0';
			send_bytes++;
		}
		else {
			memcpy(aux, buf + i, newtio.c_cc[VMIN]);
			send_bytes = newtio.c_cc[VMIN];
		}

		res = write(al.fd, aux, send_bytes);

		printf("Sent: %s send_bytes: %d\n", aux, send_bytes);

		sleep(1);

		i+=newtio.c_cc[VMIN];

	}

	return res;

}

