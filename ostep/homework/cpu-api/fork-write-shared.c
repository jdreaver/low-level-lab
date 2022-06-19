#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	// Shared file
	FILE *f = fopen("./fork-write-shared.output", "w+");

	int rc = fork();
	if (rc < 0) {
		fprintf(stderr, "Failed to fork()\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) {
		fprintf(f, "Hello from child process (pid: %d)\n", getpid());
	} else {
		fprintf(f, "I am the parent of %d (pid: %d)\n", rc, getpid());
	}

	return 0;
}
