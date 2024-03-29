#include "user_button.h"

#ifdef STM32F401xE

#include "stm32f4xx.h"

#include <stdint.h>

void user_button_enable(void)
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
	EXTI->IMR  |=  (1 << USER_BUTTON_PIN);
	// Disable the 'rising edge' trigger (button release).
	EXTI->RTSR &= ~(1 << USER_BUTTON_PIN);
	// Enable the 'falling edge' trigger (button press).
	EXTI->FTSR |=  (1 << USER_BUTTON_PIN);

	// Enable EXTI15_10 interrupt line for button
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

#else
  #error "Unknown board. Can't set up user_button.c"
#endif
