#include "parser.h"


int parser(char *url, ftp_info *info) {
	int check_return;

	check_return = check_regex(url);

	if (check_return == URL_VALID)
		 return url_parser(url, info);

	else
		return check_return;
}

int check_regex(char *url) {
	regex_t regex;
	int reti;

	/* Compila a expressao regular */
	/* ftp://[<user>:<password>@]<host>/<url-path>
			<user> -> [[A-Za-z0-9_-]+
			<password> -> [A-Za-z0-9]+
			<host> -> [A-Za-z0-9\\.]+
			<url-path> -> ([A-Za-z0-9_-\\.]+\\/)+
	*/
	reti = regcomp(&regex, "ftp://\\[[A-Za-z0-9_-]+:[A-Za-z0-9]+@\\][A-Za-z0-9\\.]+\\/([A-Za-z0-9_-]+\\/)+([A-Za-z0-9_-]+\\.)", REG_EXTENDED);
	if (reti)
	    return REG_ERROR;

	/* Executa a expressao regular */
	// exemplo de regex a funcionar
	reti = regexec(&regex, url, 0, NULL, 0);

	/* Validate regexSimple */
	if (!reti) {
		printf("URL valido\n");
		return URL_VALID;
	}
	else if (reti == REG_NOMATCH)
		return URL_NOTVALID;

	else
		return REGEX_URL_ERROR;

	/* Liberta a memoria alocada */
	regfree(&regex);
}

int url_parser(char* url, ftp_info * info) {
	const char d1[2] = "[";
	const char d2[2] = ":";
	const char d3[2] = "@]";
	const char d4[2] = "/";
	char *token 		 = (char*)malloc(MAX_SIZE);
	char *user 	 		 = (char*)malloc(MAX_SIZE);
	char *password 	 = (char*)malloc(MAX_SIZE);
	char *host 	 		 = (char*)malloc(MAX_SIZE);
	char *path 			 = (char*)malloc(MAX_SIZE);

	//ftp://[<user>:<password>@]<host>/<url-path>

	token = strtok(url, d1);
	token = strtok(NULL, d1);

	if (token != NULL) {
		// USER
		user = strtok(token, d2);
		token = strtok(NULL, d2);

		if (token != NULL) {
			// PASSWORD
			password = strtok(token, d3);
			token = strtok(NULL, d3);

			if (token != NULL) {
				// HOST
				host = strtok(token, d4);
				token = strtok(NULL, "");

				if (token != NULL) {
					// URL PATH
					path = token;
				}
				else return URL_HOST_ERROR;
			}
			else return URL_PASS_ERROR;
		}
		else return URL_USER_ERROR;
	}
	else return URL_ERROR;

	strcpy(info->user, user);
	strcpy(info->password, password);
	strcpy(info->host, host);
	strcpy(info->path, path);

	token = strrchr(path, '/');

	if (token && *(token + 1))
		strcpy(info->file_name, token+1);

	return SUCCESS_PARSER;
}
