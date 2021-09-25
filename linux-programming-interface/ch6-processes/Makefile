CC=gcc
CFLAGS=-I../lib -Wall -Wextra -pedantic

.PHONY: all
all: pid_env

pid_env: pid_env.c ../lib/error_functions.c
	$(CC) $(CFLAGS) -o $@ $^
