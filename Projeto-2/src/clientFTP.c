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
#include <errno.h>
#include "parser.h"

/**
 * @brief Funcao que verifica os argumentos passados pelo utilizador.
 * @param argc - numero de argumentos.
 * @param argv - argumentos inseridos.
 */
void check_args(int argc, char ** argv) {
	if (argc != 3) {
		printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
		exit(1);
	}
}

/*	struct hostent {
			char    *h_name;	Official name of the host.
			char    **h_aliases;	A NULL-terminated array of alternate names for the host.
			int     h_addrtype;	The type of address being returned; usually AF_INET.
			int     h_length;	The length of the address in bytes.
			char    **h_addr_list;	A zero-terminated array of network addresses for the host.
};*/
int get_address() {
			// TODO: mudar o hostname de localhost para o buf3
			if ((h=gethostbyname("localhost")) == NULL) {
					herror("gethostbyname");
					exit(1);
			}

			printf("Host name  : %s\n", h->h_name);
			printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

			return 0;
}

int main(int argc, char** argv) {
	char* buf1;
	char* buf2;
	char* buf3;
	char* buf4;

	int	sockfd, parse_ret;
	struct	sockaddr_in server_addr;

	// verifica se os argumentos
	check_args(argc,argv);

	// caso nao seja passado um url
	if (strcmp(argv[2],"default") == 0) {
		buf1 = "miguel-teresa\n";
		buf2 = "rcombuefixe\n";
		buf3 = "192.168.50.111\n";
		buf4 = "file.txt";
	}
	// caso tenha um url
	else {
		parse_ret = parser(argv[2], &buf1, &buf2, &buf3, &buf4);

		// ERROR MESSAGES
		switch (parse_ret) {
			case REG_ERROR:
				printf("[Erro Regex] Nao foi possivel compilar a expressao regular.\n");
				return REG_ERROR;
				break;
			case URL_NOTVALID:
				printf("[Erro Regex] URL inserido nao e valido.\n");
				return URL_NOTVALID;
				break;
			case REGEX_URL_ERROR:
				printf("[Erro Regex] A expressao regular nao encontrou correspondencia.\n");
				return REGEX_URL_ERROR;
				break;
			case URL_ERROR:
				printf("[Erro URL] Nao foi possivel fazer parse.");
				return URL_ERROR;
				break;
			case URL_USER_ERROR:
				printf("[Erro User] Nao foi possivel fazer parse.");
				return URL_USER_ERROR;
				break;
			case URL_PASS_ERROR:
				printf("[Erro Password] Nao foi possivel fazer parse.");
				return URL_PASS_ERROR;
				break;
			case URL_HOST_ERROR:
				printf("[Erro Host] Nao foi possivel fazer parse.");
				return URL_HOST_ERROR;
				break;
			default:
				break;
		}

		printf("user: \"%s\"\n", buf1);
		printf("password: \"%s\"\n", buf2);
		printf("host: \"%s\"\n", buf3);
		printf("url path: \"%s\"\n", buf4);
	}

	// obter o endereço
	get_address();

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(h->h_name);	/*32 bit Internet address network byte ordered*/
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
    	/*send a string to the server*/
	//bytes = write(sockfd, buf, strlen(buf));
	//printf("Bytes escritos %d\n", bytes);

	close(sockfd);
}
