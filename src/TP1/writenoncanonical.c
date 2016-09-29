/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd, res; // c
    struct termios oldtio,newtio;
    char buf[255], aux[255];
    int i=0; //sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
		perror("tcgetattr");
		exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = OPOST;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */


    tcflush(fd, TCIFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
    }

    /*
    Terminal settings done, now handle the input
    */

    // read line from stdin
    //fgets(buf, 255, stdin);

	printf("String to send: ");
    gets(buf);

    // String size
    int num = strlen(buf);
    
    while(i < num+1) {

		int send_bytes = 0;

		memset(aux, 0, newtio.c_cc[VMIN]);

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

		res = write(fd, aux, send_bytes);

		printf("Sent: %s send_bytes: %d\n", aux, send_bytes);

		sleep(1);

		i+=newtio.c_cc[VMIN];

	}

    fprintf(stdout, "Listening...\n");

	i = 0;
	memset(aux, 0, sizeof(aux));

    while (STOP==FALSE) {       /* loop for input */

		memset(buf, 0, sizeof(buf));

		res = read(fd, buf, newtio.c_cc[VMIN]);   	/* returns after 5 chars have been input */
		buf[res]=0;     							/* so we can printf... */
		printf(":%s:%d\n", buf, res);
		
		if (buf[res-1] == '\0')
			STOP=TRUE;

		memcpy(aux+i, buf, res);
		i+=res;

    }
    
	printf("The message \"%s\" have been delivered successfully.\n", aux);


    sleep(1);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
		perror("tcsetattr");
		exit(-1);
    }

    close(fd);
    return 0;
}
