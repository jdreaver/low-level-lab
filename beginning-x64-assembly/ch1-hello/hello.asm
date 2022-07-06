section .data
        msg db  "hello, world",0
section .bss
section .text
        global _start
_start:
        mov     rax, 1          ; 1 = write
        mov     rdi, 1          ; 1 = to stdout
        mov     rsi, msg        ; string to display in rsi
        mov     rdx, 12         ; length of the string, without 0
        syscall                 ; display the string
        mov     rax, 60         ; 60 = exit
        mov     rdi, 0          ; 0 = success
        syscall                 ; quit
