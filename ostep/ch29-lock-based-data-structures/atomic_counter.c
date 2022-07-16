#include "atomic_counter.h"

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
	if (pthread_mutex_init(&counter->mutex, NULL)) {
		fprintf(stderr, "error creating mutex in %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	return counter;
}

void atomic_counter_increment(struct atomic_counter *counter)
{
	if (pthread_mutex_lock(&counter->mutex)) {
		fprintf(stderr, "error locking mutex in %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	counter->count += 1;
	if (pthread_mutex_unlock(&counter->mutex)) {
		fprintf(stderr, "error unlocking mutex in %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

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
