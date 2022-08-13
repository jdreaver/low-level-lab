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
	.rs_pin = make_lcd_pin(B, 9), // TODO: PB9 might be special
	.rw_pin = make_lcd_pin(A, 6),
	.e_pin = make_lcd_pin(A, 7),
	.data_pins = {
		make_lcd_pin(B, 6),
		make_lcd_pin(A, 9),
		make_lcd_pin(A, 8),
		make_lcd_pin(B, 10),
		make_lcd_pin(B, 4),
		make_lcd_pin(B, 5),
		make_lcd_pin(B, 3),
		make_lcd_pin(A, 10),
	},
};

void start(void)
{
	// Enable GPIO ports
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	usart2_enable();
	systick_enable_passive();
	hd44780u_init(&lcd);
}
