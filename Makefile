CC = gcc -Wall -D_POSIX_C_SOURCE=200809L -std=c99 -pedantic
DEBUG= -DDEBUG -g

9pexec: 9pexec.o srv.o 9p.o serialize.o 9pehandler.o forker.o
	$(CC) $(DEBUG) -o $@ $^

9pexec.o: 9pexec.c
	$(CC) $(DEBUG) -o $@ -c $<

%.o: %.c %.h
	$(CC) $(DEBUG) -o $@ -c $<

clean:
	- @rm *.o
