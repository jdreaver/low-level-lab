section .data
        msg1 db "Hello, World!",10,0 ; String with NL and 0
        msg1Len         equ $-msg1-1 ; measure length, minus the 0
        msg2 db "Alive and Kicking!",10,0
        msg2Len         equ $-msg2-1 ; measure length, minus the 0
        radius dq       357
        pi dq           3.14
section .bss
section .text
        global _start
_start:
        push    rbp             ; function prologue
        mov     rbp,rsp         ; function prologue

        mov     rax, 1          ; 1 = write
        mov     rdi, 1          ; 1 = to stdout
        mov     rsi, msg1       ; string to display
        mov     rdx, msg1Len    ; length of the string
        syscall

        mov     rax, 1          ; 1 = write
        mov     rdi, 1          ; 1 = to stdout
        mov     rsi, msg2       ; string to display
        mov     rdx, msg2Len    ; length of the string
        syscall

        mov     rsp,rbp         ; function epilogue
        pop     rbp             ; function epilogue

        mov     rax, 60         ; 60 = exit
        mov     rdi, 0          ; 0 = success
        syscall                 ; quit
