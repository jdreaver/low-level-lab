#include "file_utils.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <inttypes.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void print_usage_and_exit()
{
	fprintf(stderr, "Usage: myls [-n] [directory]\n");
	exit(EXIT_FAILURE);
}

void print_file_list_entry(DIR *dir, struct dirent *entry)
{
	int dir_fd = dirfd(dir);
	if (dir_fd == -1) {
		perror("error getting dirfd");
		exit(EXIT_FAILURE);
	}

	struct stat stat_buf;
	if ((fstatat(dir_fd, entry->d_name, &stat_buf, AT_SYMLINK_NOFOLLOW))) {
		fprintf(stderr, "fstatat() error on %s: %s\n", entry->d_name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	errno = 0;
	struct passwd *user_passwd = getpwuid(stat_buf.st_uid);
	if (!user_passwd && errno) {
		fprintf(stderr, "getpwuid() error on %s: %s\n", entry->d_name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	errno = 0;
	struct group *file_group = getgrgid(stat_buf.st_gid);
	if (!file_group && errno) {
		fprintf(stderr, "getgrgid() error on %s: %s\n", entry->d_name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct tm t;
	tzset();
	if (localtime_r(&(stat_buf.st_mtim.tv_sec), &t) == NULL) {
		fprintf(stderr, "localtime_r() error on %s: %s\n", entry->d_name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	char time_buf[100];
	if (!strftime(time_buf, 100, "%B %d %H:%M", &t)) {
		fprintf(stderr, "strftime() error on %s\n", entry->d_name);
		exit(EXIT_FAILURE);
	}

	print_file_permissions(stat_buf);
	printf(" %" PRIuMAX " ", (uintmax_t)stat_buf.st_nlink);
	printf("%s %s ", user_passwd->pw_name, file_group->gr_name);
	printf("%" PRIuMAX " ", (uintmax_t)stat_buf.st_size);
	printf("%s ", time_buf);
	printf("%s\n", entry->d_name);
	// TODO: If symlink, then target
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
		if (strncmp(entry->d_name, ".", 2) == 0)
			continue;
		if (strncmp(entry->d_name, "..", 3) == 0)
			continue;

		if (list_opt)
			print_file_list_entry(dir, entry);
		else
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
