#pragma once

#include <inttypes.h>
#include <stdlib.h>

/**
 * Stores bytes for an image of a given width/height (in pixels), where pixels
 * are stored left to right, then bottom to top, just like in a BMP image.
 */
struct image {
	size_t width;
	size_t height;
	uint8_t bits_per_pixel; /**< Must by >= 8 and a multiple of 8 */
	uint8_t *bytes;
};

/**
 * Creates an image from some bytes. Note that it is assumed the image owns the
 * bytes, and they will be freed when the image is freed.
 */
struct image *image_create(size_t width, size_t height, uint8_t bits_per_pixel, uint8_t *bytes);

/**
 * Frees image and bytes contained in image.
 */
void image_destroy(struct image* image);

/**
 * Rotates an image in place by 90 degrees.
 */
void image_rotate(struct image *input);
