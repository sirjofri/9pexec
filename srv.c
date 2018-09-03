#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "srv.h"

void
error(char *msg)
{
	perror(msg);
	exit(1);
}

unsigned int
srv_start(int port)
{
	if (!port)
		error("No port number!");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("Error opening socket!");

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("Error on binding!");

	listen(sockfd, 5);

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("Error on accept");

	return newsockfd;

#ifdef INFORMATION_PLEASE_IGNORE
	memset(&buffer, 0, BUFSIZE);
	n = read(newsockfd, buffer, 255);
	if (n < 0)
		error("Error reading from socket");
	printf("Message: %s\n", buffer);
#endif
}

unsigned int
srv_stop()
{
	int n = close(newsockfd);
	if (n < 0)
		error("Error closing socket");
	n = close(sockfd);
	if (n < 0)
		error("Error closing socket");
	return 0;
}
