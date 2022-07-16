#include "approx_counter.h"

#include "mutex_utils.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct approx_counter {
	pthread_mutex_t global_mutex;
	uint64_t global_counter;

	int num_cpus;
	pthread_mutex_t *cpu_mutexes;
	uint64_t *cpu_counters;

	uint64_t sync_threshold;
};

struct approx_counter *approx_counter_create(int num_cpus, uint64_t sync_threshold)
{
	struct approx_counter *counter = malloc(sizeof(*counter));

	pthread_mutex_init_or_fail(&counter->global_mutex);
	counter->global_counter = 0;

	counter->num_cpus = num_cpus;
	counter->cpu_mutexes = malloc(sizeof(*counter->cpu_mutexes) * counter->num_cpus);
	counter->cpu_counters = malloc(sizeof(*counter->cpu_counters) * counter->num_cpus);

	for (int i = 0; i < counter->num_cpus; i++) {
		pthread_mutex_init_or_fail(&counter->cpu_mutexes[i]);
		counter->cpu_counters[i] = 0;
	}

	counter->sync_threshold = sync_threshold;

	return counter;
}

void approx_counter_increment(struct approx_counter *counter, int core_id)
{
	pthread_mutex_t mutex = counter->cpu_mutexes[core_id];
	pthread_mutex_lock_or_fail(&mutex);
	counter->cpu_counters[core_id] += 1;

	// Sync to global counter
	if (counter->cpu_counters[core_id] >= counter->sync_threshold) {
		pthread_mutex_lock_or_fail(&counter->global_mutex);
		counter->global_counter += counter->cpu_counters[core_id];
		pthread_mutex_unlock_or_fail(&counter->global_mutex);
		counter->cpu_counters[core_id] = 0;
	}

	pthread_mutex_unlock_or_fail(&mutex);
}

uint64_t approx_counter_get(struct approx_counter *counter)
{
	uint64_t count = counter->global_counter;
	for (int i = 0; i < counter->num_cpus; i++)
		count += counter->cpu_counters[i];
	return count;
}

void approx_counter_destroy(struct approx_counter *counter)
{
	pthread_mutex_destroy(&counter->global_mutex);
	for (int i = 0; i < counter->num_cpus; i++)
		pthread_mutex_destroy(&counter->cpu_mutexes[i]);
	free(counter->cpu_mutexes);
	free(counter->cpu_counters);
	free(counter);
}
