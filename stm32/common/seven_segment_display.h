/**
 * Single digit 7 segment display counter.
 *
 * NOTE: This module assumes that any GPIO ports for pins have been enabled and
 * the pins are initialed to act as output.
 *
 * My Elegoo kit came with a 5161AH. Data sheet here:
 * - http://www.xlitx.com/datasheet/5161AH.pdf
 * - http://www.lanpade.com/7-segment-led-dot-matrix/5161ah.html
 *
 * Good pinout diagram
 * (https://www.electronicsforu.com/resources/7-segment-display-pinout-understanding)
 * copied here. Lower case letters correspond to segments and DP (decimal
 * point), and GND is ground (note only one ground needs to be connected):
 *
 *   g f  GND  a b
 *  _|_|___|___|_|__
 * |                |
 * |    ___a___     |
 * |   |       |    |
 * |  f|       |b   |
 * |   |___g___|    |
 * |   |       |    |
 * |  e|       |c   |
 * |   |_______|  DP|
 * |       d      . |
 * |________________|
 *   | |   |   | |
 *   e d  GND  c DP
 *
**/

#pragma once

#ifdef STM32F401xE

#include "stm32f4xx.h"

enum seven_segment_display_segment {
	SEVEN_SEGMENT_DISPLAY_SEGMENT_A,
	SEVEN_SEGMENT_DISPLAY_SEGMENT_B,
	SEVEN_SEGMENT_DISPLAY_SEGMENT_C,
	SEVEN_SEGMENT_DISPLAY_SEGMENT_D,
	SEVEN_SEGMENT_DISPLAY_SEGMENT_E,
	SEVEN_SEGMENT_DISPLAY_SEGMENT_F,
	SEVEN_SEGMENT_DISPLAY_SEGMENT_G,
	SEVEN_SEGMENT_DISPLAY_SEGMENT_DP,
};

#define SEVEN_SEGMENT_DISPLAY_NUM_SEGMENTS (SEVEN_SEGMENT_DISPLAY_SEGMENT_DP + 1)
#define SEVEN_SEGMENT_DISPLAY_NUM_GLYPHS 11

/**
 * Memory location and mask used to set pin value for register.
 */
struct segment_pin {
	volatile uint32_t *reg;
	uint32_t mask;
};

struct seven_segment_display {
	struct segment_pin segment_pins[SEVEN_SEGMENT_DISPLAY_NUM_SEGMENTS];
};

/**
 * Clears the entire display so no segments are on.
 */
void seven_segment_display_clear(struct seven_segment_display *display);

/**
 * Clears the display and shows a given digit glyph.
 *
 * Must provide an integer in [0, 10], where 10 turns all glyphs on. If a number
 * is provided outside of [0, 10], we round down to 10.
 */
void seven_segment_display_show_digit_glyph(struct seven_segment_display *display, uint8_t digit);

/**
 * Turn on a given segment. Note: does not clear beforehand.
 */
void seven_segment_display_show_segment(struct seven_segment_display *display, enum seven_segment_display_segment segment);

#else
  #error "Unknown board. Can't set up seven_segment_display.h"
#endif
