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
void check_args(int argc, char **argv);

/**
	* @brief Funcao obter o endereço ip.
	* @param info - informacoes sobre o ftp.
	* @return Retorna 0 em caso de sucesso.
*/
int get_address();

/**
	* @brief Funcao para conectar ao servidor.
	* @param info - informacoes sobre o ftp.
	* @param type - tipo de conexao.
	* @param port - porta.
	* @return Retorna 0 em caso de sucesso.
*/
int connect_to_server(ftp_info *info, int sockfd, int port);

/**
	* @brief Funcao para escrever um comando.
	* @param info - informacoes sobre o ftp.
	* @param cmd - comando a enviar.
	* @param size - tamanho do comando a enviar.
	* @return Retorna 0 em caso de sucesso, -1 caso ocorra um erro ao enviar
	*					o comando e -2 caso o envio do comando fique incompleto.
*/
int write_command(ftp_info *info, char *cmd, int size);

/**
	* @brief Funcao para ler uma resposta.
	* @param info - informacoes sobre o ftp.
	* @param response - resposta recebida.
	* @return Retorna 0 em caso de sucesso e -1 caso contrario.
*/
int read_response(ftp_info *info, char **response);

/**
	* @brief Funcao para entrar no modo passivo.
	* @param info - informacoes sobre o ftp.
	* @param cmd - comando a ser enviado.
	* @return Retorna o valor da nova porta.
*/
int enter_passive_mode(ftp_info *info, char *cmd);

/**
	* @brief Funcao para calcular o valor da nova porta.
	* @param response - resposta recebida que contem os
	* 									valores para calcular o valor da nova porta.
	* @return Retorna o valor da nova porta.
*/
int calculate_port(char *response);

/**
	* @brief Funcao para verificar se o ficheiro existe.
	* @param info - informacoes sobre o ftp.
	* @param cmd - comando a ser enviado.
	* @return Retorna 0 em caso de sucesso.
*/
int check_file(ftp_info *info, char *cmd);

/**
	* @brief Funcao para fazer o download de um ficheiro
	* @param info - informacoes sobre o ftp.
	* @return Retorna 0 em caso de sucesso.
*/
int download_file(ftp_info *info);

/**
 * @brief Funcao para imprimir uma mensagem de erro de acordo com o codigo de retorno passado.
 * @param parse_ret - codigo de retorno.
 * @return Retorna o codigo de erro correspondente.
 */
int check_errors(int parse_ret);
