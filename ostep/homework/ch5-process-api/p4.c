#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	printf("Starting program\n");
	int rc = fork();
	if (rc < 0) {
		fprintf(stderr, "Failed to fork()\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) {
		printf("Hello from child process (pid: %d)\n", getpid());

		close(STDOUT_FILENO);
		open("./p4.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

		char *args[3] = {"wc", "p4.c", NULL};
		execvp(args[0], args);

	} else {
		int rc_wait = wait(NULL);
		printf("I am the parent of %d (rc_wait: %d) (pid: %d)\n", rc, rc_wait, getpid());
	}

	return 0;
}
