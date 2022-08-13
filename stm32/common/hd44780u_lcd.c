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

void hd44780u_send_bits(struct hd44780u_lcd *lcd, uint8_t data)
{
	// Set data lines. Assumes pins are already set to be output!
	for (size_t i = 0; i < HD44780U_LCD_NUM_DATA_PINS; i++)
		hd44780u_lcd_pin_set_value(&lcd->data_pins[i], data & (1 << i));

	// Total E cycle time is recorded as 1 microsecond, but we add extra
	// waits to be safe.
	lcd->delay_microseconds_fn(1);
	hd44780u_lcd_pin_set_value(&lcd->e_pin, true);
	lcd->delay_microseconds_fn(1);
	hd44780u_lcd_pin_set_value(&lcd->e_pin, false);
	lcd->delay_microseconds_fn(1);
}

void hd44780u_send_instruction(struct hd44780u_lcd *lcd, uint8_t instruction)
{
	hd44780u_lcd_pin_set_value(&lcd->rs_pin, true); // Data register
	hd44780u_lcd_pin_set_value(&lcd->rw_pin, false); // Write register
	hd44780u_send_bits(lcd, instruction);
}

void hd44780u_send_data(struct hd44780u_lcd *lcd, uint8_t data)
{
	hd44780u_lcd_pin_set_value(&lcd->rs_pin, false); // Instruction register
	hd44780u_lcd_pin_set_value(&lcd->rw_pin, false); // Write register
	hd44780u_send_bits(lcd, data);
}

void hd44780u_config_display_on_off_control(struct hd44780u_lcd *lcd, bool display, bool cursor, bool blinking)
{
	// 0 0 0 0 1 D C B: Sets entire display (D) on/off, cursor on/off
	// (C), and blinking of cursor position character (B)
	uint8_t value = (1 << 3) | (display << 2) | (cursor << 1) | (blinking << 0);
	hd44780u_send_instruction(lcd, value);
	lcd->delay_microseconds_fn(37);
}

void hd44780u_init(struct hd44780u_lcd *lcd)
{
	// Defaults from the data sheet:
	//
	// Initializing by Internal Reset Circuit
	//
	// An internal reset circuit automatically initializes the HD44780U when
	// the power is turned on. The following instructions are executed
	// during the initialization. The busy flag (BF) is kept in the busy
	// state until the initialization ends (BF = 1). The busy state lasts
	// for 10 ms after VCC rises to 4.5 V.
	//
	// 1. Display clear
	// 2. Function set:
	//    DL = 1; 8-bit interface data
	//    N = 0; 1-line display
	//    F = 0; 5 × 8 dot character font
	// 3. Display on/off control:
	//    D = 0; Display off
	//    C = 0; Cursor off
	//    B = 0; Blinking off
	// 4. Entry mode set:
	//    I/D = 1; Increment by 1
	//    S = 0; No shift

	// Wait 10ms for initialization. (We could read busy flag here, but this
	// is easier).
	//lcd->delay_microseconds_fn(1000 * 10);
	lcd->delay_microseconds_fn(1000 * 100);

	// Initial pin configs. These pins are always outputs.
	hd44780u_lcd_pin_config_output(&lcd->rs_pin);
	hd44780u_lcd_pin_config_output(&lcd->rw_pin);
	hd44780u_lcd_pin_config_output(&lcd->e_pin);
	hd44780u_lcd_pin_set_value(&lcd->rs_pin, false); // Instruction register
	hd44780u_lcd_pin_set_value(&lcd->rw_pin, false); // Write register

	// Set data pins as output for now
	hd44780u_config_data_pins_output(lcd);

	// TODO: Do all of the reset stuff manually
	// Initial config
	hd44780u_config_display_on_off_control(lcd, true, true, true);

	// Write an H
	hd44780u_send_data(lcd, 0b01001001);
}
