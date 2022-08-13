/**
 * Hello world for HD44780U LCD screen.
 *
 * Used just GPIOC pins. I need 11 pins for control, and there are only 10 GPIOC
 * pins on the left of my Nucleo STM32F401RE, so one pin (PC7) is on the right
 * side of the board.
 */

#include "hd44780u_lcd.h"
#include "system_clock.h"
#include "usart2.h"

#define make_lcd_pin(num) { \
	.mode_reg = &GPIOC->MODER, \
	.mode_reg_clear_mask = ~(GPIO_MODER_MODE ## num ## _0 | GPIO_MODER_MODE ## num ## _1), \
	.mode_reg_output_mask = GPIO_MODER_MODE ## num ## _0,		\
	.mode_reg_input_mask = 0,					\
	.output_data_reg = &GPIOC->ODR,					\
	.output_data_reg_mask = GPIO_ODR_OD ## num,			\
	}

struct hd44780u_lcd lcd = {
	.delay_microseconds_fn = systick_delay_microseconds,
	.rs_pin = make_lcd_pin(11),
	.rw_pin = make_lcd_pin(10),
	.e_pin = make_lcd_pin(12),
	.data_pins = {
		make_lcd_pin(13),
		make_lcd_pin(14),
		make_lcd_pin(15),
		make_lcd_pin(2),
		make_lcd_pin(1),
		make_lcd_pin(3),
		make_lcd_pin(0),
		make_lcd_pin(7),
	},
};

void start(void)
{
	// Enable GPIOC since all of our pins are GPIOC
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	usart2_enable();
	systick_enable_passive();
	hd44780u_init(&lcd);
}
