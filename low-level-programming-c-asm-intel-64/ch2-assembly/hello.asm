global _start

section .data
message:
        db	'hello, world', 10

section .text
_start:
        mov	rax, 1          ; syscall number for write stored in rax
        mov	rdi, 1          ; arg #1 in rdi: where to write? 1 for stdout
        mov	rsi, message    ; arg #2 in rsi: where does the string start?
        mov	rdx, 14         ; arg #3 in rdx: how many bytes to write?
        syscall

        mov	rax, 60         ; syscall number for exit
        xor	rdi, rdi        ; arg #1: Exit status. This sets it to zero
        syscall
