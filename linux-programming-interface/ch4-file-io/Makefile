CC=gcc
CFLAGS=-I.

.PHONY: all
all: copy seek_io tee

copy: copy.c
	$(CC) $(CFLAGS) -o copy copy.c error_functions.c

seek_io: seek_io.c
	$(CC) $(CFLAGS) -o seek_io seek_io.c error_functions.c

tee: tee.c
	$(CC) $(CFLAGS) -o tee tee.c error_functions.c
