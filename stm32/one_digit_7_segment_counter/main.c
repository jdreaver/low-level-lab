/**
 * Example taken from https://create.arduino.cc/projecthub/stannano/one-digit-7-segment-led-display-70b1a0
 *
 * My Elegoo kit came with a 5161AH. Data sheet here:
 * - http://www.xlitx.com/datasheet/5161AH.pdf
 * - http://www.lanpade.com/7-segment-led-dot-matrix/5161ah.html
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
#define ONE_SECOND_COUNTER      1000

static volatile uint8_t counter = 0;

void start(void)
{
	// Enable TIM2 clock and interrupt line
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	NVIC_EnableIRQ(TIM2_IRQn);

	// Set up TIM2 for 1Hz interrupts
	TIM2->ARR = ONE_SECOND_COUNTER - 1;
	TIM2->PSC = PRESCALER_VALUE;
	TIM2->EGR |= TIM_EGR_UG; // Tell clock to update. Is this even needed?
	TIM2->DIER |= TIM_DIER_UIE; // Enable hardware interrupt
	TIM2->CR1 |= TIM_CR1_CEN; // Enable counter (must be done at end)

	// We are going to use 6 pins of connector CN9 and two pins from CN5
	// (the first two pings of CN9, PA3 and PA2, are special and are used
	// for some debugging function) which are all on GPIO A and B, so we
	// need to enable those. In order, from bottom to top, they are:

	// CN9 2. PA10
	// CN9 3. PB3
	// CN9 4. PB5
	// CN9 5. PB4
	// CN9 6. PB10
	// CN9 7. PA8
	// CN5 0. PA9
	// CN5 2. PB6
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

	// Turn them all on just to test
	GPIOA->ODR |= GPIO_ODR_OD10;
	GPIOB->ODR |= GPIO_ODR_OD3;
	GPIOB->ODR |= GPIO_ODR_OD5;
	GPIOB->ODR |= GPIO_ODR_OD4;
	GPIOB->ODR |= GPIO_ODR_OD10;
	GPIOA->ODR |= GPIO_ODR_OD8;
	GPIOA->ODR |= GPIO_ODR_OD9;
	GPIOB->ODR |= GPIO_ODR_OD6;
}

void TIM2_IRQHandler(void)
{
        // Check the update flag. It'll be set every time the timer overflows
        if(TIM2->SR & TIM_SR_UIF) {
	    // Turn off all segments
	    GPIOA->ODR &= ~(GPIO_ODR_OD10);
	    GPIOB->ODR &= ~(GPIO_ODR_OD3);
	    GPIOB->ODR &= ~(GPIO_ODR_OD5);
	    GPIOB->ODR &= ~(GPIO_ODR_OD4);
	    GPIOB->ODR &= ~(GPIO_ODR_OD10);
	    GPIOA->ODR &= ~(GPIO_ODR_OD8);
	    GPIOA->ODR &= ~(GPIO_ODR_OD9);
	    GPIOB->ODR &= ~(GPIO_ODR_OD6);

	    // Only turn on one segment
	    switch (counter) {
	    case 0:
		    GPIOA->ODR |= GPIO_ODR_OD10;
		    break;
	    case 1:
		    GPIOB->ODR |= GPIO_ODR_OD3;
		    break;
	    case 2:
		    GPIOB->ODR |= GPIO_ODR_OD5;
		    break;
	    case 3:
		    GPIOB->ODR |= GPIO_ODR_OD4;
		    break;
	    case 4:
		    GPIOB->ODR |= GPIO_ODR_OD10;
		    break;
	    case 5:
		    GPIOA->ODR |= GPIO_ODR_OD8;
		    break;
	    case 6:
		    GPIOA->ODR |= GPIO_ODR_OD9;
		    break;
	    case 7:
		    GPIOB->ODR |= GPIO_ODR_OD6;
		    break;
	    }

	    // Increment counter
	    counter = (counter + 1) % 8;

            // Reset the flag, so that we can catch the next tick
            TIM2->SR &= ~TIM_SR_UIF;

        }
}
