/**
 * Enable USART2 interface.
 *
 * To use this module, you must call `usart2_enable()`. You probably also want
 * to override _write so newlib knows where to send e.g. puts() and printf()
 * output (we can't/shouldn't do this in a header-only library).
 *
 *     // Override the 'write' newlib method to implement 'printf' over USART.
 *     int _write(int handle, char* data, int size) {
 *         return usart2_write(handle, data, size);
 *     }
 */

#pragma once

#ifdef STM32F401xE

#include "stm32f4xx.h"

// Default system clock is HSI clock source, which is 16MHz
#define BAUD_RATE 9600
#define DEFAULT_HSI_CLOCK_HZ 16000000UL

static void usart2_enable(void)
{
	// Pins PA3 and PA2 are the USART pins on STM32F401RE. We need to enable
	// GPIOA and set the MODER for PA2/PA3 to Alternate Function Mode (10).
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= (0b10 << GPIO_MODER_MODE2_Pos);
	GPIOA->MODER |= (0b10 << GPIO_MODER_MODE3_Pos);

	// Need to configure what alternate function the pins will perform. See
	// Figure 17. Selecting an alternate function on STM32F401xB/C and
	// STM32F401xD/E on pg 151 of reference manual. Need to set these to AF7
	// for USART2.
	GPIOA->AFR[0] |= (0b0111 << GPIO_AFRL_AFSEL2_Pos);
	GPIOA->AFR[0] |= (0b0111 << GPIO_AFRL_AFSEL3_Pos);

	// USART2 is located on APB1
	RCC->APB1ENR |= (RCC_APB1ENR_USART2EN);

	// Enable USART2
	USART2->CR1 |= USART_CR1_UE; // USART enable
	USART2->CR1 |= USART_CR1_TE; // Transmitter enable
	USART2->CR1 |= USART_CR1_RE; // Receiver enable

	// Compute baud rate register
	uint16_t uart_div = DEFAULT_HSI_CLOCK_HZ / BAUD_RATE;
	uint16_t brr = (uart_div / 16) << USART_BRR_DIV_Mantissa_Pos;
	brr |= (uart_div % 16) << USART_BRR_DIV_Fraction_Pos;
	USART2->BRR = brr;

	// Enable the USART peripheral.
	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
}

static int usart2_write(__attribute__((unused)) int handle, char* data, int size) {
	int count = size;
	while(count--) {
		while(!(USART2->SR & USART_SR_TXE)) {};
		USART2->DR = *data++;
	}
	return size;
}

#else
  #error "Unknown board. Can't define USER_BUTTON_PIN."
#endif
