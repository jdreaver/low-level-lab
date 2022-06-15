        .data
number1:        .quad 42
number2:        .quad 41
fmt1:           .asciz "NUMBER1 >= NUMBER2\n"
fmt2:           .asciz "NUMBER1 < NUMBER2\n"

        .text

        .global main
main:
        mov     (number1),%rax
        mov     (number2),%rbx

        cmp     %rbx,%rax
        jge     greater

        ## %rax is smaller
        mov     $fmt2,%rdi
        mov     $0,%rax
        call    printf
        jmp     exit

greater:
        mov     $fmt1,%rdi
        mov     $0,%rax
        call    printf

exit:
        mov     $60, %rax       # 60 = exit
        mov     $0, %rdi        # 0 = success
        syscall                 # quit
