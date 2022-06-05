#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "error.h"
#include "parser.h"

/*
 * Eats up all `isspace()` characters and any comment lines in `source`.
 */
static void eat_whitespace_comments(char **source)
{
	while (*source != NULL && **source != '\0') {
		if (isspace(**source)) {
			(*source)++;
			continue;
		}

		// If we find a comment (//) then eat the rest of the line
		if (**source == '/' && *(source + 1) != NULL && **(source + 1) == '/') {
			while (*source != NULL && **source != '\0' && **source != '\n') {
				(*source)++;
			}
		}
		break;
	}
}

/*
 * Parses a decimal address, which is a non-negative number between 0 and 2^15 -
 * 1 = 32767. This is done character by character to carefully catch overflow.
 */
static enum asm_parse_error parse_a_address(char **source, uint16_t *address)
{
	*address = 0;
	for (; *source != NULL && isdigit(**source); (*source)++) {
		// Multiply current address by 10, checking for overflow.
		if (*address > UINT8_MAX / 10) {
			return ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE;
		}
		*address *= 10;

		// Add digit, checking for overflow
		uint16_t d = **source - '0';
		if (*address > UINT8_MAX - d) {
			return ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE;
		}
		*address += d;
	}

	if (*address > 32767) {
		return ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE;
	}

	return ASM_PARSE_ERROR_NO_ERROR;
}

/*
 * Parses an A instruction, assuming whitespace and comments have already been
 * eaten, and returns a pointer to the source after the A instruction.
 */
static enum asm_parse_error parse_a_instruction(char **source, struct asm_a_instruction *instruction)
{
	assert(*source != NULL);

	// A instructions must start with @
	if (**source != '@') {
		return ASM_PARSE_ERROR_A_INSTRUCTION_MISSING_AT_SYMBOL;
	}

	source++;
	if (**source == '\0') {
		return ASM_PARSE_ERROR_UNEXPECTED_EOF;
	}

	// If the next character is a number, we must have an address literal.
	// Otherwise, assume a label.
	if (isdigit(**source)) {
		uint16_t address;
		enum asm_parse_error address_error = parse_a_address(source, &address);
		if (address_error != ASM_PARSE_ERROR_NO_ERROR) {
			return address_error;
		}
		instruction->address = address;
	} else {
		// TODO
	}

	// Check for newline or EOF. If one of those isn't present, there
	// is extra crud in the input.
	if (**source != '\0' || !isspace(**source)) {
		return ASM_PARSE_ERROR_EXTRA_INPUT;
	}

	return ASM_PARSE_ERROR_NO_ERROR;
}

/*
 * Parses the next line of text in `source`. Returns an error (or
 * `ASM_PARSE_ERROR_NO_ERROR` if none was encountered), and sets `instruction`
 * to NULL if the line was empty or just comments.
 */
static enum asm_parse_error parse_instruction_line(char **source, struct instruction *instruction)
{

}

void asm_a_instruction_destroy(struct asm_a_instruction instruction)
{
	switch (instruction.type) {
	case ASM_A_INST_LABEL:
		free(instruction.label);
		break;
	case ASM_A_INST_ADDRESS:
		// No dynamic memory in address
		break;
	}
}

void asm_instruction_destroy(struct asm_instruction instruction)
{
	switch (instruction.type) {
	case ASM_INST_A:
		break;
	case ASM_INST_C:
		// No dynamic memory in C instructions
		break;
	}
}

void asm_declaration_destroy(struct asm_declaration declaration)
{
	switch (declaration.type) {
	case ASM_DECL_LABEL:
		free(declaration.label);
		break;
	case ASM_DECL_INSTRUCTION:
		asm_instruction_destroy(declaration.instruction);
		break;
	}
}
