#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>


#define PORT 5000
#define MAX_LEN 1024

typedef struct sockaddr_in addr;

int socket_init();


