#include "bmp.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: main <input-file.bmp>\n");
		exit(EXIT_FAILURE);
	}

	// Open input file
	char *input_filename = argv[1];
	FILE *input_file = fopen(input_filename, "r");
	if (input_file == NULL) {
		perror("failed to open input file");
		exit(EXIT_FAILURE);
	}

	// Get size of input file
	fseek(input_file, 0 , SEEK_END);
	long file_size = ftell(input_file);
	fseek(input_file, 0 , SEEK_SET);

	printf("input file size: %lu\n", file_size);

	// Load input file into array
	void *input_bytes = malloc(file_size);
	if ((fread(input_bytes, 1, file_size, input_file)) != (size_t)file_size) {
		if (ferror(input_file)) {
			fprintf(stderr, "Error reading input file\n");
			exit(EXIT_FAILURE);
		}
	}

	struct bmp_file *file = bmp_file_from_bytes(input_bytes, (size_t)file_size);

	struct bmp_file_header *header = file->header;
	printf("signature: %.2s\n", header->signature);
	printf("file_size_bytes: %" PRIu32 "\n", header->file_size_bytes);
	printf("pixel_offset: %" PRIu32 "\n", header->pixel_offset);

	struct bmp_dib_header *dib_header = file->dib_header;
	printf("image_width: %" PRIi32 "\n", dib_header->image_width);
	printf("image_height: %" PRIi32 "\n", dib_header->image_height);
	printf("image_size_bytes: %" PRIu32 "\n", dib_header->image_size_bytes);
	printf("bits_per_pixel: %" PRIu16 "\n", dib_header->bits_per_pixel);

	fclose(input_file);
	bmp_file_free(file);
	free(input_bytes);

	return 0;
}
