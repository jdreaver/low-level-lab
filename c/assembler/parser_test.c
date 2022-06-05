#include "unity.h"

#include <assert.h>
#include <string.h>

#include "parser.h"
// N.B. Including whole source file to get at static functions
#include "parser.c"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_eat_whitespace_comments()
{
	char *empty_line = "";
	eat_whitespace_comments(&empty_line);
	TEST_ASSERT_EQUAL_STRING("", empty_line);

	char *simple_comment = "   // asdskljlk dfslkjk dslk // sdf// //";
	eat_whitespace_comments(&simple_comment);
	TEST_ASSERT_EQUAL_STRING("", simple_comment);

	char *non_comment = "   // asdskljlk dfslkjk dslk // sdf// //\nhello!";
	eat_whitespace_comments(&non_comment);
	TEST_ASSERT_EQUAL_STRING("hello!", non_comment);

	char *non_comment_then_space = "   // asdskljlk dfslkjk dslk // sdf// //\n   hello!";
	eat_whitespace_comments(&non_comment_then_space);
	TEST_ASSERT_EQUAL_STRING("hello!", non_comment_then_space);
}


void test_parse_a_instruction()
{

	// Missing @ symbol
	char *input = "";
	struct asm_a_instruction instruction;
	enum asm_parse_error err = parse_a_instruction(&input, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_A_INSTRUCTION_MISSING_AT_SYMBOL, err);

	// Simple success
	input = "@123";
	err = parse_a_instruction(&input, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_NO_ERROR, err);
	TEST_ASSERT_EQUAL_INT(ASM_A_INST_ADDRESS, instruction.type);
	TEST_ASSERT_EQUAL_UINT16(123, instruction.address);

	// Bare @
	input = "@";
	err = parse_a_instruction(&input, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_UNEXPECTED_EOF, err);

	// Logical overflow (fits in uint16_t, but is too large for address)
	input = "@32768";
	err = parse_a_instruction(&input, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE, err);

	// uint16_t overflow
	input = "@32768231432848329789437289";
	err = parse_a_instruction(&input, &instruction);
	TEST_ASSERT_EQUAL_INT(ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE, err);

	// TODO: Add this once we support labels, since this goes down label path
	/* puts("Testing negative @-398"); */
	/* input = "@-398"; */
	/* err = parse_a_instruction(&input, &instruction); */
	/* assert(err == ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE); */
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_eat_whitespace_comments);
	RUN_TEST(test_parse_a_instruction);
	return UNITY_END();
}
