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


void i2c_start(void)
{
	// Generate START
	I2C1->CR1 |= I2C_CR1_START;

	// Wait for SB (Start Bit) flag to get set (which means START was sent)
	while (!(I2C1->SR1 & I2C_SR1_SB));
}

void i2c_write_data(uint8_t data)
{
	// Wait for TXE bit to set, which means data register is empty
	while (!(I2C1->SR1 & I2C_SR1_TXE));

	// Load data into data register
	I2C1->DR = data;

	// Wait for byte transfer (BTF) to finish
	while (!(I2C1->SR1 & I2C_SR1_BTF));
}

void i2c_write_data_multi(uint8_t *data, size_t size)
{
	while (size--) {
		// Wait for TXE bit to set, which means data register is empty
		while (!(I2C1->SR1 & I2C_SR1_TXE));

		// Load data into data register
		I2C1->DR = *data++;
	}

	// Wait for byte transfer (BTF) to finish
	while (!(I2C1->SR1 & I2C_SR1_BTF));
}

void i2c_write_address(uint8_t address)
{
	// N.B. I don't think we need this bit shift. Ignore it?
	// uint8_t address_data = (address << 1);
	// address_data &= ~(1); // Unset last bit
	// i2c_write_data(address_data);

	i2c_write_data(address);

	// Wait for ADDR flag (address was sent)
	while (!(I2C1->SR1 & I2C_SR1_ADDR));

	// Clear ADDR bit by reading SR1/SR2
	__attribute__((unused)) uint8_t temp = I2C1->SR1 | I2C1->SR2;
}

void i2c_stop(void)
{
	// Generate START
	I2C1->CR1 |= I2C_CR1_STOP;
}

void start(void)
{
	usart2_enable();
	tim2_enable();

	// Enable I2C1
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	// PB9 is I2C1_SDA, PB8 is I2C1_SCL. Enable GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	// Need to set PB8/9 to alternate function
	GPIOB->MODER |= (0b10 << GPIO_MODER_MODER8_Pos);
	GPIOB->MODER |= (0b10 << GPIO_MODER_MODER9_Pos);

	// TODO: Do I need to set "open drain" on these pins?

	// Need to configure what alternate function the pins will perform. See
	// Figure 17. Selecting an alternate function on STM32F401xB/C and
	// STM32F401xD/E on pg 151 of reference manual. Need to set these to AF4
	// for I2C1.
	GPIOB->AFR[1] |= (4 << GPIO_AFRH_AFSEL8_Pos);
	GPIOB->AFR[1] |= (4 << GPIO_AFRH_AFSEL9_Pos);

	// Reset I2C just in case
	I2C1->CR1 |= I2C_CR1_SWRST;
	I2C1->CR1 &= ~(I2C_CR1_SWRST);

	printf("Pin setup done.\r\n");

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
	printf("I2C1_CR2_FREQ = %lu\r\n", I2C1->CR2 & I2C_CR2_FREQ_Msk);

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
	// TODO: Is CCR supposed to include rise time?
	printf("I2C1_CCR = %lu\r\n", I2C1->CCR & I2C_CCR_CCR_Msk);

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
	printf("I2C1_TRISE = %lu\r\n", I2C1->TRISE & I2C_TRISE_TRISE_Msk);

	// Enable I2C
	I2C1->CR1 |= I2C_CR1_PE;

	i2c_start();

	// Send slave address and set last bit 0 for write (NOTE: DS1307 clock
	// address is 0b1101000)
	uint8_t address = 0b1101000;
	i2c_write_address(address);

	// TODO: Check for timeout flag?

	i2c_stop();
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
