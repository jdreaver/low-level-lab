#include "atomic_counter.h"

#include "mutex_utils.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct atomic_counter {
	uint64_t count;
	pthread_mutex_t mutex;
};

struct atomic_counter *atomic_counter_create()
{
	struct atomic_counter *counter = malloc(sizeof(*counter));
	counter->count = 0;
	pthread_mutex_init_or_fail(&counter->mutex);
	return counter;
}

void atomic_counter_increment(struct atomic_counter *counter)
{
	pthread_mutex_lock_or_fail(&counter->mutex);
	counter->count += 1;
	pthread_mutex_unlock_or_fail(&counter->mutex);
}

uint64_t atomic_counter_get(struct atomic_counter *counter)
{
	return counter->count;
}

void atomic_counter_destroy(struct atomic_counter *counter)
{
	pthread_mutex_destroy(&counter->mutex);
	free(counter);
}
