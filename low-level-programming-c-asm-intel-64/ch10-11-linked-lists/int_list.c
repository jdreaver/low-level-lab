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

static void _int_list_print_fn(int x)
{
	printf("%d -> ", x);
}

void int_list_print(struct int_list *list)
{
	int_list_foreach(list, _int_list_print_fn);
	printf("NULL");
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

void int_list_foreach(struct int_list *list, void (*f)(int))
{
	while (list != NULL) {
		f(list->value);
		list = list-> next;
	}
}

void int_list_map_mut(struct int_list *list, int (*f)(int))
{
	while (list != NULL) {
		list->value = f(list->value);
		list = list-> next;
	}
}
