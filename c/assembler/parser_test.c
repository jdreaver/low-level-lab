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

	// TODO: Add this once we support labels, since this goes down label path
	// puts("Testing negative @-398");
	// input = "@-398";
	// err = parse_a_instruction(&state, &instruction);
	// assert(err == ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_eat_line_space_comments);
	RUN_TEST(test_parse_a_instruction);
	return UNITY_END();
}
