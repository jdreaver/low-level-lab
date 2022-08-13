#include "system_clock.h"
#include "user_led.h"
#include "usart2.h"

#include "stm32f4xx.h"

#include <stdio.h>

// This could be a #define if we really wanted, but I like having some data here
// to test the linker script.
const uint32_t delay_milliseconds = 500;

// Override the 'write' clib method to implement 'printf' over UART.
int _write(__attribute__((unused)) int handle, char* data, int size) {
	return usart2_write(handle, data, size);
}

void start(void)
{
	usart2_enable();
	user_led_enable();
	systick_enable_passive();

	while (1) {
		user_led_turn_on();
		printf("led on\r\n");
		systick_delay_microseconds(1000 * delay_milliseconds);

		user_led_turn_off();
		printf("led off\r\n");
		systick_delay_microseconds(1000 * delay_milliseconds);
	}
}
