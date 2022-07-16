#pragma once

#include <stdint.h>

/**
 * A nonatomic_counter is a counter that isn't threadsafe.
 */
struct nonatomic_counter;

struct nonatomic_counter *nonatomic_counter_create();
void nonatomic_counter_increment(struct nonatomic_counter *counter);
uint64_t nonatomic_counter_get(struct nonatomic_counter *counter);
void nonatomic_counter_destroy(struct nonatomic_counter *counter);
