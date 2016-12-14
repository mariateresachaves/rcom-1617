#include "clientFTP.h"

void check_args(int argc, char ** argv) {
	if (argc != 3) {
		printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
		exit(1);
	}
}

int get_address(ftp_info * info) {
	if ((h=gethostbyname(info->host)) == NULL) {
			herror("gethostbyname");
			exit(1);
	}

	printf("Host name  : %s\n", h->h_name);
	printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

	strcpy(info->server_address, inet_ntoa(*((struct in_addr *)h->h_addr)));

	return 0;
}

int connect_to_server(ftp_info * info, int type, int port) {
	struct	sockaddr_in server_addr;

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(info->server_address);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */

	/*open an TCP socket*/
	if (type == NORMAL) {
		if ((info->sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
	    		perror("socket()");
			exit(0);
	    	}

		printf("Socket connected port:%d ip:%s\n", info->sockfd, info->server_address);
		/*connect to the server*/
	    	if(connect(info->sockfd,
			   (struct sockaddr *)&server_addr,
			   sizeof(server_addr)) < 0){
			perror("connect()");
			exit(0);
		}
	}
	else if (type == DATA) {
		if ((info->data_sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
	    		perror("socket()");
			exit(0);
	    	}

		printf("Socket connected port:%d ip:%s\n", info->data_sockfd, info->server_address);
		/*connect to the server*/
	    	if(connect(info->data_sockfd,
			   (struct sockaddr *)&server_addr,
			   sizeof(server_addr)) < 0){
			perror("connect()");
			exit(0);
		}
	}

	return 0;
}

int write_command(ftp_info * info, char* cmd, int size) {
	int bytes;

	printf("A enviar comando -> %s", cmd);

	bytes = write(info->sockfd, cmd, size);

	if (bytes <= 0){
		printf("[Erro Write] Nao foi possivel enviar o comando: %s", cmd);
		return SEND_CMD_ERROR;
	}

	if (bytes != size){
		printf("[Erro Write] Nao foi possivel enviar todos os dados.\n");
		return SEND_CMD_INCOMPLETE;
	}

	return SEND_CMD_SUCCESS;

}

int read_response(ftp_info * info, char ** response) {

	if (read(info->sockfd, *response, MAX_SIZE) <= 0){
		printf("[Erro Read] Nao foi possivel receber uma resposta.\n");
		return RECEIVE_CMD_ERROR;
	}

	printf("Resposta recebida com sucesso -> %s", *response);

	return RECEIVE_CMD_SUCCESS;
}

int enter_passive_mode(ftp_info * info, char* cmd) {
	
	char * response = (char*)malloc(MAX_SIZE);

	sprintf(cmd, "PASV\n");

	if (write_command(info, cmd, strlen(cmd)) != SEND_CMD_SUCCESS){
		perror("[Erro Write]\n");
		exit(-1);
	}

	memset(response, 0, MAX_SIZE);
	if (read_response(info, &response) != RECEIVE_CMD_SUCCESS){
		perror("[Erro Read]\n");
		exit(-1);
	}
	
	return calculate_port(response);

	
}

int calculate_port(char * response) {
	char* str2;
	char* str = malloc(MAX_SIZE);
	int port;
	str = strtok(response,"("); 	//retira texto antes do "("
	str = strtok(NULL,")");	//retira ")"
	
	strtok(str,",");	//retira as ","
	strtok(NULL,",");
	strtok(NULL,",");
	strtok(NULL,",");
	str2 = strtok(NULL,",");
	port = strtol(str2, NULL, 10) * 256;
	str2 = strtok(NULL,",");
	port += strtol(str2, NULL, 10);
	
	return port;
}

int check_file(ftp_info * info,char * cmd) {
	char * response = (char*)malloc(MAX_SIZE);

	// Enviar a string "retr BUF4" para o sockfd
	sprintf(cmd, "retr %s\n", info->path);
	if (write_command(info, cmd, strlen(cmd)) != SEND_CMD_SUCCESS) {
		perror("[Erro Write]\n");
		exit(-1);
	}

	// Ler a string "retr BUF4" no sockfd2
	memset(response, 0, MAX_SIZE);
	if (read_response(info, &response) != RECEIVE_CMD_SUCCESS) {
		perror("[Erro Read]\n");
		exit(-1);
	}
	
	strtok(response," ");

	if (strcmp(response,"150") != 0){
		printf("[Erro File] Ficheiro nao encontrado.\n");
		exit(-2);
	}
	
	return 0;
}

int download_file(ftp_info * info) {
	FILE * fd_file;
	int bytes;
	char * buf = (char*)malloc(MAX_SIZE);

	fd_file = fopen(info->file_name, "w");

	if (fd_file == NULL) {
		printf("[Erro File] Nao foi possivel abrir o ficheiro.\n");
		exit(-1);
	}

	printf("Downloading %s...\n", info->file_name);

	while((bytes = read(info->data_sockfd, buf, sizeof(char)*MAX_SIZE)) > 0) {
		fwrite(buf, bytes, 1, fd_file);
	}

	printf("Download finished.\n");

	close(info->data_sockfd);
	fclose(fd_file);
	return 0;
}

int check_errors(int parse_ret) {
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

	return 0;
}
