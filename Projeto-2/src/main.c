#include "clientFTP.h"

int main(int argc, char** argv) {
	char cmd[MAX_SIZE] = "";
	char * response = (char*)malloc(MAX_SIZE);
	int new_port = 0;
	int parse_ret;

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
		printf("file name: \"%s\"\n", info->file_name);
	}

	// obter o endereço
	get_address(info);

	connect_to_server(info, NORMAL, SERVER_PORT);

	/*send a string to the server*/
	// Enviar o comando "user BUF1" para o sockfd

	if (read_response(info, &response) != RECEIVE_CMD_SUCCESS){
		perror("[Erro Read]\n");
		exit(-1);
	}
	
	sprintf(cmd, "USER %s\n", info->user);
	
	if (write_command(info, cmd, strlen(cmd)) != SEND_CMD_SUCCESS){
		perror("[Erro Write]\n");
		exit(-1);
	}


	// Ler o código da string "331 Password required for BUF1" do sockfd
	memset(response, 0, MAX_SIZE);
	if (read_response(info, &response) != RECEIVE_CMD_SUCCESS){
		perror("[Erro Read]\n");
		exit(-1);
	}

	sprintf(cmd, "PASS %s\n", info->password);
	
	if (write_command(info, cmd, strlen(cmd)) != SEND_CMD_SUCCESS){
		perror("[Erro Write]\n");
		exit(-1);
	}


	// Ler o código da string "230 User BUF1 logged in"
	memset(response, 0, MAX_SIZE);
	if (read_response(info, &response) != RECEIVE_CMD_SUCCESS){
		perror("[Erro Read]\n");
		exit(-1);
	}
	
	// Estamos logados

	// Enviar a string "pasv" para o sockfd
	// Ler os dois ultimos números da string "227 Entering Passive Mode (193,136,28,12,19,91)"
	//		por exemplo percorrer a string até encontrar um ) e ir buscar os dois números anteriores

	// Com os dois números lidos calcular o valor da nova porta.
	//			porta = Num1 * 256 + Num2

	new_port = enter_passive_mode(info, cmd);
	printf("PORTA: %d\n",new_port);

	// Abrir um sockfd2 com a nova porta que foi calculada
	connect_to_server(info, DATA, new_port);
	
	// Enviar a string "retr BUF4" para o sockfd
	// Ler a string "retr BUF4" no sockfd2
	check_file(info,cmd);

	// Fazer o download do ficheiro
	download_file(info);


	close(info->sockfd);

	return 0;
}
