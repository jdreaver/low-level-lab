#include "bmp.h"

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


	fclose(input_file);
	free(input_bytes);

	return 0;
}
