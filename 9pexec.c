#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "9p.h"
#include "9pehandler.h"
#include "serialize.h"
#include "srv.h"

#define BUFSIZE 256

void usage();

void loop();

unsigned int clsock, n;
unsigned char buffer[BUFSIZE];

void
loop()
{
	if (clsock <= 0) {
		fprintf(stderr, "Congratulations, you found a bug! (need to srv_start first)\n");
		exit(1);
	}
	memset(&buffer, 0, sizeof(buffer));
	n = read(clsock, buffer, sizeof(uint32_t));
	if (n < 0)
		error("Error reading from socket");
	if (n == 0)
		return;
	uint32_t msgsize = deuint32_t(buffer);
	if (msgsize == 0)
		loop();
	uint32_t bytes_left = msgsize - sizeof(uint32_t);
	n = read(clsock, buffer+sizeof(uint32_t), bytes_left);
	if (n < 0)
		error("Error reading from socket");
	if (n == 0)
		return;
	printf("Message: %x %x %x %x %x %x %x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
	struct message tmsg = buftmsg(buffer);
	msgdump(&tmsg);
	struct message rmsg = handle(&tmsg);
	unsigned char *rmsgbuf = msgtbuf(&rmsg);
	uint32_t rmsgsize = deuint32_t(rmsgbuf);
	n = write(clsock, rmsgbuf, rmsgsize);
	if (n < 0)
		error("Error writing to socket");
	loop();
}

int
main(int argc, char **argv)
{
	printf("Hello from 9exec!\n");

	if (argc < 3)
		usage(argv[0]);

	clsock = srv_start(atoi(argv[1]));

	loop();

	srv_stop();
}

void
usage(char *arg0)
{
	fprintf(stderr, "Usage: %s port command\n", arg0);
	exit(1);
}
