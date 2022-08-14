#include "hd44780u_lcd.h"

#include <stddef.h>
#include <stdbool.h>

void hd44780u_lcd_pin_config_output(struct hd44780u_lcd_pin *pin)
{
	*(pin->mode_reg) &= pin->mode_reg_clear_mask;
	*(pin->mode_reg) |= pin->mode_reg_output_mask;
}

void hd44780u_config_data_pins_output(struct hd44780u_lcd *lcd)
{
	for (size_t i = 0; i < HD44780U_LCD_NUM_DATA_PINS; i++)
		hd44780u_lcd_pin_config_output(&lcd->data_pins[i]);
}

void hd44780u_lcd_pin_config_input(struct hd44780u_lcd_pin *pin)
{
	*(pin->mode_reg) &= pin->mode_reg_clear_mask;
	*(pin->mode_reg) |= pin->mode_reg_input_mask;
}

void hd44780u_lcd_pin_set_value(struct hd44780u_lcd_pin *pin, bool value)
{
	// Assumes pin already set to output!
	if (value)
		*(pin->output_data_reg) |= pin->output_data_reg_mask;
	else
		*(pin->output_data_reg) &= ~(pin->output_data_reg_mask);
}

void e_pin_cycle(struct hd44780u_lcd *lcd)
{
	// Total E cycle time is recorded as 1 microsecond, but we add extra
	// waits to be safe.
	lcd->delay_microseconds_fn(1000);
	hd44780u_lcd_pin_set_value(&lcd->e_pin, true);
	lcd->delay_microseconds_fn(1000);
	hd44780u_lcd_pin_set_value(&lcd->e_pin, false);
	lcd->delay_microseconds_fn(1000);
}

void hd44780u_send_bits(struct hd44780u_lcd *lcd, uint8_t data, bool is_4_bit)
{
	// NOTE: Assumes pins are already set to be output!

	// Higher order bits first
	for (size_t i = 0; i < HD44780U_LCD_NUM_DATA_PINS; i++)
		hd44780u_lcd_pin_set_value(&lcd->data_pins[i], data & (1 << (i + 4)));
	e_pin_cycle(lcd);

	// Lower order bits
	if (is_4_bit) {
		for (size_t i = 0; i < HD44780U_LCD_NUM_DATA_PINS; i++)
			hd44780u_lcd_pin_set_value(&lcd->data_pins[i], data & (1 << i));
		e_pin_cycle(lcd);
	}
}

void hd44780u_send_instruction(struct hd44780u_lcd *lcd, uint8_t instruction, bool is_4_bit)
{
	hd44780u_lcd_pin_set_value(&lcd->rs_pin, false); // Instruction register
	hd44780u_lcd_pin_set_value(&lcd->rw_pin, false); // Write
	hd44780u_send_bits(lcd, instruction, is_4_bit);
}

void hd44780u_send_data(struct hd44780u_lcd *lcd, uint8_t data)
{
	hd44780u_lcd_pin_set_value(&lcd->rs_pin, true); // Data register
	hd44780u_lcd_pin_set_value(&lcd->rw_pin, false); // Write
	hd44780u_send_bits(lcd, data, true);
}

void hd44780u_lcd_clear(struct hd44780u_lcd *lcd)
{
	// 0 0 0 0 0 0 0 1 Clears entire display and sets DDRAM address 0 in
	// address counter.
	hd44780u_send_instruction(lcd, 1, true);
	lcd->delay_microseconds_fn(1000 * 1);
}

void hd44780u_config_entry_mode(struct hd44780u_lcd *lcd, bool move_direction, bool shift)
{
	// 0 0 0 0 0 1 I/D S Sets cursor move direction (I/D) and specifies
	// display shift (S). These operations are performed during data write
	// and read.
	uint8_t value = (1 << 2) | (move_direction << 1) | (shift << 0);
	hd44780u_send_instruction(lcd, value, true);

	// Wait at least 37 microseconds
	lcd->delay_microseconds_fn(1000);
}

void hd44780u_config_function_set(struct hd44780u_lcd *lcd, bool set_8_bit, bool is_4_bit)
{
	// 0 0 1 DL N F — — Sets interface data length (DL), number of display
	// lines (N), and character font (F).
	//
	// DL == 1: 8 bits
	// DL == 0: 4 bits (we want this generally, except on startup)
	// If N is 1, then F doesn't matter (it is set to 5x8 dots)
	//
	// Given these values, we want DL = 1, N = 1, and F doesn't matter
	bool data_length = set_8_bit;
	bool display_lines = true;
	bool font = false;

	uint8_t value = (1 << 5) | (data_length << 4) | (display_lines << 3) | (font << 2);
	hd44780u_send_instruction(lcd, value, is_4_bit);
	lcd->delay_microseconds_fn(37);
}


void hd44780u_config_display_on_off_control(struct hd44780u_lcd *lcd, bool display, bool cursor, bool blinking)
{
	// 0 0 0 0 1 D C B: Sets entire display (D) on/off, cursor on/off
	// (C), and blinking of cursor position character (B)
	uint8_t value = (1 << 3) | (display << 2) | (cursor << 1) | (blinking << 0);
	hd44780u_send_instruction(lcd, value, true);

	// Wait at least 37 microseconds
	lcd->delay_microseconds_fn(1000);
}

void hd44780u_lcd_init(struct hd44780u_lcd *lcd)
{
	// Wait for more than 15 ms after V CC rises to 4.5 V
	lcd->delay_microseconds_fn(1000 * 30);

	// Initial pin configs. These pins are always outputs.
	hd44780u_lcd_pin_config_output(&lcd->rs_pin);
	hd44780u_lcd_pin_config_output(&lcd->rw_pin);
	hd44780u_lcd_pin_config_output(&lcd->e_pin);
	hd44780u_lcd_pin_set_value(&lcd->rs_pin, false); // Instruction register
	hd44780u_lcd_pin_set_value(&lcd->rw_pin, false); // Write register

	// Set data pins as output for now
	hd44780u_config_data_pins_output(lcd);

	// Function set (8 bit)
	hd44780u_config_function_set(lcd, true, false);

	// Wait for more than 4.1 ms
	lcd->delay_microseconds_fn(1000 * 6);

	// Function set (8 bit)
	hd44780u_config_function_set(lcd, true, false);

	// Wait for more than 100 μs
	lcd->delay_microseconds_fn(1000);

	// Function set (8 bit)
	hd44780u_config_function_set(lcd, true, false);

	// Function set (set 4 bit, but send as 8 bit)
	hd44780u_config_function_set(lcd, false, false);

	// Function set (set 4 bit, and send as 4 bit bit)
	hd44780u_config_function_set(lcd, false, true);

	// Display off
	hd44780u_config_display_on_off_control(lcd, false, false, false);

	// Display clear
	hd44780u_lcd_clear(lcd);

	// Entry mode set
	hd44780u_config_entry_mode(lcd, true, false);

	// Initial config
	hd44780u_config_display_on_off_control(lcd, true, true, true);
}

void hd44780u_lcd_write_char(struct hd44780u_lcd *lcd, char c)
{
	hd44780u_send_data(lcd, c);
}

void hd44780u_lcd_write_string(struct hd44780u_lcd *lcd, char *str)
{
	while (*str)
		hd44780u_lcd_write_char(lcd, *(str++));
}
