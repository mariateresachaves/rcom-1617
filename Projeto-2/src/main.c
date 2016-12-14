#include "clientFTP.h"

int main(int argc, char** argv) {
	char *cmd;
	int	parse_ret;
  struct	sockaddr_in server_addr;

  ftp_info* info = malloc(sizeof(ftp_info));
	memset(info, 0, sizeof(ftp_info));

	// verifica se os argumentos
	check_args(argc,argv);

	// caso nao seja passado um url
	if (strcmp(argv[2],"default") == 0) {
    strcpy(info->user, "miguel-teresa-6");
    strcpy(info->password, "rcombuefixe");
    strcpy(info->host, "ftp.up.pt");
    strcpy(info->path, "file.txt");
	}
	// caso tenha um url
	else {
		parse_ret = parser(argv[2], info);

		if(check_errors(parse_ret)) // deu um erro
      return parse_ret;

		printf("user: \"%s\"\n", info->user);
		printf("password: \"%s\"\n", info->password);
		printf("host: \"%s\"\n", info->host);
		printf("url path: \"%s\"\n", info->path);
	}

	// obter o endereço
	get_address(info);

	// Just to test TODO: Remove this!
	// TODO: desde aqui-------------
	cmd = "user ";
	create_cmd(info, &cmd);

	printf("A enviar comando.... [%s]\n", cmd);

	//TODO: até aqui-------------

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(h->h_name);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */

	/*open an TCP socket*/
	if ((info->sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(info->sockfd,
	           (struct sockaddr *)&server_addr,
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		exit(0);
	}

	/*send a string to the server*/
	// Enviar o comando "user BUF1" para o sockfd
	cmd = "user ";
	create_cmd(info, &cmd);
	//bytes = write(info->sockfd, cmd, strlen(cmd));
	printf("A enviar comando.... [%s]\n", cmd);

	// Ler o código da string "331 Password required for BUF1" do sockfd

	// Enviar a string "pass BUF2" para o sockfd

	// Ler o código da string "230 User BUF1 logged in"

	// Enviar a string "pasv" para o sockfd

	// Ler os dois ultimos números da string "227 Entering Passive Mode (193,136,28,12,19,91)"
	//		por exemplo percorrer a string até encontrar um ) e ir buscar os dois números anteriores

	// Com os dois números lidos calcular o valor da nova porta.
	//			porta = Num1 * 256 + Num2

	// Abrir um sockfd2 com a nova porta que foi calculada

	// Enviar a string "retr BUF4" para o sockfd

	// Ler a string "retr BUF4" no sockfd2

	//bytes = write(sockfd, buf, strlen(buf));
	//printf("Bytes escritos %d\n", bytes);

	close(info->sockfd);
}
