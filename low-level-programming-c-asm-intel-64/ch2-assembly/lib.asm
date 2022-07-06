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
        ; Save rdi (first argument) since that is a caller-save register
        mov	r8, rdi

        ; Compute string length. rdi is already set to string
        call	strlen

        ; Write syscall (rax holds string length)
        mov	rdx, rax        ; arg #3 in rdx: how many bytes to write?
        mov	rax, 1          ; syscall number for write stored in rax
        mov	rdi, 1          ; arg #1 in rdi: where to write? 1 for stdout
        mov	rsi, r8         ; arg #2 in rsi: where does the string start?
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
