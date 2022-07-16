#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void pthread_mutex_init_or_fail(pthread_mutex_t *mutex)
{
	int err = pthread_mutex_init(mutex, NULL);
	if (err) {
		fprintf(stderr, "error creating mutex (code %d) in %s at %s:%d\n", err, __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}

void pthread_mutex_lock_or_fail(pthread_mutex_t *mutex)
{
	int err = pthread_mutex_lock(mutex);
	if (err) {
		fprintf(stderr, "error locking mutex (code %d) in %s at %s:%d\n", err, __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}

void pthread_mutex_unlock_or_fail(pthread_mutex_t *mutex)
{
	int err = pthread_mutex_unlock(mutex);
	if (err) {
		fprintf(stderr, "error unlocking mutex (code %d) in %s at %s:%d\n", err, __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}
