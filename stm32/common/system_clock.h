/**
 * Core definitions for system clocks.
 */

#pragma once

#ifdef STM32F401xE

#include "stm32f4xx.h"

#define HSI_CLOCK_HZ 16000000UL

// The default system clock is the HSI clock, which is 16 MHz
#define DEFAULT_SYSTEM_CLOCK_HZ HSI_CLOCK_HZ

/**
 * Enables systick to constantly count down from the max LOAD value with no
 * interrupt. This is mandatory if we later want to use systick for the
 * `systick_delay_ms()` function. Notably, this function **does not** enable the
 * systick interrupt. The goal is to set a very high count reload value and
 * constantly count down so we can use this for delay spin loops.
 *
 * This also sets systick to use the default AHB/8 clock speed, not the
 * processor clock. This is more useful than the processor clock because it is
 * still plenty fast, but allows a much wider range of wait times.
 */
void systick_enable_passive(void);

/**
 * Converts current systick value to ascending microseconds. Assumes systick was
 * enabled with AHB/8 clock, likely via `systick_enable_passive()`.
 *
 * Remember that this wraps around!
 */
uint32_t systick_passive_value_microseconds(void);

/**
 * Uses systick counter to delay by a given amount of microseconds.
 *
 * This function handles wraparound, but care must be taken not to choose too
 * high of a value for the delay. For STM32, the max count value is 2^24 - 1 ~=
 * 16.7e6, which on a 16 MHz clock (/8 by default, so 2 MHz) we will hit that
 * count every 8 seconds or so, so any delay longer than 8 seconds will probably
 * produce a delay much shorter than 8 seconds.
 */
void systick_delay_microseconds(uint32_t microseconds);

#else
  #error "Unknown board. Can't set up system_clock.h"
#endif
