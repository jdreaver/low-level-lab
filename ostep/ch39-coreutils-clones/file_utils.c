#include "file_utils.h"

#include <stdio.h>
#include <sys/stat.h>

void print_file_permissions(struct stat sb)
{
	if (S_ISDIR(sb.st_mode)) {
		printf("d");
	} else if (S_ISLNK(sb.st_mode)) {
		printf("l");
	} else {
		printf("-");
	}
	printf((sb.st_mode & S_IRUSR) ? "r" : "-");
	printf((sb.st_mode & S_IWUSR) ? "w" : "-");
	printf((sb.st_mode & S_IXUSR) ? "x" : "-");
	printf((sb.st_mode & S_IRGRP) ? "r" : "-");
	printf((sb.st_mode & S_IWGRP) ? "w" : "-");
	printf((sb.st_mode & S_IXGRP) ? "x" : "-");
	printf((sb.st_mode & S_IROTH) ? "r" : "-");
	printf((sb.st_mode & S_IWOTH) ? "w" : "-");
	printf((sb.st_mode & S_IXOTH) ? "x" : "-");

}
