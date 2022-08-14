/**
 * HD44780U LCD driver module.
 *
 * Assumes 4 bit operation mode to save GPIO pins.
 *
 * Resources:
 * - Data sheet: https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 * - Implementation: https://github.com/Matiasus/HD44780
 * - Another implementation: https://github.com/murar8/stm32-HD44780
**/

#pragma once

#include <stdint.h>

/**
 * Memory location and mask used to set pin value for register.
 */
struct hd44780u_lcd_pin {
	volatile uint32_t *mode_reg;
	uint32_t mode_reg_clear_mask;
	uint32_t mode_reg_output_mask;
	uint32_t mode_reg_input_mask;

	volatile uint32_t *output_data_reg;
	uint32_t output_data_reg_mask;
};

#define HD44780U_LCD_NUM_DATA_PINS 4

struct hd44780u_lcd {
	void (*delay_microseconds_fn)(uint32_t microseconds);
	struct hd44780u_lcd_pin rs_pin; ///< Register select (0 == instruction, 1 == data)
	struct hd44780u_lcd_pin rw_pin; ///< Read/Write (0 == write, 1 == read)
	struct hd44780u_lcd_pin e_pin;  ///< Enable (1 == start data read/write, 0 == end it)
	struct hd44780u_lcd_pin data_pins[HD44780U_LCD_NUM_DATA_PINS]; ///< DB4-DB7 (just higher order in 4 bit mode)
};

void hd44780u_lcd_init(struct hd44780u_lcd *lcd);
void hd44780u_lcd_clear(struct hd44780u_lcd *lcd);
void hd44780u_lcd_write_char(struct hd44780u_lcd *lcd, char c);
void hd44780u_lcd_write_string(struct hd44780u_lcd *lcd, char *str);
void hd44780u_lcd_move_to_second_line(struct hd44780u_lcd *lcd);
