#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "parser.h"

int main(int argc, char **argv)
{

	FILE *fp = argc > 1 ? fopen(argv[1], "r") : stdin;
	if (!fp) {
		perror("fopen failed");
		exit(EXIT_FAILURE);
	}

	int page_size = sysconf(_SC_PAGESIZE);

	size_t input_size = 0;
	char *input = NULL;
	while (true) {
		// Increase buffer size
		input_size += page_size;
		if ((input = realloc(input, input_size)) == NULL) {
			fprintf(stderr, "realloc error in %s at %s:%d\n", __func__, __FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}

		// Copy input to buffer
		char *result = fgets(input, page_size, fp);
		if (result == NULL) {
			break;
		}
	}

	if (fp != stdin) {
		fclose(fp);
	}

	asm_declarations declarations = asm_declarations_create();
	enum asm_parse_error err = parse_asm_declarations(input, &declarations);

	free(input);
	asm_declarations_destroy(declarations);

	printf("error: %d\n", err);
}
