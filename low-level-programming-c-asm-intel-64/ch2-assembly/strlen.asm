section .data
test_string:
        db	"012345", 0
section .text
global _start

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

_start:
        mov	rdi, test_string
        call	strlen

        ; Call exit with the output of strlen in rdx
        mov	rdi, rax
        mov	rax, 60
        syscall
