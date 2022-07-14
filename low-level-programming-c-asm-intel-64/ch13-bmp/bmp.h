#pragma once

#include <stdint.h>
#include <stdio.h>

/**
 * Main sources for BMP file format:
 * - The book
 * - https://en.wikipedia.org/wiki/BMP_file_format
 * - https://engineering.purdue.edu/ece264/16au/hw/HW13
 */


/**
 * Main header for a BMP file.
 */
struct __attribute__((packed)) bmp_file_header {
	char signature[2]; /**< Magic identifier: 0x424d, which is "BM" */
	uint32_t file_size_bytes; /**< File size includes header */
	uint16_t unused1;
	uint16_t unused2;
	uint32_t pixel_offset; /**< File offset to pixel array */
};

/**
 * DIB (device independent bitmap) header, comes after the file header.
 */
struct __attribute__((packed)) bmp_dib_header {
	uint32_t dib_header_size;
	int32_t image_width;
	int32_t image_height;
	uint16_t color_planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t image_size_bytes;
	int32_t horizontal_resolution;
	int32_t vertical_resolution;
	uint32_t color_palette_size;
	uint32_t important_colors;
};

/**
 * Contains the raw bytes for a BMP file
 */
struct bmp_file {
	struct bmp_file_header *header;
	struct bmp_dib_header *dib_header;
	uint8_t *pixel_bytes;
	void *raw_bytes;
};

/**
 * Maps the bmp_file struct onto raw input bytes.
 */
struct bmp_file *bmp_file_from_bytes(void *bytes, size_t len);

/**
 * Frees the bmp_file struct, but _does not_ free the underlying raw input
 * bytes.
 */
void bmp_file_free(struct bmp_file *file);
