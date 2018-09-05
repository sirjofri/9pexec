CC = gcc -Wall -D_POSIX_C_SOURCE=200809L -std=c99 -pedantic -g

9pexec: 9pexec.o srv.o 9p.o serialize.o 9pehandler.o forker.o
	$(CC) -o $@ $^

9pexec.o: 9pexec.c
	$(CC) -o $@ -c $<

%.o: %.c %.h
	$(CC) -o $@ -c $<

clean:
	- @rm *.o
