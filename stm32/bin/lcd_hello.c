/**
 * Hello world for HD44780U LCD screen.
 *
 */

#include "hd44780u_lcd.h"
#include "system_clock.h"
#include "usart2.h"

#define make_lcd_pin(gpio, num) { \
	.mode_reg = &GPIO ## gpio->MODER, \
	.mode_reg_clear_mask = ~(GPIO_MODER_MODE ## num ## _0 | GPIO_MODER_MODE ## num ## _1), \
	.mode_reg_output_mask = GPIO_MODER_MODE ## num ## _0,		\
	.mode_reg_input_mask = 0,					\
	.output_data_reg = &GPIO ## gpio->ODR,			\
	.output_data_reg_mask = GPIO_ODR_OD ## num,			\
	}

struct hd44780u_lcd lcd = {
	.delay_microseconds_fn = systick_delay_microseconds,
	.rs_pin = make_lcd_pin(C, 11),
	.rw_pin = make_lcd_pin(C, 10),
	.e_pin = make_lcd_pin(C, 12),
	.data_pins = {
		make_lcd_pin(A, 13),
		make_lcd_pin(A, 14),
		make_lcd_pin(A, 15),
		make_lcd_pin(C, 2),
		make_lcd_pin(C, 1),
		make_lcd_pin(C, 3),
		make_lcd_pin(C, 0),
		make_lcd_pin(C, 7),
	},
};

void start(void)
{
	// Enable GPIO ports
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	usart2_enable();
	systick_enable_passive();
	hd44780u_init(&lcd);
}
