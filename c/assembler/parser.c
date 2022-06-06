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
		enum asm_parse_error symbol_error = asm_parse_symbol(state, &instruction->label);
		if (symbol_error != ASM_PARSE_ERROR_NO_ERROR) {
			return symbol_error;
		}
		instruction->type = ASM_A_INST_LABEL;
	}

	return ASM_PARSE_ERROR_NO_ERROR;
}

/*
 * Compares two strings using the minimum of the two given sizes.
 *
 * TODO: Move this to a util module
 */
static int strncmp_min(const char *str1, const char *str2, size_t s1, size_t s2)
{
	size_t s = s1;
	if (s2 < s) {
		s = s2;
	}
	return strncmp(str1, str2, s);
}

/*
 * Parses an C instruction starting at the current parse position. C
 * instructions look like: DEST=COMP;JUMP
 */
static enum asm_parse_error parse_c_instruction(struct parser_state *state, struct asm_c_instruction *instruction)
{
	// Parsing a C instruction works like this:
	// 1. Start parsing until a space or newline, assuming we are parsing COMP
        // 2. If we find an =, then move COMP to DEST, and start with COMP again
        // 3. If we find a ;, then start on JUMP
        // 4. We need one of DEST or JUMP for a valid instruction
        // 5. Also, our components need to be non-empty and match to valid components

	size_t start = state->pos;

	size_t comp_start = 0;
	size_t comp_end = 0;

	bool found_dest = false;
	size_t dest_start = 0;
	size_t dest_end = 0;

	bool found_jump = false;
	size_t jump_start = 0;
	size_t jump_end = 0;

	char current_char;
	while ((current_char = parser_state_current_char(state)) && current_char != ' ' && current_char != '\n') {
		if (current_char == '=') {
			if (found_dest || found_jump) {
				return ASM_PARSE_ERROR_C_MALFORMED;
			}
			found_dest = true;
			dest_start = start;
			dest_end = state->pos;
			start = state->pos + 1;
		} else if (current_char == ';') {
			if (found_jump) {
				return ASM_PARSE_ERROR_C_MALFORMED;
			}
			found_jump = true;
			comp_start = start;
			comp_end = state->pos;
			start = state->pos + 1;
		}
		parser_state_advance(state);
	}

	// If we found a jump, then jump_start/end is start/end. Otherwise, it
	// is comp start/end.
	if (found_jump) {
		jump_start = start;
		jump_end = state->pos;
	} else {
		comp_start = start;
		comp_end = state->pos;
	}

	// We must have a comp or dest
	if (!found_dest && !found_jump) {
		return ASM_PARSE_ERROR_C_MALFORMED;
	}

	// Pattern match dest
	enum asm_c_dest dest = ASM_C_DEST_NULL;
	if (found_dest) {
		const char *dest_str = state->source + dest_start;
		size_t dest_len = dest_end - dest_start;
		if (dest_len == 0) {
			return ASM_PARSE_ERROR_C_DEST_MALFORMED;
		} else if (strncmp_min(dest_str, "M", 1, dest_len) == 0) {
			dest = ASM_C_DEST_M;
		} else {
			return ASM_PARSE_ERROR_C_DEST_MALFORMED;
		}
	};

	// Pattern match comp
	enum asm_c_a_comp comp;
	const char *comp_str = state->source + comp_start;
	size_t comp_len = comp_end - comp_start;
	if (comp_len == 0) {
		return ASM_PARSE_ERROR_C_A_COMP_MALFORMED;
	} else if (strncmp_min(comp_str, "0", 1, comp_len) == 0) {
		comp = ASM_C_A_COMP_ZERO;
	} else {
		return ASM_PARSE_ERROR_C_A_COMP_MALFORMED;
	};


	// Pattern match jump
	enum asm_c_jump jump = ASM_C_JUMP_NULL;
	if (found_jump) {
		const char *jump_str = state->source + jump_start;
		size_t jump_len = jump_end - jump_start;
		if (jump_len == 0) {
			return ASM_PARSE_ERROR_C_JUMP_MALFORMED;
		} else if (strncmp_min(jump_str, "JGT", 1, jump_len) == 0) {
			jump = ASM_C_JUMP_JEQ;
		} else {
			return ASM_PARSE_ERROR_C_JUMP_MALFORMED;
		};

	}

	instruction->dest = dest;
	instruction->a_comp = comp;
	instruction->jump = jump;

	return ASM_PARSE_ERROR_NO_ERROR;
}

/*
 * Parses the next line of text in `source`. Returns an error (or
 * `ASM_PARSE_ERROR_NO_ERROR` if none was encountered), or also
 * `ASM_PARSE_ERROR_NO_DECLARATION` if no instruction was found on the current
 * line. Also asserts that we are indeed at the end of a line (or the end of the
 * string), and otherwise returns `ASM_PARSE_ERROR_EXTRA_INPUT`.
 */
static enum asm_parse_error parse_declaration_line(struct parser_state *state, struct asm_declaration *declaration)
{
	// Eat whitespace (until end of line)
	eat_line_space_comments(state);

	// Run parser for an declaration
	enum asm_parse_error err;
	struct asm_a_instruction a_instruction;
	struct asm_c_instruction c_instruction;

	switch (parser_state_current_char(state)) {
	case '\0':
		return ASM_PARSE_ERROR_NO_DECLARATION;
	case '\n':
		// Eat newline
		parser_state_advance(state);
		return ASM_PARSE_ERROR_NO_DECLARATION;
        case '@':
		 err = parse_a_instruction(state, &a_instruction);
		 if (err != ASM_PARSE_ERROR_NO_ERROR) {
			 return err;
		 }
		 declaration->type = ASM_DECL_INSTRUCTION;
		 declaration->instruction.type = ASM_INST_A;
		 declaration->instruction.a_instruction = a_instruction;
		 break;
        case '(':
		fprintf(stderr, "TODO support labels %s at %s:%d\n", __func__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	default:
		 err = parse_c_instruction(state, &c_instruction);
		 if (err != ASM_PARSE_ERROR_NO_ERROR) {
			 return err;
		 }
		 declaration->type = ASM_DECL_INSTRUCTION;
		 declaration->instruction.type = ASM_INST_C;
		 declaration->instruction.c_instruction = c_instruction;
	}

	// Eat any trailing whitespace
	eat_line_space_comments(state);

	// Check for newline or EOF. If one of those isn't present, there is
	// extra crud in the input.
	char last_char = parser_state_current_char(state);
	if (last_char && last_char != '\n') {
		return ASM_PARSE_ERROR_EXTRA_INPUT;
	}

	// Eat newline (if this is EOF we don't advance)
	parser_state_advance(state);

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

static void asm_instruction_destroy(struct asm_instruction instruction)
{
	switch (instruction.type) {
	case ASM_INST_A:
		asm_a_instruction_destroy(instruction.a_instruction);
		break;
	case ASM_INST_C:
		// No dynamic memory in C instructions
		break;
	}
}

static void asm_declaration_destroy(struct asm_declaration declaration)
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

asm_declarations asm_declarations_create()
{
	asm_declarations decls = {.declarations = NULL, .len = 0, .capacity = 0};
	return decls;
}

static void asm_declarations_append(asm_declarations *declarations, struct asm_declaration declaration)
{
	if (declarations->capacity == declarations->len) {
		if (declarations->capacity == 0) {
			declarations->capacity = 1;
		} else {
			declarations->capacity *= 2;
		}
		size_t new_size = declarations->capacity * sizeof(struct asm_declaration);
		if ((declarations->declarations = realloc(declarations->declarations, new_size)) == NULL) {
			fprintf(stderr, "strdup realloc error in %s at %s:%d\n", __func__, __FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}
	}

	declarations->declarations[declarations->len] = declaration;
	declarations->len += 1;
}

enum asm_parse_error parse_asm_declarations(char *source, asm_declarations *declarations)
{
	struct parser_state state = parser_state_create(source);
	while (parser_state_current_char(&state)) {
		struct asm_declaration declaration;
		enum asm_parse_error err = parse_declaration_line(&state, &declaration);
		if (err == ASM_PARSE_ERROR_NO_ERROR) {
			asm_declarations_append(declarations, declaration);
		} else if (err != ASM_PARSE_ERROR_NO_DECLARATION) {
			return err;
		}
	}

	return ASM_PARSE_ERROR_NO_ERROR;
}

void asm_declarations_destroy(asm_declarations declarations)
{
	for (size_t i = 0; i < declarations.len; i++) {
		asm_declaration_destroy(declarations.declarations[i]);
	}
	free(declarations.declarations);
}
