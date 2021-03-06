#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "9p.h"
#include "9pehandler.h"
#include "forker.h"
#include "serialize.h"
#include "srv.h"

#define BUFSIZE 256

void usage();

void loop();

unsigned int clsock, n;
unsigned char buffer[BUFSIZE];

int _stdin[2];
int _stdout[2];
int _stderr[2];

#define cstdin  (_stdin[1])
#define cstdout (_stdout[0])
#define cstderr (_stderr[0])

void
loop()
{
	unsigned char *rmsgbuf;
	uint32_t tmsgsize, bytes_left, rmsgsize;
	struct message tmsg, rmsg;

	if (clsock <= 0) {
		fprintf(stderr, "Congratulations, you found a bug! (need to srv_start first)\n");
		exit(1);
	}
	memset(buffer, 0, sizeof(buffer)); // neccessary?
	n = read(clsock, buffer, sizeof(uint32_t));
	if (n < 0)
		error("Error reading from socket");
	if (n == 0)
		return;
	tmsgsize = deuint32_t(buffer);
	if (tmsgsize == 0)
		loop();
	bytes_left = tmsgsize - sizeof(uint32_t);
	n = read(clsock, buffer+sizeof(uint32_t), bytes_left);
	if (n < 0)
		error("Error reading from socket");
	if (n == 0)
		return;
	tmsg = buftmsg(buffer);
#ifdef DEBUG
	write(1, buffer, BUFSIZE); //dump (BUFSIZE = alignment in hexdump)
	msgdump(&tmsg);
#endif
	rmsg = handle(&tmsg);
	rmsgbuf = msgtbuf(&rmsg);
	free_msg(&tmsg);
	//free_msg(&rmsg); // somewhere here is a bug (why not with tmsg?)
#ifdef DEBUG
	msgdump(&rmsg);
	write(1, rmsgbuf, BUFSIZE); //dump
#endif
	rmsgsize = deuint32_t(rmsgbuf);
	n = write(clsock, rmsgbuf, rmsgsize);
	free(rmsgbuf);
	if (n < 0)
		error("Error writing to socket");
	loop();
}

int
main(int argc, char **argv)
{
	if (argc < 3)
		usage(argv[0]);

	if (pipe(_stdin) < 0)
		error("Cannot create pipe stdin");
	if (pipe(_stdout) < 0)
		error("Cannot create pipe stdout");
	if (pipe(_stderr) < 0)
		error("Cannot create pipe stderr");

	myfork(_stdin, _stdout, _stderr, argc-2, &argv[2]);

	// test read from child process
	char readbuffer[80];
	read(_stdout[0], readbuffer, sizeof(readbuffer));
	printf("Received string: %s\n", readbuffer);

	//return 1; // debug, do not start network services
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
