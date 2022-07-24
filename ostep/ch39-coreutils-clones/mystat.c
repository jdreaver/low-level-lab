/** Copy of stat.
 *
 * Source code of stat is useful to peruse when making this:
 * https://github.com/coreutils/coreutils/blob/master/src/stat.c
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: mystat [file/dir]\n");
		exit(EXIT_FAILURE);
	}
	char *input_path = argv[1];

	struct stat stat_buf;
	// N.B. Using lstat so we _don't_ follow symbolic link.
	if ((lstat(input_path, &stat_buf))) {
		fprintf(stderr, "stat() error on %s: %s\n", input_path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("  File: %s\n", input_path);

	printf("  Size: %-10" PRIiMAX "\t", (intmax_t)stat_buf.st_size);
	printf("Blocks: %-10" PRIiMAX " ", (intmax_t)stat_buf.st_blocks);
	printf("IO Block: %-6" PRIiMAX " ", (intmax_t)stat_buf.st_blksize);

        if (S_ISREG(stat_buf.st_mode)) {
	        puts("regular file");
	} else if (S_ISDIR(stat_buf.st_mode)) {
	        puts("directory");
	} else if (S_ISCHR(stat_buf.st_mode)) {
	        puts("character device");
	} else if (S_ISBLK(stat_buf.st_mode)) {
	        puts("block device");
	} else if (S_ISFIFO(stat_buf.st_mode)) {
	        puts("FIFO (named pipe)");
	} else if (S_ISLNK(stat_buf.st_mode)) {
	        puts("symbolic link");
	} else if (S_ISSOCK(stat_buf.st_mode)) {
	        puts("socket");
	} else {
		puts("UNKNOWN FILE TYPE");
	}

	printf("Device: %" PRIuMAX ",", (uintmax_t)major(stat_buf.st_dev));
	printf("%" PRIuMAX "\t", (uintmax_t)minor(stat_buf.st_dev));
	printf("Inode: %-10" PRIuMAX "  ", (uintmax_t)stat_buf.st_ino);
	printf("Links: %" PRIuMAX "\n", (uintmax_t)stat_buf.st_nlink);

	printf("Access: (%04o/", stat_buf.st_mode & 0777);
	printf((S_ISDIR(stat_buf.st_mode)) ? "d" : "-");
	printf((stat_buf.st_mode & S_IRUSR) ? "r" : "-");
	printf((stat_buf.st_mode & S_IWUSR) ? "w" : "-");
	printf((stat_buf.st_mode & S_IXUSR) ? "x" : "-");
	printf((stat_buf.st_mode & S_IRGRP) ? "r" : "-");
	printf((stat_buf.st_mode & S_IWGRP) ? "w" : "-");
	printf((stat_buf.st_mode & S_IXGRP) ? "x" : "-");
	printf((stat_buf.st_mode & S_IROTH) ? "r" : "-");
	printf((stat_buf.st_mode & S_IWOTH) ? "w" : "-");
	printf((stat_buf.st_mode & S_IXOTH) ? "x" : "-");
	printf(")\n");

	// TODO: Keep running stat to get more!

	return 0;
}
