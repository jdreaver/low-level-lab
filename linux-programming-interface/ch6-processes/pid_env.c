#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

extern char **environ;

int main()
{
	pid_t pid = getpid();
	printf("PID: %d\n", pid);

	puts("Printing environment");
	for (char **ep = environ; *ep != NULL; ep++) {
		puts(*ep);
	}

	char *hello = getenv("HELLO");
	if (hello != NULL) {
		printf("Found HELLO: %s\n", hello);
	} else {
		puts("Couldn't find HELLO env var");
	}
}
