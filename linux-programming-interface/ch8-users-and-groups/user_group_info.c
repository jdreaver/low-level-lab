#include<errno.h>
#include<grp.h>
#include<pwd.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>

#include "lib.h"

int main(int argc, char *argv[])
{
	if (argc < 3 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s [user|group] <name>\n", argv[0]);
	}

	char *user_or_group = argv[1];
	char *name = argv[2];

	if (strcmp(user_or_group, "user") == 0) {
		printf("Fetching user info for %s\n", name);
		struct passwd *info = getpwnam(name);
		if (info == NULL) {
			if (errno != 0) {
				errExit("error fetching user");
			} else {
				fatal("could not find user");
			}
		}

		printf("pw_name:   %s\n", info->pw_name);
		printf("pw_passwd: %s\n", info->pw_passwd);
		printf("pw_uid:    %u\n", info->pw_uid);
		printf("pw_gid:    %u\n", info->pw_gid);
		printf("pw_gecos:  %s\n", info->pw_gecos);
		printf("pw_dir:    %s\n", info->pw_dir);
		printf("pw_shell:  %s\n", info->pw_shell);

	} else if (strcmp(user_or_group, "group") == 0) {
		printf("Fetching group info for %s\n", name);
		struct group *info = getgrnam(name);
		if (info == NULL) {
			if (errno != 0) {
				errExit("error fetching group");
			} else {
				fatal("could not find group");
			}
		}

		printf("gr_name:   %s\n", info->gr_name);
		printf("gr_passwd: %s\n", info->gr_passwd);
		printf("gr_gid:    %u\n", info->gr_gid);
		printf("gr_mem:    ");

		for (char **mem = info->gr_mem; *mem != NULL; mem++) {
			printf("%s ", *mem);
		}

		/* Alternatively: */
		/* for (int i = 0; info->gr_mem[i] != NULL; i++) { */
		/* 	printf("%s ", info->gr_mem[i]); */
		/* } */

		puts("");
	} else {
		fprintf(stderr, "first argument must be 'user' or 'group'\n");
		usageErr("%s [user|group] <name>\n", argv[0]);
	}
}
