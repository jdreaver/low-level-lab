#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	// Create pipe
	int pipe_ends[2];
	int ret = pipe(pipe_ends);
	if (ret != 0) {
		perror("failed to create pipe\n");
		exit(EXIT_FAILURE);
	}

	// Fork first (write) child
	int write_child_pid = fork();
	if (write_child_pid < 0) {
		fprintf(stderr, "Failed to fork()\n");
		exit(EXIT_FAILURE);
	} else if (write_child_pid == 0) {
		char buf[4096];
		printf("Hello from write child process (pid: %d)\n", getpid());
		int chars_written = snprintf(buf, 4096, "(in buffer) Hello from write child process (pid: %d)\n", getpid());
		if (chars_written < 0) {
			fprintf(stderr, "Failed to write message to buffer\n");
			exit(EXIT_FAILURE);
		}
		if (write(pipe_ends[1], buf, chars_written + 1) < 0) {
			perror("failed to write to pipe\n");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}

	// Fork second (read) child
	int read_child_pid = fork();
	if (read_child_pid < 0) {
		fprintf(stderr, "Failed to fork()\n");
		exit(EXIT_FAILURE);
	} else if (read_child_pid == 0) {
		printf("Hello from read child process (pid: %d)\n", getpid());

		char buf[4096];
		if (read(pipe_ends[0], buf, 4096) < 0) {
			perror("failed to read from pipe\n");
			exit(EXIT_FAILURE);
		}

		printf("From read process (pid: %d), read: %s\n", getpid(), buf);
		exit(EXIT_SUCCESS);
	}

	printf("Waiting for children in parent (pid: %d)\n", getpid());
	// Everything past here is in parent because of exit() calls in children
	if (waitpid(write_child_pid, NULL, 0) < 0) {
		perror("failed wait for write child\n");
		exit(EXIT_FAILURE);
	}
	if (waitpid(read_child_pid, NULL, 0) < 0) {
		perror("failed wait for read child\n");
		exit(EXIT_FAILURE);
	}
	printf("Done waiting for children in parent (pid: %d)\n", getpid());

	return 0;
}
