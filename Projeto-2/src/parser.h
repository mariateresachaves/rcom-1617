#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "Utilities.h"

/**
  * @brief Funcao que faz parse do url inserido pelo utilizador.
  * @param url - url inserido pelo utilizador.
  * @param info - informacoes sobre o ftp.
  * @return Retorna 0 em caso de sucesso.
*/
int parser(char *url, ftp_info *info);

/**
  * @brief Funcao para verificar se o url inserido faz
  *        match com ftp://[<user>:<password>@]<host>/<url-path>.
  * @param url - url inserido pelo utilizador.
  * @return Retorna 0 em caso de sucesso.
*/
int check_regex(char *url);

/**
  * @brief Funcao para obter as informacoes atraves do url.
  * @param url - url inserido pelo utilizador.
  * @param info - informacoes sobre o ftp.
  * @return Retorna 0 em caso de sucesso.
*/
int url_parser(char *url, ftp_info *info);
