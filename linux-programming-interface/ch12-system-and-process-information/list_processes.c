// Lists processes owned by the given user

#include<ctype.h>
#include<dirent.h>
#include<errno.h>
#include<pwd.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>

#include "lib.h"

uid_t get_user_id(char *username);

struct dirent *safe_readdir(DIR *dirp);

struct procinfo {
	char name[256];
	int  owner;
};

struct procinfo proc_status_real_user_id(char *name);

int main(int argc, char *argv[])
{
	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s <username>\n", argv[0]);
	}

	char* username = argv[1];
	uid_t user_id = get_user_id(username);

	printf("User %s has id %d\n", username, user_id);

	// Open /proc
	DIR *proc_dir = opendir("/proc");
	if (proc_dir == NULL) {
		errExit("failed to open /proc");
	}

	// List /proc
	struct dirent *entry;
	while ((entry = safe_readdir(proc_dir)) != NULL) {
		// Skip non-integer filenames
		if (!isdigit(entry->d_name[0])) {
			continue;
		}
		struct procinfo info = proc_status_real_user_id(entry->d_name);
		if (info.owner == (int)user_id) {
			printf("%s: %s\n", entry->d_name, info.name);
		}
	}
}

uid_t get_user_id(char *username)
{
	struct passwd *info = getpwnam(username);
	if (info == NULL) {
		if (errno != 0) {
			errExit("error fetching user");
		} else {
			char buf[100];
			snprintf(buf, 100, "Couldn't find user named %s", username);
			fatal(buf);
		}
	}

	return info->pw_uid;
}

// Wrapper around readdir that sets errno = 0 beforehand and exits on
// error.
struct dirent *safe_readdir(DIR *dirp)
{
	errno = 0;
	struct dirent *entry = readdir(dirp);
	if (entry == NULL && errno != 0) {
		errExit("error reading directory");
	}
	return entry;
}

// Finds the real user ID for a /proc/<pid>/status file
struct procinfo proc_status_real_user_id(char *process_id)
{
	char path[100];
	snprintf(path, 100, "/proc/%s/status", process_id);
	FILE *f = fopen(path, "r");

	char line[100];
	struct procinfo info = {
		.name = "",
		.owner = -1
	};
	while ((fgets(line, 100, f))) {
		if (strncmp("Name:", line, 5) == 0) {
			sscanf(line, "Name: %s", info.name);
		}
		if (strncmp("Uid:", line, 4) == 0) {
			sscanf(line, "Uid: %d", &info.owner);
		}
	}
	if (ferror(f)) {
		fatal("error reading proc file");
	}

	return info;
}
