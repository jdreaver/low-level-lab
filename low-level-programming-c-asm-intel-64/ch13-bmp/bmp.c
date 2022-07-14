#include "bmp.h"
#include "image.h"

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>

struct bmp_file *bmp_file_from_bytes(void *bytes, size_t len)
{
	struct bmp_file *file = malloc(sizeof(*file));
	if (file == NULL) {
		fprintf(stderr, "failed to allocate bmp_file");
		exit(EXIT_FAILURE);
	}
	file->raw_bytes = bytes;

	size_t current_size = sizeof(struct bmp_file_header);
	if (current_size > len) {
		fprintf(stderr, "fatal: input BMP file too small (%lu bytes) for header\n", len);
		goto error;
	}
	file->header = bytes;

	current_size += sizeof(struct bmp_dib_header);
	if (current_size > len) {
		fprintf(stderr, "fatal: input BMP file too small (%lu bytes) for DIB header\n", len);
		goto error;
	}
	file->dib_header = (struct bmp_dib_header *) ((uint8_t *)bytes + sizeof(struct bmp_file_header));

	size_t pixel_offset = file->header->pixel_offset;
	if (pixel_offset > len) {
		fprintf(stderr, "fatal: input BMP file too small (%lu bytes) for pixel offset (%lu)\n", len, pixel_offset);
		goto error;
	}
	file->pixel_bytes = (uint8_t *)bytes + pixel_offset;

	return file;

error:
	free(file);
	exit(EXIT_FAILURE);
}


void bmp_file_free(struct bmp_file *file)
{
	free(file);
}

static struct image *bmp_to_image(struct bmp_file *input)
{
	size_t width = input->dib_header->image_width;
	size_t height = input->dib_header->image_height;
	uint8_t bits_per_pixel = input->dib_header->bits_per_pixel;

	// Need to copy bytes without padding. BMP pads rows so they are a
	// multiple of 4 bytes.
	assert(bits_per_pixel % 8 == 0);
	size_t bytes_per_pixel = bits_per_pixel / 8;
	size_t num_bytes = sizeof(uint8_t) * width * height * bytes_per_pixel;
	uint8_t *bytes = malloc(num_bytes);
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width * bytes_per_pixel; j++) {
			bytes[i * width + j] = input->pixel_bytes[i * width + j];
		}
	}

	struct image *image = image_create(width, height, bits_per_pixel, bytes);
	return image;
}

void bmp_file_rotate(struct bmp_file *file)
{
	// Create an image and rotate it
	struct image *image = bmp_to_image(file);
	image_rotate(image);

	// Copy bytes back to image
	//
	// TODO: Can't do this in-place, since the number of bytes needed for
	// pixel data can change due to padding.
	assert(image->bits_per_pixel % 8 == 0);
	size_t bytes_per_pixel = image->bits_per_pixel / 8;

	image_destroy(image);
}
