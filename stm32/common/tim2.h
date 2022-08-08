/**
 * Enable TIM2 timer.
 *
 * To use this module, you must call `tim2_enable()`, and then call
 * `tim2_set_timeout_ms(timeout_ms)`.
 */

#pragma once

#include <stdint.h>

void tim2_enable(void);
void tim2_set_timeout_ms(uint32_t timeout_ms);
