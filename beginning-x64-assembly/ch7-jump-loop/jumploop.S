        .data
number: .quad   1000000000
fmt:    .asciz  "The sum from 0 to %ld is %ld\n"
        .bss
        .text
        .global main
main:
        mov     $0, %rbx         # counter
        mov     $0, %rax         # sum in rax
jloop:
        add     %rbx, %rax
        inc     %rbx
        cmp     (number), %rbx
        jle     jloop

        mov     $fmt, %rdi
        mov     (number), %rsi
        mov     %rax, %rdx
        mov     $0, %rax
        call    printf

        mov     $60, %rax       # 60 = exit
        mov     $0, %rdi        # 0 = success
        syscall                 # quit
