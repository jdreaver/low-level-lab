CC=gcc
CFLAGS=-I../lib -Wall -Wextra -pedantic

.PHONY: all
all: free_and_sbrk

free_and_sbrk: free_and_sbrk.c ../lib/error_functions.c
	$(CC) $(CFLAGS) -o $@ $^
