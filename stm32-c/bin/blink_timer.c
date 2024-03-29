#include "tim2.h"
#include "usart2.h"
#include "user_button.h"
#include "user_led.h"

#include "stm32f4xx.h"

#include <stdbool.h>
#include <stdio.h>

#define FAST_BLINK_INTERVAL_MS  100
#define SLOW_BLINK_INTERVAL_MS  500

static volatile bool clock_fast = false;

// Override the 'write' clib method to implement 'printf' over UART.
int _write(__attribute__((unused)) int handle, char* data, int size) {
	return usart2_write(handle, data, size);
}

void reset_tim2()
{
	if (clock_fast) {
		tim2_set_timeout_ms(FAST_BLINK_INTERVAL_MS);
	} else {
		tim2_set_timeout_ms(SLOW_BLINK_INTERVAL_MS);
	}
}

void start(void)
{
	user_button_enable();
	user_led_enable();
	usart2_enable();

	// N.B. Set up clock after LED so we see output on first tick.
	tim2_enable();
	reset_tim2();

	printf("start() is done. Waiting for interrupt.\r\n");

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

		user_led_toggle();
        }
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI->PR & (1 << USER_BUTTON_PIN)) {
		// Clear the EXTI status flag.
		EXTI->PR |= (1 << USER_BUTTON_PIN);

		// Toggle the clock speed variable and reset clock
		clock_fast = !clock_fast;
		reset_tim2();
	}
}
