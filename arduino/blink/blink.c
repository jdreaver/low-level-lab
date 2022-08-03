#include <stdint.h>

// On the ATMEGA2560, the builtin LED is on pin 13 just like the Arduino Uno.
// However, digital pin 13 goes to PB7 (see
// https://docs.arduino.cc/hacking/hardware/PinMapping2560)
#define LED_PORTB PORTB7
#define LED_DDB DDB7

#define DDRB (*(volatile uint8_t *)0x24)
#define DDB7 (1 << 7)
#define LED_DDB DDB7

#define PORTB (*(volatile uint8_t *)0x25)
#define PORTB7 (1 << 7)
#define LED_PORTB PORTB7

int main()
{
	// Initialize LED to output mode
	DDRB |= LED_DDB;

	while(1) {
		// Set pin to HIGH (turn on LED). Also loop here to simulate delay.
		for (uint32_t i = 0; i < 1000000; i++) {
			PORTB |= LED_PORTB;
		}

		// Set pin to LOW (turn off LED)
		for (uint32_t i = 0; i < 1000000; i++) {
			PORTB &= ~LED_PORTB;
		}
	}
}
