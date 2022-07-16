#include "nonatomic_counter.h"

#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct nonatomic_thread_args {
	struct nonatomic_counter *counter;
	uint64_t count_max;
};

static void *nonatomic_thread_handler(void *arg)
{
	struct nonatomic_thread_args *thread_args = (struct nonatomic_thread_args *) arg;
	for (uint64_t i = 0; i < thread_args->count_max; i++)
		nonatomic_counter_increment(thread_args->counter);
	return NULL;
}

int main()
{
	// TODO: Make these command line args.
	uint16_t num_threads = 4;
	uint64_t count_max = 1000000;

	// Set up threads
	pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);

	// Non-atomic counter. This is here just to show that without locks the
	// threads step on one another and we get an inaccurate count.
	struct nonatomic_counter *nonatomic_counter = nonatomic_counter_create();
	for (size_t i = 0; i < num_threads; i++) {
		struct nonatomic_thread_args arg = {
			.counter = nonatomic_counter,
			.count_max = count_max / num_threads,
		};
		pthread_create(&threads[i], NULL, nonatomic_thread_handler, &arg);
	}
	for (size_t i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);

	printf("nonatomic counter final value, expected: %" PRIu64 ", got: %" PRIu64 "\n", count_max, nonatomic_counter_get(nonatomic_counter));

	nonatomic_counter_destroy(nonatomic_counter);

	free(threads);
}
