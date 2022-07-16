#include "nonatomic_counter.h"

#include <stdint.h>
#include <stdlib.h>

struct nonatomic_counter {
	uint64_t count;
};

struct nonatomic_counter *nonatomic_counter_create()
{
	struct nonatomic_counter *counter = malloc(sizeof(*counter));
	counter->count = 0;
	return counter;
}

void nonatomic_counter_increment(struct nonatomic_counter *counter)
{
	counter->count += 1;
}

uint64_t nonatomic_counter_get(struct nonatomic_counter *counter)
{
	return counter->count;
}

void nonatomic_counter_destroy(struct nonatomic_counter *counter)
{
	free(counter);
}
