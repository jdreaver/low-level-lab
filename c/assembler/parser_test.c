#include <assert.h>
#include <string.h>

#include "parser.h"
// N.B. Including whole source file to get at static functions
#include "parser.c"

void assert_strings_equal(char *s1, char *s2, const char *__file, unsigned int __line, const char *__function) {
	if (strcmp(s1, s2) != 0) {
		fprintf(stderr, "%s:%u: %s: Strings not equal!\ns1: %s\ns2: %s\n", __file, __line, __function, s1, s2);
		exit(EXIT_FAILURE);
	}
}

void test_eat_whitespace_comments()
{
	puts("Testing empty line");
	char *empty_line = "";
	eat_whitespace_comments(&empty_line);
	assert_strings_equal(empty_line, "", __FILE__, __LINE__, __func__);

	puts("Testing comment line");
	char *simple_comment = "   // asdskljlk dfslkjk dslk // sdf// //";
	eat_whitespace_comments(&simple_comment);
	assert_strings_equal(simple_comment, "", __FILE__, __LINE__, __func__);

	puts("Testing comment line followed by non-comment");
	char *non_comment = "   // asdskljlk dfslkjk dslk // sdf// //\nhello!";
	eat_whitespace_comments(&non_comment);
	assert_strings_equal(non_comment, "hello!", __FILE__, __LINE__, __func__);

	puts("Testing comment line followed by non-comment and space");
	char *non_comment_then_space = "   // asdskljlk dfslkjk dslk // sdf// //\n   hello!";
	eat_whitespace_comments(&non_comment_then_space);
	assert_strings_equal(non_comment_then_space, "hello!", __FILE__, __LINE__, __func__);
}


void test_parse_a_instruction()
{
	puts("Testing no @");
	char *input = "";
	struct asm_a_instruction instruction;
	enum asm_parse_error err = parse_a_instruction(&input, &instruction);
	assert(err == ASM_PARSE_ERROR_A_INSTRUCTION_MISSING_AT_SYMBOL);

	puts("Testing simple @123");
	input = "@123";
	err = parse_a_instruction(&input, &instruction);
	assert(err == ASM_PARSE_ERROR_NO_ERROR);
	assert(instruction.type == ASM_A_INST_ADDRESS);
	assert(instruction.address == 123);

	puts("Testing bare @");
	input = "@";
	err = parse_a_instruction(&input, &instruction);
	assert(err == ASM_PARSE_ERROR_UNEXPECTED_EOF);

	puts("Testing logical overflow @32768");
	input = "@32768";
	err = parse_a_instruction(&input, &instruction);
	assert(err == ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE);

	puts("Testing uint16_t overflow @32768231432848329789437289");
	input = "@32768231432848329789437289";
	err = parse_a_instruction(&input, &instruction);
	assert(err == ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE);

	// TODO: Add this once we support labels, since this goes down label path
	/* puts("Testing negative @-398"); */
	/* input = "@-398"; */
	/* err = parse_a_instruction(&input, &instruction); */
	/* assert(err == ASM_PARSE_ERROR_A_INSTRUCTION_ADDRESS_TOO_LARGE); */
}

int main()
{
	puts("test_eat_whitespace_comments()");
	test_eat_whitespace_comments();
	puts("test_parse_a_instruction()");
	test_parse_a_instruction();
}
