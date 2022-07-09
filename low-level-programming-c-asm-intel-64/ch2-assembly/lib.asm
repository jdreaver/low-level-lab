; Library functions used by other assembly programs

section .text

; Calls the exit() syscall with the argument (rax)
global exit
exit:
        mov	rdi, rax
        mov	rax, 60
        syscall

; Prints the number of non-null chars in a null-terminated string
global strlen
strlen:
        xor	rax, rax        ; rax will hold string length. Init to 0

.loop:
        cmp	byte [rdi + rax], 0 ; Check if current byte is null terminator.
                                    ; byte modifier is necessary here since 0
                                    ; doesn't have enough information to say it
                                    ; is just a single byte.
        je	.end

        inc	rax             ; Increment counter
        jmp	.loop

.end:
        ret

; Prints a null-terminated string to stdout (argument is memory address of the
; first char of the string)
global print_string
print_string:
        ; Save rdi (first argument) since that is a caller-save register and
        ; might get trampled when we call strlen.
        push	rdi

        ; Compute string length. rdi is already set to string
        call	strlen

        ; Write syscall (rax holds string length)
        mov	rdx, rax        ; arg #3 in rdx: how many bytes to write?
        mov	rax, 1          ; syscall number for write stored in rax
        mov	rdi, 1          ; arg #1 in rdi: where to write? 1 for stdout
        pop	rsi             ; arg #2 in rsi: where does the string start?
        syscall

        ret

; Prints a single character to stdout (argument is actual character code, not
; pointer)
global print_char
print_char:
        ; Push argument to the stack
        push	rdi

        ; Call print_string on the stack pointer (the last byte of rdi holds our
        ; character, but because of little-endian, it is pointed to by rsi)
        mov	rdi, rsp
        call	print_string

        ; Repair stack (remember we wrote all of rdi, which is 8 bytes!)
        add	rsp, 8

        ret

global print_newline
print_newline:
        mov	rdi, 0xA
        call	print_char
        ret

; Prints a 64 bit (8 byte) unsigned integer to stdout
global print_uint
print_uint:
        ; Store argument in rax, since that is where div will operate. div
        ; actually operates on the 128 bit integer formed by concatenating
        ; rdx:rax, but we will zero out rdx before every div.
        mov	rax, rdi

        ; Before we modify stack, store location of next digit in rdi, which is
        ; the current value of rsp. This will be the start point for printing.
        mov	rdi, rsp

        ; Allocate 24 bytes on the stack, since the largest possible decimal
        ; value of a 64 bit unsigned int is 2^64-1, which is
        ; 18,446,744,073,709,551,615 (20 digits). We do this with push so we
        ; zero it out, so we end up allocation 24 bytes, which is fine.
        push	0
        push	0
        push	0

        ; Last byte in buffer will be null byte so we can call print_string
        dec	rdi

        ; We will always divide by 10. Store in r8
        mov	r8, 10

.loop:
        ; Decrement offset from rsp
        dec	rdi

        ; Zero out rdx for div so we are just operating on rax
        xor	rdx, rdx

        ; Divide by 10 (in r8) to get next digit from remainder.
        div	r8

        ; Remainder is in rdx. Store remainder + 0x30 (ASCII code for zero) in
        ; buffer. Remember we just want one byte!
        add	dl, 0x30
        mov	[rdi], dl

        ; If rax is non-zero, continue loop again
        test	rax, rax
        jnz	.loop

.end:
        ; Print buffer. rdi already stores pointer to head of string.
        call	print_string

        ; Restore the stack
        add	rsp, 24

        ret

; Prints a 64 bit (8 byte) signed integer to stdout
global print_int
print_int:
        ; If the integer is positive, just print as if uint
        test	rdi, rdi
        jns	print_uint      ; Is this janky?

        ; Print a '-' because we are negative
        mov	rbx, rdi        ; rbx is callee-save, so it will survive the print_char
        mov	rdi, '-'
        call	print_char

        ; Negate the arg and print
        mov	rdi, rbx
        neg	rdi
        jmp	print_uint

; Read a single character from stdin. If stdin is empty and has ended, return 0.
global read_char
read_char:
        ; Create single byte buffer to read into. Initialize buffer to zero in
        ; case stdin is finished and we don't read anything.
        push	0

        ; Read syscall
        mov	rax, 0          ; syscall number for read stored in rax
        mov	rdi, 0          ; arg #1 in rdi: where to read? 0 for stdin
        mov	rsi, rsp        ; arg #2 in rsi: buffer to read into
        mov	rdx, 1          ; arg #3 in rdx: count of how many bytes to read
        syscall

        ; Store buffer in rax to return it
        pop	rax

        ret

; Reads a single word from stdin into a provided buffer. arg1 is a pointer to
; the buffer, and arg2 is the buffer length. Returns the length of the word read
; in (including terminating null byte)
global read_word
read_word:
        ; We will use r12-r15, but those are callee-saved, so we need to save
        ; them and restore before returning.
        push	r12
        push	r13
        push	r14
        push	r15

        ; If buffer length is zero for some stupid reason, return
        test	rsi, rsi
        jz	.end_return_zero

        ; Initialize some variables
        mov	r12, rdi        ; Buffer
        mov	r13, rsi        ; Stack length
        xor	r14, r14        ; Index into stack

.while_whitespace:
        ; Skip over all initial whitespace
        call	read_char
        mov	r15, rax        ; Char we just read is always in r15

        ; If we are at the end of the stream, peace out
        test	rax, rax
        jz	.end_return_zero

        ; Check for whitespace
        mov	rdi, rax
        call	is_whitespace
        test	rax, rax
        jnz	.while_whitespace

        ; Keep calling read_char in a loop and store in buffer as long as we
        ; aren't reading whitespace.
.read_into_buffer:
        ; Store next char in buffer
        mov	[r12 + r14], r15b

        ; Increment index
        inc	r14

        call	read_char
        mov	r15, rax

        ; If we are at the end of the stream, peace out
        test	r15, r15
        jz	.end_success

        ; Check for whitespace
        mov	rdi, r15
        call	is_whitespace
        test	rax, rax
        jnz	.end_success

        ; Check if we are past the end of the buffer
        cmp	r14, r13
        je	.end_return_zero

        ; Didn't find any space, so loop
        jmp	.read_into_buffer

.end_return_zero:
        ; Return 0
        xor	rax, rax
        jmp	.end

.end_success:
        ; Set current location to null
        mov	byte [r12 + r14], 0

.end:
        ; Return word length, store in rax
        mov	rax, r14

        ; Restore r12-r15
        pop	r12
        pop	r13
        pop	r14
        pop	r15

        ; Return (rax already holds length)
        ret

; Accepts one argument, and returns 1 if the char is whitespace (space, tab,
; newline, carriage return), else returns 0
is_whitespace:
        ; Check for space (0x20 in ASCII)
        cmp	dil, 0x20
        je	.end_found

        ; Check for tab (0x9 in ASCII)
        cmp	dil, 0x9
        je	.end_found

        ; Check for line feed (0xA in ASCII)
        cmp	dil, 0xA
        je	.end_found

        ; Check for carriage return (0xD in ASCII)
        cmp	dil, 0xD
        je	.end_found

        ; Note whitespace, return 0
        xor	rax, rax
        ret

.end_found:
        mov	rax, 1
        ret

; Accepts a null-terminated string and tries to parse an unsigned number from
; its start. Returns the number parsed in rax, its characters count in rdx.
;
; Does not check for overflow!
global parse_uint
parse_uint:
        xor	rax, rax        ; Parsed uint
        xor	rcx, rcx        ; Character count (stored in rcx b/c we need rdx for mul)
        mov	r8, 10          ; Store constant 10 here (needed for mul)
.loop:
        ; Subtract ASCII code for zero from current char
        mov	r9b, byte [rdi + rcx]
        sub	r9b, 0x30

        ; Check if current char is > 9 (it is >= 0 b/c unsigned)
        cmp	r9b, 9
        ja	.end

        ; Check passed, multiply and add
        xor	rdx, rdx
        mul	r8
        and	r9, 0x0f
        add	rax, r9

        ; Continue looping
        inc	rcx
        jmp	.loop

.end
        mov	rdx, rcx
        ret

; Accepts a null-terminated string and tries to parse a signed number from its
; start. Returns the number parsed in rax; its characters count in rdx
; (including sign if any). No spaces between sign and digits are allowed.
global parse_int
parse_int:
        mov	r9b, byte [rdi]
        cmp	r9b, '-'
        jne	parse_uint

        ; We must be negative
        inc	rdi             ; Skip '-'
        call	parse_uint
        neg	rax
        inc	rdx             ; Account for '-'
        ret

; Accepts two pointers to strings and compares them. Returns 1 if they are
; equal, otherwise 0.
global string_equals
string_equals:
        mov	r8b, byte [rdi]
        mov	r9b, byte [rsi]

        inc	rdi
        inc	rsi

        ; Check if not equal
        cmp	r8b, r9b
        jne	.fail

        ; They are equal. If one is null we will return, otherwise loop
        test	r8b, r8b
        jnz	string_equals

        ; Success
        mov	rax, 1
        ret

.fail
        xor	rax, rax
        ret

; Accepts a pointer to a string, a pointer to a buffer, and buffer’s length.
; Copies string to the destination. The destination address is returned if the
; string fits the buffer; otherwise zero is returned.
global string_copy
string_copy:
        ; Cache args because we are about to call strlen
        push	rdi
        push	rsi
        push	rdx

        call	strlen          ; N.B. rdi is already pointing to our string

        pop	rdx
        pop	rsi
        pop	rdi

        cmp	rax, rdx
        jae	.fail           ; Use >= to account for null byte

        xor	r8, r8          ; Index var
.loop
        mov	r9b, byte [rdi + r8]
        mov	byte [rsi + r8], r9b
        inc	r8
        test	r9b, r9b
        jnz	.loop           ; Use <= to account for null byte

        ; Success
        mov	rax, rsi
        ret

.fail
        xor	rax, rax
        ret
