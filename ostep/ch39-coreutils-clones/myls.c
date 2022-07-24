#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void print_usage_and_exit()
{
	fprintf(stderr, "Usage: myls [-n] [directory]\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	bool list_opt = false;

	char opt;
	while ((opt = getopt(argc, argv, "l")) != -1) {
		switch (opt) {
		case 'l':
			list_opt = true;
			break;
		default: // This is '?' for unknown option
			print_usage_and_exit();
		}

	}

	// Optionally a single directory name
	char *directory = NULL;
	if (optind == argc - 1)
		directory = argv[optind];
	else if (optind < argc - 1)
		// Too many positional arguments
		print_usage_and_exit();

	// Set directory to current directory if none provided
	if (!directory)
		directory = ".";

	DIR *dir = opendir(directory);
	if (!dir) {
		fprintf(stderr, "failed to open directory %s: %s\n", directory, strerror(errno));
		exit(EXIT_FAILURE);
	}

	errno = 0;
	struct dirent *entry;
	while ((entry = readdir(dir))) {
		printf("%s\n", entry->d_name);
	}
	if (errno) {
		perror("readdir() error");
		exit(EXIT_FAILURE);
	}

	if (list_opt)
		printf("Would list\n");

	closedir(dir);

	return 0;
}
