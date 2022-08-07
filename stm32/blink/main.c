#include "user_led.h"

#include "stm32f4xx.h"

// This could be a #define if we really wanted, but I like having some data here
// to test the linker script.
const uint32_t loop_length = 500000;

void start(void)
{
	user_led_enable();

	while (1) {
		user_led_turn_on();
		for (uint32_t i = 0; i < loop_length; i++) {
			asm("nop");
		}

		user_led_turn_off();
		for (uint32_t i = 0; i < loop_length; i++) {
			asm("nop");
		}
	}
}
