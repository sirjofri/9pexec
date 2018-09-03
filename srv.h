#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

unsigned int sockfd, newsockfd, clilen, n;
struct sockaddr_in serv_addr, cli_addr;

void error(char *msg);

unsigned int srv_start(int port);
unsigned int srv_stop();
