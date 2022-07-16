#pragma once

#include <stdint.h>

/**
 * A atomic_counter is a counter that is threadsafe thanks to mutexes.
 */
struct atomic_counter;

struct atomic_counter *atomic_counter_create();
void atomic_counter_increment(struct atomic_counter *counter);
uint64_t atomic_counter_get(struct atomic_counter *counter);
void atomic_counter_destroy(struct atomic_counter *counter);
