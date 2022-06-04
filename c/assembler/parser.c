#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "error.h"
#include "parser.h"

void asm_instruction_destroy(struct asm_instruction instruction)
{

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
