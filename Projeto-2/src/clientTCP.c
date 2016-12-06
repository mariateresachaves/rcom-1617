/*      (C)2000 FEUP  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>

#define SERVER_PORT 21

char* SERVER_ADDR = malloc(255);

int main(int argc, char** argv){

	int	sockfd;
	struct	sockaddr_in server_addr;
	int	bytes;

	// TODO: Fazer o parser da string: "ftp://[<user>:<password>@]<host>/<url-path>"
	//	 E colocar no buf1 -> user
	//		      buf2 -> password
	//		      buf3 -> host
	//		      buf4 -> url-path

	char 	buf1[] = "miguel-teresa\n";
	char 	buf2[] = "lindo\n";
	char	buf3[] = "192.168.50.111\n";
	char	buf4[] = "file.txt";

	
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//gethostbyname - retorna o endereço ip do server
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(sockfd, 
	           (struct sockaddr *)&server_addr, 
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		exit(0);
	}

	printf("%s\n",SERVER_ADDR);
    	/*send a string to the server*/
	//bytes = write(sockfd, buf1, strlen(buf1));
	//printf("Bytes escritos %d\n", bytes);

	//read string from user

	close(sockfd);
	exit(0);
}


