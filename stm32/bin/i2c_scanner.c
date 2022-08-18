/**
 * Scans for any I2C peripherals and prints addresses we find to UART.
 */

#include "system_clock.h"
#include "tim2.h"
#include "usart2.h"

#include "stm32f4xx.h"

#include <stdio.h>

// Override the 'write' clib method to implement 'printf' over UART.
int _write(__attribute__((unused)) int handle, char* data, int size) {
	return usart2_write(handle, data, size);
}

void start(void)
{
	usart2_enable();
	tim2_enable();

	// Enable I2C1
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	// PB9 is I2C1_SDA, PB8 is I2C1_SCL. Enable GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	// The following is the required sequence in master mode.
	// - Program the peripheral input clock in I2C_CR2 Register in order to generate correct
	//   timings
	// - Configure the clock control registers
	// - Configure the rise time register
	// - Program the I2C_CR1 register to enable the peripheral
	// - Set the START bit in the I2C_CR1 register to generate a Start condition

	// APB1 uses default system clock, which is 16MHz. We set FREQ to the
	// frequency in MHz.
	uint8_t cr2_clock_freq = DEFAULT_SYSTEM_CLOCK_HZ / 1000000UL;
	I2C1->CR2 |= ((cr2_clock_freq << I2C_CR2_FREQ_Pos) & I2C_CR2_FREQ_Msk);

	// Configure clock control. We just assume standard mode (100KHz). In
	// standard mode, T_high = T_low = CCR * T_PCLK1, where T_PCLK1 is the
	// time between clock pulses (1 / freq). A 100 KHz clock has a cycle of
	// 10 microseconds, so T_high = T_low = 5 microseconds. Thus:
	//     CCR = 5 microseconds / T_PCLK1
	//         = 5 * 10^-6 * f_PCLK1
        //         = 5 * f_PCLK1 / 1000000
        //         = 80 (assuming 16 MHz clock speed)
	//
	// Good explanation, even uses 16 MHz:
	// http://fastbitlab.com/stm32-i2c-lecture-12-i2c-serial-clock-settings-with-explanations/
	I2C1->CCR &= ~(I2C_CCR_FS); // 0 == standard mode
	uint16_t ccr_val = 5UL * DEFAULT_SYSTEM_CLOCK_HZ / 1000000UL;
	I2C1->CCR |= ((ccr_val << I2C_CCR_CCR_Pos) & I2C_CCR_CCR_Msk);

	// Configure TRISE. Good explanation:
	// http://fastbitlab.com/i2c-rise-time-calculation/
	//
	// Standard mode, so max is 1000ns. We can compute TRISE as:
	//     TRISE = 1000 ns / T_PCLK1 + 1
	//           = 1000e-9 * freq + 1
        //           = freq / 1000000 + 1
	//           = 16 + 1 = 17 (for 16 MHz clock)
	uint16_t trise = DEFAULT_SYSTEM_CLOCK_HZ / 1000000UL + 1;
	I2C1->TRISE = ((trise << I2C_TRISE_TRISE_Pos) & I2C_TRISE_TRISE_Msk);

	// Enable I2C
	I2C1->CR1 |= I2C_CR1_PE;

	// Generate START
	I2C1->CR1 |= I2C_CR1_START;

	// Wait for SB (Status Busy) flag
	while (I2C1->SR1 & I2C_SR1_SB);

	// TODO: Check for timeout flag?

	// Send slave address and set last bit 0 for write (NOTE: DS1307 clock
	// address is 0b1101000)
	uint8_t address = 0b1101000;
	uint8_t address_data = (address << 1);
	address_data &= ~(1); // Unset last bit
	I2C1->DR = address_data;

	// TODO: Everything below here is hokey. Left off
	// fastbitlab.com/implementation-of-i2c-master-sending-data-api-part-4/

	// Wait for ADDR flag
	while (!(I2C1->SR1 & I2C_SR1_ADDR));

}

void TIM2_IRQHandler(void)
{
        // Check the update flag. It'll be set every time the timer overflows
        if(TIM2->SR & TIM_SR_UIF) {
		// Reset the flag, so that we can catch the next overflow
		TIM2->SR &= ~TIM_SR_UIF;

		// TODO: Scan
        }
}
