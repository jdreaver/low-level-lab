#include "tim2.h"

#ifdef STM32F401xE

#include "system_clock.h"

#include "stm32f4xx.h"

// Set up prescaler values so we have a 1ms tick. Note that the TIMx_PSC
// registers are 16 bit values, so the prescaler values need to be in between 1
// and 65536. Also note that the chip adds 1 to TIMx_PSC to avoid divide by
// zero.
//
// Default clock speed on STM32F401RE is 16MHz via the HSI clock source. We set
// the prescalar value to 16000, which takes our 16 MHz clock down to 1000 Hz
// ticks. This also lets us express the blink interval in integer milliseconds.
#define PRESCALER_VALUE ((DEFAULT_SYSTEM_CLOCK_HZ) / 1000)

void tim2_enable(void)
{
	// Enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Enable TIM2 interrupt line
	NVIC_EnableIRQ(TIM2_IRQn);
}

void tim2_set_timeout_ms(uint32_t timeout_ms)
{
	// Disable time counter
	TIM2->CR1 &= ~(TIM_CR1_CEN);

	// Next, reset the peripheral
	RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
	RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);

	// Set TIM2 ARR. This is what the counter will count up to before it
	// triggers the interrupt. This value is actually 32 bits for TIM2 (and
	// TIM5). The "- 1" is because the counter starts at zero.
	TIM2->ARR = timeout_ms - 1;

	// Set TIM2 prescaler (computed above). "-1" is because one gets added
	// to this value to avoid divide by zero.
	TIM2->PSC = PRESCALER_VALUE - 1;

	// Send an update event to reset the timer and apply settings.
	TIM2->EGR  |= TIM_EGR_UG;

	// Enable the hardware interrupt.
	TIM2->DIER |= TIM_DIER_UIE;

	// Enable TIM2 counter (should be done at the very end)
	TIM2->CR1 |= TIM_CR1_CEN;
}

#else
  #error "Unknown board. Can't set up tim2.c"
#endif
