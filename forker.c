#include <stdio.h>
#include <stdlib.h>

#include "forker.h"

#define TRY(cmd, msg) if((cmd) < 0) { \
	perror((msg)); \
	exit(1); \
}

void
myfork(int *_in, int *_out, int *_err, int argc, char **argv)
{
	pid_t child;

	if ((child = fork()) < 0) {
		perror("Cannot fork");
		exit(1);
	}

	if (child == 0) {
		// I am the child
		TRY(close(_in[1]), "Cannot close stdin-pipe");
		TRY(close(_out[0]), "Cannot close stdout-pipe");
		TRY(close(_err[0]), "Cannot close stderr-pipe");

		TRY(close(STDIN_FILENO), "Cannot close stdin");
		TRY(close(STDOUT_FILENO), "Cannot close stdout");
		TRY(close(STDERR_FILENO), "Cannot close stderr");

		TRY(dup(_in[0]), "Cannot duplicate stdin");
		TRY(dup(_out[1]), "Cannot duplicate stdout");
		TRY(dup(_err[1]), "Cannot duplicate stderr");

		char **args = malloc(argc*sizeof(char *) + 1);
		for (int i=0; i<argc; i++) {
			args[i] = argv[i];
		}
		args[argc] = 0x0;

		execvp(args[0], args);
	} else {
		// I am the parent
		TRY(close(_in[0]), "Cannot close parent stdin");
		TRY(close(_out[1]), "Cannot close parent stdout");
		TRY(close(_err[1]), "Cannot close parent stderr");
	}
}
