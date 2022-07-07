; Library functions used by other assembly programs

section .data

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
        ; Store first two args on stack to save them
        push	rdi
        push	rsi

        ; TODO: We have to keep calling read until we don't see whitespace. That
        ; is, we have to trim leading whitespace.
        ;
        ; Also, the book solution just calls read_char over and over, which
        ; isn't as elegant, but is probably a good option.

        ; Call read() into buffer
        mov	rax, 0          ; syscall number for read stored in rax
        mov	rdx, rsi        ; arg #3 in rdx: count of how many bytes to read
        mov	rsi, rdi        ; arg #2 in rsi: buffer to read into
        mov	rdi, 0          ; arg #1 in rdi: where to read? 0 for stdin
        syscall

        ; Look for whitespace
        pop	rsi             ; Buffer length
        pop	rdi             ; Buffer head
        xor	rax, rax        ; Length of parsed word (we will return this)
.find_whitespace:
        ; Check if we are past the end of the buffer
        cmp	rax, rsi
        je	.end_not_found

        ; If we find whitespace before end, replace with null byte (making sure
        ; to keep track of total string length). Store in cl (lower byte of rcx)
        mov	cl, [rdi + rax]

        ; Check for space (0x20 in ASCII)
        cmp	cl, 0x20
        je	.end_found

        ; Check for tab (0x9 in ASCII)
        cmp	cl, 0x9
        je	.end_found

        ; Check for line feed (0xA in ASCII)
        cmp	cl, 0xA
        je	.end_found

        ; Didn't find any space. Increment and loop
        inc	rax

.end_not_found:
        ; Return 0
        xor	rax, rax
        ret

.end_found:
        ; Set current location to null
        mov	byte [rdi + rax], 0

        ; Return (rax already holds length)
        ret
