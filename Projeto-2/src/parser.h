#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "Utilities.h"

int parser(char* url, char** user, char** password, char** host, char** url_path);
int check_regex(char* url);
int url_parser(char* url, char** user, char** password, char** host, char** url_path);
