#include "stm32f401xe.h"

// This could be a #define if we really wanted, but I like having some data here
// to test the linker script.
const uint32_t loop_length = 500000;

int start(void)
{
	// Need to enable the AHB1 peripheral clock. See Section 6.3.9 RCC AHB1
        // peripheral clock enable register (RCC_AHB1ENR) in the Reference
        // Manual. AHB1 is offset 0x30 from RCC, and GPIOA is bit 0 in register.
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// Enable PA5 (pin 5 of GPIOA) as an output bit, which controls the LED.
	// (All of GPIOA_MODER is set to 0 on reset, so we just need to set our
	// one bit to set MODER5 to 01.)
	//
        // From User Manual section 6.4 LEDs: "User LD2: the green LED is a user
        // LED connected to ARDUINO® signal D13 corresponding to STM32 I/O PA5
        // (pin 21) or PB13 (pin 34) depending on the STM32 target. Refer to
        // Table 11 to Table 23 when: - the I/O is HIGH value, the LED is on -
        // the I/O is LOW, the LED is off"
        //
        // Figure 18. NUCLEO-F401RE and Table 16. ARDUINO® connectors on NUCLEO-F401RE
        // and NUCLEO-F411RE have PA5 next to D13, and PA5 == Pin 5 of GPIOA
	//
	// (Information needed if we weren't using stm32f401xe.h header files)
	// MODER is offset of 0x0 from register base. MODER5 is bits 10/11 (see
	// 8.4.1 GPIO port mode register (GPIOx_MODER) (x = A..E and H) on page
	// 158 of Reference Manual). Need to set bits 10/11 to 01 for output
	// mode.
	GPIOA->MODER |= GPIO_MODER_MODER5_0;

	// (Information needed if we weren't using stm32f401xe.h header files)
	// ODR is offset 0x14 from GPIO base, and we want pin 5, which is the
	// 5th bit. See 8.4.6 GPIO port output data register (GPIOx_ODR) (x =
	// A..E and H) on page 160 of reference manual.
	GPIOA->ODR |= GPIO_ODR_OD5;
	while (1) {
		// Turn LED on
		GPIOA->ODR |= GPIO_ODR_OD5;
		for (uint32_t i = 0; i < loop_length; i++) {
			asm("nop");
		}

		// Turn LED off
		GPIOA->ODR &= ~GPIO_ODR_OD5;
		for (uint32_t i = 0; i < loop_length; i++) {
			asm("nop");
		}
	}
}
