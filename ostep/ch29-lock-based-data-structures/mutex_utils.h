#pragma once

#include <pthread.h>

void pthread_mutex_init_or_fail(pthread_mutex_t *mutex);
void pthread_mutex_lock_or_fail(pthread_mutex_t *mutex);
void pthread_mutex_unlock_or_fail(pthread_mutex_t *mutex);
