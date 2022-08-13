/**
 * HD44780U LCD driver module.
 *
 * Data sheet: https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 *
 * Assumes 8 bit operation mode, not 4 bit.
**/

#pragma once

#include <stdint.h>

/**
 * Memory location and mask used to set pin value for register.
 */
struct hd4478u_lcd_pin {
	volatile uint32_t *reg;
	uint32_t mask;
};

#define HD4478U_LCD_NUM_DATA_PINS 8

/**
 * Struct containing pin definitions for HD4478U LCD.
 */
struct hd4478u_lcd {
	struct hd4478u_lcd_pin rs_pin; ///< Register select (0 == instruction, 1 == data)
	struct hd4478u_lcd_pin rw_pin; ///< Read/Write (0 == write, 1 == read)
	struct hd4478u_lcd_pin e_pin;  ///< Enable (1 == start data read/write, 0 == end it)
	struct hd4478u_lcd_pin data_pins[HD4478U_LCD_NUM_DATA_PINS];
};
