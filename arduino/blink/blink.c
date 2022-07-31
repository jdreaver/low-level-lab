#include <avr/io.h>
#include <util/delay.h>

#define MS_DELAY 2000

// On the ATMEGA2560, the builtin LED is on pin 13 just like the Arduino Uno.
// However, digital pin 13 goes to PB7 (see
// https://docs.arduino.cc/hacking/hardware/PinMapping2560)
#define LED_PORTB PORTB7
#define LED_DDB DDB7

int main()
{
	// Initialize LED to output mode
	DDRB |= _BV(LED_DDB);

	while(1) {
		// Set pin to HIGH (turn on LED)
		PORTB |= _BV(LED_PORTB);
		_delay_ms(MS_DELAY);

		// Set pin to LOW (turn off LED)
		PORTB &= ~_BV(LED_PORTB);
		_delay_ms(MS_DELAY);
	}
}
