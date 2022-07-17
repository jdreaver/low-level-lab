#define _GNU_SOURCE // Needed for sys/sysinfo.h stuff

#include "approx_counter.h"
#include "atomic_counter.h"
#include "nonatomic_counter.h"

#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <time.h>

void stick_this_thread_to_core(int core_id) {
	int num_cores = get_nprocs();
	if (core_id < 0 || core_id >= num_cores) {
		fprintf(stderr, "core_id %d exceeds num_cores %d in %s at %s:%d\n", core_id, num_cores, __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core_id, &cpuset);

	pthread_t current_thread = pthread_self();
	if (pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset)) {
		fprintf(stderr, "pthread_set_affinity_np failed in %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}

struct nonatomic_thread_args {
	int core_id;
	struct nonatomic_counter *counter;
	uint64_t count_max;
};

static void *nonatomic_thread_handler(void *arg)
{
	struct nonatomic_thread_args *thread_args = (struct nonatomic_thread_args *) arg;
	stick_this_thread_to_core(thread_args->core_id);
	for (uint64_t i = 0; i < thread_args->count_max; i++)
		nonatomic_counter_increment(thread_args->counter);
	return NULL;
}

struct atomic_thread_args {
	int core_id;
	struct atomic_counter *counter;
	uint64_t count_max;
};

static void *atomic_thread_handler(void *arg)
{
	struct atomic_thread_args *thread_args = (struct atomic_thread_args *) arg;
	stick_this_thread_to_core(thread_args->core_id);
	for (uint64_t i = 0; i < thread_args->count_max; i++)
		atomic_counter_increment(thread_args->counter);
	return NULL;
}

struct approx_thread_args {
	int core_id;
	struct approx_counter *counter;
	uint64_t count_max;
};

static void *approx_thread_handler(void *arg)
{
	struct approx_thread_args *thread_args = (struct approx_thread_args *) arg;
	stick_this_thread_to_core(thread_args->core_id);
	for (uint64_t i = 0; i < thread_args->count_max; i++)
		approx_counter_increment(thread_args->counter, thread_args->core_id);
	return NULL;
}

int main()
{
	// TODO: Have command line args to run a given type of counter with
	// different parameters, and then orchestrate all of this with a
	// top-level bash script.

	uint16_t num_threads = 8;
	uint64_t count_max = 100000000;

	struct timespec start, finish;
	double elapsed_millis;

	// Set up threads
	pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);

	// Non-atomic counter. This is here just to show that without locks the
	// threads step on one another and we get an inaccurate count.
	struct nonatomic_counter *nonatomic_counter = nonatomic_counter_create();
	struct nonatomic_thread_args *nonatomic_args = malloc(sizeof(*nonatomic_args) * num_threads);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (size_t i = 0; i < num_threads; i++) {
		nonatomic_args[i].core_id = i;
		nonatomic_args[i].counter = nonatomic_counter;
		nonatomic_args[i].count_max = count_max / num_threads;

		if (pthread_create(&threads[i], NULL, nonatomic_thread_handler, &nonatomic_args[i])) {
			fprintf(stderr, "error creating nonatomic counter pthread\n");
			exit(EXIT_FAILURE);
		}
	}
	for (size_t i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed_millis = (finish.tv_sec - start.tv_sec) * 1000.0;
	elapsed_millis += (finish.tv_nsec - start.tv_nsec) / 1000000.0;

	printf("nonatomic_counter time spent (ms): %f\n", elapsed_millis);
	printf("nonatomic counter final value, expected: %" PRIu64
	       ", got: %" PRIu64 "\n",
	       count_max, nonatomic_counter_get(nonatomic_counter));

	nonatomic_counter_destroy(nonatomic_counter);
	free(nonatomic_args);

	// Atomic counter
	struct atomic_counter *atomic_counter = atomic_counter_create();
	struct atomic_thread_args *atomic_args = malloc(sizeof(*atomic_args) * num_threads);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (size_t i = 0; i < num_threads; i++) {
		atomic_args[i].core_id = i;
		atomic_args[i].counter = atomic_counter;
		atomic_args[i].count_max = count_max / num_threads;

		if (pthread_create(&threads[i], NULL, atomic_thread_handler, &atomic_args[i])) {
			fprintf(stderr, "error creating atomic counter pthread\n");
			exit(EXIT_FAILURE);
		}
	}
	for (size_t i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed_millis = (finish.tv_sec - start.tv_sec) * 1000.0;
	elapsed_millis += (finish.tv_nsec - start.tv_nsec) / 1000000.0;

	printf("atomic_counter time spent (ms): %f\n", elapsed_millis);
	printf("atomic counter final value, expected: %" PRIu64
	       ", got: %" PRIu64 "\n",
	       count_max, atomic_counter_get(atomic_counter));

	atomic_counter_destroy(atomic_counter);
	free(atomic_args);

	// Approx counter
	uint64_t sync_threshold = 1024;
	struct approx_counter *approx_counter = approx_counter_create(num_threads, sync_threshold);
	struct approx_thread_args *approx_args = malloc(sizeof(*approx_args) * num_threads);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (size_t i = 0; i < num_threads; i++) {
		approx_args[i].core_id = i;
		approx_args[i].counter = approx_counter;
		approx_args[i].count_max = count_max / num_threads;

		if (pthread_create(&threads[i], NULL, approx_thread_handler, &approx_args[i])) {
			fprintf(stderr, "error creating approx counter pthread\n");
			exit(EXIT_FAILURE);
		}
	}
	for (size_t i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed_millis = (finish.tv_sec - start.tv_sec) * 1000.0;
	elapsed_millis += (finish.tv_nsec - start.tv_nsec) / 1000000.0;

	printf("approx_counter time spent (ms): %f\n", elapsed_millis);
	printf("approx counter final value, expected: %" PRIu64
	       ", got: %" PRIu64 "\n",
	       count_max, approx_counter_get(approx_counter));

	approx_counter_destroy(approx_counter);
	free(approx_args);

	free(threads);
}
