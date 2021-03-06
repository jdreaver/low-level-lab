; Test file for lib.asm. Calls every routine in that file.

extern exit
extern strlen
extern print_string
extern print_char
extern print_newline
extern print_uint
extern print_int
extern read_char
extern read_word
extern parse_uint
extern parse_int
extern string_equals
extern string_copy

section .data
test_string:
        db	"012345", 0

strlen_failed_msg:
        db	"strlen had incorrect return value!", 0xA, 0

string_equal_failed_msg:
        db	"string_equal had incorrect return value!", 0xA, 0

string_copy_failed_msg:
        db	"string_equal had incorrect return value!", 0xA, 0

read_char_prompt:
        db	"type a character to read: ", 0

read_word_prompt:
        db	"type a word to read (<= 8 chars): ", 0

test_uint:
        db	"0456132", 0

test_int:
        db	"-0456132", 0

section .bss
string_copy_buffer:
        resb	7

section .text

global _start
_start:
        ; Call strlen
        mov	rdi, test_string
        call	strlen
        cmp	rax, 6
        jne	.strlen_failed

        ; Call print_string
        mov	rdi, test_string
        call    print_string

        ; Call print_char and print_newline a few times
        call	print_newline
        mov	rdi, '_'
        call	print_char
        call	print_newline
        mov	rdi, '|'
        call	print_char
        call	print_newline

        ; Call print_uint
        mov	rdi, 0
        call	print_uint
        call	print_newline

        mov	rdi, 12345678901234
        call	print_uint
        call	print_newline

        ; Call print_int
        mov	rdi, 0
        call	print_int
        call	print_newline

        mov	rdi, 12345678901234
        call	print_int
        call	print_newline

        mov	rdi, -3
        call	print_int
        call	print_newline

        mov	rdi, -12345678901234
        call	print_int
        call	print_newline

        ; Call read_char and print
        mov	rdi, read_char_prompt
        call	print_string
        call	read_char
        mov	rdi, rax
        call	print_char
        call	print_newline

        ; Call read_word (must be length <= 8)
        mov	rdi, read_word_prompt
        call	print_string
        push	0
        mov	rdi, rsp
        mov	rsi, 8
        call	read_word
        mov	rdi, rsp
        call	print_string
        call	print_newline

        ; Call parse_uint
        mov	rdi, test_uint
        call	parse_uint
        mov	rdi, rax
        call	print_uint
        call	print_newline

        ; Call parse_int (on both a positive and negative number)
        mov	rdi, test_int
        call	parse_int
        mov	rdi, rax
        call	print_int
        call	print_newline

        mov	rdi, test_uint
        call	parse_int
        mov	rdi, rax
        call	print_int
        call	print_newline

        ; Call string equal
        mov	rdi, test_string
        mov	rsi, test_string
        call	string_equals
        test	rax, rax
        jz	.string_equal_failed

        mov	rdi, test_string
        mov	rsi, test_uint
        call	string_equals
        test	rax, rax
        jnz	.string_equal_failed

        ; Call string_copy
        mov	rdi, test_string
        mov	rsi, string_copy_buffer
        mov	rdx, 7
        call	string_copy
        cmp	rax, string_copy_buffer
        jne	.string_copy_failed
        mov	rdi, rax
        call	print_string
        call	print_newline

        ; string_copy but buffer too big
        mov	rdi, read_char_prompt
        mov	rsi, string_copy_buffer
        mov	rdx, 7
        call	string_copy
        test	rax, rax
        jnz	.string_copy_failed

        ; Call exit()
        mov	rax, 0
        call	exit

.strlen_failed:
        ; Print failure message
        mov	rdi, strlen_failed_msg
        call	print_string

        mov	rax, 1
        call	exit

.string_equal_failed:
        ; Print failure message
        mov	rdi, string_equal_failed_msg
        call	print_string

        mov	rax, 1
        call	exit

.string_copy_failed:
        ; Print failure message
        mov	rdi, string_copy_failed_msg
        call	print_string

        mov	rax, 1
        call	exit
