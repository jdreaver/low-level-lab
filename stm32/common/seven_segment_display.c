#include "seven_segment_display.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef STM32F401xE

// Segments to turn on for a given number (10 is all of them).
bool glyphs[SEVEN_SEGMENT_DISPLAY_NUM_GLYPHS][SEVEN_SEGMENT_DISPLAY_NUM_SEGMENTS] = {
	{ 1, 1, 1, 1, 1, 1, 0, 0 }, // 0
	{ 0, 1, 1, 0, 0, 0, 0, 0 }, // 1
	{ 1, 1, 0, 1, 1, 0, 1, 0 }, // 2
	{ 1, 1, 1, 1, 0, 0, 1, 0 }, // 3
	{ 0, 1, 1, 0, 0, 1, 1, 0 }, // 4
	{ 1, 0, 1, 1, 0, 1, 1, 0 }, // 5
	{ 1, 0, 1, 1, 1, 1, 1, 0 }, // 6
	{ 1, 1, 1, 0, 0, 0, 0, 0 }, // 7
	{ 1, 1, 1, 1, 1, 1, 1, 0 }, // 8
	{ 1, 1, 1, 0, 0, 1, 1, 0 }, // 9
	{ 1, 1, 1, 1, 1, 1, 1, 1 }, // 10
};

void seven_segment_display_clear(struct seven_segment_display *display)
{
	for (size_t i = 0; i < SEVEN_SEGMENT_DISPLAY_NUM_SEGMENTS; i++) {
		struct segment_pin pin = display->segment_pins[i];
		*pin.reg &= ~(pin.mask);
	}
}

void seven_segment_display_show_digit_glyph(struct seven_segment_display *display, uint8_t digit)
{
	if (digit > SEVEN_SEGMENT_DISPLAY_NUM_GLYPHS - 1)
		digit = SEVEN_SEGMENT_DISPLAY_NUM_GLYPHS;

	seven_segment_display_clear(display);

	for (size_t i = 0; i < SEVEN_SEGMENT_DISPLAY_NUM_SEGMENTS; i++) {
		if (glyphs[digit][i]) {
			enum seven_segment_display_segment segment = i;
			seven_segment_display_show_segment(display, segment);
		}
	}
}

void seven_segment_display_show_segment(struct seven_segment_display *display, enum seven_segment_display_segment segment)
{
	struct segment_pin pin = display->segment_pins[segment];
	*pin.reg |= pin.mask;
}


#else
  #error "Unknown board. Can't set up seven_segment_display.c"
#endif
