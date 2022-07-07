; Test file for lib.asm. Calls every routine in that file.

extern exit
extern strlen
extern print_string
extern print_char
extern print_newline
extern print_uint
extern print_int

section .data
test_string:
        db	"012345", 0

strlen_failed_msg:
        db	"strlen had incorrect return value!", 0xA, 0

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

        ; Call exit()
        mov	rax, 0
        call	exit

.strlen_failed:
        ; Print failure message
        mov	rdi, strlen_failed_msg
        call	print_string

        mov	rax, 1
        call	exit
