#pragma once

#include <stdint.h>

/**
 * An approx_counter is an approximate counter that had an atomic_counter per
 * CPU thread, and syncs these counters into a global counter after reaching a
 * certain threshold.
 */
struct approx_counter;

struct approx_counter *approx_counter_create(uint64_t sync_threshold);
void approx_counter_increment(struct approx_counter *counter, int core_id);

/**
 * Gets value of global counter and all CPU counters.
 */
uint64_t approx_counter_get(struct approx_counter *counter);

void approx_counter_destroy(struct approx_counter *counter);
