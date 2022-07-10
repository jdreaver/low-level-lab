#pragma once

/*
 * Simple linked list for ints. Stores a value and a possibly NULL pointer to
 * the next item in the list.
 */
struct int_list {
	int value;
	struct int_list *next;
};

/*
 * Initialize an int_list with a given value.
 */
struct int_list *int_list_create(int value);

/*
 * Iterates through an int_list, freeing every element.
 */
void int_list_free(struct int_list *list);

/*
 * Initialize an int_list with a given value and prepend to the given list.
 */
struct int_list *int_list_add_front(int value, struct int_list *next);

/*
 * Prints the given int_list to stdout.
 */
void int_list_print(struct int_list *list);

/*
 * Computes the sum of all elements of the given list. Returns 0 if list is
 * empty.
 */
int int_list_sum(struct int_list *list);
