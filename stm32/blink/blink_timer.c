#include <stdbool.h>

#include "stm32f401xe.h"

// Set up prescaler values. Note that the TIMx_PSC registers are 16 bit values,
// so the prescaler values need to be in between 1 and 65536. Also note that the
// chip adds 1 to TIMx_PSC to avoid divide by zero.
//
// We set the prescalar value to 16000, which takes our 16 MHz clock down to
// 1000 Hz ticks. This also lets us express the blink interval in integer
// milliseconds.
#define PRESCALER_VALUE         (16000 - 1)
#define FAST_BLINK_INTERVAL_MS  100
#define SLOW_BLINK_INTERVAL_MS  500
static volatile bool clock_fast = false;

// User button (blue button) on STM32F401RE is located in PC13, which is pin 13,
// so we configure EXTI13, which is on EXTICR4 (for pins 12 through 15).
#define BUTTON_PIN 13

void reset_TIM2()
{
	// Disable time counter
	TIM2->CR1 &= ~(TIM_CR1_CEN);

	// Next, reset the peripheral
	RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
	RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);

	// Set TIM2 ARR. This is what the counter will count up to before it
	// triggers the interrupt. This value is actually 32 bits for TIM2 (and
	// TIM5). The "- 1" is because the counter starts at zero.
	if (clock_fast) {
		TIM2->ARR = FAST_BLINK_INTERVAL_MS - 1;
	} else {
		TIM2->ARR = SLOW_BLINK_INTERVAL_MS - 1;
	}

	// Set TIM2 prescaler (computed above)
	TIM2->PSC = PRESCALER_VALUE;

	// Send an update event to reset the timer and apply settings.
	TIM2->EGR  |= TIM_EGR_UG;

	// Enable the hardware interrupt.
	TIM2->DIER |= TIM_DIER_UIE;

	// Enable TIM2 counter (should be done at the very end)
	TIM2->CR1 |= TIM_CR1_CEN;
}

int start(void)
{
	// Enable GPIOA clock for LED
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// Enable GPIOC clock for button
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	// Enable PA5 (pin 5 of GPIOA) as an output bit, which controls the LED.
	// (All of GPIOA_MODER is set to 0 on reset, so we just need to set our
	// one bit to set MODER5 to 01.)
	GPIOA->MODER |= GPIO_MODER_MODER5_0;

	// N.B. GPIOC13 is configured correctly by default. MODER defaults to 00
	// (input), and PUPDR defaults to no pull-up/pull-down.

	// Enable SYSCFG, which is in APB2, so we can configure EXTI (extended
	// interrupts) so we can listen to the User button on the board.
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// Set SYSCFG to connect the button EXTI line to GPIOC (button is on pin
	// 2, which is PC13).
	//
	// TODO: Abstract selecting the correct EXTICR[] index based on pin
	// number.
	SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI13_Msk);
	SYSCFG->EXTICR[3] |=  (SYSCFG_EXTICR4_EXTI13_PC);

	// Setup the button's EXTI line as an interrupt.
	EXTI->IMR  |=  (1 << BUTTON_PIN);
	// Disable the 'rising edge' trigger (button release).
	EXTI->RTSR &= ~(1 << BUTTON_PIN);
	// Enable the 'falling edge' trigger (button press).
	EXTI->FTSR |=  (1 << BUTTON_PIN);

	// Enable EXTI15_10 interrupt line
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	// Enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Enable TIM2 interrupt line
	NVIC_EnableIRQ(TIM2_IRQn);

	reset_TIM2();

	// While loop is useful if debugging w/ GDB because we can immediately
	// have a stack frame and backtrace, so we can e.g. query for variable
	// values.
	// while (1);
}

void TIM2_IRQHandler(void)
{
        // Check the update flag. It'll be set every time the timer overflows
        if(TIM2->SR & TIM_SR_UIF) {
            // Reset the flag, so that we can catch the next overflow
            TIM2->SR &= ~TIM_SR_UIF;

            // Toggle LED pin
	    GPIOA->ODR ^= GPIO_ODR_OD5;
        }
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI->PR & (1 << BUTTON_PIN)) {
		// Clear the EXTI status flag.
		EXTI->PR |= (1 << BUTTON_PIN);

		// Toggle the clock speed variable and reset clock
		clock_fast = !clock_fast;
		reset_TIM2();
	}
}
