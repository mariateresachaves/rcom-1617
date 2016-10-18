#include "datalink.h"

volatile int STOP = FALSE;
char buf[MAX_SIZE];
int times = 0, flag_timer = 0;

int SUCCESS_UA = 0;

char SET[5] = {FLAG,A,C_TRANSMITTER,BCC1,FLAG};
char UA[5] = {FLAG,A,C_RECEIVER,BCC2,FLAG}; 

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

char * llread() {

	int res;
	char aux[MAX_SIZE];

	memset(aux, 0, MAX_SIZE);

	while (STOP==FALSE) {       /* loop for input */
		
		
		if(SUCCESS_UA == 1) { // Receiver
		    receive_message(aux);
		    
		    SUCCESS_UA = 0;
		}
		else {
		    res = read(al.fd, buf, newtio.c_cc[VMIN]); /* returns after 5 chars have been input */
				
		    while (1) {

			    if (res == 0) // estado 1
				    continue;

			    if (buf[0] == FLAG) { //estado 2
				    if (buf[1] != A) //estado 3
					    continue;

				    if (buf[2] == C_RECEIVER) { //estado 4				
					    if (buf[3] == (A ^ C_RECEIVER)) { //estado 5
						    if (buf[4] == FLAG) {
						    
					           if (strcmp(buf, UA) == 0) {
					                printFlags(buf, "UA");
						            sendFlags(SET, "SET");
						            break;	
					            }
					            						    
						        // Waits for a set
		                        while(ll.numTransmissions != times) {
						        
						            alarm(3);
						            
						            res = read(al.fd, buf, newtio.c_cc[VMIN]);
						        
						            while(!flag_timer) {
						                
						                receive_message(buf); //transmitter
						                flag_timer = 0;
						                STOP = TRUE;
						                alarm(0);
						                
							        }
						        }
						
							    
						    }
					    }
				    } else if (buf[2] == C_TRANSMITTER) {
						if (buf[3] == (A ^ C_TRANSMITTER)) { //estado 5
							if (buf[4] == FLAG) {
								if (strcmp(buf,SET) == 0) {
											
											printf("Received: ");
									        printFlags(buf, "SET");
									        
									        // Waits for a set
									        while(ll.numTransmissions != times) {
									        
									            sendFlags(UA, "UA");
							
									            // turn alarm on
									            alarm(3);
											
		                                        res = read(al.fd, buf, newtio.c_cc[VMIN]);

		                                        if(res > 0) {
		                                        	
		                                            if (strcmp(buf, SET) == 0) {
		                                            
		                                                SUCCESS_UA = 1;
														printf("Received: ");
		                                                printFlags(buf, "SET");
														alarm(0);
		                                                break;
		                                                
		                                            }
		                                            
		                                        }
		                                        
		                                        // turn alarm off
		                                        //alarm(0);
									        }
									        
									        break;
									    
								}
							}
						}
					}
			    }
		    }
	   }
	}

	sleep(1);
	
	printf("Read: %s\n", aux);
	
	STOP = FALSE;

	return aux;

}

int llwrite(char * buf) {

	int i=0, res;
	char aux[MAX_SIZE];

	// String size
    int num = strlen(buf);
    
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
	printf("---Sending Flags---\n");
	printFlags(flagToSend, type);
	printf("-------------------\n");

}

int send_message(char * message, int send_bytes) {

    int res;


    // Waits for a set
    while(ll.numTransmissions != times) {
    
        printf("\nTrying to send SET.....\n\n");
    
        sendFlags(SET, "SET");
        
        // turn alarm on
        alarm(3);
        
        res = read(al.fd, buf, newtio.c_cc[VMIN]);
    
        if(res > 0) {
        
            if (strcmp(buf, UA) == 0) {
                printFlags(buf, "UA");
                sendFlags(SET, "SET");
                break;
            }
            
        }
        
        alarm(0);
        
        //if(STOP == TRUE)
    }
    
    

    sendFlags(SET, "SET");
    
    //sleep(1);

    res = write(al.fd, message, send_bytes);
    
    return res;

}

void receive_message(char * aux) {

    int res;
    
    buf[res] = 0;
    
    res = read(al.fd, buf, newtio.c_cc[VMIN]);

    printf("Received: %s:%d\n", buf, res);
    
    if(buf[res-1] == STOP_BYTE)
        STOP = TRUE;

    strcat(aux, buf);
}
