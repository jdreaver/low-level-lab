#include "unity.h"

#include <assert.h>
#include <string.h>

#include "parser.h"
// N.B. Including whole source file to get at static functions
#include "parser.c"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_eat_line_space_comments()
{
	// Empty line
	struct parser_state state = parser_state_create("");
	eat_line_space_comments(&state);
	TEST_ASSERT_EQUAL_size_t(state.pos, 0);

	// Simple comment, no leading whitespace
	state = parser_state_create("// blah");
	eat_line_space_comments(&state);
	TEST_ASSERT_EQUAL_CHAR('\0', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(7, state.pos);

	// Simple comment, leading whitespace
	state = parser_state_create("   // blah");
	eat_line_space_comments(&state);
	TEST_ASSERT_EQUAL_CHAR('\0', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(10, state.pos);

	// Just whitespace
	state = parser_state_create("   ");
	eat_line_space_comments(&state);
	TEST_ASSERT_EQUAL_CHAR('\0', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(3, state.pos);

	// Eat until next line
	state = parser_state_create("   // hello\ngoodbye");
	eat_line_space_comments(&state);
	TEST_ASSERT_EQUAL_CHAR('\n', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(11, state.pos);

	// Eat whitespace until non whitespace
	state = parser_state_create("   hello\ngoodbye");
	eat_line_space_comments(&state);
	TEST_ASSERT_EQUAL_CHAR('h', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(3, state.pos);
}

void test_parse_a_instruction()
{
	// Missing @ symbol (empty string)
	struct asm_a_instruction instruction;
	struct parser_state state = parser_state_create("");
	enum asm_parse_error err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_A_INSTRUCTION_MISSING_AT_SYMBOL, err);

	// Doesn't start with @
	state = parser_state_create("not@");
	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_A_INSTRUCTION_MISSING_AT_SYMBOL, err);

	// Simple success
	state = parser_state_create("@123");
	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	TEST_ASSERT_EQUAL_INT(ASM_A_INST_ADDRESS, instruction.type);
	TEST_ASSERT_EQUAL_UINT16(123, instruction.address);

	// Bare @
	state = parser_state_create("@");
	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_UNEXPECTED_EOF, err);

	// Min allowed value
	state = parser_state_create("@0");
	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	TEST_ASSERT_EQUAL_INT(ASM_A_INST_ADDRESS, instruction.type);
	TEST_ASSERT_EQUAL_UINT16(0, instruction.address);

	// Max allowed value
	state = parser_state_create("@32767");
	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	TEST_ASSERT_EQUAL_INT(ASM_A_INST_ADDRESS, instruction.type);
	TEST_ASSERT_EQUAL_UINT16(32767, instruction.address);

	// Logical overflow (fits in uint16_t, but is too large for address)
	state = parser_state_create("@32768");
	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE, err);

	// uint16_t overflow
	state = parser_state_create("@32768231432848329789437289");
	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE, err);

	// Label
	state = parser_state_create("@hello");
	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	TEST_ASSERT_EQUAL_INT(ASM_A_INST_LABEL, instruction.type);
	TEST_ASSERT_EQUAL_STRING("hello", instruction.label);

	// Doesn't support negative addresses
	state = parser_state_create("@-398");
 	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_INVALID_SYMBOL_START, err);

	// Invalid symbol start char
	state = parser_state_create("@ hello");
 	err = parse_a_instruction(&state, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_INVALID_SYMBOL_START, err);

	asm_a_instruction_destroy(instruction);
}

void test_parse_declaration_line()
{
	// Successfully parse A instruction
	struct asm_declaration declaration;
	struct parser_state state = parser_state_create("  @hello // abc123");
	enum asm_parse_error err = parse_declaration_line(&state, &declaration);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	TEST_ASSERT_EQUAL_INT(ASM_INST_A, declaration.instruction.type);
	TEST_ASSERT_EQUAL_INT(ASM_A_INST_LABEL, declaration.instruction.a_instruction.type);
	TEST_ASSERT_EQUAL_STRING("hello", declaration.instruction.a_instruction.label);
	TEST_ASSERT_EQUAL_CHAR('\0', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(18, state.pos);

	// Newline after with more instructions
	state = parser_state_create("  @hello \n @more");
	err = parse_declaration_line(&state, &declaration);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	TEST_ASSERT_EQUAL_INT(ASM_INST_A, declaration.instruction.type);
	TEST_ASSERT_EQUAL_INT(ASM_A_INST_LABEL, declaration.instruction.a_instruction.type);
	TEST_ASSERT_EQUAL_STRING("hello", declaration.instruction.a_instruction.label);
	TEST_ASSERT_EQUAL_CHAR(' ', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(10, state.pos);

	// Empty string
	state = parser_state_create("");
	err = parse_declaration_line(&state, &declaration);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_DECLARATION, err);

	// Empty line once whitespace eaten
	state = parser_state_create("  // blah");
	err = parse_declaration_line(&state, &declaration);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_DECLARATION, err);
	TEST_ASSERT_EQUAL_CHAR('\0', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(9, state.pos);

	// Empty line once whitespace eaten, but more after
	state = parser_state_create("  // blah\n@hello");
	err = parse_declaration_line(&state, &declaration);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_DECLARATION, err);
	TEST_ASSERT_EQUAL_CHAR('@', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(10, state.pos);

	// Extra input that isn't whitespace or comment
	state = parser_state_create("@hello bad");
	err = parse_declaration_line(&state, &declaration);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_EXTRA_INPUT, err);
	TEST_ASSERT_EQUAL_CHAR('b', parser_state_current_char(&state));
	TEST_ASSERT_EQUAL_size_t(7, state.pos);

	asm_declaration_destroy(declaration);
}

void test_parse_asm_declarations()
{
	// Empty input
	asm_declarations declarations = asm_declarations_create();
	enum asm_parse_error err = parse_asm_declarations("", &declarations);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	asm_declarations_destroy(declarations);

	// Just space and comments
	declarations = asm_declarations_create();
	err = parse_asm_declarations("//my file\n\n  \n\n //", &declarations);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	asm_declarations_destroy(declarations);

	// Single A instruction
	declarations = asm_declarations_create();
	err = parse_asm_declarations("//my file\n@hello", &declarations);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	TEST_ASSERT_EQUAL_size_t(1, declarations.len);
	TEST_ASSERT_EQUAL_INT(ASM_INST_A, declarations.declarations[0].instruction.type);
	TEST_ASSERT_EQUAL_INT(ASM_A_INST_LABEL, declarations.declarations[0].instruction.a_instruction.type);
	TEST_ASSERT_EQUAL_STRING("hello", declarations.declarations[0].instruction.a_instruction.label);
	asm_declarations_destroy(declarations);
}


int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_eat_line_space_comments);
	RUN_TEST(test_parse_a_instruction);
	RUN_TEST(test_parse_declaration_line);
	RUN_TEST(test_parse_asm_declarations);
	return UNITY_END();
}
