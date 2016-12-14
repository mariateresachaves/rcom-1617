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

int read_response(ftp_info * info, char ** response){

	if (read(info->sockfd, *response, MAX_SIZE) <= 0){
		printf("[Erro Read] Nao foi possivel receber uma resposta.\n");
		return RECEIVE_CMD_ERROR;
	}

	printf("Resposta recebida com sucesso -> %s", *response);

	return RECEIVE_CMD_SUCCESS;
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
