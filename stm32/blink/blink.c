#include <stdint.h>

// From User Manual section 6.4 LEDs: "User LD2: the green LED is a user LED
// connected to ARDUINO® signal D13 corresponding to STM32 I/O PA5 (pin 21) or
// PB13 (pin 34) depending on the STM32 target. Refer to Table 11 to Table 23
// when:
// - the I/O is HIGH value, the LED is on
// - the I/O is LOW, the LED is off"
//
// Figure 18. NUCLEO-F401RE and Table 16. ARDUINO® connectors on NUCLEO-F401RE
// and NUCLEO-F411RE have PA5 next to D13, and PA5 == Pin 5 of GPIOA


// Register addresses (page 52 of STM32F401RE data sheet, page 38 of reference
// manual)
#define PERIPH_BASE           0x40000000UL /*!< Peripheral base address in the alias region */
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)

// Need to enable the AHB1 peripheral clock. See Section 6.3.9 RCC AHB1
// peripheral clock enable register (RCC_AHB1ENR) in the Reference Manual. AHB1
// is offset 0x30 from RCC, and GPIOA is bit 0 in register.
#define RCC_AHB1ENR         *(volatile uint32_t *)(RCC_BASE + 0x30)
#define RCC_AHB1ENR_GPIOAEN (1UL << 0)

// MODER is offset of 0x0 from register base. MODER5 is bits 10/11 (see 8.4.1
// GPIO port mode register (GPIOx_MODER) (x = A..E and H) on page 158 of
// Reference Manual). Need to set bits 10/11 to 01 for output mode.
#define GPIOA_MODER         *(volatile uint32_t *)(GPIOA_BASE + 0x00)
#define GPIO_MODER_MODER5_0 (1UL << 10)

// ODR is offset 0x14 from GPIO base, and we want pin 5, which is the 5th bit.
// See 8.4.6 GPIO port output data register (GPIOx_ODR) (x = A..E and H) on page
// 160 of reference manual.
#define GPIOA_ODR     *(volatile uint32_t *)(GPIOA_BASE + 0x14)
#define GPIO_ODR_OD5  (1UL << 5)

// This could be a #define if we really wanted, but I like having some data here
// to test the linker script.
const uint32_t loop_length = 500000;

void Reset_Handler(void)
{
	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// Enable PA5 (pin 5 of GPIOA) as an output bit, which controls the LED.
	// (All of GPIOA_MODER is set to 0 on reset, so we just need to set our
	// one bit to set MODER5 to 01.)
	GPIOA_MODER |= GPIO_MODER_MODER5_0;

	GPIOA_ODR |= GPIO_ODR_OD5;
	while (1) {
		// Turn LED on
		GPIOA_ODR |= GPIO_ODR_OD5;
		for (uint32_t i = 0; i < loop_length; i++); // arbitrary delay

		// Turn LED off
		GPIOA_ODR &= ~GPIO_ODR_OD5;
		for (uint32_t i = 0; i < loop_length; i++); // arbitrary delay
	}
}

/* Vector table. This needs to be stored at 0x00000004, which with the default
   BOOT pin configuration is aliased to flash memory at 0x08000004. Note that
   the first element (stack pointer location) is set in the linker script. */
unsigned long *vector_table[] __attribute__((section(".vector_table"))) =
{
    (unsigned long *)Reset_Handler,
};
