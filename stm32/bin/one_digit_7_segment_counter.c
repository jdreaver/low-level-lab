/**
 * Single digit 7 segment display counter.
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
 * See seven_segment_display.h for pin diagram.
 *
 * Here is how I hooked the pins up. The pins are ordered from the top down on
 * the Nucleo (note we can't use PA3 and PA2 because they are there for
 * debugging). I chose PA and PB pins so we only needed two GPIO ports:
 *
 * - a: CN5 2 (PB6)
 * - b: CN5 0 (PA9)
 * - c: CN9 7 (PA8)
 * - d: CN9 6 (PB10)
 * - e: CN9 5 (PB4)
 * - f: CN9 4 (PB5)
 * - g: CN9 3 (PB3)
 * - DP: CN9 2 (PA10)
**/

#include "seven_segment_display.h"
#include "tim2.h"
#include "user_button.h"

#include "stm32f4xx.h"

#include <stdbool.h>

#define HALF_SECOND_COUNTER 500

static volatile uint8_t counter = 0;

// Two different modes to operate in
enum visual_mode {
	VISUAL_MODE_GLYPHS,
	VISUAL_MODE_CYCLE,
};

volatile enum visual_mode current_mode = VISUAL_MODE_GLYPHS;

// Ordered pins corresponding to numbers in the comment for this file
struct seven_segment_display display = {
	.segment_pins = {
		{ &GPIOB->ODR, GPIO_ODR_OD6 },
		{ &GPIOA->ODR, GPIO_ODR_OD9 },
		{ &GPIOA->ODR, GPIO_ODR_OD8 },
		{ &GPIOB->ODR, GPIO_ODR_OD10 },
		{ &GPIOB->ODR, GPIO_ODR_OD4 },
		{ &GPIOB->ODR, GPIO_ODR_OD5 },
		{ &GPIOB->ODR, GPIO_ODR_OD3 },
		{ &GPIOA->ODR, GPIO_ODR_OD10 },
	},
};

void start(void)
{
	user_button_enable();

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

	// N.B. Set up timer after GPIO so we see output immediately
	tim2_enable();
	tim2_set_timeout_ms(HALF_SECOND_COUNTER);
}

void TIM2_IRQHandler(void)
{
        // Check the update flag. It'll be set every time the timer overflows
        if(TIM2->SR & TIM_SR_UIF) {
		if (current_mode == VISUAL_MODE_GLYPHS) {
			seven_segment_display_show_digit_glyph(&display, counter);
			counter = (counter + 1) % SEVEN_SEGMENT_DISPLAY_NUM_GLYPHS;
		} else {
			seven_segment_display_clear(&display);
			seven_segment_display_show_segment(&display, counter);
			counter = (counter + 1) % SEVEN_SEGMENT_DISPLAY_NUM_SEGMENTS;
		}

		// Reset the flag, so that we can catch the next tick
		TIM2->SR &= ~(TIM_SR_UIF);
        }
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI->PR & (1 << USER_BUTTON_PIN)) {
		// Clear the EXTI status flag.
		EXTI->PR |= (1 << USER_BUTTON_PIN);

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

		// Restart timer for more immediate feedback
		tim2_set_timeout_ms(HALF_SECOND_COUNTER);
	}
}
