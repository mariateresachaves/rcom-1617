#define MAX_SIZE 255

/* CLIENT FTP */
#define SERVER_PORT 21

struct hostent *h;

/* REGEX RETURNS */
#define URL_VALID 0
#define REG_ERROR 1
#define URL_NOTVALID 2
#define REGEX_URL_ERROR 3

/* PARSER RETURNS*/
#define URL_ERROR 4
#define URL_USER_ERROR 5
#define URL_PASS_ERROR 6
#define URL_HOST_ERROR 7
#define SUCCESS_PARSER 0
