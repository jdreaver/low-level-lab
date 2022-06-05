#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "parser.h"

/*
 * Keeps track of parser location in input string and prevents footguns like
 * traversing past lines or the end of the string.
 */
struct parser_state {
	/** Input string */
	const char *source;
	/** Current position in the input string */
	size_t pos;
	// TODO: size_t lineno; (for error reporting. keep track of this in advance)
};

struct parser_state parser_state_create(const char *source)
{
	struct parser_state state = {.source = source, .pos = 0};
	return state;
}

/*
 * Returns the character at the current pos.
 */
static char parser_state_current_char(const struct parser_state *state)
{
	return state->source[state->pos];
}

/*
 * Returns the next character past the current pos, or \0 if we are already at
 * the end of the string.
 */
static char parser_state_peek(struct parser_state *state)
{
	if (!parser_state_current_char(state)) {
		return '\0';
	}
	return state->source[state->pos + 1];
}

/*
 * Advances the current position (if we aren't at the end).
 */
static void parser_state_advance(struct parser_state *state)
{
	if (parser_state_current_char(state)) {
		state->pos++;
	}
}

/*
 * Advances through current line while eating space chars (' ') and comments
 * (//)
 */
static void eat_line_space_comments(struct parser_state *state)
{
	// First eat all whitespace
	while (parser_state_current_char(state) == ' ') {
		parser_state_advance(state);
	}

	// Check if we are at a comment
	if (parser_state_current_char(state) == '/' && parser_state_peek(state) == '/') {
		while (parser_state_current_char(state) && parser_state_current_char(state) != '\n') {
			parser_state_advance(state);
		}
	}
}

/*
 * Parses a decimal address, which is a non-negative number between 0 and 2^15 -
 * 1 = 32767. This is done character by character to carefully catch overflow.
 */
static enum asm_parse_error parse_a_address(struct parser_state *state, uint16_t *address)
{
	*address = 0;
	while (true) {
		char current_char = parser_state_current_char(state);
		if (!current_char || !isdigit(current_char)) {
			break;
		}

		// Multiply current address by 10, checking for overflow.
		if (*address > UINT16_MAX / 10) {
			return ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE;
		}
		*address *= 10;

		// Add digit, checking for overflow
		uint16_t d = current_char - '0';
		if (*address > UINT16_MAX - d) {
			return ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE;
		}
		*address += d;

		parser_state_advance(state);
	}

	if (*address > 32767) {
		return ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE;
	}

	return ASM_PARSE_ERROR_NO_ERROR;
}

static bool valid_symbol_start_char(char c)
{
	return isalpha(c) || c == '_' || c == '.' || c == '$' || c == ':';
}

static bool valid_symbol_char(char c)
{
	return isdigit(c) || valid_symbol_start_char(c);
}

/*
 * Parses a symbol. A symbol can be any sequence of letters, digits, underscore
 * (_), dot (.), dollar sign ($), and colon (:) that does not begin with a
 * digit.
 *
 * Note that this function mallocs the symbol string, which should be freed
 * later!
 */
static enum asm_parse_error asm_parse_symbol(struct parser_state *state, char **symbol)
{
	size_t start = state->pos;
	if (!(valid_symbol_start_char(parser_state_current_char(state)))) {
		return ASM_PARSE_ERROR_INVALID_SYMBOL_START;
	}

	while (true) {
		parser_state_advance(state);
		if (!(valid_symbol_char(parser_state_current_char(state)))) {
			break;
		}
	}

	const char *symbol_start = state->source + start;
	size_t symbol_len = state->pos - start;
	if ((*symbol = strndup(symbol_start, symbol_len)) == NULL) {
		fprintf(stderr, "strdup malloc error in %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	return ASM_PARSE_ERROR_NO_ERROR;
}

/*
 * Parses an A instruction starting at the current parse position.
 */
static enum asm_parse_error parse_a_instruction(struct parser_state *state, struct asm_a_instruction *instruction)
{
	// A instructions must start with @
	if (parser_state_current_char(state) != '@') {
		return ASM_PARSE_ERROR_A_INSTRUCTION_MISSING_AT_SYMBOL;
	}
	parser_state_advance(state);

	if (parser_state_current_char(state) == '\0') {
		return ASM_PARSE_ERROR_UNEXPECTED_EOF;
	}

	// If the next character is a number, we must have an address literal.
	// Otherwise, assume a label.
	if (isdigit(parser_state_current_char(state))) {
		uint16_t address;
		enum asm_parse_error address_error = parse_a_address(state, &address);
		if (address_error != ASM_PARSE_ERROR_NO_ERROR) {
			return address_error;
		}
		instruction->type = ASM_A_INST_ADDRESS;
		instruction->address = address;
	} else {
		char *symbol;
		enum asm_parse_error symbol_error = asm_parse_symbol(state, &symbol);
		if (symbol_error != ASM_PARSE_ERROR_NO_ERROR) {
			return symbol_error;
		}
		instruction->type = ASM_A_INST_LABEL;
		instruction->label = symbol;
	}

	return ASM_PARSE_ERROR_NO_ERROR;
}

/*
 * Parses the next line of text in `source`. Returns an error (or
 * `ASM_PARSE_ERROR_NO_ERROR` if none was encountered), and sets `instruction`
 * to NULL if the line was empty or just comments.
 */
static enum asm_parse_error parse_instruction_line(struct parser_state *state, struct asm_instruction *instruction)
{
	// TODO: Eat whitespace until end of line

	// TODO: Run parser

	// TODO: Eat whitespace again

	// Check for newline or EOF. If one of those isn't present, there is
	// extra crud in the input.
	char last_char = parser_state_current_char(state);
	if (!last_char || last_char != '\n') {
		return ASM_PARSE_ERROR_EXTRA_INPUT;
	}

	return ASM_PARSE_ERROR_NO_ERROR;
}

static void asm_a_instruction_destroy(struct asm_a_instruction instruction)
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
