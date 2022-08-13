/**
 * Core definitions for system clocks.
 */

#pragma once

#ifdef STM32F401xE

#include "stm32f4xx.h"

#define HSI_CLOCK_HZ 16000000UL

// The default system clock is the HSI clock, which is 16 MHz
#define DEFAULT_SYSTEM_CLOCK_HZ HSI_CLOCK_HZ

#else
  #error "Unknown board. Can't set up system_clock.h"
#endif
