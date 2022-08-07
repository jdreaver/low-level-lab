#include "user_button.h"

#include "stm32f4xx.h"

#include <stdbool.h>
#include <stdio.h>

// Set up prescaler values. Note that the TIMx_PSC registers are 16 bit values,
// so the prescaler values need to be in between 1 and 65536. Also note that the
// chip adds 1 to TIMx_PSC to avoid divide by zero.
//
// We set the prescalar value to 16000, which takes our 16 MHz clock down to
// 1000 Hz ticks. This also lets us express the blink interval in integer
// milliseconds.
#define PRESCALER_VALUE         (16000 - 1)
#define FAST_BLINK_INTERVAL_MS  100
#define SLOW_BLINK_INTERVAL_MS  500
static volatile bool clock_fast = false;

// Set up USART. Default system clock is HSI clock source, which is 16MHz
#define BAUD_RATE 9600
#define DEFAULT_HSI_CLOCK_HZ 16000000UL

void reset_TIM2()
{
	// Disable time counter
	TIM2->CR1 &= ~(TIM_CR1_CEN);

	// Next, reset the peripheral
	RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
	RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);

	// Set TIM2 ARR. This is what the counter will count up to before it
	// triggers the interrupt. This value is actually 32 bits for TIM2 (and
	// TIM5). The "- 1" is because the counter starts at zero.
	if (clock_fast) {
		TIM2->ARR = FAST_BLINK_INTERVAL_MS - 1;
	} else {
		TIM2->ARR = SLOW_BLINK_INTERVAL_MS - 1;
	}

	// Set TIM2 prescaler (computed above)
	TIM2->PSC = PRESCALER_VALUE;

	// Send an update event to reset the timer and apply settings.
	TIM2->EGR  |= TIM_EGR_UG;

	// Enable the hardware interrupt.
	TIM2->DIER |= TIM_DIER_UIE;

	// Enable TIM2 counter (should be done at the very end)
	TIM2->CR1 |= TIM_CR1_CEN;
}

void usart2_enable(void)
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

// Override the 'write' clib method to implement 'printf' over UART.
int _write(__attribute__((unused)) int handle, char* data, int size) {
	int count = size;
	while(count--) {
		while(!(USART2->SR & USART_SR_TXE)) {};
		USART2->DR = *data++;
	}
	return size;
}

void start(void)
{
	user_button_enable();
	usart2_enable();

	// Enable GPIOA clock for LED
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// Enable PA5 (pin 5 of GPIOA) as an output bit, which controls the LED.
	// (All of GPIOA_MODER is set to 0 on reset, so we just need to set our
	// one bit to set MODER5 to 01.)
	GPIOA->MODER |= GPIO_MODER_MODER5_0;
	GPIOA->MODER &= ~(GPIO_MODER_MODER5_1);

	// Enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Enable TIM2 interrupt line
	NVIC_EnableIRQ(TIM2_IRQn);

	reset_TIM2();

	puts("start() is done. Waiting for interrupt.\r\n");

	// While loop is useful if debugging w/ GDB because we can immediately
	// have a stack frame and backtrace, so we can e.g. query for variable
	// values.
	// while (1);
}

void TIM2_IRQHandler(void)
{
        // Check the update flag. It'll be set every time the timer overflows
        if(TIM2->SR & TIM_SR_UIF) {
		// Reset the flag, so that we can catch the next overflow
		TIM2->SR &= ~TIM_SR_UIF;

		// Toggle LED pin
		GPIOA->ODR ^= GPIO_ODR_OD5;
        }
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI->PR & (1 << USER_BUTTON_PIN)) {
		// Clear the EXTI status flag.
		EXTI->PR |= (1 << USER_BUTTON_PIN);

		// Toggle the clock speed variable and reset clock
		clock_fast = !clock_fast;
		reset_TIM2();
	}
}
