#include "image.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct image *image_create(size_t width, size_t height, uint8_t bits_per_pixel, uint8_t *bytes)
{
	if ((bits_per_pixel < 8) || (bits_per_pixel % 8 != 0)) {
		fprintf(stderr, "invalid bits_per_pixel %" PRIu8 ". Must be >= 8 and multiple of 8.", bits_per_pixel);
		exit(EXIT_FAILURE);
	}

	struct image *image = malloc(sizeof(*image));
	if (image == NULL) {
		fprintf(stderr, "failed to allocate image");
		exit(EXIT_FAILURE);
	}

	image->width = width;
	image->height = height;
	image->bits_per_pixel = bits_per_pixel;
	image->bytes = bytes;

	return image;
}

void image_rotate(struct image *input)
{
	// Make a copy of input bytes to make things easier
	assert(input->bits_per_pixel % 8 == 0);
	size_t bytes_per_pixel = input->bits_per_pixel / 8;
	size_t num_bytes = sizeof(uint8_t) * input->width * input->height * bytes_per_pixel;
	uint8_t *copied = malloc(num_bytes);
	memcpy(copied, input->bytes, num_bytes);

	// Rotate pixels
	size_t new_width = input->height;
	size_t new_height = input->width;
	for (size_t i = 0; i < input->height; i++) {
		for (size_t j = 0; j < input->width; j++) {
			size_t input_start = i * input->width + j;
			size_t output_start = j * new_width + i;
			for (size_t k = 0; k < bytes_per_pixel; k++) {
				copied[output_start + k] = input->bytes[input_start + k];
			}
		}
	}

	// Swap width/height
	input->width = new_width;
	input->height = new_height;

	free(copied);
}
