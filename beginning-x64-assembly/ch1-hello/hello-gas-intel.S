.intel_syntax noprefix

        .data
msg:    .asciz  "hello, world"

        .global main

        .text
main:
        mov     rax, 1
        mov     rdi, 1
        mov     rsi, msg
        mov     rdx, 12
        syscall
        mov     rax, 60
        mov     rdi, 0
        syscall
