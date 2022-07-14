#include "bmp.h"

#include <inttypes.h>
#include <stdlib.h>

struct bmp_file *bmp_file_from_bytes(void *bytes, size_t len)
{
	struct bmp_file *file = malloc(sizeof(*file));
	file->raw_bytes = bytes;

	size_t current_size = sizeof(struct bmp_file_header);
	if (current_size > len) {
		fprintf(stderr, "fatal: input BMP file too small (%lu bytes) for header\n", len);
		exit(EXIT_FAILURE);
	}
	file->header = bytes;

	current_size += sizeof(struct bmp_dib_header);
	if (current_size > len) {
		fprintf(stderr, "fatal: input BMP file too small (%lu bytes) for DIB header\n", len);
		exit(EXIT_FAILURE);
	}
	file->dib_header = (struct bmp_dib_header *) ((uint8_t *)bytes + sizeof(struct bmp_file_header));

	size_t pixel_offset = file->header->pixel_offset;
	if (pixel_offset > len) {
		fprintf(stderr, "fatal: input BMP file too small (%lu bytes) for pixel offset (%lu)\n", len, pixel_offset);
		exit(EXIT_FAILURE);
	}
	file->pixel_bytes = (uint8_t *)bytes + pixel_offset;

	return file;
}


void bmp_file_free(struct bmp_file *file)
{
	free(file);
}
