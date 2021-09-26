CC=gcc
CFLAGS=-Ilib -Wall -Wextra -pedantic

.PHONY: all
all: bin/copy bin/seek_io bin/tee bin/pid_env bin/free_and_sbrk bin/user_group_info

bin/copy: ch4-file-io/copy.c lib/error_functions.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

bin/seek_io: ch4-file-io/seek_io.c lib/error_functions.c lib/num_args.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

bin/tee: ch4-file-io/tee.c lib/error_functions.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

bin/pid_env: ch6-processes/pid_env.c lib/error_functions.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

bin/free_and_sbrk: ch7-memory-allocation/free_and_sbrk.c lib/error_functions.c lib/num_args.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

bin/user_group_info: ch8-users-and-groups/user_group_info.c lib/error_functions.c lib/num_args.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^
