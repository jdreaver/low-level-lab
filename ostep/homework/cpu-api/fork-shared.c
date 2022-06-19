#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	// Shared variable used by parent and child.
	int x = 100;

	int rc = fork();
	if (rc < 0) {
		fprintf(stderr, "Failed to fork()\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) {
		x = 200;
		printf("Hello from child process (pid: %d) (x: %d)\n", getpid(), x);
	} else {
		wait(NULL);
		printf("I am the parent of %d (x: %d) (pid: %d)\n", rc, x, getpid());
	}

	return 0;
}
