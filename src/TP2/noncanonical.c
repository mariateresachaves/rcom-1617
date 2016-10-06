/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define STOP_BYTE '\0'

void printFlags(char * flags);

volatile int STOP=FALSE;

char flag = 0x7E;
char a = 0x03;
char c = 0x06;
char bcc1 = 0x00;

int main(int argc, char** argv)
{
    int fd, res;
    struct termios oldtio,newtio;
    char buf[255], aux[255];
	char ua[5] = {flag,a,c,bcc1,flag};


    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
		perror("tcgetattr");
		exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

	while (STOP==FALSE) {       /* loop for input */
		res = read(fd,buf, newtio.c_cc[VMIN]); /* returns after 5 chars have been input */
		
		buf[res]=0;  /* so we can printf... */
		
		//testing SET
		if (buf[0] == 0x7e){
			printf("SET: ");
			printFlags(buf);
		} else {
			printf(":%s:%d\n", buf, res);

			if (buf[res-1] == STOP_BYTE)
				STOP = TRUE;
			
			//res=write(fd, ua, 5);

			//Testing UA
			printf("UA: ");
			printFlags(ua);

			res=write(fd, buf, res);
		}  
	}

//llopen()

	/*int estado=1;

	while(1){
		switch(estado){
			case(1):
				if(res!=0 || buf[0]==0x7E){
					estado=2;
					res--;	
				}
				break;
			case(2):
				if(res!=0 && buf[1]!=0x7E){
					estado=3;
					res--;
				}
			case(3):
				if

		}
		
		
	}*/

	
	sleep(1);

  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */




    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}

void printFlags(char * flags){
	int n=0;
	while (n<5){
		printf("%x ", flags[n]);
		n++;
	}
	printf("\n");
}
