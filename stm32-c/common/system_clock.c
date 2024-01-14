#include "system_clock.h"

#ifdef STM32F401xE

#include "stm32f4xx.h"

// N.B. If we wanted to have systick create interrupt every 500ms and enable it,
// we could use the CMSIS function:
// SysTick_Config(DEFAULT_SYSTEM_CLOCK_HZ / 2);
// NVIC_EnableIRQ(SysTick_IRQn);

void systick_enable_passive(void)
{
	// Enable systick to count down from max value constantly
	SysTick->LOAD = SysTick_LOAD_RELOAD_Msk - 1UL; // -1 is part of spec b/c of countdown
	SysTick->VAL = 0UL;

	// Use AHB/8 for systick. AHB by default is just system clock, so this
	// is a bit slower than using the processor clock, but still plenty
	// fast.
	SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk);

	// **Don't** generate interrupt.
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);

	// Enable systick counter
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

// By default systick uses AHB/8, which has a clock speed equal to the system
// default divided by 8. The factor of 1000000 is to convert to microseconds.
#define SYSTICK_MICROSECOND_FACTOR (DEFAULT_SYSTEM_CLOCK_HZ / 8 / 1000000)

uint32_t systick_passive_value_microseconds(void)
{
	// systick is a descending counter. First convert to ascending.
	uint32_t ascending_count = SysTick->LOAD - (SysTick->VAL & SysTick_VAL_CURRENT_Msk);

	return ascending_count / SYSTICK_MICROSECOND_FACTOR;
}

// TODO: Is this function too complicated? Any implications for timing? Probably
// just for really short timings.
void systick_delay_microseconds(uint32_t microseconds)
{
	uint32_t start = systick_passive_value_microseconds();
	uint32_t end = start + microseconds;

	// Check for wraparound. We will almost certainly wrap around before
	// uint32_t would wrap around.
	uint32_t max = (SysTick->LOAD + 1) / SYSTICK_MICROSECOND_FACTOR;
	end %= max;

	if (start < end) {
		while ((systick_passive_value_microseconds() >= start) && (systick_passive_value_microseconds() < end));
	} else {
		// This branch is taken if the microsecond delay would cause a wraparound.
		while ((systick_passive_value_microseconds() >= start) || (systick_passive_value_microseconds() < end));
	}
}


#else
  #error "Unknown board. Can't set up system_clock.c"
#endif
