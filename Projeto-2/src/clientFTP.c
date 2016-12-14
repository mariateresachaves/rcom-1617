#include "clientFTP.h"

void check_args(int argc, char ** argv) {
	if (argc != 3) {
		printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
		exit(1);
	}
}

int get_address(ftp_info * info) {
			// TODO: mudar o hostname de localhost para o buf3
			if ((h=gethostbyname(info->host)) == NULL) {
					herror("gethostbyname");
					exit(1);
			}

			printf("Host name  : %s\n", h->h_name);
			printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

			return 0;
}

void create_cmd(ftp_info * info, char** cmd) {
	char* buf = info->user;
	char src[50], dest[50];

	strcpy(src, buf);
	strcpy(dest, *cmd);

	strcat(dest, src);

	*cmd = dest;
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
