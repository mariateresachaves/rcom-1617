#define MAX_SIZE 1024

/* REGEX RETURNS */
#define URL_VALID 0
#define REG_ERROR 1
#define URL_NOTVALID 2
#define REGEX_URL_ERROR 3

/* PARSER RETURNS */
#define SUCCESS_PARSER 0
#define URL_ERROR 4
#define URL_USER_ERROR 5
#define URL_PASS_ERROR 6
#define URL_HOST_ERROR 7

/* CLIENT_FTP RETURNS */
#define SEND_CMD_SUCCESS 0
#define SEND_CMD_ERROR -1
#define SEND_CMD_INCOMPLETE -2
#define RECEIVE_CMD_SUCCESS 0
#define RECEIVE_CMD_ERROR -1


/* CLIENT FTP */
#define IP_SIZE 15
#define SERVER_PORT 21
#define DATA 1
#define NORMAL 2

struct hostent *h;

typedef struct {
	char server_address[IP_SIZE];
	int  sockfd;
	int  data_sockfd;
	char file_name[MAX_SIZE];
	char user[MAX_SIZE];
	char password[MAX_SIZE];
	char host[MAX_SIZE];
	char path[MAX_SIZE];
} ftp_info;
