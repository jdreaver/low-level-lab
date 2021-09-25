CC=gcc
CFLAGS=-I../lib -Wall -Wextra -pedantic

.PHONY: all
all: copy seek_io tee

copy: copy.c ../lib/error_functions.c
	$(CC) $(CFLAGS) -o $@ $^

seek_io: seek_io.c ../lib/error_functions.c
	$(CC) $(CFLAGS) -o $@ $^

tee: tee.c ../lib/error_functions.c
	$(CC) $(CFLAGS) -o $@ $^
