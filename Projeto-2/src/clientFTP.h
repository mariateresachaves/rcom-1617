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
void check_args(int argc, char ** argv);

/*	struct hostent {
			char    *h_name;	Official name of the host.
			char    **h_aliases;	A NULL-terminated array of alternate names for the host.
			int     h_addrtype;	The type of address being returned; usually AF_INET.
			int     h_length;	The length of the address in bytes.
			char    **h_addr_list;	A zero-terminated array of network addresses for the host.
};*/
int get_address();

void create_cmd(ftp_info * info, char** cmd);

int check_errors(int parse_ret);

int write_command(ftp_info * info, char* cmd, int size);

int read_response(ftp_info * info, char ** response);
