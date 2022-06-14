extern printf
section .data
        msg db  "Hello, world!",0
        fmtstr db       "This is our string: %s",10,0
section .bss
section .text
        global main
main:
        push    rbp
        mov     rbp,rsp

        mov     rdi, fmtstr     ; first arg to printf
        mov     rsi, msg        ; second arg to print
        mov     rax, 0          ; No xmm registers involved
        call    printf

        mov     rsp,rbp
        pop     rbp
        mov     rax, 60         ; 60 = exit
        mov     rdi, 0          ; 0 = success
        syscall                 ; quit
