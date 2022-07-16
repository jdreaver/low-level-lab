#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void pthread_mutex_init_or_fail(pthread_mutex_t *mutex)
{
	if (pthread_mutex_init(mutex, NULL)) {
		fprintf(stderr, "error creating mutex in %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}

void pthread_mutex_lock_or_fail(pthread_mutex_t *mutex)
{
	if (pthread_mutex_lock(mutex)) {
		fprintf(stderr, "error locking mutex in %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}

void pthread_mutex_unlock_or_fail(pthread_mutex_t *mutex)
{
	if (pthread_mutex_unlock(mutex)) {
		fprintf(stderr, "error unlocking mutex in %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}
