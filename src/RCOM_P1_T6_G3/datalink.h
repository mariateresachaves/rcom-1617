/*! \file */

#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "utilities.h"

#define MAX_FRAME_SIZE 4000

#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FLAG_INICIAL 0
#define FLAG_FINAL 1
#define BCC_DECODE 2
#define SEND_UA_BACK 3
#define EXIT_STATE_MACHINE 4

#define SEND_CMD 0
#define RCV_UA 1
#define RCV_DISC 2
#define RCV_I_BACK 3
#define EXIT_STATE_MACHINE_2 4

#define BCC1_FAIL 0
#define BCC2_FAIL 1
#define NO_ERROR 2

int CONNECTION;

int count_msg_dropped;
int duplicate_found;
int n_retransmissions;
int n_timeouts;

/**
 * @brief Funcao para instalar a rotina do timer.
 */
void alarm_handler();

/**
 * @brief Funcao para definir o newtio.c_cflag dependendo do baudrate.
 */
void def_c_cflag();

/**
 * @brief Funcao para abrir a porta-serie.
 * @param port - nome da porta.
 * @return Retorna o descritor da porta.
 */
int open_port(char * port);

/**
 * @brief Funcao para fechar a porta-serie.
 * @param fd - descritor da porta.
 * @return Retorna 0 sem caso de sucesso e -1 caso contrario.
 */
int close_port(int fd);

/**
 * @brief Funcao para verificar se o bcc1 e o bcc2 estao corretos.
 * @param buf - pacote a verificar.
 * @param size - tamanho do pacote.
 * @return Retorna 0 caso o bcc1 esteja errado, 1 caso o bcc2 esteja errado e 2 caso o bcc1 e o bcc2 estejam corretos.
 */
int validate_bcc(char * buf, int size);

/**
 * @brief Funcao para gerar o bcc para um pacote.
 * @param buf - pacote para o qual vai ser gerado um bcc.
 * @param size - tamanho do pacote.
 * @return Retorna 0 caso seja uma trama de informacao e retorna o campo de controlo caso contrario.
 */
char generate_bcc(char * buf, int size);

/**
 * @brief Funcao para aplicar a tecnica de stuffing.
 * @param buf - pacote.
 * @param size - tamanho do pacote.
 */
void stuffing(char * buf, int * size);

/**
 * @brief Funcao para fazer a operacao inversa da tecnica de stuffing.
 * @param buf - pacote.
 * @param size - tamanho do pacote.
 */
void destuffing(char * buf, int * size);

/**
 * @brief Funcao para enviar uma trama.
 * @param fd - descritor da porta.
 * @param type - campo de controlo.
 * @param address - campo de endereco.
 * @param data - dados da trama.
 * @param size - tamanho da trama.
 * @return Retorna o numero de bytes lidos em caso de sucesso e -1 caso contrario.
 */
int send_flags(int fd, char type, char address, char * data, int size);

/**
 * @brief Funcao para verificar se uma trama e duplicada.
 * @param buf - trama.
 * @param size - tamanho da trama.
 * @return Retorna 0 em caso de sucesso e 1 caso contrario.
 */
int check_duplicate(char * buf, int size);

/**
 * @brief Funcao que recebe dados.
 * @param fd - descritor da porta.
 * @param type - nome do tipo do campo de controlo.
 * @param address - campo de endereco.
 * @param control - campo de controlo.
 * @param data - onde sera guardada a informacao lida.
 * @param mode - indica se e um emissor ou um recetor.
 * @return Retorna o tamanho dos dados recebidos e 0 em casso de erro.
 */
int get_message(int fd, char * type, char * address, char * control, char * data, int mode);

/**
 * @brief Funcao que escreve dados.
 * @param fd - descritor da porta.
 * @param type - nome do tipo do campo de controlo.
 * @param address - campo de endereco.
 * @param control - campo de controlo.
 * @param data - dados a enviar.
 * @param size - tamanho dos dados.
 * @return Retorna o tamanho dos dados caso envie uma trama de informacao com sucesso, ou 1 caso envie uma outra trama com sucesso e -1 caso contrario.
 */
int write_message(int fd, char * type, char address, char control, char * data, int size);

/**
 * @brief Funcao que estabelece a comunicacao entre o recetor e o emissor.
 * @param port - porta a ser aberta para efetuar a comunicacao.
 * @return Retorna 0 em caso de sucesso e -1 caso contrario.
 */
int llopen(char * port);

/**
 * @brief Funcao que escreve um pacote.
 * @param fd - descritor da porta
 * @param buf - pacote a enviar.
 * @param size - tamanho do pacote.
 * @return Retorna o tamanho dos dados enviados em caso de sucesso e -1 caso contrario.
 */
int llwrite(int fd, char * buf, int size);

/**
 * @brief Funcao que le um pacote.
 * @param fd - descritor da porta.
 * @param buf - onde sera guardada a informacao lida.
 * @return Retorna -2 caso receba um disconnect, 0 em caso de sucesso e -1 caso contrario.
 */
int llread(int fd, char * buf);

/**
 * @brief Funcao que fecha a conexao entre o emissor e o recetor.
 * @param fd - descritor da porta.
 * @return Retorna 1 em caso de sucesso e -1 caso contrario.
 */
int llclose(int fd);

/**
 * @brief Funcao que imprime as estatisticas da transmissao do ficheiro.
 */
void statistics();

#endif

