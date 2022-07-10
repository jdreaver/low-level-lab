#include <stdio.h>
#include <stdlib.h>

#include "int_list.h"


struct int_list *int_list_create(int value)
{
	struct int_list *list = malloc(sizeof(struct int_list));
	if (list == NULL) {
		fprintf(stderr, "%s:%d: %s: Fatal: failed to allocation new \n", __FILE__, __LINE__, __FUNCTION__);
		abort();
	}
	list->value = value;
	return list;
}

void int_list_free(struct int_list *list)
{
	while (list != NULL) {
		struct int_list *next = list->next;
		free(list);
		list = next;
	}
}

struct int_list *int_list_add_front(int value, struct int_list *next)
{
	struct int_list *list = int_list_create(value);
	list->next = next;
	return list;
}

void int_list_print(struct int_list *list)
{
	while (list != NULL) {
		printf("%d", list->value);
		if (list->next)
			printf(" -> ");
		list = list->next;
	}
}

int int_list_sum(struct int_list *list)
{
	int sum = 0;
	while (list != NULL) {
		sum += list->value;
		list = list->next;
	}
	return sum;
}
