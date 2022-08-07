/** Single digit 7 segment display
 *
 * This program operates in two modes:
 * 1. Glyph mode: cycles through the numbers 0-9 and the lower right dot on the
 *    display
 * 2. Cycle mode: cycles through the segments on the display
 *
 * Pressing the user button resets the current counter and also changes the
 * display mode.
 *
 * Example inspired from
 * https://create.arduino.cc/projecthub/stannano/one-digit-7-segment-led-display-70b1a0
 *
 * My Elegoo kit came with a 5161AH. Data sheet here:
 * - http://www.xlitx.com/datasheet/5161AH.pdf
 * - http://www.lanpade.com/7-segment-led-dot-matrix/5161ah.html
 *
 * Here is how the pins were lined up:
 *
 * 7 6 G 5 4
 * | | | | |
 * _ _ _ _ _
 * |  __   |
 * | |  |  |
 * | |__|  | lined up on breadboard -> 7 6 5 4 3 2 1 0
 * | |  |  |
 * | |__|  |
 * _ _ _ . _
 * | | | | |
 * 3 2 G 1 0
 *
 * where G is ground, and the pins are ordered from the bottom up on the Nucleo
 * starting from the third CN9 pin (can't use the first two, PA3 and PA2,
 * because they are there for debugging).
 *
 * - 0: CN9 2 (PA10)
 * - 0: CN9 3 (PB3)
 * - 0: CN9 4 (PB5)
 * - 0: CN9 5 (PB4)
 * - 0: CN9 6 (PB10)
 * - 0: CN9 7 (PA8)
 * - 0: CN5 0 (PA9)
 * - 0: CN5 2 (PB6)
 *
 * The numbers above correspond to the segments:
 *
 *    ___5___
 *   |       |
 * 6 |       | 4
 *   |___7___|
 *   |       |
 * 3 |       | 1
 *   |_______|    0
 *       2        . (the bottom right dot thing)
 *
**/

#include "stm32f4xx.h"

#include <stdbool.h>

// Set up prescaler values so we have a 1ms tick. Note that the TIMx_PSC
// registers are 16 bit values, so the prescaler values need to be in between 1
// and 65536. Also note that the chip adds 1 to TIMx_PSC to avoid divide by
// zero.
//
// We set the prescalar value to 16000, which takes our 16 MHz clock down to
// 1000 Hz ticks. This also lets us express the blink interval in integer
// milliseconds.
#define PRESCALER_VALUE         (16000 - 1)
#define ONE_SECOND_COUNTER      500

static volatile uint8_t counter = 0;

// Two different modes to operate in
enum visual_mode {
	VISUAL_MODE_GLYPHS,
	VISUAL_MODE_CYCLE,
};

volatile enum visual_mode current_mode = VISUAL_MODE_GLYPHS;

// Simple tuple to hold ports so we can flip them
struct gpio_odr_port {
	GPIO_TypeDef *gpio_odr_reg;
	uint32_t odr_mask;
};

// Ordered pins corresponding to numbers in the comment for this file
struct gpio_odr_port gpio_odr_ports[8] = {
	{ GPIOA, GPIO_ODR_OD10 },
	{ GPIOB, GPIO_ODR_OD3 },
	{ GPIOB, GPIO_ODR_OD5 },
	{ GPIOB, GPIO_ODR_OD4 },
	{ GPIOB, GPIO_ODR_OD10 },
	{ GPIOA, GPIO_ODR_OD8 },
	{ GPIOA, GPIO_ODR_OD9 },
	{ GPIOB, GPIO_ODR_OD6 },
};

// Segments to turn on for a given number (10 is the dot in the bottom right)
bool glyphs[11][8] = {
	{ 0, 1, 1, 1, 1, 1, 1, 0 }, // 0
	{ 0, 1, 0, 0, 1, 0, 0, 0 }, // 1
	{ 0, 0, 1, 1, 1, 1, 0, 1 }, // 2
	{ 0, 1, 1, 0, 1, 1, 0, 1 }, // 3
	{ 0, 1, 0, 0, 1, 0, 1, 1 }, // 4
	{ 0, 1, 1, 0, 0, 1, 1, 1 }, // 5
	{ 0, 1, 1, 1, 0, 1, 1, 1 }, // 6
	{ 0, 1, 0, 0, 1, 1, 0, 0 }, // 7
	{ 0, 1, 1, 1, 1, 1, 1, 1 }, // 8
	{ 0, 1, 0, 0, 1, 1, 1, 1 }, // 9
	{ 1, 0, 0, 0, 0, 0, 0, 0 }, // 10
};

void show_glyph(bool glyph[8])
{
	for (uint8_t segment = 0; segment < 8; segment++) {
		struct gpio_odr_port port = gpio_odr_ports[segment];
		if (glyph[segment]) {
			port.gpio_odr_reg->ODR |= port.odr_mask;
		} else {
			port.gpio_odr_reg->ODR &= ~(port.odr_mask);
		}
	}
}

// User button (blue button) on STM32F401RE is located in PC13, which is pin 13,
// so we configure EXTI13, which is on EXTICR4 (for pins 12 through 15).
#define BUTTON_PIN 13

// TODO: Move this to a library file
void enable_user_button(void)
{
	// Enable GPIOC clock for button
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	// Enable SYSCFG, which is in APB2, so we can configure EXTI (extended
	// interrupts) so we can listen to the User button on the board.
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// N.B. GPIOC13 is configured correctly by default. MODER defaults to 00
	// (input), and PUPDR defaults to no pull-up/pull-down.

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

	// Enable EXTI15_10 interrupt line for button
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void start(void)
{
	enable_user_button();

	// Enable TIM2 clock and interrupt line
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	NVIC_EnableIRQ(TIM2_IRQn);

	// All of our pins are on GPIOA and GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	// Set all these suckers to output
	GPIOA->MODER |= GPIO_MODER_MODER10_0;
	GPIOA->MODER &= ~(GPIO_MODER_MODER10_1);

	GPIOB->MODER |= GPIO_MODER_MODER3_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODER3_1);

	GPIOB->MODER |= GPIO_MODER_MODER5_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODER5_1);

	GPIOB->MODER |= GPIO_MODER_MODER4_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODER4_1);

	GPIOB->MODER |= GPIO_MODER_MODER10_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODER10_1);

	GPIOA->MODER |= GPIO_MODER_MODER8_0;
	GPIOA->MODER &= ~(GPIO_MODER_MODER8_1);

	GPIOA->MODER |= GPIO_MODER_MODE9_0;
	GPIOA->MODER &= ~(GPIO_MODER_MODER9_1);

	GPIOB->MODER |= GPIO_MODER_MODER6_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODER6_1);

	// Set up TIM2 for 1Hz interrupts
	TIM2->ARR = ONE_SECOND_COUNTER - 1;
	TIM2->PSC = PRESCALER_VALUE;
	TIM2->EGR |= TIM_EGR_UG; // Tell clock to update. Is this even needed?
	TIM2->DIER |= TIM_DIER_UIE; // Enable hardware interrupt
	TIM2->CR1 |= TIM_CR1_CEN; // Enable counter (must be done at end)
}

void TIM2_IRQHandler(void)
{
        // Check the update flag. It'll be set every time the timer overflows
        if(TIM2->SR & TIM_SR_UIF) {
		if (current_mode == VISUAL_MODE_GLYPHS) {
			show_glyph(glyphs[counter]);
			counter = (counter + 1) % 11;
		} else {
			bool glyph[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			for (uint8_t segment = 0; segment < 8; segment++) {
				if (segment == counter)
					glyph[segment] = 1;
			}
			show_glyph(glyph);
			counter = (counter + 1) % 8;
		}

		// Reset the flag, so that we can catch the next tick
		TIM2->SR &= ~(TIM_SR_UIF);
        }
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI->PR & (1 << BUTTON_PIN)) {
		// Clear the EXTI status flag.
		EXTI->PR |= (1 << BUTTON_PIN);

		// Change glyph mode
		switch (current_mode) {
		case VISUAL_MODE_GLYPHS:
			current_mode = VISUAL_MODE_CYCLE;
			break;
		case VISUAL_MODE_CYCLE:
			current_mode = VISUAL_MODE_GLYPHS;
			break;
		}

		// Reset counter
		counter = 0;
	}
}
