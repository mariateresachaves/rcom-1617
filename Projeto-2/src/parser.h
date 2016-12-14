#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "Utilities.h"

int parser(char* url, ftp_info * info);
int check_regex(char* url);
int url_parser(char* url, ftp_info * info);
