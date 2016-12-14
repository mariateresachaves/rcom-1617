#include <regex.h>
#include "parser.h"

void parser(char* url, char** user, char** password, char** host, char** url_path) {
	regex_t regex;
	int reti;
	char msgbuf[100];

	/* Compila a expressao regular */
	/* ftp://[<user>:<password>@]<host>/<url-path>
			<user> -> [[A-Za-z0-9_-]+
			<password> -> [A-Za-z0-9]+ //TODO: mudar regex
			<host> -> [0-9]+ //TODO: mudar regex
			<url-path> -> [A-Za-z0-9_-]+ //TODO: mudar regex
	*/
	reti = regcomp(&regex, "ftp://\\[[A-Za-z0-9_-]+:[A-Za-z0-9]+\\]@[0-9]+\\/[A-Za-z0-9_-]+", REG_EXTENDED);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	    exit(1);
	}

	/* Executa a expressao regular */
	reti = regexec(&regex, "ftp://[miguel-teresa_6:pass123]@123/url-path_name", 0, NULL, 0);
	if (!reti) {
	    puts("Match");
	}
	else if (reti == REG_NOMATCH) {
	    puts("No match");
	}
	else {
	    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
	    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
	    exit(1);
	}

	/* Liberta a memoria alocada */
	regfree(&regex);
}
