#include <regex.h>
#include "parser.h"

void parser(char* url, char** user, char** password, char** host, char** url_path) {
	regex_t regex;
	int reti;
	char msgbuf[100];

	/* Compila a expressao regular */
	reti = regcomp(&regex, "abc", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	    exit(1);
	}

	/* Executa a expressao regular */
	reti = regexec(&regex, "abc", 0, NULL, 0);
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

	// TODO: Fazer o parser da string: "ftp://[<user>:<password>@]<host>/<url-path>"
	//	 E colocar no buf1 -> user
	//		      buf2 -> password
	//		      buf3 -> host
	//		      buf4 -> url-path


	printf("ENTREI NO PARSER; FODASSE RCOM\n");
}
