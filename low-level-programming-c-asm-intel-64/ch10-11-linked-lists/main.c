#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "int_list.h"

int square(int x)
{
	return x * x;
}

int main(void)
{
	struct int_list *list = NULL;

	/* Read integers from stdin */
	int value;
	int status;
	while ((status = scanf("%d", &value)) == 1) {
		list = int_list_add_front(value, list);
	}
	if (status == EOF && errno) {
		perror("failed reading input");
		exit(1);
	}
	if (status != EOF) {
		fprintf(stderr, "matching failure: couldn't read integer from input\n");
		exit(1);
	}

	printf("here is your list: ");
	int_list_print(list);
	puts("");

	printf("list sum: %d\n", int_list_sum(list));

	printf("list squares: ");
	int_list_map_mut(list, square);
	int_list_print(list);
	puts("");

	int_list_free(list);
	return 0;
}
