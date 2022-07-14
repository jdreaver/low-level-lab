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
	uint16_t type; /**< Magic identifier: 0x424d, which is "BM" */
	uint32_t file_size_bytes; /**< File size includes header */
	uint16_t unused1;
	uint16_t unused2;
	uint32_t pixel_offest; /**< File offset to pixel array */
};
